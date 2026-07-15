// nFTDFileTransferDialog.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "nFTDServerDlg.h"
#include "nFTDFileTransferDialog.h"
#include "afxdialogex.h"

#include <thread>
#include <chrono>	//20260714 by claude. std::this_thread::sleep_for(worker 스레드 pause 대기) 용.
#include <algorithm>
#include <map>			//20260714 by claude. 파일 아이콘 확장자별 캐시(대량 리스트 채우기 가속)
#include <shobjidl.h>	//20260713 by claude. ITaskbarList3(작업표시줄 진행율)

#include "Common/MemoryDC.h"

extern HMODULE g_hRes;

// CnFTDFileTransferDialog 대화 상자

IMPLEMENT_DYNAMIC(CnFTDFileTransferDialog, CSCThemeDlg)

CnFTDFileTransferDialog::CnFTDFileTransferDialog(CWnd* pParent /*=nullptr*/)
	: CSCThemeDlg(IDD_FILETRANSFER, pParent)
{
	m_ProgressData.nTime = 0;
	m_ProgressData.nTransferedFile = 0;
	m_ProgressData.bBlocked = FALSE;
	m_ProgressData.fSpeed = 0;
	m_ProgressData.ulReceivedSize.QuadPart = 0;
	m_ProgressData.ulRemainTimeSecond.QuadPart = 0;
	m_ProgressData.ulTotalSize.QuadPart = 0;
}

CnFTDFileTransferDialog::~CnFTDFileTransferDialog()
{
}

void CnFTDFileTransferDialog::OnDestroy()
{
	TRACE(_T("CnFTDFileTransferDialog::OnDestroy()\n"));
	while (m_thread_transfer_started)
	{
		TRACE(_T("wait until transfer thread is finished\n"));
		Wait(1000);
	}

	//20260713 by claude. 전송창 종료 시 작업표시줄 빨강(TBPF_ERROR) 리셋. 워커 스레드가 만든 m_pTaskbar 는 이미 Release 됐고
	//COM 은 아파트먼트 종속이라 UI 스레드인 여기서 그 객체를 쓸 수 없으므로, UI 스레드에서 임시 객체를 새로 만들어 NOPROGRESS 로 지운다.
	if (m_taskbar_hwnd)
	{
		HRESULT hr_init = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		ITaskbarList3* p_taskbar = NULL;
		if (SUCCEEDED(::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&p_taskbar))) && p_taskbar)
		{
			p_taskbar->HrInit();
			p_taskbar->SetProgressState(m_taskbar_hwnd, TBPF_NOPROGRESS);
			p_taskbar->Release();
		}
		//S_FALSE(이미 초기화됨)도 SUCCEEDED — CoInitialize 성공 건은 CoUninitialize 로 균형을 맞춘다.
		//RPC_E_CHANGED_MODE(실패)면 우리가 초기화한 게 아니므로 해제하지 않는다.
		if (SUCCEEDED(hr_init))
		{
			::CoUninitialize();
		}
	}

	__super::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CnFTDFileTransferDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_static_message);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_STATIC_INDEX_BYTES, m_static_index_bytes);
	DDX_Control(pDX, IDC_STATIC_REMAIN_SPEED, m_static_remain_speed);
	DDX_Control(pDX, IDC_STATIC_COPY, m_static_copy);
}


BEGIN_MESSAGE_MAP(CnFTDFileTransferDialog, CSCThemeDlg)
	ON_BN_CLICKED(IDOK, &CnFTDFileTransferDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CnFTDFileTransferDialog::OnBnClickedCancel)
	ON_WM_WINDOWPOSCHANGED()
	//타이틀바/테두리 처리(LBUTTONDOWN/SETCURSOR/PAINT/ERASEBKGND/GETMINMAXINFO)는 base CSCThemeDlg 에 위임.
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//최신 Common 의 CSCSystemButtons 는 min/max/close 를 WM_SYSCOMMAND 가 아니라 Message_CSCSystemButtons 로 parent 에
	//보낸다. 핸들러가 없으면 닫기 버튼이 무반응이므로(메인 dlg 와 동일 패턴) 여기서 처리한다.
	ON_REGISTERED_MESSAGE(Message_CSCSystemButtons, &CnFTDFileTransferDialog::on_message_CSCSystemButtons)
	ON_STN_CLICKED(IDC_STATIC_MESSAGE, &CnFTDFileTransferDialog::OnStnClickedStaticMessage)
	ON_COMMAND(ID_MENU_TRANSFER_OPEN_FOLDER, &CnFTDFileTransferDialog::OnMenuTransferOpenFolder)
	ON_COMMAND(ID_MENU_TRANSFER_COPY_PATH_TO_CLIPBOARD, &CnFTDFileTransferDialog::OnMenuTransferCopyPathToClipboard)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CnFTDFileTransferDialog 메시지 처리기

BOOL CnFTDFileTransferDialog::OnInitDialog()
{
	CSCThemeDlg::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	init_list();

	m_resize.Create(this);
	m_resize.SetMinimumTrackingSize(CSize(400, 320));
	m_resize.Add(IDC_STATIC_COPY, 50, 0, 0, 0);
	m_resize.Add(IDC_STATIC_MESSAGE, 0, 0, 100, 0);
	m_resize.Add(IDC_STATIC_INDEX_BYTES, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_REMAIN_SPEED, 50, 0, 50, 0);
	m_resize.Add(IDC_PROGRESS, 0, 0, 100, 0);
	m_resize.Add(IDC_LIST, 0, 0, 100, 100);

	set_color_theme(theApp.m_color_theme);
	m_messagebox.create(this, _S(NFTD_IDS_FILETRANSFER));
	m_messagebox.set_color_theme(theApp.m_color_theme);
	SetWindowText(_S(NFTD_IDS_FILETRANSFER));
	set_titlebar_height(TOOLBAR_TITLE_HEIGHT);
	show_titlebar_logo(false);
	set_as_toolbar();
	//set_draw_border(true);

	m_sys_buttons.set_button_width(TOOLBAR_TITLE_BUTTON_WIDTH);

	m_static_copy.set_back_image(_T("GIF"), IDR_GIF_COPY, m_theme.cr_back);
	m_static_copy.fit_to_back_image(false);

	m_static_message.set_back_color(m_theme.cr_back);
	m_static_message.set_no_prefix(true);	//파일명의 '&' 를 니모닉 접두로 먹지 않고 글자 그대로 표시(SS_NOPREFIX 미지정 컨트롤이라 강제 필요).
	//m_static_message.word

	m_static_index_bytes.set_back_color(m_theme.cr_back);
	m_static_remain_speed.set_back_color(m_theme.cr_back);

	m_progress.set_style(CSCSliderCtrl::style_progress);
	m_progress.set_text_style(CSCSliderCtrl::text_style_percentage);
	//m_progress.set_track_color(gRGB(36, 160, 212), gRGB(230, 230, 230));
	m_progress.set_track_color(Gdiplus::Color::RoyalBlue, gRGB(230, 230, 230));
	//m_progress.set_inactive_color(white);

	//borderless 윈도우 스타일 정규화(WS_THICKFRAME/WS_SYSMENU/... 부여 + SWP_FRAMECHANGED)는
	//CSCThemeDlg::OnInitDialog() 이 정석대로 처리한다(borderless dialog.md Step 4). 깜빡임 감소용 WS_CLIPCHILDREN 만 추가.
	//(예전엔 SetWindowLong 으로 스타일을 통째로 WS_CLIPCHILDREN 하나로 덮어써 base 의 WS_THICKFRAME 등을 지웠다.)
	ModifyStyle(0, WS_CLIPCHILDREN);

	RestoreWindowPosition(&theApp, this, _T("CnFTDFileTransferDialog"));
	Wait(10);
	CenterWindow(GetParent());

	//20260713 by claude. 작업표시줄 버튼을 가진 메인 창 HWND 를 UI 스레드에서 캡처(전송 워커 스레드에서 taskbar 진행율 갱신에 사용).
	m_taskbar_hwnd = AfxGetMainWnd() ? AfxGetMainWnd()->GetSafeHwnd() : NULL;

	m_thread_transfer = std::thread(&CnFTDFileTransferDialog::thread_transfer, this);
	m_thread_transfer.detach();

	//모든 전송이 완료된 후 옵션에 따라 전송창을 닫아야하는데 위 thread가 언제 종료되었는지
	//현재는 알 수 없다. m_thread_transfer_started 플래그로 우선 판단한다.
	//단, 모든 전송이 완료되었어도 1건이라도 실패가 발생했다면 창을 닫지 않도록 한다.
	SetTimer(timer_check_thread_transfer, 2000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CnFTDFileTransferDialog::OnBnClickedOk()
{
}


void CnFTDFileTransferDialog::OnBnClickedCancel()
{
	if (m_thread_transfer_started)
	{
		m_static_copy.pause_gif(-1);
		m_pServerManager->m_DataSocket.set_transfer_pause(true);
		
		m_messagebox.set_title(_S(NFTD_IDS_FILETRANSFER_CANCEL));
		int res = m_messagebox.DoModal(_S(NFTD_IDS_MSGBOX_CANCELTRANSFER), MB_OKCANCEL);
		if (res == IDCANCEL)
		{
			m_static_copy.play_gif();
			m_pServerManager->m_DataSocket.set_transfer_pause(false);
			return;
		}

		TRACE(_T("파일전송 취소 처리\n"));
		m_static_copy.stop_gif();
		m_pServerManager->m_DataSocket.set_transfer_stop();

		//20260715 by claude. '중단 요청'만 켠다. 예전엔 여기서 m_thread_transfer_started = false 로 만들었는데, 그 플래그는
		//'스레드 생존'을 뜻해서 OnDestroy 의 대기 루프가 '이미 끝났다'로 오인하고 곧장 창을 파괴했다. 그 시점에 스레드는
		//아직 recv_file 안에 있어(1GB 파일이면 수 분) 파괴된 m_list/m_static_copy 를 건드려 죽었다.
		//이제 스레드 종료 대기는 아래 CDialogEx::OnCancel() → OnDestroy 의 while(m_thread_transfer_started) 가 실제로 수행한다.
		m_transfer_cancelled = true;
	}

	m_list.save_column_width(&theApp, _T("CnFTDFileTransferDialog list 3col"));

	CDialogEx::OnCancel();
}

//CSCSystemButtons(최소화/최대화/닫기)는 최신 Common 에서 Message_CSCSystemButtons 를 parent 로 보낸다.
//parent 인 본 dlg 가 직접 cmd 를 처리해야 버튼이 동작한다(핸들러 없으면 닫기 무반응 — 어제 Common 적용 후 회귀).
LRESULT CnFTDFileTransferDialog::on_message_CSCSystemButtons(WPARAM wParam, LPARAM lParam)
{
	CSCSystemButtonsMessage* msg = (CSCSystemButtonsMessage*)wParam;

	switch (msg->cmd)
	{
		case SC_MINIMIZE:
			ShowWindow(SW_MINIMIZE);
			break;
		case SC_RESTORE:
		case SC_MAXIMIZE:
			ShowWindow(IsZoomed() ? SW_RESTORE : SW_MAXIMIZE);
			break;
		case SC_CLOSE:
			//종료 절차(전송 중이면 취소 확인, 완료면 창 닫기)는 OnBnClickedCancel 에 일원화돼 있다.
			OnBnClickedCancel();
			break;
	}

	return 0;
}

//20260713 by claude. 전송 완료 후 표시된 요약("전송 성공 : N, 실패 : M  클릭하여 확인") 문구를 클릭하면
//실패 항목들을 순환하며 보여준다. 현재 위치(m_fail_view_index)를 +1(마지막→0 순환)한 뒤 그 실패 항목을 리스트에서 가시화·선택한다.
//(전송 완료 시점에 이미 0번 실패 항목을 보여주므로, 첫 클릭은 그 다음(1번) 항목으로 이동한다.)
void CnFTDFileTransferDialog::OnStnClickedStaticMessage()
{
	if (m_fail_items.empty())
		return;

	m_fail_view_index = (m_fail_view_index + 1) % (int)m_fail_items.size();
	m_list.select_item(m_fail_items[m_fail_view_index], true, true, true);
}

BOOL CnFTDFileTransferDialog::FileTransferInitalize(CnFTDServerManager* pServerManager, std::deque<WIN32_FIND_DATA>* filelist,
			ULARGE_INTEGER* pulRemainDiskSpace, int srcSide, int dstSide, CString from, CString to, BOOL isAutoClose, LPCTSTR lpszStartPath)
{
	m_filelist.clear();
	m_filelist.assign(filelist->begin(), filelist->end());

	m_srcSide = srcSide;
	m_dstSide = dstSide;
	m_transfer_from = from;
	m_transfer_to = to;
	m_pServerManager = pServerManager;
	m_auto_close = isAutoClose;

	m_ProgressData.ulRemainDiskSpace.QuadPart = pulRemainDiskSpace->QuadPart;

	return TRUE;
}

void CnFTDFileTransferDialog::init_list()
{
	CString headings;

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	headings.Format(_T("%s,200;%s,100;%s,60"), _S(NFTD_IDS_LISTCTRL_NAME), _S(NFTD_IDS_LISTCTRL_SIZE), _S(NFTD_IDS_LISTCTRL_STATUS));
	m_list.set_headings(headings);

	m_list.set_color_theme(m_theme.get_color_theme());
	m_list.set_border_color(Gdiplus::Color::DarkGray);

	m_list.set_font_size(theApp.GetProfileInt(_T("list"), _T("font size"), 9));
	m_list.set_font_name(theApp.GetProfileString(_T("list"), _T("font name"), _S(IDS_FONT)));

	m_list.set_header_height(22);

	m_list.set_use_own_imagelist(true);
	m_list.set_shell_imagelist(&theApp.m_shell_imagelist);
	m_list.set_line_height(20);

	m_list.set_column_text_align(col_filesize, HDF_RIGHT);
	m_list.set_column_text_align(col_status, HDF_CENTER);

	m_list.set_column_data_type(col_status, CSCListCtrl::column_data_type_progress);
	m_list.show_progress_text();

	m_list.restore_column_width(&theApp, _T("CnFTDFileTransferDialog list 3col"));

	//20260714 by claude. 창 리사이즈 시 이름 컬럼이 남는 폭을 흡수하도록(크기·상태는 폭 유지). 크기/상태 뒤엔 빈 영역뿐이라 이름이 늘어야 의미가 있다.
	//실제 폭 재계산은 CSCListCtrl::OnSize 가 m_fixed_width_column 을 보고 자동 수행(리스트는 m_resize 로 창과 함께 리사이즈됨).
	m_list.set_fixed_width_column(col_filename);

	m_list.allow_edit(false);
	m_list.allow_sort(false);

	//전송창 리스트는 우클릭 메뉴가 없다. CSCListCtrl 기본(자체 메뉴 true)을 끄고 parent 로 위임(핸들러 없어 표시 안 됨) — 기존 동작 유지.
	m_list.set_use_own_context_menu(false);
	m_list.use_indent_from_prefix_space(true);
}

//20260713 by claude. 작업표시줄 버튼 진행율(0~100) 갱신. 전송 갯수 기준 값(화면 진행바와 동일)을 그대로 넘겨받는다. m_pTaskbar 는
//전송 워커 스레드에서 생성됐고, 이 함수도 그 스레드(및 소켓 send/recv_file)에서 호출되므로 같은 COM 아파트먼트다.
void CnFTDFileTransferDialog::set_taskbar_progress(int percent)
{
	if (!m_pTaskbar || !m_taskbar_hwnd)
	{
		return;
	}
	if (percent < 0)
	{
		percent = 0;
	}
	if (percent > 100)
	{
		percent = 100;
	}
	m_pTaskbar->SetProgressValue(m_taskbar_hwnd, (ULONGLONG)percent, 100);
}

void CnFTDFileTransferDialog::thread_transfer()
{
	int i;

	m_thread_transfer_started = true;

	//20260713 by claude. 작업표시줄 진행율 준비 — 이 워커 스레드에서 COM 초기화 후 taskbar 객체를 만든다(이후 갱신도 같은 스레드=같은 아파트먼트).
	//NORMAL 상태(초록)로 시작. 종료 시(아래) NOPROGRESS + Release + CoUninitialize.
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbar))) && m_pTaskbar)
	{
		m_pTaskbar->HrInit();
		if (m_taskbar_hwnd)
		{
			m_pTaskbar->SetProgressState(m_taskbar_hwnd, TBPF_NORMAL);
		}
	}

	m_static_message.set_text(_S(IDS_MAKE_FILE_FOLDER_LIST));

	//폴더인 항목은 그 항목을 유지한 채 하위 모든 폴더, 파일 목록을 찾아서 추가시킨다.
	for (i = 0; i < m_filelist.size(); i++)
	{
		if (m_filelist[i].dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{

			std::deque<WIN32_FIND_DATA> dq;

			//로컬인 경우는 직접 찾지만 remote인 경우는 요청해서 채워야 한다.
			if (m_srcSide == SERVER_SIDE)
			{
				find_all_files(m_filelist[i].cFileName, &dq, _T(""), true, true);
			}
			else
			{
				m_pServerManager->get_filelist(m_filelist[i].cFileName, &dq, true);
			}

			if (dq.size() > 0)
			{
				m_filelist.insert(m_filelist.begin() + i + 1, dq.begin(), dq.end());
				//20260714 by claude. dq(재귀 확장된 하위 전체)는 재확장하지 않도록 건너뛴다. i 를 dq 마지막 항목에 두면
				//for 의 i++ 가 그 다음(원래의 형제 항목)으로 정확히 이동한다. 예전 '+1' 은 형제 1개를 건너뛰어(폴더 여러 개
				//동시 전송 시 뒤 폴더가 확장 안 되던 원인) 제거.
				i = i + dq.size();
			}
		}
	}

	/*
	std::sort(m_filelist.begin(), m_filelist.end(), [](WIN32_FIND_DATA a, WIN32_FIND_DATA b)
	{
		return (_tcscmp(a.cFileName, b.cFileName) < 0);
	}
	);
	*/

	int folder_count = 0;
	int file_count = 0;
	int image_index = -1;
	int depth;				//폴더 depth에 따라 listctrl에서 들여쓰기 위해.
	bool is_folder;
	CString filename;
	CString folder;
	CString size_str;
	ULARGE_INTEGER file_size;

	m_ProgressData.total_count = m_filelist.size();
	m_ProgressData.ulTotalSize.QuadPart = 0;
	m_ProgressData.ulReceivedSize.QuadPart = 0;

	m_progress.SetRange(0, m_filelist.size());

	//20260714 by claude. 대량(수천 항목) 리스트 채우기 가속:
	// (1) begin/end_bulk_insert — 항목마다 하던 sync_scrollbar·Invalidate·재도색을 억제하고 루프 종료 후 1회만 반영(주 병목).
	// (2) 폴더 아이콘은 디스크 접근 없는 캐시값(get_folder_icon) 사용 — 기존 "c:\\windows" 조회는 폴더마다 PathFileExists+SHGetFileInfo(디스크) 였다.
	// (3) 파일 아이콘은 확장자별 캐시 — GetSystemImageListIcon(file) 은 SHGFI_USEFILEATTRIBUTES 라 같은 확장자면 같은 아이콘 → 동종 파일 수천개에서 shell 호출을 확장자 종류 수로 축소.
	int folder_icon = theApp.m_shell_imagelist.get_folder_icon();
	std::map<CString, int> ext_icon_cache;

	m_list.begin_bulk_insert();

	//list에 그 내용을 채워준다.
	for (i = 0; i < m_filelist.size(); i++)
	{
		//TRACE(_T("src[%3d] = %s\n"), i, m_filelist[i].cFileName);
		is_folder = (m_filelist[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

		if (is_folder)
		{
			size_str.Empty();
			image_index = folder_icon;
			folder_count++;
		}
		else
		{
			file_size.LowPart = m_filelist[i].nFileSizeLow;
			file_size.HighPart = m_filelist[i].nFileSizeHigh;
			m_ProgressData.ulTotalSize.QuadPart += file_size.QuadPart;

			size_str = get_size_str(file_size.QuadPart);

			CString ext = get_part(m_filelist[i].cFileName, fn_ext);
			ext.MakeLower();
			auto it = ext_icon_cache.find(ext);
			if (it != ext_icon_cache.end())
				image_index = it->second;
			else
			{
				image_index = theApp.m_shell_imagelist.GetSystemImageListIcon(0, m_filelist[i].cFileName, false);
				ext_icon_cache[ext] = image_index;
			}
			file_count++;
		}

		filename = get_part(m_filelist[i].cFileName, fn_name);

		//depth 표시를 위해 앞에 depth만큼 공백을 추가한다. m_transfer_from을 뺀 후 '\\'의 개수를 세어야 한다.
		folder = m_filelist[i].cFileName;
		folder.Replace(m_transfer_from, _T(""));
		depth = get_char_count(folder, '\\') - 1;

		//"파일명,200;크기,100;상태,60". col_filename(0)=아이콘+들여쓰기+파일명, col_filesize(1), col_status(2).
		m_list.insert_item(i, image_index, duplicate_str(_T(" "), depth) + filename, size_str, _T(""));

		//20260714 by claude. 대량 추가 진행 표시 — 좌상단에 "추가한 인덱스 / 총 파일수". 매 항목 갱신은 비싸므로 128 간격으로 스로틀.
		if ((i & 0x7F) == 0)
			m_static_index_bytes.set_textf(_T("%d / %d"), i + 1, (int)m_filelist.size());
	}

	m_list.end_bulk_insert();

	TRACE(_T("file = %d, folder = %d\n"), file_count, folder_count);

	//디스크 남은 용량 체크
	if (false)//m_ProgressData.ulTotalSize.QuadPart > m_ProgressData.ulRemainDiskSpace.QuadPart)
	{
		CString message;
		message.LoadString(NFTD_IDS_MSGBOX_DISKFULL);

		m_messagebox.DoModal(message);

		EndDialog(0);
		return;
	}


	//전체 데이터 송수신 양은 항상 100%로 환산한다. 그래야 용량에 관계없이 표현가능하다.
	m_progress.SetPos(0);
	set_taskbar_progress(0);	//20260713 by claude. 작업표시줄 진행율(전송 갯수 기준, 화면 진행바와 동일 값) 미러.
	m_progress.SetRange(0, 100);

	int res;
	//CString to;
	CString msg;
	WIN32_FIND_DATA to;
	CString start_time;
	CString end_time;

	/*
	//데이터 전송을 위한 소켓을 연결하고 (전송이 모두 완료되면 DataClose())
	//지금은 하나의 소켓을 연결하고 모든 파일들을 순차적으로 전송하지만
	//차후에는 하나의 파일마다 하나의 소켓을 할당해서 전송되도록 해야만 멀티전송이 가능하다.
	//최대 10개의 소켓을 생성하고 대기중인 소켓을 연결해서 전송해야 한다.
	if (!m_pServerManager->DataConnect())
	{
		msg.LoadString(g_hRes, NFTD_IDS_TRANSFER_FAIL);
		m_static_message.set_text(msg, Gdiplus::Color::Red);
		return;
	}
	*/

	m_thread_transfer_started = true;
	//GetDlgItem(IDCANCEL)->EnableWindow(true);

	//전송 상태를 메시지로 받아 표시해봤으나 메시지 처리 방식은 매우 딜레이가 심함.
	//컨트롤들을 건네받아 컨트롤에서 직접 표시함.
	//m_pServerManager->m_DataSocket.set_ui_controls(this, &m_progress, &m_list);
	m_pServerManager->m_DataSocket.SetFileWriteMode(WRITE_UNKNOWN);


	//추후 양방향 파일목록이 섞이도록 수정할 경우는 아래 for문 안으로 들어가야 한다.
	m_static_copy.set_back_image_mirror(m_dstSide == SERVER_SIDE);

	//취소 시 창 닫기 판단용: 이 시점까지 실제로 전송 완료(success/overwrite)된 항목 수.
	//1개 이상 전송됐으면 취소해도 "완료 후 닫기" 체크박스를 따르고(결과 확인 위해 남길 수 있게), 0개면 볼 게 없으니 무조건 닫는다.
	int transferred_count = 0;

	//20260713 by claude. 스킵된 항목 수(전송 결과 skip + 동일 경로 스킵). 완료 요약("성공/스킵/실패")에 표시한다.
	int skipped_count = 0;

	//20260713 by claude. 새 전송 시작 시 실패 항목 기록을 리셋. 이후 실패마다 해당 인덱스를 push_back 한다.
	m_fail_items.clear();
	m_fail_view_index = 0;

	//20260710 by claude. 표시용 평균 전송속도의 세션 기준점 초기화(목록 빌드 시간은 제외). 실제 시작 clock 은 소켓 코드가 첫 청크에서 잡는다(0=미시작).
	m_ProgressData.session_bytes.QuadPart = 0;
	m_ProgressData.session_start_clock = 0;

	//20260714 by claude. is_same_device(같은 PC 내 local↔local 여부) = 커맨드라인 peer IP(__targv[4])와 내 IP 비교로 결정되는
	//세션 불변값이라 루프 밖에서 1회만 계산한다. 예전엔 매 파일마다 get_my_ip()(GetNetworkInformation 조회)를 호출해 낭비였다.
	bool is_same_device = (__argc > 4) && (get_my_ip().Compare(__targv[4]) == 0);

	//실제 파일 송수신 시작
	for (i = 0; i < m_filelist.size(); i++)
	{
		res = transfer_result_fail;

		//전송중에 취소한 경우 이 플래그를 보고 중지시킨다.
		//20260715 by claude. 판정 기준을 '스레드 생존'(m_thread_transfer_started)에서 '중단 요청'(m_transfer_cancelled)으로 분리.
		if (m_transfer_cancelled)
		{
			m_static_copy.stop_gif();
			break;
		}

		//20260714 by claude. 일시정지(취소 확인창 / 디버그 스페이스바)는 send_file/recv_file 의 청크 루프에서만 검사됐다.
		//already-exists 스킵이 다수인 전송(예: 스킵 252/전송 48)에선 스킵 파일이 청크 루프 전에 return 하므로, 그 구간에
		//pause 를 걸어도 안 걸렸다(종료버튼 눌러도 안 멈추고 메시지박스만 뜨던 원인). 파일 간에도 여기서 검사해 즉시 반영한다.
		//20260714 by claude. worker 스레드에서는 Wait()(메시지 펌프 busy-spin) 대신 sleep_for 를 쓴다 — Wait()는 UI 스레드 전용
		//(PeekMessage(NULL)이 그 스레드 소유 창만 대상이고 sleep 없이 CPU 를 스핀). worker 는 순수 sleep 이 정석.
		while (m_pServerManager->m_DataSocket.get_transfer_pause() && !m_transfer_cancelled)
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		if (m_transfer_cancelled)
		{
			m_static_copy.stop_gif();
			break;
		}

		//20260710 by claude. 파일 1000개 초과 전송이 첫 반복에서 즉시 break 되던 버그 제거.
		//m_filelist.size() 는 루프 내내 상수라, 목록이 1000개를 넘으면 i=0 에서 바로 빠져나가 아무것도 전송 안 됐다
		//(대량 폴더 전송 시 '목록만 만들고 시작 안 함' 증상의 원인). 취소 방어는 위의 m_transfer_cancelled 검사가 담당한다.
		//현재 전송중인 파일명 표시
		m_static_message.set_textf(_T("%s"), m_filelist[i].cFileName);// get_part(m_filelist[i].cFileName, fn_name));

		//진행 카운트(x/total)를 항목 종류(폴더/파일/스킵)와 무관하게 루프 최상단에서 올린다.
		//폴더 생성 분기는 바이트 진행 콜백이 없어, 마지막 항목들이 폴더면 카운트가 멈춰 15/19 처럼 표시되던 문제를 방지.
		m_static_index_bytes.set_textf(_T("%d / %d (%s / %s)"), i + 1, m_ProgressData.total_count,
			get_size_str(m_ProgressData.ulReceivedSize.QuadPart, -1), get_size_str(m_ProgressData.ulTotalSize.QuadPart, -1));

		memcpy(&to, &m_filelist[i], sizeof(to));

		//dst 경로 설정. cFileName(fullpath)에서 m_transfer_from 값을 m_transfer_to 값으로 변경해준다.
		CString temp = m_filelist[i].cFileName;
		if (is_drive_root(m_transfer_from))
			temp.Replace(m_transfer_from, m_transfer_to + _T("\\"));
		else
			temp.Replace(m_transfer_from, m_transfer_to);
		temp.Replace(_T("\\\\"), _T("\\"));

		//20260710 by claude. 원본/대상 fullpath 가 MAX_PATH(260) 를 넘으면 to.cFileName[MAX_PATH] 버퍼 오버플로 +
		//수신측 CreateFile 실패로 소켓 교환이 무한 대기(hang)하던 문제 — 목록 특정 항목에서 전송이 멈추던 원인.
		//진입 전에 길이를 검사해 상태 컬럼에 "length exceed" 를 빨강으로 표시하고 이 파일은 건너뛴다(다음 파일로).
		if (temp.GetLength() >= MAX_PATH || _tcslen(m_filelist[i].cFileName) >= MAX_PATH)
		{
			m_list.set_text(i, col_status, _T("length exceed"));
			m_list.set_text_color(i, col_status, Gdiplus::Color::Red);
			m_auto_close = false;	//fail 과 동일하게, 사용자가 인지하도록 자동 닫기 방지.
			m_fail_items.push_back(i);
			logWriteE(_T("path length exceeds MAX_PATH, skip: %s"), m_filelist[i].cFileName);

			ULARGE_INTEGER exceed_size;
			exceed_size.LowPart = m_filelist[i].nFileSizeLow;
			exceed_size.HighPart = m_filelist[i].nFileSizeHigh;
			m_ProgressData.ulReceivedSize.QuadPart += exceed_size.QuadPart;	//전체 진행률이 100% 로 마무리되도록 크기 반영.
			m_static_index_bytes.set_textf(_T("%d / %d (%s / %s)"), i + 1, m_ProgressData.total_count,
				get_size_str(m_ProgressData.ulReceivedSize.QuadPart, -1), get_size_str(m_ProgressData.ulTotalSize.QuadPart, -1));
			m_progress.SetPos((int)((__int64)(i + 1) * 100 / max((DWORD)1, m_ProgressData.total_count)));	//완료 파일수 기준 진행률 스냅.
			continue;
		}

		_tcscpy(to.cFileName, temp);
		
		//하나의 파일/폴더가 전송되면 대상 리스트에도 바로 목록이 표시되는 것이 좋으나
		//하위 폴더까지 들어가서 표시할 필요는 없다. 현재의 m_transfer_to 경로에 생성되는 파일/폴더만 리스트에 표시한다.
		temp.Replace(m_transfer_to, _T(""));
		bool insert_item_after_transfer_success = (get_char_count(temp, '\\') == 1);


		//현재 전송중인 항목을 선택표시하면 왠지 산만하다. 그냥 스크롤만 처리하자.
		//m_list.select_item(i, true, true, true);
		//네이티브 EnsureVisible 는 CSCListCtrl 의 하단 여백(m_bottom_reserve) 모델을 무시해 마지막 행이 잘린 채 남는다.
		//커스텀 ensure_visible(visible_last)로 라우팅해야 정본 스크롤 모델(마지막 행 완전 표시)을 그대로 탄다.
		m_list.ensure_visible(i, CSCListCtrl::visible_last);

		ULARGE_INTEGER	filesize;
		filesize.LowPart = 0;
		filesize.HighPart = 0;

		//폴더 전송. 폴더는 실제 폴더를 전송하지 않고 동일한 이름으로 폴더를 생성해주면 된다.
		if (m_filelist[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (m_pServerManager->create_directory(to.cFileName, m_dstSide, true))
			{
				res = transfer_result_success;
				m_list.set_text(i, col_status, _T("100"));
				//change_directory를 할 경우 모든 파일들을 각각의 폴더까지 들어가서 전송되는 것을 보여주지만 이는 산만하다는 단점이 된다.
				//그냥 폴더든 파일이든 현재 대상 폴더의 경로와 동일할때만 리스트에 추가해서 보여줄 뿐 하위 폴더까지 들어가서 보여주지 않는다.
				//((CnFTDServerDlg*)(AfxGetApp()->GetMainWnd()))->change_directory(to, m_dstSide);
			}
		}
		//파일 전송
		else
		{
			filesize.LowPart = m_filelist[i].nFileSizeLow;
			filesize.HighPart = m_filelist[i].nFileSizeHigh;

			logWrite(_T("send %d/%d %s to %s"), i + 1, m_filelist.size(), m_filelist[i].cFileName, to.cFileName);

			//한 PC에서 실행할 경우 src와 dst가 동일한 폴더/파일이라면 당연히 전송할 수 없다.
			if (is_same_device && CString(m_filelist[i].cFileName).CompareNoCase(CString(to.cFileName)) == 0)
			{
				m_list.set_text(i, col_status, _S(IDS_SRC_DST_SAME_PATH));
				logWrite(_T("fullpath가 동일하므로 스킵."));
				skipped_count++;

				//ULARGE_INTEGER filesize;
				//filesize.HighPart = m_filelist[i].nFileSizeHigh;
				//filesize.LowPart = m_filelist[i].nFileSizeLow;
				m_ProgressData.ulReceivedSize.QuadPart += filesize.QuadPart;
				m_static_index_bytes.set_textf(_T("%d / %d (%s / %s)"), i + 1, m_ProgressData.total_count,
					get_size_str(m_ProgressData.ulReceivedSize.QuadPart, -1), get_size_str(m_ProgressData.ulTotalSize.QuadPart, -1));
				m_progress.SetPos((int)((__int64)(i + 1) * 100 / max((DWORD)1, m_ProgressData.total_count)));	//완료 파일수 기준 진행률 스냅.
				continue;
			}

			start_time = get_cur_datetime_str(2);

			//받기 전에 리스트는 해당 폴더로 자동 변경되도록? 너무 산만할 듯 하다.
			//송신
			if (m_dstSide == CLIENT_SIDE)
			{
				res = m_pServerManager->m_DataSocket.send_file(this, i, m_filelist[i], to, m_ProgressData);
			}
			else
			{
				res = m_pServerManager->m_DataSocket.recv_file(this, i, m_filelist[i], to, m_ProgressData);
			}

			end_time = get_cur_datetime_str(2);

			//취소됐으면 결과 표시 없이 루프를 빠져나간다.
			//20260715 by claude. 판정 기준을 '중단 요청'(m_transfer_cancelled)으로 분리. 창 파괴는 OnDestroy 가 이 스레드의
			//종료를 기다린 뒤에 일어나므로, 여기서 m_static_copy/m_list 를 만지는 것은 이제 안전하다.
			if (m_transfer_cancelled)
			{
				m_static_copy.stop_gif();
				break;
			}

			switch (res)
			{
				case transfer_result_success :
					m_list.set_text(i, col_status, _T("100"));
					logWrite(_T("success. %s"), m_filelist[i].cFileName);
					break;
				//파일전송을 취소하는 것은 파일전송창을 닫는 액션이므로 리스트에 굳이 cancel 표시를 남길 필요는 없다.
				//이 코드로 인해 창이 종료될 때 set_text()를 함으로써 에러가 발생한다.
				case transfer_result_cancel :
					//m_list.set_text(i, col_status, _T("cancel"));
					logWrite(_T("cancel."));
					m_static_copy.stop_gif();
					break;
				case transfer_result_skip :
					m_list.set_text(i, col_status, _T("skip"));
					logWrite(_T("skip."));
					skipped_count++;
					break;
				case transfer_result_overwrite :
					m_list.set_text(i, col_status, _T("overwrite"));
					logWrite(_T("overwrite."));
					break;
				default :
					//전송 완료 후 창 닫기 옵션이 true라고 해도 fail이 1건이라도 발생하면 창을 닫지 않아야만
					//사용자가 fail 내용을 인지할 수 있도록 해야 한다.
					m_auto_close = false;
					m_fail_items.push_back(i);
					logWriteE(_T("fail."));
					m_list.set_text(i, col_status, _T("fail"));
					m_list.set_text_color(i, col_status, Gdiplus::Color::Red);
			}
		}

		//20260713 by claude. 전체 진행률을 '완료 파일수' 기준으로 스냅((i+1)/total). 소켓 콜백이 없는 폴더 생성과
		//실패(fail) 항목도 진행률에 반영되어, 실패가 1건 있어도 마지막 항목에서 100%로 마무리된다(바이트 기준 99% 잔류 방지).
		//파일 전송 중의 부드러운 갱신은 소켓 코드가 (index+진행분)/total 로 담당하고, 여기서 매 항목 완료 시 정확히 스냅한다.
		m_progress.SetPos((int)((__int64)(i + 1) * 100 / max((DWORD)1, m_ProgressData.total_count)));
		set_taskbar_progress((int)((__int64)(i + 1) * 100 / max((DWORD)1, m_ProgressData.total_count)));	//작업표시줄 미러(전송 갯수 기준).

		if ((res == transfer_result_success || res == transfer_result_overwrite))
		{
			transferred_count++;

			//전송이 완료되면 리스트를 새로고침하거나
			//해당 항목을 수동으로 리스트에 추가하고 선택상태로 표시, 스크롤되게 보여줘야 한다.
			if (insert_item_after_transfer_success)
				((CnFTDServerDlg*)(AfxGetApp()->GetMainWnd()))->add_transfered_file_to_dst_list(m_dstSide, to);

			//또한 파일전송 히스토리에도 기록한다.
			m_pServerManager->request_file_transfer_history(get_part(m_filelist[i].cFileName, fn_name), i2S(filesize.QuadPart), m_dstSide == SERVER_SIDE, start_time, end_time);
		}
		else if (res == transfer_result_cancel)
		{
			//동일이름 처리창(CExistFileDlg)에서 옵션(이어서/덮어쓰기/건너뛰기)을 고르지 않고 X(닫기)를 누른 경우 =
			//"남은 파일을 더 전송할 의사 없음" = 전송작업 전체 취소. ("닫기=취소" 로 정한 이유: Windows 탐색기의 파일 충돌
			//대화상자도 X=복사 취소 / CExistFileDlg 가 모달이라 그때 전송창 취소버튼을 못 눌러 X 가 유일한 중단 수단 / 예전 동작 일치.)
			//
			//창을 닫을지 규칙:
			//  - 전송 완료 1개 이상 → "완료 후 닫기" 체크박스(m_auto_close 원래값)를 따른다(결과 확인 위해 남길 수 있게).
			//  - 전송 완료 0개      → 볼 게 없으니 체크박스와 무관하게 닫는다(여기서 m_auto_close=true 강제).
			//닫기는 워커 스레드에서 직접 못 하므로(모달은 UI 스레드 소유) m_auto_close 플래그로 OnTimer(UI 스레드)에 위임한다.
			if (transferred_count == 0)
				m_auto_close = true;

			m_static_copy.stop_gif();
			break;
		}
	}

	//20260713 by claude. 전송 완료 요약을 m_static_message 에 항상 표시한다: "성공 : %d, 스킵 : %d, 실패 : %d"(IDS_TRANSFER_RESULT).
	//실패가 1건 이상일 때만: (1) 뒤에 "여기를 클릭하여 확인"(IDS_TRANSFER_RESULT_CONFIRM)을 덧붙이고 Red 로 표시,
	//(2) m_auto_close 강제 해제(옵션이 true 라도 창을 닫지 않음 — 각 실패 지점에서 이미 false 로 두지만 방어적으로 재확정),
	//(3) 첫 실패 항목으로 자동 스크롤+선택해 사용자가 바로 확인하도록 한다. transferred_count = 100% 전송 성공 건수.
	CString summary;
	summary.Format(_S(IDS_TRANSFER_RESULT), transferred_count, skipped_count, (int)m_fail_items.size());
	if (!m_fail_items.empty())
	{
		summary += _S(IDS_TRANSFER_RESULT_CONFIRM);
		m_static_message.set_tagged_text(summary);

		m_auto_close = false;

		//첫 실패 항목(0번)부터 보여준다. 이후 요약 문구 클릭 시 다음 실패 항목으로 순환한다(OnStnClickedStaticMessage).
		m_fail_view_index = 0;
		//select_item(index, select, after_unselect, insure_visible) — 선택+타 항목 해제+가시화까지 한 번에.
		m_list.select_item(m_fail_items.front(), true, true, true);
	}
	else
	{
		//실패 없음: 요약만 기본색으로 표시(클릭 확인 문구 없음). 창 닫기 여부는 사용자 체크박스(m_auto_close)를 따른다.
		m_static_message.set_tagged_text(summary);
	}

	//전송이 모두 완료되면 dataSocket은 닫아준다.
	//20250318 scpark 간혹 전송이 종료되면서 m_pServerManager가 원래와 다른 값을 가짐으로 인해 프로그램이 종료되는 현상이 발생했고
	//그 원인을 찾아야하지만 우선 맨 처음 생성했던 maindlg의 serverManager와 동일한 값이 아닐 경우는 garbage라 생각하고 스킵시킨다.
	//logWrite(_T("m_pServerManager = %p"), m_pServerManager);
	//CnFTDServerManager* manager_original = ((CnFTDServerDlg*)(AfxGetApp()->GetMainWnd()))->m_pServerManager_original;
	//if (m_pServerManager == manager_original && m_pServerManager != NULL && m_pServerManager != INVALID_HANDLE_VALUE && m_thread_transfer_started)
	//	m_pServerManager->DataClose();

	//전송 완료: stop_gif()는 애니 스레드를 끝내고 화면에서도 지워버린다(설계상). 완료 후엔 프레임이 남아야 하므로
	//pause_gif(0)으로 첫 프레임에 정지시켜 그대로 보이게 한다(pos>=0 은 비-토글이라 아래 OnTimer 재호출에도 안전).
	m_static_copy.pause_gif(0);
	m_thread_transfer_started = false;

	//20260713 by claude. 작업표시줄 진행율 마무리. 실패가 1건 이상이면 TBPF_ERROR(빨강) + 100% 로 표시해, 창이 뒤에 가려져도
	//작업표시줄 버튼만으로 실패를 인지하게 한다. 실패가 없으면 NOPROGRESS 로 오버레이 제거. 이후 COM 정리.
	//ERROR 상태는 shell 이 해당 HWND(메인창) 버튼에 유지하므로 Release/CoUninitialize 후에도 남으며, 사용자가 창을 활성화하면 자연 해제된다.
	if (m_pTaskbar)
	{
		if (m_taskbar_hwnd)
		{
			if (!m_fail_items.empty())
			{
				m_pTaskbar->SetProgressValue(m_taskbar_hwnd, 100, 100);
				m_pTaskbar->SetProgressState(m_taskbar_hwnd, TBPF_ERROR);
			}
			else
			{
				m_pTaskbar->SetProgressState(m_taskbar_hwnd, TBPF_NOPROGRESS);
			}
		}
		m_pTaskbar->Release();
		m_pTaskbar = NULL;
	}
	::CoUninitialize();

	//GetDlgItem(IDCANCEL)->EnableWindow(true);
	TRACE(_T("exit thread_transfer()\n"));

	((CnFTDServerDlg*)(AfxGetApp()->GetMainWnd()))->refresh_selection_status(true);
}

//OnLButtonDown / OnSetCursor / OnGetMinMaxInfo / OnPaint / OnEraseBkgnd 제거:
//타이틀바 드래그 이동·가장자리 resize·resize 커서·NC/배경 paint·maximize 경계는 모두 base CSCThemeDlg 가
//정석 패턴(borderless dialog.md: WM_NCHITTEST 트랩 + OnNcPaint/OnPaint + OnGetMinMaxInfo)으로 처리한다.
//예전엔 get_corner_index() 로 client 가장자리를 직접 판정해 SC_SIZE 를 보내고 resize 커서를 SetCursor 하던
//방식이었으나 base 위임으로 일원화한다. OnWindowPosChanged 만 위치 저장을 위해 남기되 base 를 경유한다.

void CnFTDFileTransferDialog::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	//base(CSCThemeDlg) 경유 — adjust_sys_buttons() 등 타이틀바 시스템 버튼 정렬이 base 에서 수행된다.
	CSCThemeDlg::OnWindowPosChanged(lpwndpos);

	SaveWindowPosition(&theApp, this, _T("CnFTDFileTransferDialog"));
}
/*
LRESULT CnFTDFileTransferDialog::on_message_server_socket(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	int percent = (int)lParam;
	ULARGE_INTEGER file_size;

	file_size.LowPart = m_filelist[index].nFileSizeLow;
	file_size.HighPart = m_filelist[index].nFileSizeHigh;

	m_list.set_text(index, col_status, i2S(percent));
	TRACE(_T("[%d]. %d%% sent\n"), index, percent);
	return 0;
}
*/

void CnFTDFileTransferDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_check_thread_transfer)
	{
		KillTimer(timer_check_thread_transfer);

		if (!m_thread_transfer_started)
		{
			//완료 상태: stop 이 아니라 pause 로 마지막 프레임을 남긴다(창이 안 닫히는 경우 gif 가 사라지지 않도록).
			m_static_copy.pause_gif(0);

			if (m_auto_close)
			{
				CDialogEx::OnOK();
			}
		}
		else
		{
			SetTimer(timer_check_thread_transfer, 1000, NULL);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CnFTDFileTransferDialog::PreTranslateMessage(MSG* pMsg)
{
#ifdef _DEBUG
	//디버그 빌드 한정: 전송 중 스페이스바로 일시정지/재개 토글(전송 흐름 관찰·문제 재현용). 리스트로 전파되지 않게 여기서 소비한다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE && m_thread_transfer_started && m_pServerManager)
	{
		m_transfer_paused_debug = !m_transfer_paused_debug;
		m_pServerManager->m_DataSocket.set_transfer_pause(m_transfer_paused_debug);
		if (m_transfer_paused_debug)
			m_static_copy.pause_gif(-1);
		else
			m_static_copy.play_gif();
		return TRUE;
	}
#endif

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CnFTDFileTransferDialog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//전송 결과 리스트 위에서 우클릭했을 때만 메뉴를 띄운다.
	if (pWnd == NULL || pWnd->GetSafeHwnd() != m_list.GetSafeHwnd())
		return;

	//키보드(Shift+F10)로 호출되면 point=(-1,-1) → 커서 위치로 대체(스크롤 무관, native get_item_rect 회피).
	if (point.x == -1 && point.y == -1)
		point = (CPoint)GetMessagePos();

	//우클릭 위치의 항목이 없으면(빈 영역) 메뉴를 띄우지 않는다 — 이 메뉴는 항목 대상 명령만 갖는다.
	//CSCListCtrl::HitTest 는 smooth-aware 로 오버라이드돼 있어 native 이름 그대로 써도 스크롤(m_scroll_y) 반영된 항목이 나온다.
	CPoint pt = point;
	m_list.ScreenToClient(&pt);
	int item = m_list.HitTest(pt);
	if (item < 0)
		return;

	//우클릭한 항목을 선택 상태로 만들어 이후 명령이 그 항목을 대상으로 하게 한다(타 항목 해제, 스크롤은 안 함).
	m_list.select_item(item, true, true, false);

	CMenu menu;
	menu.LoadMenu(IDR_MENU_TRANSFER_CONTEXT);
	CMenu* pMenu = menu.GetSubMenu(0);
	if (pMenu == NULL)
		return;

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CnFTDFileTransferDialog::OnMenuTransferOpenFolder()
{
	CString path = get_selected_item_local_path();
	if (path.IsEmpty() || ::GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)	//전송 실패 등으로 로컬에 없으면 무시.
		return;

	//탐색기에서 상위 폴더를 열고 해당 항목을 선택 상태로 표시(파일/폴더 공통). "explorer /select," 관용구.
	CString param;
	param.Format(_T("/select,\"%s\""), path);
	ShellExecute(NULL, _T("open"), _T("explorer"), param, NULL, SW_SHOWNORMAL);
}

void CnFTDFileTransferDialog::OnMenuTransferCopyPathToClipboard()
{
	CString path = get_selected_item_local_path();
	if (path.IsEmpty())
		return;

	copy_to_clipboard(m_hWnd, path);
}

CString CnFTDFileTransferDialog::get_selected_item_local_path()
{
	std::deque<int> dq;
	m_list.get_selected_items(&dq);
	if (dq.size() == 0)
		return _T("");

	int idx = dq[0];	//리스트 항목 index 는 m_filelist index 와 1:1(빌드 시 삽입 순서, 정렬 없음).
	if (idx < 0 || idx >= (int)m_filelist.size())
		return _T("");

	//송신(소스=로컬)이면 소스 실경로. 수신(대상=로컬)이면 대상 실경로 — thread_transfer 의 dst 산출과 동일 규칙.
	if (m_srcSide == SERVER_SIDE)
		return m_filelist[idx].cFileName;

	CString path = m_filelist[idx].cFileName;
	if (is_drive_root(m_transfer_from))
		path.Replace(m_transfer_from, m_transfer_to + _T("\\"));
	else
		path.Replace(m_transfer_from, m_transfer_to);
	path.Replace(_T("\\\\"), _T("\\"));
	return path;
}
