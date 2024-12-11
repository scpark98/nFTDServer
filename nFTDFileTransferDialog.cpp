// nFTDFileTransferDialog.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "nFTDFileTransferDialog.h"
#include "afxdialogex.h"

#include <thread>
#include <algorithm>

#include "../../Common/MemoryDC.h"
#include "MessageDlg.h"

extern HMODULE g_hRes;

// CnFTDFileTransferDialog 대화 상자

IMPLEMENT_DYNAMIC(CnFTDFileTransferDialog, CDialogEx)

CnFTDFileTransferDialog::CnFTDFileTransferDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILETRANSFER, pParent)
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

void CnFTDFileTransferDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_static_message);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDCANCEL, m_button_cancel);
}


BEGIN_MESSAGE_MAP(CnFTDFileTransferDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CnFTDFileTransferDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CnFTDFileTransferDialog::OnBnClickedCancel)
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SETCURSOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//ON_REGISTERED_MESSAGE(Message_CnFTDServerSocket, &CnFTDFileTransferDialog::on_message_server_socket)
END_MESSAGE_MAP()


// CnFTDFileTransferDialog 메시지 처리기

BOOL CnFTDFileTransferDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	init_list();
	init_shadow();

	m_resize.Create(this);
	m_resize.SetMinimumTrackingSize(CSize(480, 360));
	m_resize.Add(IDC_STATIC_MESSAGE, 0, 0, 100, 0);
	m_resize.Add(IDC_PROGRESS, 0, 0, 100, 0);
	m_resize.Add(IDCANCEL, 100, 0, 0, 0);
	m_resize.Add(IDC_LIST, 0, 0, 100, 100);

	Gdiplus::Color cr_back = Gdiplus::Color::White;
	m_static_message.set_back_color(cr_back);

	m_sys_buttons.create(this, 1, 0, 26, 26, SC_CLOSE);
	m_sys_buttons.set_back_color(Gdiplus::Color::White);

	m_progress.set_style(CSCSliderCtrl::style_progress);
	m_progress.set_text_style(CSCSliderCtrl::text_style_dual_text);
	m_progress.set_text_color(white);
	m_progress.set_font_size(8);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPCHILDREN);// | WS_CLIPSIBLINGS);

	RestoreWindowPosition(&theApp, this, _T("CnFTDFileTransferDialog"));

	std::thread th(&CnFTDFileTransferDialog::thread_transfer, this);
	th.detach();

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
		m_pServerManager->m_DataSocket.set_transfer_pause(true);
		int res = AfxMessageBox(_T("파일전송을 종료합니다."), MB_OKCANCEL);
		if (res == IDCANCEL)
		{
			m_pServerManager->m_DataSocket.set_transfer_pause(false);
			return;
		}

		TRACE(_T("파일전송 취소 처리\n"));
		m_pServerManager->m_DataSocket.set_transfer_stop();
		m_pServerManager->m_DataSocket.Close();
		m_thread_transfer_started = false;

		//취소 처리 플래그에 따라 정상적인 취소 절차가 모두 처리된 후에 창이 종료되어야 하므로
		//여기서 CDialogEx::OnCancel();까지 가서는 안된다. 그냥 리턴한다.
		//return;
		Wait(1000);
	}

	m_list.save_column_width(&theApp, _T("CnFTDFileTransferDialog list"));

	CDialogEx::OnCancel();
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
	m_list.set_headings(_T("파일명,100;크기,100;상태,60"));
	//m_list.set_color_theme(CVtListCtrlEx::color_theme_dark_gray);
	//m_list.set_line_height(theApp.GetProfileInt(_T("list name"), _T("line height"), 80));

	m_list.set_font_size(theApp.GetProfileInt(_T("list"), _T("font size"), 9));
	m_list.set_font_name(theApp.GetProfileString(_T("list"), _T("font name"), _T("맑은 고딕")));

	m_list.load_column_width(&theApp, _T("CnFTDFileTransferDialog list"));
	m_list.set_header_height(22);

	//
	m_list.set_use_own_imagelist(true);
	m_list.set_shell_imagelist(&theApp.m_shell_imagelist);
	m_list.set_line_height(20);

	m_list.set_column_text_align(col_filesize, HDF_RIGHT);
	m_list.set_column_text_align(col_status, HDF_CENTER);

	//m_list.set_header_text_align(col_filesize, HDF_CENTER);
	//m_list.set_header_text_align(2, HDF_CENTER);

	//m_list.set_column_data_type(list_score, CVtListCtrlEx::column_data_type_percentage_grid);
	m_list.set_column_data_type(col_status, CVtListCtrlEx::column_data_type_progress);
	m_list.show_progress_text();
	m_list.set_progress_color(Gdiplus::Color(79, 187, 255));
	m_list.set_progress_text_color(Gdiplus::Color::Black);

	m_list.load_column_width(&theApp, _T("CnFTDFileTransferDialog list"));

	m_list.allow_edit(false);
	m_list.allow_sort(false);
	m_list.use_indent_from_prefix_space(true);
}

void CnFTDFileTransferDialog::init_shadow()
{
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_shadow.Create(GetSafeHwnd());
	m_shadow.SetSize(4);	// -19 ~ 19
	m_shadow.SetSharpness(19);	// 0 ~ 19
	m_shadow.SetDarkness(128);	// 0 ~ 254
	m_shadow.SetPosition(0, 0);	// -19 ~ 19
	m_shadow.SetColor(RGB(0, 0, 0));
}

void CnFTDFileTransferDialog::thread_transfer()
{
	int i;

	m_static_message.set_text(_T("파일, 폴더 목록을 생성중입니다..."));

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

			m_filelist.insert(m_filelist.begin() + i + 1, dq.begin(), dq.end());
			i = i + dq.size() + 1;
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

	//list에 그 내용을 채워준다.
	for (i = 0; i < m_filelist.size(); i++)
	{
		m_progress.SetPos(i + 1);

		//TRACE(_T("src[%3d] = %s\n"), i, m_filelist[i].cFileName);
		is_folder = (m_filelist[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

		if (is_folder)
		{
			size_str.Empty();
			image_index = theApp.m_shell_imagelist.GetSystemImageListIcon(_T("c:\\windows"), true);
			folder_count++;
		}
		else
		{
			file_size.LowPart = m_filelist[i].nFileSizeLow;
			file_size.HighPart = m_filelist[i].nFileSizeHigh;
			m_ProgressData.ulTotalSize.QuadPart += file_size.QuadPart;

			size_str = get_size_string(file_size.QuadPart);

			image_index = theApp.m_shell_imagelist.GetSystemImageListIcon(m_filelist[i].cFileName, false);
			file_count++;
		}

		filename = get_part(m_filelist[i].cFileName, fn_name);

		//depth 표시를 위해 앞에 depth만큼 공백을 추가한다. m_transfer_from을 뺀 후 '\\'의 개수를 세어야 한다.
		folder = m_filelist[i].cFileName;
		folder.Replace(m_transfer_from, _T(""));
		depth = get_char_count(folder, '\\') - 1;

		//"파일명,100;크기,100;상태,60;"
		m_list.insert_item(i, image_index, make_string(_T(" "), depth) + filename, size_str, _T(""));
	}

	TRACE(_T("file = %d, folder = %d\n"), file_count, folder_count);

	//디스크 남은 용량 체크
	if (false)//m_ProgressData.ulTotalSize.QuadPart > m_ProgressData.ulRemainDiskSpace.QuadPart)
	{
		CString message;
		message.LoadString(NFTD_IDS_MSGBOX_DISKFULL);

		CMessageDlg dlgMessage;
		//dlgMessage.SetMessage(MsgType::TypeOK, message);
		dlgMessage.DoModal();

		EndDialog(0);
		return;
	}


	//전체 데이터 송수신 양은 항상 100%로 환산한다. 그래야 용량에 관계없이 표현가능하다.
	m_progress.SetPos(0);
	m_progress.SetRange(0, 100);

	int res;
	CString from;
	CString to;
	CString msg;

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

	m_thread_transfer_started = true;
	m_button_cancel.EnableWindow(true);

	//전송 상태를 메시지로 받아 표시해봤으나 메시지 처리 방식은 매우 딜레이가 심함.
	//컨트롤들을 건네받아 컨트롤에서 직접 표시함.
	m_pServerManager->m_DataSocket.set_ui_controls(&m_progress, &m_list);

	//실제 파일 송수신 시작
	for (i = 0; i < m_filelist.size(); i++)
	{
		//전송중에 취소한 경우 이 플래그를 보고 중지시킨다.
		if (m_thread_transfer_started == false)
			break;

		//현재 전송중인 파일명 표시
		m_static_message.set_textf(-1, _T("%s"), get_part(m_filelist[i].cFileName, fn_name));

		m_list.select_item(i, true, true, true);
		//m_progress.set_text(_T("%d / %d (%s / %s)"), i + 1, m_filelist.size(),
		//	get_size_string(m_ProgressData.ulReceivedSize.QuadPart), get_size_string(m_ProgressData.ulTotalSize.QuadPart));

		//dst 경로 설정. cFileName(fullpath)에서 src 폴더명을 dst 폴더명으로 변경해준다.
		to.Format(_T("%s\\%s"), m_transfer_to, get_part(m_filelist[i].cFileName, fn_name));

		ULARGE_INTEGER	filesize;
		filesize.LowPart = 0;
		filesize.HighPart = 0;

		//폴더 전송
		if (m_filelist[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (m_pServerManager->create_directory(to, m_dstSide, true))
			{
				m_list.set_text(i, col_status, _T("100"));
			}
		}
		//파일 전송
		else
		{
			filesize.LowPart = m_filelist[i].nFileSizeLow;
			filesize.HighPart = m_filelist[i].nFileSizeHigh;

			//송신
			if (m_dstSide == CLIENT_SIDE)
			{
				res = m_pServerManager->m_DataSocket.send_file(i, m_filelist[i], to, m_ProgressData);

				//success
				if (res == transfer_result_success)
				{
					m_list.set_text(i, col_status, _T("100"));
					logWrite(_T("%3d / %3d sent ok. %s"), i + 1, m_filelist.size(), m_filelist[i].cFileName);
				}
				//cancel
				else if (res == transfer_result_cancel)
				{
					logWrite(_T("사용자에 의한 전송 취소."));
					break;//AfxMessageBox(_T("전송이 취소되었습니다."));
				}
				//error
				else
				{

				}
			}
			//수신
			else
			{
				res = m_pServerManager->m_DataSocket.recv_file(i, m_filelist[i], to, m_ProgressData);
				logWrite(_T("recv_file res = %d"), res);

				//success
				if (res == transfer_result_success)
				{
					logWrite(_T("%3d / %3d recv ok. %s"), i + 1, m_filelist.size(), m_filelist[i].cFileName);
				}
				//cancel
				else if (res == transfer_result_cancel)
				{
					logWrite(_T("사용자에 의한 전송 취소."));
					break;//AfxMessageBox(_T("수신이 취소되었습니다."));
				}
				//error
				else
				{

				}
			}

		}

		//m_progress.set_text(_T("%d / %d (%s / %s)"), i + 1, m_filelist.size(),
		//	get_size_string(m_ProgressData.ulReceivedSize.QuadPart), get_size_string(m_ProgressData.ulTotalSize.QuadPart));
	}
	/*
	if (!m_pServerManager->DataConnect())
	{
		AfxMessageBox(_T("m_pServerManager->DataConnect() fail."));
		return;
	}

	// 전체 파일 리스트 얻어오기
	if (!m_pServerManager->FileTransferInitalize(m_pFileList, &m_list, &m_ListQueue, m_ProgressData.ulTotalSize, m_dwSide, m_strStartPath))
	{
		return;
	}
	*/

	//전송이 모두 완료되면 dataSocket은 닫아준다.
	m_pServerManager->DataClose();

	m_thread_transfer_started = false;
	m_button_cancel.EnableWindow(true);
	TRACE(_T("exit thread_transfer()\n"));
}


void CnFTDFileTransferDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (!IsZoomed() && (m_corner_index >= corner_left) && (m_corner_index <= corner_bottomright))
	{
		DefWindowProc(WM_SYSCOMMAND, SC_SIZE + m_corner_index, MAKELPARAM(point.x, point.y));
	}
	else if (!IsZoomed() && (m_corner_index == corner_inside))
	{
		DefWindowProc(WM_NCLBUTTONDOWN, HTCAPTION, MAKEWORD(point.x, point.y));
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CnFTDFileTransferDialog::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this, _T("CnFTDFileTransferDialog"));
}


BOOL CnFTDFileTransferDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (IsZoomed())
		return CDialogEx::OnSetCursor(pWnd, nHitTest, message);

	CPoint pt;
	CRect rc;

	GetClientRect(rc);

	GetCursorPos(&pt);
	ScreenToClient(&pt);

	m_corner_index = get_corner_index(rc, pt, 10);
	//TRACE(_T("m_corner_index = %d\n"), m_corner_index);

	if (m_corner_index == corner_left || m_corner_index == corner_right)
	{
		::SetCursor(theApp.LoadStandardCursor(IDC_SIZEWE));
		return true;
	}
	else if (m_corner_index == corner_top || m_corner_index == corner_bottom)
	{
		::SetCursor(theApp.LoadStandardCursor(IDC_SIZENS));
		return true;
	}
	else if (m_corner_index == corner_topleft || m_corner_index == corner_bottomright)
	{
		::SetCursor(theApp.LoadStandardCursor(IDC_SIZENWSE));
		return true;
	}
	else if (m_corner_index == corner_topright || m_corner_index == corner_bottomleft)
	{
		::SetCursor(theApp.LoadStandardCursor(IDC_SIZENESW));
		return true;
	}
	else if (m_corner_index != corner_inside)
	{
		m_corner_index = -1;
	}

	::SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
	return true;
}


void CnFTDFileTransferDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CSize sz;
	bool is_auto_hide = get_taskbar_state(ABS_AUTOHIDE, &sz);

	lpMMI->ptMaxSize.y -= (is_auto_hide ? 2 : sz.cy);

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CnFTDFileTransferDialog::OnPaint()
{
	CPaintDC dc1(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	CRect rc;

	GetClientRect(rc);

	CMemoryDC dc(&dc1, &rc);

	//타이틀바 영역
	CRect rTitle = rc;
	rTitle.bottom = rTitle.top + TITLEBAR_HEIGHT;
	//draw_rectangle(&dc, rTitle, Gdiplus::Color::Transparent, m_sys_buttons.m_theme.cr_back);

	//프로그램 아이콘을 그려주고
	CRect rIcon = rTitle;
	HICON hIcon = load_icon(AfxGetInstanceHandle(), IDR_MAINFRAME, 16);
	rIcon.left = 8;
	rIcon.right = rIcon.left + 16;
	draw_icon(&dc, hIcon, rIcon);
	//dc.DrawIcon(2, (TITLEBAR_HEIGHT - 16) / 2, hIcon);
	::DestroyIcon(hIcon);

	//타이틀바 표시
	rTitle.left = rIcon.right + 8;
	CFont* font = GetFont();
	CFont* pOldFont = (CFont*)dc.SelectObject(font);
	dc.SetBkMode(TRANSPARENT);

	if (_tcscmp(__targv[1], _T("-p")) == 0)
		dc.SetTextColor(RGB(96, 96, 128));
	else
		dc.SetTextColor(RGB(52, 99, 193));

	dc.DrawText(_T("파일 전송"), rTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//테두리
	draw_rectangle(&dc, rc, Gdiplus::Color::RoyalBlue);

	dc.SelectObject(pOldFont);
}


void CnFTDFileTransferDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rc;
	GetClientRect(rc);

	if (m_sys_buttons.m_hWnd == NULL)
		return;

	m_sys_buttons.adjust(rc.top + 1, rc.right - 1);

	Invalidate();
}


BOOL CnFTDFileTransferDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
	return CDialogEx::OnEraseBkgnd(pDC);
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