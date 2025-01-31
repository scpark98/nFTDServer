// nFTDFileTransferDialog.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "nFTDServerDlg.h"
#include "nFTDFileTransferDialog.h"
#include "afxdialogex.h"

#include <thread>
#include <algorithm>

#include "../../Common/MemoryDC.h"
#include "MessageDlg.h"

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
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SETCURSOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//ON_REGISTERED_MESSAGE(Message_CnFTDServerSocket, &CnFTDFileTransferDialog::on_message_server_socket)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CnFTDFileTransferDialog 메시지 처리기

BOOL CnFTDFileTransferDialog::OnInitDialog()
{
	CSCThemeDlg::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	init_list();
	//init_shadow();

	m_resize.Create(this);
	m_resize.SetMinimumTrackingSize(CSize(400, 320));
	m_resize.Add(IDC_STATIC_COPY, 50, 0, 0, 0);
	m_resize.Add(IDC_STATIC_MESSAGE, 0, 0, 100, 0);
	m_resize.Add(IDC_STATIC_INDEX_BYTES, 0, 0, 50, 0);
	m_resize.Add(IDC_STATIC_REMAIN_SPEED, 50, 0, 50, 0);
	m_resize.Add(IDC_PROGRESS, 0, 0, 100, 0);
	m_resize.Add(IDC_LIST, 0, 0, 100, 100);

	set_color_theme(CSCThemeDlg::color_theme_linkmemine);
	set_titlebar_height(TOOLBAR_TITLE_HEIGHT);
	show_titlebar_logo(false);
	m_sys_buttons.set_button_width(TOOLBAR_TITLE_BUTTON_WIDTH);


	m_static_copy.set_back_image(_T("GIF"), IDR_GIF_COPY, Gdiplus::Color::White);
	m_static_copy.fit_to_back_image(false);

	m_static_message.set_back_color(m_cr_back);

	m_static_index_bytes.set_back_color(Gdiplus::Color::White);
	m_static_remain_speed.set_back_color(Gdiplus::Color::White);

	m_progress.set_style(CSCSliderCtrl::style_progress_line);
	m_progress.set_active_color(RGB(0, 134, 218));
	m_progress.set_back_color(white);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_CLIPCHILDREN);// | WS_CLIPSIBLINGS);

	RestoreWindowPosition(&theApp, this, _T("CnFTDFileTransferDialog"));
	Wait(10);
	CenterWindow(GetParent());

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
		m_static_copy.pause_animation(-1);
		m_pServerManager->m_DataSocket.set_transfer_pause(true);
		CMessageDlg dlg(_S(NFTD_IDS_MSGBOX_CANCELTRANSFER), MB_OKCANCEL);

		int res = dlg.DoModal();//AfxMessageBox(_S(NFTD_IDS_MSGBOX_CANCELTRANSFER), MB_OKCANCEL);
		if (res == IDCANCEL)
		{
			m_static_copy.play_animation();
			m_pServerManager->m_DataSocket.set_transfer_pause(false);
			return;
		}

		TRACE(_T("파일전송 취소 처리\n"));
		m_static_copy.stop_animation();
		m_pServerManager->m_DataSocket.set_transfer_stop();
		m_pServerManager->m_DataSocket.Close();
		m_thread_transfer_started = false;

		//취소 처리 플래그에 따라 정상적인 취소 절차가 모두 처리된 후에 창이 종료되어야 하므로
		//여기서 CDialogEx::OnCancel();까지 가서는 안된다. 그냥 리턴한다.
		//return;
		Wait(500);
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
	CString headings;

	headings.Format(_T("%s,200;%s,100;%s,60"), _S(NFTD_IDS_LISTCTRL_NAME), _S(NFTD_IDS_LISTCTRL_SIZE), _S(NFTD_IDS_LISTCTRL_STATUS));
	m_list.set_headings(headings);
	//m_list.set_color_theme(CVtListCtrlEx::color_theme_dark_gray);
	//m_list.set_line_height(theApp.GetProfileInt(_T("list name"), _T("line height"), 80));

	m_list.set_font_size(theApp.GetProfileInt(_T("list"), _T("font size"), 9));
	m_list.set_font_name(theApp.GetProfileString(_T("list"), _T("font name"), _S(IDS_FONT)));

	//m_list.load_column_width(&theApp, _T("CnFTDFileTransferDialog list"));
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
	m_list.set_back_alternate_color(true, Gdiplus::Color(242, 242, 242));
	//m_list.set_progress_text_color(Gdiplus::Color::Black);

	m_list.load_column_width(&theApp, _T("CnFTDFileTransferDialog list"));

	m_list.allow_edit(false);
	m_list.allow_sort(false);
	m_list.use_indent_from_prefix_space(true);
}
/*
void CnFTDFileTransferDialog::init_shadow()
{
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_shadow.Create(GetSafeHwnd());
	m_shadow.SetSize(8);	// -19 ~ 19
	m_shadow.SetSharpness(19);	// 0 ~ 19
	m_shadow.SetDarkness(128);	// 0 ~ 254
	m_shadow.SetPosition(0, 0);	// -19 ~ 19
	m_shadow.SetColor(RGB(0, 0, 0));
}
*/
void CnFTDFileTransferDialog::thread_transfer()
{
	int i;

	m_thread_transfer_started = true;

	m_static_message.set_text(_S(IDS_MAKE_FILE_FOLDER_LIST));

	//폴더인 항목은 그 항목을 유지한 채 하위 모든 폴더, 파일 목록을 찾아서 추가시킨다.
	for (i = 0; i < m_filelist.size(); i++)
	{
		if (m_filelist[i].dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			logWriteD(_T("Analyze %s folder for the prepare transfering..."), m_filelist[i].cFileName);

			std::deque<WIN32_FIND_DATA> dq;

			//로컬인 경우는 직접 찾지만 remote인 경우는 요청해서 채워야 한다.
			if (m_srcSide == SERVER_SIDE)
			{
				logWriteD(_T("find_all_files for local"));
				find_all_files(m_filelist[i].cFileName, &dq, _T(""), true, true);
			}
			else
			{
				logWriteD(_T("get_filelist for remote"));
				m_pServerManager->get_filelist(m_filelist[i].cFileName, &dq, true);
			}

			logWriteD(_T("insert dq. total = %d"), dq.size());
			if (dq.size() > 0)
			{
				m_filelist.insert(m_filelist.begin() + i + 1, dq.begin(), dq.end());
				i = i + dq.size() + 1;
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

	//list에 그 내용을 채워준다.
	for (i = 0; i < m_filelist.size(); i++)
	{
		//TRACE(_T("src[%3d] = %s\n"), i, m_filelist[i].cFileName);
		is_folder = (m_filelist[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

		if (is_folder)
		{
			size_str.Empty();
			image_index = theApp.m_shell_imagelist.GetSystemImageListIcon(0, _T("c:\\windows"), true);
			folder_count++;
		}
		else
		{
			file_size.LowPart = m_filelist[i].nFileSizeLow;
			file_size.HighPart = m_filelist[i].nFileSizeHigh;
			m_ProgressData.ulTotalSize.QuadPart += file_size.QuadPart;

			size_str = get_size_str(file_size.QuadPart);

			image_index = theApp.m_shell_imagelist.GetSystemImageListIcon(0, m_filelist[i].cFileName, false);
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
	//CString to;
	CString msg;
	WIN32_FIND_DATA to;

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
	//GetDlgItem(IDCANCEL)->EnableWindow(true);

	//전송 상태를 메시지로 받아 표시해봤으나 메시지 처리 방식은 매우 딜레이가 심함.
	//컨트롤들을 건네받아 컨트롤에서 직접 표시함.
	//m_pServerManager->m_DataSocket.set_ui_controls(this, &m_progress, &m_list);
	m_pServerManager->m_DataSocket.SetFileWriteMode(WRITE_UNKNOWN);


	//실제 파일 송수신 시작
	for (i = 0; i < m_filelist.size(); i++)
	{
		res = transfer_result_fail;

		m_static_copy.set_back_image_mirror(m_dstSide == SERVER_SIDE);

		//전송중에 취소한 경우 이 플래그를 보고 중지시킨다.
		if (m_thread_transfer_started == false)
			break;

		//현재 전송중인 파일명 표시
		m_static_message.set_textf(-1, _T("%s"), get_part(m_filelist[i].cFileName, fn_name));

		memcpy(&to, &m_filelist[i], sizeof(to));

		//dst 경로 설정. cFileName(fullpath)에서 m_transfer_from 값을 m_transfer_to 값으로 변경해준다.
		CString temp = m_filelist[i].cFileName;
		if (is_drive_root(m_transfer_from))
			temp.Replace(m_transfer_from, m_transfer_to + _T("\\"));
		else
			temp.Replace(m_transfer_from, m_transfer_to);
		temp.Replace(_T("\\\\"), _T("\\"));
		_tcscpy(to.cFileName, temp);
		
		//하나의 파일/폴더가 전송되면 대상 리스트에도 바로 목록이 표시되는 것이 좋으나
		//하위 폴더까지 들어가서 표시할 필요는 없다. 현재의 m_transfer_to 경로에 생성되는 파일/폴더만 리스트에 표시한다.
		temp.Replace(m_transfer_to, _T(""));
		bool insert_item_after_transfer_success = (get_char_count(temp, '\\') == 1);


		//현재 전송중인 항목을 선택표시하면 왠지 산만하다. 그냥 EnsureVisible()만 처리하자.
		//m_list.select_item(i, true, true, true);
		m_list.EnsureVisible(i, FALSE);

		ULARGE_INTEGER	filesize;
		filesize.LowPart = 0;
		filesize.HighPart = 0;

		bool is_same_device = false;
		if (get_my_ip().Compare(__targv[4]) == 0)
			is_same_device = true;

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

				ULARGE_INTEGER filesize;
				filesize.HighPart = m_filelist[i].nFileSizeHigh;
				filesize.LowPart = m_filelist[i].nFileSizeLow;
				m_ProgressData.ulReceivedSize.QuadPart += filesize.QuadPart;
				m_static_index_bytes.set_textf(-1, _T("%d / %d (%s / %s)"), i + 1, m_ProgressData.total_count,
					get_size_str(m_ProgressData.ulReceivedSize.QuadPart), get_size_str(m_ProgressData.ulTotalSize.QuadPart));
				continue;
			}

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

			switch (res)
			{
				case transfer_result_success :
					m_list.set_text(i, col_status, _T("100"));
					logWrite(_T("success. %s"), m_filelist[i].cFileName);
					break;
				case transfer_result_cancel :
					m_list.set_text(i, col_status, _T("cancel"));
					logWrite(_T("cancel."));
					break;
				case transfer_result_skip :
					m_list.set_text(i, col_status, _T("skip"));
					logWrite(_T("skip."));
					break;
				case transfer_result_overwrite :
					m_list.set_text(i, col_status, _T("overwrite"));
					logWrite(_T("overwrite."));
					break;
				default :
					//전송 완료 후 창 닫기 옵션이 true라고 해도 fail이 1건이라도 발생하면 창을 닫지 않아야만
					//사용자가 이를 인지할 수 있다.
					m_auto_close = false;
					logWriteE(_T("fail."));
					m_list.set_text(i, col_status, _T("fail"));
					m_list.set_text_color(i, col_status, Gdiplus::Color::Red);
			}
		}

		//전송이 완료되면 리스트를 새로고침하거나
		//해당 항목을 수동으로 리스트에 추가하고 선택상태로 표시, 스크롤되게 보여줘야 한다.
		if ((res == transfer_result_success || res == transfer_result_overwrite) && insert_item_after_transfer_success)
		{
			((CnFTDServerDlg*)(AfxGetApp()->GetMainWnd()))->add_transfered_file_to_dst_list(m_dstSide, to);
		}
	}

	//전송이 모두 완료되면 dataSocket은 닫아준다.
	m_pServerManager->DataClose();

	m_thread_transfer_started = false;
	//GetDlgItem(IDCANCEL)->EnableWindow(true);
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
	CSCThemeDlg::OnPaint();
	/*
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
	*/
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

void CnFTDFileTransferDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_check_thread_transfer)
	{
		KillTimer(timer_check_thread_transfer);

		if (!m_thread_transfer_started)
		{
			m_static_copy.stop_animation();

			if (m_auto_close)
			{
				CDialogEx::OnCancel();
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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		TRACE(_T("CnFTDFileTransferDialog keydown\n"));
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
