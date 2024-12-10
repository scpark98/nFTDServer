
// nFTDServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "nFTDServer.h"
#include "nFTDServerDlg.h"
#include "afxdialogex.h"

#include <thread>

#include "../../Common/Functions.h"
#include "../../Common/MemoryDC.h"

#include "nFTDFileTransferDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CnFTDServerDlg 대화 상자



CnFTDServerDlg::CnFTDServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NFTDSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CnFTDServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_LOCAL, m_tree_local);
	DDX_Control(pDX, IDC_LIST_LOCAL, m_list_local);
	DDX_Control(pDX, IDC_TREE_REMOTE, m_tree_remote);
	DDX_Control(pDX, IDC_LIST_REMOTE, m_list_remote);
	DDX_Control(pDX, IDC_PATH_LOCAL, m_path_local);
	DDX_Control(pDX, IDC_PATH_REMOTE, m_path_remote);
	DDX_Control(pDX, IDC_SPLITTER_CENTER, m_splitter_center);
	DDX_Control(pDX, IDC_SPLITTER_LEFT, m_splitter_left);
	DDX_Control(pDX, IDC_SPLITTER_RIGHT, m_splitter_right);
	DDX_Control(pDX, IDC_STATIC_COUNT_LOCAL, m_static_count_local);
	DDX_Control(pDX, IDC_STATIC_COUNT_REMOTE, m_static_count_remote);
	DDX_Control(pDX, IDC_PROGRESS_SPACE_LOCAL, m_progress_space_local);
	DDX_Control(pDX, IDC_PROGRESS_SPACE_REMOTE, m_progress_space_remote);
}

BEGIN_MESSAGE_MAP(CnFTDServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDOK, &CnFTDServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CnFTDServerDlg::OnBnClickedCancel)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(Message_CPathCtrl, &CnFTDServerDlg::on_message_CPathCtrl)
	ON_REGISTERED_MESSAGE(Message_CVtListCtrlEx, &CnFTDServerDlg::on_message_CVtListCtrlEx)
	ON_REGISTERED_MESSAGE(Message_CSCTreeCtrl, &CnFTDServerDlg::on_message_CSCTreeCtrl)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LOCAL, &CnFTDServerDlg::OnTvnSelchangedTreeLocal)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_REMOTE, &CnFTDServerDlg::OnTvnSelchangedTreeRemote)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_REMOTE, &CnFTDServerDlg::OnNMDblclkListRemote)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOCAL, &CnFTDServerDlg::OnNMDblclkListLocal)
END_MESSAGE_MAP()


// CnFTDServerDlg 메시지 처리기

BOOL CnFTDServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetWindowText(m_title);

	set_color_theme(CSCColorTheme::color_theme_default);

	m_resize.Create(this);
	m_resize.Add(IDC_PATH_LOCAL, 0, 0, 50, 0);
	m_resize.Add(IDC_TREE_LOCAL, 0, 0, 0, 100);
	m_resize.Add(IDC_SPLITTER_LEFT, 0, 0, 0, 100);
	m_resize.Add(IDC_LIST_LOCAL, 0, 0, 50, 100);
	m_resize.Add(IDC_PROGRESS_SPACE_LOCAL, 0, 100, 0, 0);
	m_resize.Add(IDC_STATIC_COUNT_LOCAL, 50, 100, 0, 0);

	m_resize.Add(IDC_SPLITTER_CENTER, 50, 0, 0, 100);

	m_resize.Add(IDC_PATH_REMOTE, 50, 0, 50, 0);
	m_resize.Add(IDC_TREE_REMOTE, 50, 0, 0, 100);
	m_resize.Add(IDC_SPLITTER_RIGHT, 50, 0, 0, 100);
	m_resize.Add(IDC_LIST_REMOTE, 50, 0, 50, 100);
	m_resize.Add(IDC_PROGRESS_SPACE_REMOTE, 50, 100, 0, 0);
	m_resize.Add(IDC_STATIC_COUNT_REMOTE, 100, 100, 0, 0);

	m_splitter_left.SetType(CControlSplitter::CS_VERT);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_TREE_LOCAL);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_PROGRESS_SPACE_LOCAL);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_LIST_LOCAL);

	m_splitter_right.SetType(CControlSplitter::CS_VERT);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_TREE_REMOTE);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_PROGRESS_SPACE_REMOTE);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_LIST_REMOTE);

	//중앙 splitter
	m_splitter_center.SetType(CControlSplitter::CS_VERT);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_PATH_LOCAL);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_LIST_LOCAL);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_PROGRESS_SPACE_LOCAL);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_STATIC_COUNT_LOCAL, SPF_RIGHT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_PATH_REMOTE);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_TREE_REMOTE, SPF_LEFT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_SPLITTER_RIGHT, SPF_LEFT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_LIST_REMOTE);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_PROGRESS_SPACE_REMOTE);

	//이 코드를 넣어야 작업표시줄에서 클릭하여 minimize, restore된다.
	//작업표시줄에서 해당 앱을 shift+우클릭하여 SYSMENU를 표시할 수 있다.
	//또한 CResizeCtrl을 이용하면 resize할 때 모든 컨트롤들의 레이아웃을 자동으로 맞춰주는데
	//아래 코드를 사용하지 않으면 타이틀바가 없는 dlg는 상단에 흰색 여백 공간이 생기는 부작용이 생긴다.
	SetWindowLong(m_hWnd, GWL_STYLE, WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN);

	//m_sys_buttons.set_color_theme(CSCColorTheme::color_theme_dark_gray);




	//for test
	while (get_process_running_count(_T("nFTDClient.exe")) > 0)
		kill_process_by_fullpath(_T("nFTDClient.exe"));

	m_progress_space_local.SetColor(RGB(36, 160, 212), RGB(230, 230, 230));
	m_progress_space_local.draw_border();
	m_progress_space_local.SetPos(30);

	m_progress_space_remote.SetColor(RGB(36, 160, 212), RGB(230, 230, 230));
	m_progress_space_remote.draw_border();
	m_progress_space_remote.SetPos(70);

	CString my_ip = get_my_ip();
	if (my_ip == CString(__targv[4]))
		ShellExecute(NULL, _T("open"), get_exe_directory() + _T("\\nFTDClient.exe"), _T("-l 443"), 0, SW_SHOWNORMAL);


	init_treectrl();
	init_listctrl();
	init_pathctrl();
	init_shadow();

	RestoreWindowPosition(&theApp, this);

	init_progress();

	std::thread t(&CnFTDServerDlg::thread_connect, this);
	t.detach();


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CnFTDServerDlg::init_treectrl()
{
	m_tree_local.set_use_drag_and_drop(true);
	m_tree_local.set_as_shell_treectrl(&theApp.m_shell_imagelist, true);
	//m_tree_local.add_drag_images(IDB_DRAG_ONE_FILE, IDB_DRAG_MULTI_FILES);
	//m_tree_local.set_path(_T("c:\\"));

	m_tree_remote.set_as_shell_treectrl(&theApp.m_shell_imagelist, false);
}

void CnFTDServerDlg::init_listctrl()
{
	m_list_local.set_as_shell_listctrl(&theApp.m_shell_imagelist, true);
	m_list_local.set_use_drag_and_drop(true);
	m_list_local.load_column_width(&theApp, _T("shell list0"));
	//m_list_local.set_path(_T("d:\\"));
	//m_list_local.add_drag_images(IDB_DRAG_ONE_FILE, IDB_DRAG_MULTI_FILES);

	m_list_remote.set_as_shell_listctrl(&theApp.m_shell_imagelist, false);
	m_list_remote.set_use_drag_and_drop(true);
}

void CnFTDServerDlg::init_pathctrl()
{
	m_path_local.set_shell_imagelist(&theApp.m_shell_imagelist);
	//m_path_local.set_path(_T("d:\\"));

	m_path_remote.set_shell_imagelist(&theApp.m_shell_imagelist);
	m_path_remote.set_is_local_device(false);
}

void CnFTDServerDlg::init_shadow()
{
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_shadow.Create(GetSafeHwnd());
	m_shadow.SetSize(14);	// -19 ~ 19
	m_shadow.SetSharpness(19);	// 0 ~ 19
	m_shadow.SetDarkness(14);	// 0 ~ 254
	m_shadow.SetPosition(0, 0);	// -19 ~ 19
	m_shadow.SetColor(RGB(0, 0, 0));
}

void CnFTDServerDlg::init_progress()
{
	CString msg;
	Gdiplus::Color cr_text = gRGB(212, 212, 212);
	Gdiplus::Color cr_back = gRGB(59, 70, 92);

	if (_tcscmp(__targv[1], _T("-p")) == 0)
	{
		msg.LoadString(NFTD_IDS_CONNECTING_AP2P);
	}
	else
	{
		msg.LoadString(NFTD_IDS_CONNECTING_P2P);
		cr_text = gRGB(212, 212, 255);
		cr_back = gRGB(52, 99, 193);
	}

	m_progressDlg.create(this, msg, 0, 0, 320, 120);
	m_progressDlg.set_text_color(cr_text);
	m_progressDlg.set_back_color(cr_back);
	m_progressDlg.set_indeterminate();
	m_progressDlg.CenterWindow();
	m_progressDlg.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	m_progressDlg.ShowWindow(SW_SHOW);
}

void CnFTDServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CnFTDServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc1(this);
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
		CFont *font = GetFont();
		CFont* pOldFont = (CFont*)dc.SelectObject(font);
		dc.SetBkMode(TRANSPARENT);

		if (_tcscmp(__targv[1], _T("-p")) == 0)
			dc.SetTextColor(RGB(96, 96, 128));
		else
			dc.SetTextColor(RGB(52, 99, 193));

		dc.DrawText(m_title, rTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		//테두리
		//draw_rectangle(&dc, rc, m_sys_buttons.m_theme.cr_selected_border);

		dc.SelectObject(pOldFont);
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CnFTDServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CnFTDServerDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this);
}


void CnFTDServerDlg::OnBnClickedOk()
{
	//m_ServerManager.refresh();
}


void CnFTDServerDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	kill_process_by_fullpath(get_exe_directory() + _T("\\nFTDClient.exe"));
	SaveLocalLastPath();

	CDialogEx::OnCancel();
}


BOOL CnFTDServerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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


void CnFTDServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
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


void CnFTDServerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CnFTDServerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (point.y < TITLEBAR_HEIGHT)
	{
		ShowWindow(IsZoomed() ? SW_RESTORE : SW_MAXIMIZE);
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CnFTDServerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CSize sz;
	bool is_auto_hide = get_taskbar_state(ABS_AUTOHIDE, &sz);

	lpMMI->ptMaxSize.y -= (is_auto_hide ? 2 : sz.cy);

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


BOOL CnFTDServerDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;

	return CDialogEx::OnEraseBkgnd(pDC);
}


void CnFTDServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_hWnd == NULL)
		return;

	CRect rc;
	GetClientRect(rc);

	if (m_sys_buttons.m_hWnd == NULL)
	{
		m_sys_buttons.create(this, 1, rc.right - 1, 44, TITLEBAR_HEIGHT, SC_MINIMIZE, SC_MAXIMIZE, SC_CLOSE);
	}
	else
	{
		m_sys_buttons.adjust(rc.top + 1, rc.right - 1);
	}

	Invalidate();
}

void CnFTDServerDlg::thread_connect()
{
	long t0 = clock();
	connect();

	//"연결중..."이라는 메시지를 표시하고 연결이 된 후 파일목록을 불러오는데
	//연결시간이 매우 짧다면 연결중 메시지가 보이지 않게 되므로 약간의 딜레이를 준다.
	//원격컴퓨터에서 네트워크 드라이브를 상시 연결모드로 해놨는데 그 네트워크 드라이브가 오프라인일 경우
	//파일목록을 로딩하는데 꽤 오랜 시간이 걸린다.
	if (clock() - t0 < 1000)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	initialize();
	Invalidate();

	m_show_main_ui = true;
	m_progressDlg.ShowWindow(SW_HIDE);
}

int CnFTDServerDlg::connect()
{
	// -standalone command-line 인 경우 dialog 의 좌우에 나타나는
	// 콤보박스와 파일리스트를 바꿔줘야한다. 즉 SERVERSIDE, CLIENTSIDE 의 숫자를 바꾼다.
	if (find_parameter(_T("-standalone")) > 0)
	{
		m_ServerManager.m_is_server_side = false;
	}

	// 뷰어 모드인지 판단
	m_bViewerMode = (find_parameter(_T("-viewermode")) > 0);

#ifdef LMM_SERVICE
	if (!m_ServerManager.SetConnectionService())
#else
	if (!m_ServerManager.SetConnection(lpCmdLine))
#endif
	{
		AfxMessageBox(_T("m_ServerManager.SetConnection(lpCmdLine) failed."));
		/*
		CString message;
		message.LoadString(NFTD_IDS_MSGBOX_RUN_1);

		CMessageDlg dlgMessage;
		dlgMessage.SetMessage(MsgType::TypeOK, message);
		dlgMessage.DoModal();

		delete[] lpCmdLine;
		ExitProcess(0);
		*/
		CDialogEx::OnCancel();
		return 0;
	}

	logWrite(_T("Connection Begin. g_FT_mode = %d"), g_FT_mode);

	if (!m_ServerManager.Connection())
	{
		AfxMessageBox(_T("m_ServerManager.Connection() Fail"));
		/*
		CString message;
		message.LoadString(NFTD_IDS_MSGBOX_RUN_2);

		CMessageDlg dlgMessage;
		dlgMessage.SetMessage(MsgType::TypeOK, message);
		dlgMessage.DoModal();

		delete[] lpCmdLine;
		ExitProcess(0);
		*/
		CDialogEx::OnCancel();
		return 0;
	}

	//delete[] lpCmdLine;

	//m_treeClient.SetOsType(m_ServerManager.m_nClientOSType);

	m_title = m_ServerManager.m_strStatusbarTitle;
	SetWindowText(m_title);

	logWrite(_T("Connection ok."));

	return true;
}

void CnFTDServerDlg::initialize()
{
	InitServerManager();		// Server Manager 설정

	CString path = convert_special_folder_to_real_path(GetLocalLastPath());
	if (path.IsEmpty() || !PathFileExists(path))
		path = get_known_folder(CSIDL_DRIVES);

	logWrite(_T("local last path = %s"), path);
	change_directory(path, SERVER_SIDE);

	path = GetRemoteLastPath();
	//if (path.IsEmpty() || !PathFileExists(path))
	//	path = get_known_folder(CSIDL_DRIVES);

	//logWrite(_T("remote last path = %s"), path);
	//change_directory(path, CLIENT_SIDE);

	if (path != _T(""))
	{
		logWrite(_T("RemotePath : %s"), path);
		if (!change_directory(path, CLIENT_SIDE))
		{
			SetDefaultPathToDesktop(1); // 실패 시 바탕화면으로 설정
		}
	}
	else
	{
		logWrite(_T("RemotePath : Desktop"));
		SetDefaultPathToDesktop(1);	// Remote 초기 경로를 바탕화면으로 설정
	}
}


void CnFTDServerDlg::InitServerManager()
{
	logWrite(_T("Begin"));

	std::map<int, CString> map;
	m_ServerManager.get_remote_system_label(&map);
	theApp.m_shell_imagelist.set_system_label(CLIENT_SIDE, &map);

	map.clear();
	m_ServerManager.get_remote_system_path(&map);
	theApp.m_shell_imagelist.set_system_path(CLIENT_SIDE, &map);


	std::deque<CString> drive_list;
	m_ServerManager.DriveList(&drive_list);
	theApp.m_shell_imagelist.set_drive_list(CLIENT_SIDE, &drive_list);

	//theApp.m_shell_imagelist.m_volumeadd_drive_list(&drive_list);

	//m_ulServerDiskSpace.QuadPart = m_ServerManager.Refresh(&m_listServerFile, &m_barTextServer, m_ServerManager.m_SERVERSIDE);
	//logWrite(_T("Server Refresh"));
	//m_ulClientDiskSpace.QuadPart = m_ServerManager.Refresh(&m_listClientFile, &m_barTextClient, m_ServerManager.m_CLIENTSIDE);
	//m_ServerManager.refresh_list(&m_list_remote, false);
	//logWrite(_T("refresh_list ok"));

	//m_ServerManager.DriveList(&m_comboServerDrv, m_ServerManager.m_SERVERSIDE);
	//logWrite(_T("Server DriveList"));

	//m_ServerManager.refresh_tree(&m_tree_remote, false);
	//logWrite(_T("refresh_tree ok"));

	//m_ServerManager.GetDesktopPath(&m_treeClient);	// 리모트의 바탕 화면 경로를 얻어서 저장
	//logWrite(_T("Get Client Path"));
}

void CnFTDServerDlg::SetDefaultPathToDesktop(int type)
{
	// Tree의 ROOT가 바탕 화면이므로
	// ROOT의 Item을 얻어서 SelectItem 하면
	// OnSelChange가 호출되어 경로가 변경됨
	// 로그오프 상태일 경우 바탕화면과 내문서가 없음
	if (false)//m_ServerManager.m_nClientOSType == OS_MAC)
	{
		//ChangeDirectory(_T("/"), MODE_CLIENT);
	}
	else
	{
		//if (m_tree_remote.CleanTree())
		//{
		//	m_treeClient.SelectItem(m_treeClient.GetCDriveItem());
		//}
		//else
		{
			m_tree_remote.SelectItem(m_tree_remote.GetRootItem());
		}
	}
}

BOOL CnFTDServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_KEYUP)
	{
		TRACE(_T("key on CnFTDServerDlg\n"));
		switch (pMsg->wParam)
		{
			case VK_RETURN :
			{
				OnBnClickedOk();
				return TRUE;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT	CnFTDServerDlg::on_message_CPathCtrl(WPARAM wParam, LPARAM lParam)
{
	CPathCtrlMessage* pMsg = (CPathCtrlMessage*)wParam;

	if (pMsg->pThis == &m_path_local)
	{
		TRACE(_T("on_message_pathctrl from m_path_local\n"));
		if (pMsg->message == CPathCtrl::message_pathctrl_path_changed)
		{
			m_list_local.set_path(pMsg->cur_path);
			m_tree_local.set_path(pMsg->cur_path);
			change_directory(pMsg->cur_path, SERVER_SIDE);
		}
	}
	else if (pMsg->pThis == &m_path_remote)
	{
		TRACE(_T("on_message_pathctrl from m_path_remote\n"));
		if (pMsg->message == CPathCtrl::message_pathctrl_path_changed)
		{
			change_directory(pMsg->cur_path, CLIENT_SIDE);
			m_tree_remote.set_path(pMsg->cur_path);
		}
	}

	return 0;
}

LRESULT	CnFTDServerDlg::on_message_CVtListCtrlEx(WPARAM wParam, LPARAM lParam)
{
	CVtListCtrlExMessage* msg = (CVtListCtrlExMessage*)wParam;

	if (msg->message == CVtListCtrlEx::message_path_changed)
	{
		CString path = *(CString*)lParam;

		if (msg->pThis == &m_list_local)
		{
			m_tree_local.set_path(path);
			m_path_local.set_path(path);
			change_directory(path, SERVER_SIDE);
		}
		else if (msg->pThis == &m_list_remote)
		{
			m_tree_remote.set_path(path);
			m_path_remote.set_path(path);
			change_directory(path, CLIENT_SIDE);
		}
	}
	else if (msg->message == CVtListCtrlEx::message_drag_and_drop)
	{
		//다른 앱에서 드롭된 경우는 대상 컨트롤들에 남아있는 drophilited 항목들의 상태를 지워줘야 한다.
		//원래는 마지막 drophilited했던 컨트롤을 기억시켰다가 해당 컨트롤만 해줘야 한다.
		if (msg->pTarget == NULL)
		{
			m_list_local.set_items_with_state(0, LVIS_DROPHILITED);
			m_list_remote.set_items_with_state(0, LVIS_DROPHILITED);
			m_tree_local.SelectDropTarget(NULL);
			m_tree_remote.SelectDropTarget(NULL);
			return 0;
		}

		CString dropped_path;
		CString droppedItemText;
		CVtListCtrlEx* pDragListCtrl = (CVtListCtrlEx*)msg->pThis;
		
		m_srcSide = (pDragListCtrl == &m_list_remote);

		if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		{
			CVtListCtrlEx* pDropListCtrl = (CVtListCtrlEx*)msg->pTarget;
			m_dstSide = (pDropListCtrl == &m_list_remote);

			int droppedIndex = pDragListCtrl->get_drop_index();

			if (droppedIndex >= 0)
				droppedItemText = pDropListCtrl->get_text(droppedIndex, CVtListCtrlEx::col_filename);

			if (droppedItemText.IsEmpty())
			{
				if (pDropListCtrl == &m_list_local)
				{
					m_dstSide = SERVER_SIDE;
					dropped_path = pDropListCtrl->get_path();
				}
				else if (pDropListCtrl == &m_list_remote)
				{
					dropped_path = m_remoteCurrentPath;
				}
				TRACE(_T("dropped on = %s\n"), dropped_path);
			}
			else
			{
				if (pDropListCtrl == &m_list_local)
				{
					m_dstSide = SERVER_SIDE;
					dropped_path = pDropListCtrl->get_path() + _T("\\") + droppedItemText;
				}
				else if (pDropListCtrl == &m_list_remote)
				{
					dropped_path = m_remoteCurrentPath + _T("\\") + droppedItemText;
				}

				//만약 dropped_path가 폴더가 아닌 파일이라면 dropped_path는 폴더명까지만 취한다.
				if (pDropListCtrl->get_text(droppedIndex, CVtListCtrlEx::col_filesize).IsEmpty() == false)
				{
					dropped_path = GetParentDirectory(dropped_path);
				}


				TRACE(_T("dropped on = %s\n"), dropped_path);
			}

			//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
			if (droppedIndex >= 0)
				pDropListCtrl->SetItemState(droppedIndex, 0, LVIS_DROPHILITED);
		}
		else if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
		{
			CSCTreeCtrl* pDropTreeCtrl = (CSCTreeCtrl*)msg->pTarget;
			HTREEITEM hItem = pDropTreeCtrl->GetDropHilightItem();

			if (hItem)
			{
				droppedItemText = pDropTreeCtrl->GetItemText(hItem);
				dropped_path = pDropTreeCtrl->get_fullpath(hItem);
				TRACE(_T("dropped on = %s (%s)\n"), droppedItemText, dropped_path);

				//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
				//pDropTreeCtrl->SetItemState(hItem, 0, TVIS_DROPHILITED);	<= 이걸로는 해제 안된다.
				pDropTreeCtrl->SelectDropTarget(NULL);
			}
		}

		std::deque<int> dq;
		pDragListCtrl->get_selected_items(&dq);

		m_transfer_list.clear();
		m_transfer_from = pDragListCtrl->get_path();
		m_transfer_to = dropped_path;

		for (int i = 0; i < dq.size(); i++)
		{
			if (m_srcSide == SERVER_SIDE)
			{
				m_transfer_list.push_back(pDragListCtrl->get_path(dq[i]));
			}
			else
			{
				m_transfer_list.push_back(pDragListCtrl->get_path(dq[i]));
			}
			TRACE(_T("dragged src %d = %s (%s)\n"), i, pDragListCtrl->get_text(dq[i], CVtListCtrlEx::col_filename), m_transfer_list.back());
		}

		file_transfer();
	}

	return 0;
}

LRESULT	CnFTDServerDlg::on_message_CSCTreeCtrl(WPARAM wParam, LPARAM lParam)
{
	CSCTreeCtrlMessage* msg = (CSCTreeCtrlMessage*)wParam;

	//drag_and_drop일 경우는 msg에 src, dst에 대한 모든 정보가 포함되어 있으므로
	//m_tree0 or m_tree1 어느 컨트롤이냐에 따라 처리할 필요는 없다.
	if (msg->message == CSCTreeCtrl::message_drag_and_drop)
	{
		//다른 앱에서 드롭된 경우는 대상 컨트롤들에 남아있는 drophilited 항목들의 상태를 지워줘야 한다.
		//원래는 마지막 drophilited했던 컨트롤을 기억시켰다가 해당 컨트롤만 해줘야 한다.
		if (msg->pTarget == NULL)
		{
			m_list_local.set_items_with_state(0, LVIS_DROPHILITED);
			m_list_remote.set_items_with_state(0, LVIS_DROPHILITED);
			m_tree_local.SelectDropTarget(NULL);
			m_tree_remote.SelectDropTarget(NULL);
			return 0;
		}

		CString droppedItemText;
		CSCTreeCtrl* pDragTreeCtrl = (CSCTreeCtrl*)msg->pThis;

		if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		{
			CVtListCtrlEx* pDropListCtrl = (CVtListCtrlEx*)msg->pTarget;

			if (pDragTreeCtrl->m_nDropIndex >= 0)
				droppedItemText = pDropListCtrl->GetItemText(pDragTreeCtrl->m_nDropIndex, 0);

			TRACE(_T("drag item = %s\n"), pDragTreeCtrl->GetItemText(pDragTreeCtrl->m_DragItem));

			if (droppedItemText.IsEmpty())
				TRACE(_T("dropped on = %s\n"), pDropListCtrl->get_path());
			else
				TRACE(_T("dropped on = %s\n"), pDropListCtrl->get_path() + _T("\\") + droppedItemText);

			//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
			if (pDragTreeCtrl->m_nDropIndex >= 0)
				pDropListCtrl->SetItemState(pDragTreeCtrl->m_nDropIndex, 0, LVIS_DROPHILITED);
		}
		else if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
		{
			CSCTreeCtrl* pDropTreeCtrl = (CSCTreeCtrl*)msg->pTarget;

			TRACE(_T("drag item = %s\n"), pDragTreeCtrl->GetItemText(pDragTreeCtrl->m_DragItem));
			TRACE(_T("dropped on = %s\n"), pDropTreeCtrl->GetItemText(pDragTreeCtrl->m_DropItem));

			//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
			//pDropTreeCtrl->SetItemState(hItem, 0, TVIS_DROPHILITED);	<= 이걸로는 해제 안된다.
			pDropTreeCtrl->SelectDropTarget(NULL);
		}

		return 0;
	}
	else if (msg->message == CSCTreeCtrl::message_request_folder_list)
	{
		int i;
		CString path = *(CString*)lParam;

		//path가 "내 PC"이면 DriveList()를, 그렇지 않으면 refresh_tree_folder()를 호출한다.
		if (path == theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DRIVES))
		{
			std::deque<CString> drive_list;
			m_ServerManager.DriveList(&drive_list);
			theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].set_drive_list(&drive_list);
			m_tree_remote.update_drive_list(path, &drive_list);
		}
		else
		{
			std::deque<WIN32_FIND_DATA> dq;
			m_ServerManager.get_folderlist(path, &dq, true);
			if (dq.size() > 0)
			{
				path = GetParentDirectory(CString(dq[0].cFileName));
				//"C:\\" 인 경우와 "C:\\Windows" 인 경우는 처리가 달라야 한다.
				int folder_length = path.GetLength() + (path.Right(1) == '\\' ? 0 : 1);

				for (i = 0; i < dq.size(); i++)
				{
					//검색된 폴더의 sub folder 유무에 따라 확장버튼 유무도 정해진다.
					std::deque<WIN32_FIND_DATA> dq_sub_folder;
					m_ServerManager.get_folderlist(dq[i].cFileName, &dq_sub_folder, true);

					//dq가 fullpath로 넘어오므로 폴더명만 넣어줘야 한다.
					TCHAR* p = dq[i].cFileName;
					p += folder_length;
					_tcscpy(dq[i].cFileName, p);
					m_tree_remote.insert_folder(&dq[i], dq_sub_folder.size() > 0);
				}
			}
		}
	}

	return 0;
}


void CnFTDServerDlg::OnTvnSelchangedTreeLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//change_directory()를 써서 local이든 remote이든 공통으로 처리하면 좋지만
	//tree가 변경된 경우는 path와 listctrl만 refresh한다든지 여러가지 부가적인 처리가 필요하여
	//local인 경우는 직접 처리한다.
	CString path = m_tree_local.get_fullpath();

	m_list_local.set_path(path);
	m_path_local.set_path(path);

	*pResult = 0;
}


void CnFTDServerDlg::OnTvnSelchangedTreeRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString path = m_tree_remote.get_fullpath(NULL);

	//remote의 바탕화면, 내 PC는 그 경로가 고정이지만 MyDocuments는 사용자가 이를 변경할 수 있으므로 별도 처리해야 한다.
	//내 문서의 기본 레이블인 "문서"를 리턴하면 이를 m_remoteDocumentPath로 변경하여 사용한다.

	//내 PC
	/*
	if (path == theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DRIVES))
		path = theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_drive_list()->at(drive_list_drives);
	//바탕화면
	else if (path == get_system_label(CSIDL_DESKTOP))
		path = theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_drive_list()->at(drive_list_desktop);
	//문서 폴더
	else if (path == get_system_label(CSIDL_MYDOCUMENTS))
		path = theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_drive_list()->at(drive_list_mydocuments);
	*/

	change_directory(path, CLIENT_SIDE);

	*pResult = 0;
}

void CnFTDServerDlg::OnNMDblclkListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = pNMItemActivate->iItem;

	CString path = m_list_local.get_path();
	CString new_path;

	if (path == get_system_label(CSIDL_DRIVES))
	{
		new_path = convert_special_folder_to_real_path(m_list_local.get_text(index, CVtListCtrlEx::col_filename));// , theApp.m_shell_imagelist.m_volume[SERVER_SIDE].get_label_map());
		m_list_local.set_path(new_path);
		m_path_local.set_path(new_path);
		m_tree_local.set_path(new_path);
	}
	else
	{
		new_path = m_list_local.get_path() + _T("\\") + m_list_local.get_text(index, CVtListCtrlEx::col_filename);
		if (PathIsDirectory(new_path))
		{
			m_list_local.set_path(new_path);
			m_path_local.set_path(new_path);
			m_tree_local.set_path(new_path);
		}
	}

	*pResult = 0;
}

void CnFTDServerDlg::OnNMDblclkListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = pNMItemActivate->iItem;

	//is a file? just return? transfer?
	if (m_list_remote.get_text(index, CVtListCtrlEx::col_filesize).IsEmpty() == false)
		return;

	//is folder?
	change_directory(m_remoteCurrentPath + _T("\\") + m_list_remote.get_text(index, CVtListCtrlEx::col_filename), CLIENT_SIDE);

	*pResult = 0;
}

BOOL CnFTDServerDlg::change_directory(CString path, DWORD dwSide)
{
	logWrite(_T("path = %s"), path);

	int i;
	BOOL result = FALSE;

	if (dwSide == SERVER_SIDE)
	{
		m_path_local.set_path(path);
		m_tree_local.set_path(path);
		m_list_local.set_path(path);

		//if (_tchdir(path) == 0)
		return true;
	}
	else
	{
		if (false)//m_ServerManager.m_nClientOSType == OS_MAC)
		{
			if (path.ReverseFind(_T('/')) == path.GetLength() - 1)
			{
				path = path.Left(path.GetLength() - 1);
			}
			if (path == _T(""))
			{
				path = _T("/");
			}
		}


		//result = m_ServerManager.change_directory(path, m_dwSide, false);
		if (true)//result)
		{
			//path가 내 PC, 바탕 화면, 문서, 로컬 디스크(C:) 등 모두 그대로 전달한다.
			m_remoteCurrentPath = path;
			SaveRemoteLastPath();

			std::deque<WIN32_FIND_DATA> dq;

			//리스트 파일목록 갱신
			m_list_remote.delete_all_items();
			//m_ServerManager.refresh_list(&dq, false);
			m_ServerManager.get_filelist(path, &dq);

			CString filename;
			CString fullpath;
			//bool is_disk_list = (path == theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DRIVES));
			for (i = 0; i < dq.size(); i++)
			{
				//if (!is_disk_list)
				//{
				//	fullpath.Format(_T("%s%s%s"), path, (path.Right(1) == '\\' ? _T("") : _T("\\")), dq[i].cFileName);
				//	_stprintf(dq[i].cFileName, _T("%s"), fullpath);
				//}
				m_list_remote.add_file(&dq[i], true);
			}

			m_list_remote.display_list(path);

			//path ctrl 갱신. dq에서 파일들을 제외하고 폴더 목록을 넘겨준다.
			std::deque<CString> folder_list;
			for (i = 0; i < dq.size(); i++)
			{
				WIN32_FIND_DATA FindFileData = dq[i];
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
					folder_list.push_back(FindFileData.cFileName);
			}
			m_path_remote.set_path(path, &folder_list);

			m_static_count_remote.set_textf(-1, _T("%d개 항목"), dq.size());
		}

		result = TRUE;
	}

	return result;
}

CString CnFTDServerDlg::GetLocalLastPath()
{
	TCHAR chValue[256] = { 0, };
	GetPrivateProfileString(_T("LOCAL"), _T("LAST_PATH"), _T(""), chValue, 256, get_exe_directory() + _T("\\filetransfer.ini"));
	return chValue;
}

CString CnFTDServerDlg::GetRemoteLastPath()
{
	TCHAR chValue[256] = { 0, };
	GetPrivateProfileString(m_ServerManager.m_strServerIP, m_ServerManager.m_strDeviceID, _T(""), chValue, 256, get_exe_directory() + _T("\\filetransfer.ini"));
	return chValue;
}

void CnFTDServerDlg::SaveRemoteLastPath()
{
#ifdef ANYSUPPORT
	return;
#endif
	WritePrivateProfileString(m_ServerManager.m_strServerIP, m_ServerManager.m_strDeviceID, m_remoteCurrentPath, get_exe_directory() + _T("\\filetransfer.ini"));
}


void CnFTDServerDlg::SaveLocalLastPath()
{
#ifdef ANYSUPPORT
	return;
#endif
	WritePrivateProfileString(_T("LOCAL"), _T("LAST_PATH"), m_list_local.get_path(), get_exe_directory() + _T("\\filetransfer.ini"));
}

void CnFTDServerDlg::set_color_theme(int theme)
{
	m_theme.set_color_theme(theme);
	m_theme.cr_text = Gdiplus::Color::DarkGray;
	m_theme.cr_back = Gdiplus::Color::White;

	m_tree_local.set_color_theme(theme);
	m_list_local.set_color_theme(theme);
	m_tree_remote.set_color_theme(theme);
	m_list_remote.set_color_theme(theme);

	m_sys_buttons.set_back_color(m_theme.cr_back);

	m_static_count_local.set_color(m_theme.cr_text, m_theme.cr_back);
	m_static_count_remote.set_color(m_theme.cr_text, m_theme.cr_back);
}

//보내기 버튼 or 받기 버튼 or drag&drop으로 전송을 시작한다.
void CnFTDServerDlg::file_transfer()
{
	if (m_transfer_list.size() == 0)
	{
		AfxMessageBox(_T("선택된 파일이 없습니다."));
		return;
	}

	//l to l, l to r, r to l, r to l 4가지 모두 나눠서 처리?? 우선 ltr, rtl 2가지만 고려한다.
	//if (m_dwSide == SERVER_SIDE && target == SERVER_SIDE)
	//{
	//	return;
	//}
	//else if (m_dwSide == CLIENT_SIDE && target == CLIENT_SIDE)
	//{
	//	return;
	//}

	std::deque<WIN32_FIND_DATA> filelist;

	//전송할 파일목록을 filelist에 채운다.
	//이 목록을 CnFTDFileTransferDialog에 전달하고
	//그 dlg의 thread에서 전체 파일 목록을 재구성한 후 순차적으로 전송을 시작한다.
	//추후에는 multithread로 파일을 전송하는 방법 또한 생각해야 한다.
	for (int i = 0; i < m_transfer_list.size(); i++)
	{
		WIN32_FIND_DATA data;
		memset(&data, 0, sizeof(data));

		//로컬인 경우는 FindFirstFile()에 그 경로를 주면 WIN32_FIND_DATA 형식을 구할 수 있다.
		if (m_srcSide == SERVER_SIDE)
		{
			HANDLE hFind = FindFirstFile(m_transfer_list[i], &data);
			FindClose(hFind);
			//FindFirstFile()로 구한 data.cFileName은 폴더명 또는 파일명만 저장되므로 fullpath로 변경해서 넘겨줘야 한다.
			_tcscpy(data.cFileName, m_transfer_list[i]);
		}
		//remote인 경우는 m_list_remote의 m_cur_folder, m_cur_file 목록에서 찾아서 그 정보를 채워야 한다.
		else
		{
			m_list_remote.get_remote_file_info(m_transfer_list[i], &data);
		}

		filelist.push_back(data);
	}

	CnFTDFileTransferDialog m_FileTransferDlg;
	//m_FileTransferDlg.m_list.set_as_shell_listctrl(&m_ShellImageList, false);

	CVtListCtrlEx* pListFile;
	ULARGE_INTEGER* pulDiskSpace;

	if (m_srcSide == SERVER_SIDE)
	{
		pListFile = &m_list_local;
		pulDiskSpace = &m_ulClientDiskSpace;
	}
	else
	{
		pListFile = &m_list_remote;
		pulDiskSpace = &m_ulServerDiskSpace;
		m_transfer_from = convert_special_folder_to_real_path(m_remoteCurrentPath, m_list_remote.get_shell_imagelist());
	}

	if (m_FileTransferDlg.FileTransferInitalize(&m_ServerManager, &filelist, pulDiskSpace,
												m_srcSide, m_dstSide, m_transfer_from, m_transfer_to, false))
	{
		m_FileTransferDlg.DoModal();
		
		//전송이 모두 끝나면 해당 폴더를 refresh해준다.
		change_directory(m_transfer_to, CLIENT_SIDE);
	}
	else
	{
		CString message;
		message.LoadString(NFTD_IDS_SELECT_FILE);

		//CMessageDlg dlgMessage;
		//dlgMessage.SetMessage(MsgType::TypeOK, message);
		//dlgMessage.DoModal();
	}

}
