
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
#include "../../Common/ini/EZIni/EZIni.h"

#include "nFTDFileTransferDialog.h"
#include "MessageDlg.h"

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
	: CSCThemeDlg(IDD_NFTDSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CnFTDServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CSCThemeDlg::DoDataExchange(pDX);
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
	DDX_Control(pDX, IDC_STATIC_LOCAL, m_static_local);
	DDX_Control(pDX, IDC_STATIC_REMOTE, m_static_remote);
	DDX_Control(pDX, IDC_CHECK_CLOSE_AFTER_ALL, m_check_close_after_all);
	DDX_Control(pDX, IDC_BUTTON_LOCAL_TO_REMOTE, m_button_local_to_remote);
	DDX_Control(pDX, IDC_BUTTON_REMOTE_TO_LOCAL, m_button_remote_to_local);
	DDX_Control(pDX, IDC_SPLITTER_CENTER2, m_splitter_center2);
	DDX_Control(pDX, IDC_LIST_LOCAL_FAVORITE, m_list_local_favorite);
	DDX_Control(pDX, IDC_LIST_REMOTE_FAVORITE, m_list_remote_favorite);
	DDX_Control(pDX, IDC_SPLITTER_LOCAL_FAVORITE, m_splitter_local_favorite);
	DDX_Control(pDX, IDC_SPLITTER_REMOTE_FAVORITE, m_splitter_remote_favorite);
	DDX_Control(pDX, IDC_SLIDER_LOCAL_DISK_SPACE, m_slider_local_disk_space);
	DDX_Control(pDX, IDC_SLIDER_REMOTE_DISK_SPACE, m_slider_remote_disk_space);
	DDX_Control(pDX, IDC_STATIC_LOCAL_DISK_SPACE, m_static_local_disk_space);
	DDX_Control(pDX, IDC_STATIC_REMOTE_DISK_SPACE, m_static_remote_disk_space);
}

BEGIN_MESSAGE_MAP(CnFTDServerDlg, CSCThemeDlg)
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
	ON_NOTIFY(NM_RCLICK, IDC_TREE_LOCAL, &CnFTDServerDlg::OnNMRClickTreeLocal)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_REMOTE, &CnFTDServerDlg::OnNMRClickTreeRemote)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LOCAL, &CnFTDServerDlg::OnNMRClickListLocal)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_REMOTE, &CnFTDServerDlg::OnNMRClickListRemote)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_SEND, &CnFTDServerDlg::OnListContextMenuSend)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_NEW_FOLDER, &CnFTDServerDlg::OnListContextMenuNewFolder)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_RENAME, &CnFTDServerDlg::OnListContextMenuRename)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_DELETE, &CnFTDServerDlg::OnListContextMenuDelete)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_REFRESH, &CnFTDServerDlg::OnListContextMenuRefresh)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_SELECT_ALL, &CnFTDServerDlg::OnListContextMenuSelectAll)
	ON_WM_TIMER()
	ON_COMMAND(ID_LIST_CONTEXT_MENU_OPEN, &CnFTDServerDlg::OnListContextMenuOpen)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_PROPERTY, &CnFTDServerDlg::OnListContextMenuProperty)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LOCAL, &CnFTDServerDlg::OnLvnItemChangedListLocal)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REMOTE, &CnFTDServerDlg::OnLvnItemChangedListRemote)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_OPEN_EXPLORER, &CnFTDServerDlg::OnListContextMenuOpenExplorer)
	ON_BN_CLICKED(IDC_CHECK_CLOSE_AFTER_ALL, &CnFTDServerDlg::OnBnClickedCheckCloseAfterAll)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_TO_REMOTE, &CnFTDServerDlg::OnBnClickedButtonLocalToRemote)
	ON_BN_CLICKED(IDC_BUTTON_REMOTE_TO_LOCAL, &CnFTDServerDlg::OnBnClickedButtonRemoteToLocal)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOCAL_FAVORITE, &CnFTDServerDlg::OnNMDblclkListLocalFavorite)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_REMOTE_FAVORITE, &CnFTDServerDlg::OnNMDblclkListRemoteFavorite)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_FAVORITE, &CnFTDServerDlg::OnListContextMenuFavorite)
	ON_COMMAND(ID_FAVORITE_CONTEXT_MENU_DELETE, &CnFTDServerDlg::OnFavoriteContextMenuDelete)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LOCAL_FAVORITE, &CnFTDServerDlg::OnNMRClickListLocalFavorite)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_REMOTE_FAVORITE, &CnFTDServerDlg::OnNMRClickListRemoteFavorite)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_LOCAL, &CnFTDServerDlg::OnLvnEndlabelEditListLocal)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_REMOTE, &CnFTDServerDlg::OnLvnEndlabelEditListRemote)
END_MESSAGE_MAP()


// CnFTDServerDlg 메시지 처리기

BOOL CnFTDServerDlg::OnInitDialog()
{
	CSCThemeDlg::OnInitDialog();

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
	m_resize.Add(IDC_STATIC_LOCAL, 0, 0, 0, 0);
	m_resize.Add(IDC_PATH_LOCAL, 0, 0, 50, 0);
	m_resize.Add(IDC_TREE_LOCAL, 0, 0, 0, 100);
	m_resize.Add(IDC_LIST_LOCAL_FAVORITE, 0, 100, 0, 0);
	m_resize.Add(IDC_SPLITTER_LOCAL_FAVORITE, 0, 100, 0, 0);
	m_resize.Add(IDC_SPLITTER_LEFT, 0, 0, 0, 100);
	m_resize.Add(IDC_LIST_LOCAL, 0, 0, 50, 100);
	m_resize.Add(IDC_SLIDER_LOCAL_DISK_SPACE, 0, 100, 0, 0);
	m_resize.Add(IDC_STATIC_LOCAL_DISK_SPACE, 0, 100, 0, 0);
	m_resize.Add(IDC_STATIC_COUNT_LOCAL, 0, 100, 50, 0);

	//m_resize.Add(IDC_SPLITTER_CENTER, 50, 0, 0, 50);
	//m_resize.Add(IDC_SPLITTER_CENTER2, 50, 50, 0, 50);

	m_resize.Add(IDC_STATIC_REMOTE, 50, 0, 0, 0);
	m_resize.Add(IDC_PATH_REMOTE, 50, 0, 50, 0);
	m_resize.Add(IDC_TREE_REMOTE, 50, 0, 0, 100);
	m_resize.Add(IDC_LIST_REMOTE_FAVORITE, 50, 100, 0, 0);
	m_resize.Add(IDC_SPLITTER_REMOTE_FAVORITE, 50, 100, 0, 0);
	m_resize.Add(IDC_SPLITTER_RIGHT, 50, 0, 0, 100);
	m_resize.Add(IDC_LIST_REMOTE, 50, 0, 50, 100);
	m_resize.Add(IDC_SLIDER_REMOTE_DISK_SPACE, 50, 100, 0, 0);
	m_resize.Add(IDC_STATIC_REMOTE_DISK_SPACE, 50, 100, 0, 0);
	m_resize.Add(IDC_STATIC_COUNT_REMOTE, 50, 100, 50, 0);

	m_resize.Add(IDC_CHECK_CLOSE_AFTER_ALL, 100, 0, 0, 0);
	m_resize.Add(IDC_BUTTON_LOCAL_TO_REMOTE, 50, 50, 0, 0);
	m_resize.Add(IDC_BUTTON_REMOTE_TO_LOCAL, 50, 50, 0, 0);

	//init_splitter();
	//왼쪽 트리와 리스트 스플리터
	m_splitter_left.SetType(CControlSplitter::CS_VERT);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_STATIC_LOCAL);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_TREE_LOCAL);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_LIST_LOCAL_FAVORITE);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_SLIDER_LOCAL_DISK_SPACE);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_PATH_LOCAL);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_LIST_LOCAL);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_STATIC_COUNT_LOCAL);

	//왼쪽 트리와 즐겨찾기 리스트 스플리터
	m_splitter_local_favorite.SetType(CControlSplitter::CS_HORZ);
	m_splitter_local_favorite.AddToTopOrLeftCtrls(IDC_TREE_LOCAL);
	m_splitter_local_favorite.AddToBottomOrRightCtrls(IDC_LIST_LOCAL_FAVORITE);


	//오른쪽 트리와 리스트 스플리터
	m_splitter_right.SetType(CControlSplitter::CS_VERT);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_STATIC_REMOTE);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_TREE_REMOTE);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_LIST_REMOTE_FAVORITE);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_SLIDER_REMOTE_DISK_SPACE);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_PATH_REMOTE);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_LIST_REMOTE);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_STATIC_COUNT_REMOTE);

	//오른쪽 트리와 즐겨찾기 리스트 스플리터
	m_splitter_remote_favorite.SetType(CControlSplitter::CS_HORZ);
	m_splitter_remote_favorite.AddToTopOrLeftCtrls(IDC_TREE_REMOTE);
	m_splitter_remote_favorite.AddToBottomOrRightCtrls(IDC_LIST_REMOTE_FAVORITE);

	/*
	//중앙 splitter
	m_splitter_center.SetType(CControlSplitter::CS_VERT);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_PATH_LOCAL);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_LIST_LOCAL);
	//m_splitter_center.AddToTopOrLeftCtrls(IDC_PROGRESS_SPACE_LOCAL);
	m_splitter_center.AddToTopOrLeftCtrls(IDC_STATIC_COUNT_LOCAL, SPF_RIGHT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_STATIC_REMOTE, SPF_LEFT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_PATH_REMOTE);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_TREE_REMOTE, SPF_LEFT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_LIST_REMOTE_FAVORITE, SPF_LEFT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_SPLITTER_RIGHT, SPF_LEFT);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_LIST_REMOTE);
	m_splitter_center.AddToBottomOrRightCtrls(IDC_PROGRESS_SPACE_REMOTE, SPF_LEFT);

	//splitter와 함께 움직이는 컨트롤이라면 양쪽 다 넣어줘야 한다.
	m_splitter_center.AddToBoth(IDC_BUTTON_LOCAL_TO_REMOTE);
	m_splitter_center.AddToBoth(IDC_BUTTON_REMOTE_TO_LOCAL);

	//중앙 보내기/받기 버튼과의 간섭때문에 중앙 스플리터를 위 아래로 분리했으므로 이 또한 함께 움직여야 한다.
	m_splitter_center.AddToBoth(IDC_SPLITTER_CENTER2);

	//중앙 splitter
	m_splitter_center2.SetType(CControlSplitter::CS_VERT);
	m_splitter_center2.AddToTopOrLeftCtrls(IDC_PATH_LOCAL);
	m_splitter_center2.AddToTopOrLeftCtrls(IDC_LIST_LOCAL);
	//m_splitter_center2.AddToTopOrLeftCtrls(IDC_PROGRESS_SPACE_LOCAL);
	m_splitter_center2.AddToTopOrLeftCtrls(IDC_STATIC_COUNT_LOCAL, SPF_RIGHT);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_STATIC_REMOTE, SPF_LEFT);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_PATH_REMOTE);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_TREE_REMOTE, SPF_LEFT);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_LIST_REMOTE_FAVORITE, SPF_LEFT);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_SPLITTER_RIGHT, SPF_LEFT);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_LIST_REMOTE);
	m_splitter_center2.AddToBottomOrRightCtrls(IDC_PROGRESS_SPACE_REMOTE, SPF_LEFT);

	//splitter와 함께 움직인다면 양쪽 다 넣어줘야 한다.
	m_splitter_center2.AddToBoth(IDC_BUTTON_LOCAL_TO_REMOTE);
	m_splitter_center2.AddToBoth(IDC_BUTTON_REMOTE_TO_LOCAL);

	//중앙 보내기/받기 버튼과의 간섭때문에 중앙 스플리터를 위 아래로 분리했으므로 이 또한 함께 움직여야 한다.
	m_splitter_center2.AddToBoth(IDC_SPLITTER_CENTER);
	*/

	//이 코드를 넣어야 작업표시줄에서 클릭하여 minimize, restore된다.
	//작업표시줄에서 해당 앱을 shift+우클릭하여 SYSMENU를 표시할 수 있다.
	//또한 CResizeCtrl을 이용하면 resize할 때 모든 컨트롤들의 레이아웃을 자동으로 맞춰주는데
	//아래 코드를 사용하지 않으면 타이틀바가 없는 dlg는 상단에 흰색 여백 공간이 생기는 부작용이 생긴다.
	SetWindowLong(m_hWnd, GWL_STYLE, WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN);

	set_titlebar_height(32);
	m_sys_buttons.set_button_width(44);
	set_back_color(Gdiplus::Color::White);
	set_title(_T("LinkMeMine File Manager"));
	set_title_bold(true);
	set_font_size(10);
	set_titlebar_text_color(Gdiplus::Color::White);
	set_titlebar_back_color(gRGB(59, 70, 92));
	set_system_buttons(SC_MINIMIZE, SC_MAXIMIZE, SC_CLOSE);
	m_sys_buttons.set_back_hover_color(get_color(gRGB(59, 70, 92), 32));

	m_static_local.set_header_images(IDB_LOCAL_PC1);
	m_static_remote.set_header_images(IDB_REMOTE_PC1);
	m_static_local.set_back_color(Gdiplus::Color::White);
	m_static_remote.set_back_color(Gdiplus::Color::White);
	m_static_local.set_font_bold();
	m_static_remote.set_font_bold();

	m_button_local_to_remote.add_image(IDB_ARROW_LEFT_TO_RIGHT);
	m_button_remote_to_local.add_image(IDB_ARROW_RIGHT_TO_LEFT);
	m_button_local_to_remote.back_color(Gdiplus::Color::White);
	m_button_remote_to_local.back_color(Gdiplus::Color::White);

	//for test
	//while (get_process_running_count(_T("nFTDClient.exe")) > 0)
	//	kill_process_by_fullpath(_T("nFTDClient.exe"));

	m_slider_local_disk_space.set_style(CSCSliderCtrl::style_progress);
	m_slider_local_disk_space.set_track_color(RGB(36, 160, 212), RGB(230, 230, 230));
	m_slider_local_disk_space.set_text_color(dimgray);
	m_slider_local_disk_space.set_text_style(CSCSliderCtrl::text_style_user_defined);
	m_slider_local_disk_space.draw_progress_border();
	m_slider_local_disk_space.SetPos(0);
	m_static_local_disk_space.set_back_color(Gdiplus::Color::White);
	m_static_local_disk_space.set_font_size(9);

	m_slider_remote_disk_space.set_style(CSCSliderCtrl::style_progress);
	m_slider_remote_disk_space.set_track_color(RGB(36, 160, 212), RGB(230, 230, 230));
	m_slider_remote_disk_space.set_text_color(dimgray);
	m_slider_remote_disk_space.set_text_style(CSCSliderCtrl::text_style_user_defined);
	m_slider_remote_disk_space.draw_progress_border();
	m_slider_remote_disk_space.SetPos(0);
	m_static_remote_disk_space.set_back_color(Gdiplus::Color::White);
	m_static_remote_disk_space.set_font_size(9);

	m_check_close_after_all.SetWindowText(_S(IDS_CLOSE_AFTER_TRANSFER));
	m_check_close_after_all.text_color(Gdiplus::Color::White);
	m_check_close_after_all.back_color(gRGB(59, 70, 92), false);
	m_check_close_after_all.use_hover(false);
	m_check_close_after_all.set_font_bold();
	int auto_close = theApp.GetProfileInt(_T("setting"), _T("auto close after all"), BST_CHECKED);
	m_check_close_after_all.SetCheck(auto_close);

	//for test
	if (get_process_running_count(get_exe_directory() + _T("\\FileTransferTest.exe")) == 0)
	{
		CString my_ip = get_my_ip();
		if (my_ip == CString(__targv[4]))
		{
			//대상 IP가 my_ip와 같고 실행중인 nFTDClient.exe가 없다면 실행시켜준다.
			//만약 nFTDClient.exe도 trace mode로 돌려야한다면 nFTDClient.exe를 먼저 trace mode로 실행시켜서 테스트해야 한다.
			if (get_process_running_count(get_exe_directory() + _T("\\nFTDClient.exe")) == 0)
				ShellExecute(NULL, _T("open"), get_exe_directory() + _T("\\nFTDClient.exe"), _T("-l 443"), 0, SW_SHOWNORMAL);
		}
	}

	init_treectrl();
	init_listctrl();
	init_pathctrl();
	init_shadow();
	laod_favorite();

	RestoreWindowPosition(&theApp, this);

	init_progressDlg();


	//20241212 scpark thread_connect()에서 접속 시도 후 initialize()를 통해 remote의 CPathCtrl, CTreeCtrl, CListCtrl등을 표시하는데
	//MFC 관련 오류가 발생한다. std::thread에서도 컨트롤에 대한 일반적인 액션들은 대부분 가능하나 생성과 관련된 뭔가 문제를 일으키는 듯 하다.
	//따라서 connection만 thread로 돌리고 connected후에 컨트롤을 초기화하는 코드는 thread밖에서 처리한다.
	std::thread t(&CnFTDServerDlg::thread_connect, this);
	t.detach();
	//thread_connect();

	SetTimer(timer_init_remote_controls, 1000, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CnFTDServerDlg::init_treectrl()
{
	m_tree_local.set_use_drag_and_drop(true);
	m_tree_local.set_as_shell_treectrl(&theApp.m_shell_imagelist, true);
	m_tree_local.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
	//m_tree_local.set_path(_T("c:\\"));

}

void CnFTDServerDlg::init_listctrl()
{
	m_list_local.set_as_shell_listctrl(&theApp.m_shell_imagelist, true);
	m_list_local.set_use_drag_and_drop(true);
	m_list_local.load_column_width(&theApp, _T("list local"));
	//m_list_local.set_path(_T("d:\\"));
	m_list_local.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
}

void CnFTDServerDlg::init_pathctrl()
{
	m_path_local.set_shell_imagelist(&theApp.m_shell_imagelist);
	//m_path_local.back_color(Gdiplus::Color::Bisque);
}

void CnFTDServerDlg::init_shadow()
{
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_shadow.Create(GetSafeHwnd());
	m_shadow.SetSize(14);	// -19 ~ 19
	m_shadow.SetSharpness(19);	// 0 ~ 19
	m_shadow.SetDarkness(128);	// 0 ~ 254
	m_shadow.SetPosition(0, 0);	// -19 ~ 19
	m_shadow.SetColor(RGB(0, 0, 0));
}

void CnFTDServerDlg::init_progressDlg()
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

void CnFTDServerDlg::laod_favorite()
{
	int i;
	CString headings;
	headings.Format(_T("폴더,40;경로,100"));

	m_list_local_favorite.set_headings(headings);
	m_list_local_favorite.load_column_width(&theApp, _T("list local favorite"));
	m_list_remote_favorite.set_headings(headings);
	m_list_remote_favorite.load_column_width(&theApp, _T("list remote favorite"));

	CString favorite_ini;

	favorite_ini.Format(_T("%s\\favorite.ini"), get_exe_directory());
	EZIni ini(favorite_ini);

	//local
	CString str = ini[_T("LOCAL")][_T("FAVORITE")];

	int index;

	std::deque<CString> dq;
	get_token_string(str, dq, _T(";"), false);

	for (i = 0; i < dq.size(); i++)
	{
		favorite_cmd(favorite_add, SERVER_SIDE, dq[i]);
		//index = m_list_local_favorite.insert_item(-1, get_part(dq[i], fn_leaf_folder));
		//m_list_local_favorite.set_text(index, 1, dq[i]);
	}

	//remote. __targv[4] = IP, __targv[7] = device_id
	str = ini[__targv[4]][__targv[7]];

	dq.clear();
	get_token_string(str, dq, _T(";"), false);

	for (i = 0; i < dq.size(); i++)
	{
		favorite_cmd(favorite_add, CLIENT_SIDE, dq[i]);
		//index = m_list_remote_favorite.insert_item(-1, get_part(dq[i], fn_leaf_folder));
		//m_list_remote_favorite.set_text(index, 1, dq[i]);
	}
}

void CnFTDServerDlg::save_favorite()
{
	CString favorite_ini;

	favorite_ini.Format(_T("%s\\favorite.ini"), get_exe_directory());
	EZIni ini(favorite_ini);

	int i;
	CString str;

	//local
	for (i = 0; i < m_list_local_favorite.size(); i++)
		str = str + m_list_local_favorite.get_text(i, 1) + _T(";");

	ini[_T("LOCAL")][_T("FAVORITE")] = str;


	//remote
	str.Empty();

	for (i = 0; i < m_list_remote_favorite.size(); i++)
		str = str + m_list_remote_favorite.get_text(i, 1) + _T(";");

	ini[__targv[4]][__targv[7]] = str;
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
		CSCThemeDlg::OnSysCommand(nID, lParam);
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
		CSCThemeDlg::OnPaint();
		return;

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
		draw_rectangle(&dc, rc, Gdiplus::Color::DarkBlue);

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
	CSCThemeDlg::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this);
}


void CnFTDServerDlg::OnBnClickedOk()
{
	//m_ServerManager.refresh();
}


void CnFTDServerDlg::OnBnClickedCancel()
{
	//접속시도중인 상태에서 종료 버튼 클릭 시 noTopMost로 변경해줘야 한다.
	if (m_progressDlg.IsWindowVisible())
		m_progressDlg.SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMessageDlg	dlg(this, _S(IDS_CONFIRM_EXIT), MB_OKCANCEL);
	if (dlg.DoModal() == IDCANCEL)
		return;

	kill_process_by_fullpath(get_exe_directory() + _T("\\nFTDClient.exe"));
	SaveLocalLastPath();

	m_list_local.save_column_width(&theApp, _T("list local"));
	m_list_local_favorite.save_column_width(&theApp, _T("list local favorite"));
	m_list_remote.save_column_width(&theApp, _T("list remote"));
	m_list_remote_favorite.save_column_width(&theApp, _T("list remote favorite"));

	save_favorite();

	CDialogEx::OnCancel();
}


BOOL CnFTDServerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (IsZoomed())
		return CSCThemeDlg::OnSetCursor(pWnd, nHitTest, message);

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


	if (m_sys_buttons.m_hWnd == NULL)
		return;

	CRect rc;
	GetClientRect(rc);
	m_sys_buttons.adjust(rc.top, rc.right);
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
	//if (clock() - t0 < 1000)
	//	std::this_thread::sleep_for(std::chrono::milliseconds(500));
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

	m_tree_remote.set_as_shell_treectrl(&theApp.m_shell_imagelist, false);
	m_tree_remote.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);

	m_list_remote.set_as_shell_listctrl(&theApp.m_shell_imagelist, false);
	m_list_remote.set_use_drag_and_drop(true);
	m_list_remote.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
	m_list_remote.load_column_width(&theApp, _T("list remote"));

	m_path_remote.set_shell_imagelist(&theApp.m_shell_imagelist);
	m_path_remote.set_is_local_device(false);
	//m_path_remote.back_color(Gdiplus::Color::Turquoise);

	//real path로 변환하여 실제 존재하는 경로가 아니라면 내 PC를 선택하고
	CString path = convert_special_folder_to_real_path(GetLocalLastPath());
	if (path.IsEmpty() || !PathFileExists(path))
		path = get_known_folder(CSIDL_DRIVES);

	//만약 real_path가 "C:\Users\scpark\Desktop"이라도 트리에서는 "바탕 화면" 항목이 선택되야 한다.

	logWrite(_T("local path = %s"), path);
	change_directory(path, SERVER_SIDE);

	path = GetRemoteLastPath();
	//if (path.IsEmpty() || !PathFileExists(path))
	//	path = get_known_folder(CSIDL_DRIVES);

	//logWrite(_T("remote last path = %s"), path);
	//change_directory(path, CLIENT_SIDE);

	if (path != _T("") && path != _T("\\"))
	{
		logWrite(_T("remote path = %s"), path);
		if (!change_directory(path, CLIENT_SIDE))
		{
			SetDefaultPathToDesktop(1); // 실패 시 바탕화면으로 설정
		}
	}
	else
	{
		logWrite(_T("remote path = Desktop"));
		SetDefaultPathToDesktop(1);	// Remote 초기 경로를 바탕화면으로 설정
	}

	//즐겨찾기로 등록된 폴더들이 유효한지 검사
	SetTimer(timer_check_favorites, 1000, NULL);
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
	if (pMsg->message == WM_KEYDOWN)
	{
		TRACE(_T("keydown on CnFTDServerDlg\n"));
		switch (pMsg->wParam)
		{
			case VK_RETURN :
				OnBnClickedOk();
				return TRUE;
			case VK_DELETE :
				if (GetFocus() == &m_list_local_favorite)
				{
					favorite_cmd(favorite_delete, SERVER_SIDE);
					return TRUE;
				}
				else if (GetFocus() == &m_list_remote_favorite)
				{
					favorite_cmd(favorite_delete, CLIENT_SIDE);
					return TRUE;
				}

				file_command(file_cmd_delete);
				return TRUE;
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

		if (m_dstSide == SERVER_SIDE)
			m_transfer_to = convert_special_folder_to_real_path(dropped_path, m_list_local.get_shell_imagelist(), 0);
		else
			m_transfer_to = convert_special_folder_to_real_path(dropped_path, m_list_remote.get_shell_imagelist(), 1);

		for (int i = 0; i < dq.size(); i++)
		{
			if (m_srcSide == SERVER_SIDE)
			{
				m_transfer_list.push_back(pDragListCtrl->get_file_data(dq[i]));
			}
			else
			{
				m_transfer_list.push_back(pDragListCtrl->get_file_data(dq[i]));
			}
			TRACE(_T("dragged src %d = %s (%s)\n"), i, pDragListCtrl->get_text(dq[i], CVtListCtrlEx::col_filename), m_transfer_list.back().cFileName);
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

	refresh_selection_status(&m_list_local);
	refresh_disk_usage(false);
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
		//빈 공간을 더블클릭한 경우는 상위 폴더로의 이동으로 동작한다.
		if (index < 0)
		{
			new_path = GetParentDirectory(m_list_local.get_path());
			change_directory(new_path, SERVER_SIDE);
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
			else
			{
				//파일일 경우는 스킵? 전송?
			}
		}
	}

	*pResult = 0;
}

void CnFTDServerDlg::OnNMDblclkListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = pNMItemActivate->iItem;

	//빈 공간을 더블클릭한 경우는 상위 폴더로의 이동으로 동작한다.
	if (index < 0)
	{
		CString new_path = GetParentDirectory(m_list_remote.get_path());
		change_directory(new_path, CLIENT_SIDE);
	}
	else
	{
		//is a file? just return? transfer?
		if (m_list_remote.get_text(index, CVtListCtrlEx::col_filesize).IsEmpty() == false)
			return;

		//is folder?
		change_directory(m_remoteCurrentPath + _T("\\") + m_list_remote.get_text(index, CVtListCtrlEx::col_filename), CLIENT_SIDE);
	}

	*pResult = 0;
}

BOOL CnFTDServerDlg::change_directory(CString path, DWORD dwSide)
{
	//logWrite(_T("path = %s"), path);

	int i;
	BOOL result = FALSE;

	if (dwSide == SERVER_SIDE)
	{
		m_path_local.set_path(path);
		m_tree_local.set_path(path, false);
		m_list_local.set_path(path);

		refresh_selection_status(&m_list_local);
		refresh_disk_usage(false);
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


		result = m_ServerManager.change_directory(path, dwSide, false);
		if (result)
		{
			//path가 내 PC, 바탕 화면, 문서, 로컬 디스크(C:) 등 모두 그대로 전달한다.
			m_remoteCurrentPath = path;
			SaveRemoteLastPath();

			std::deque<WIN32_FIND_DATA> dq;

			//리스트 파일목록 갱신
			m_list_remote.delete_all_items();
			//m_ServerManager.refresh_list(&dq, false);
			m_ServerManager.get_filelist(path, &dq, false);

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

			refresh_selection_status(&m_list_remote);
			refresh_disk_usage(true);
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
	m_theme.cr_text = Gdiplus::Color::DimGray;
	m_theme.cr_back = Gdiplus::Color::White;

	m_tree_local.set_color_theme(theme);
	m_list_local.set_color_theme(theme);
	m_tree_remote.set_color_theme(theme);
	m_list_remote.set_color_theme(theme);

	//m_sys_buttons.set_back_color(m_theme.cr_back);

	m_static_count_local.set_color(m_theme.cr_text, m_theme.cr_back);
	m_static_count_remote.set_color(m_theme.cr_text, m_theme.cr_back);
}

//보내기 버튼 or 받기 버튼 or drag&drop으로 전송을 시작한다.
//전송 조건으로 아래 값들이 채워진 후 호출되어야 한다.
//m_transfer_list	: fullpath 전송 목록
//m_srcSide			: 송신측
//m_dstSide			: 수신측
//m_transfer_from	: 송신 폴더
//m_transfer_to		: 수신 폴더
void CnFTDServerDlg::file_transfer()
{
	if (m_transfer_list.size() == 0)
		return;

	//m_transfer_from, m_transfer_to의 끝에 '\\'가 있을 경우의 보정.
	if (m_transfer_from.GetLength() > 3 && m_transfer_from.Right(1) == '\\')
		truncate(m_transfer_from, 1);
	if (m_transfer_to.GetLength() > 3 && m_transfer_to.Right(1) == '\\')
		truncate(m_transfer_to, 1);

	//트리로 drop된 경우 list가 해당 폴더를 표시하고 있지 않다면 change_directory()해준다.
	if ((m_dstSide == SERVER_SIDE && m_list_local.get_path() != m_transfer_to) ||
		(m_dstSide == CLIENT_SIDE && m_list_remote.get_path() != m_transfer_to))
		change_directory(m_transfer_to, m_dstSide);

	//l to l, l to r, r to l, r to l 4가지 모두 나눠서 처리?? 우선 ltr, rtl 2가지만 고려한다.
	//if (m_dwSide == SERVER_SIDE && target == SERVER_SIDE)
	//{
	//	return;
	//}
	//else if (m_dwSide == CLIENT_SIDE && target == CLIENT_SIDE)
	//{
	//	return;
	//}

	//std::deque<WIN32_FIND_DATA> filelist;

	//전송할 파일목록을 filelist에 채운다.
	//이 목록을 CnFTDFileTransferDialog에 전달하고
	//그 dlg의 thread에서 전체 파일 목록을 재구성한 후 순차적으로 전송을 시작한다.
	//추후에는 multithread로 파일을 전송하는 방법 또한 생각해야 한다.
	//for (int i = 0; i < m_transfer_list.size(); i++)
	{
		/*
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
		*/
	}

	CnFTDFileTransferDialog m_FileTransferDlg(this);

	CVtListCtrlEx* pListFile;
	ULARGE_INTEGER* pulDiskSpace;

	if (m_srcSide == SERVER_SIDE)
	{
		pListFile = &m_list_local;
		pulDiskSpace = &m_ulClientDiskSpace;

		//서버에서 서버로 전송할 경우
		if (m_srcSide == m_dstSide)
		{
			//from이 to와 같거나
			if (m_transfer_from == m_transfer_to)
			{
				TRACE(_T("same folder. skip\n"));
				return;
			}

			//to의 바로 위 parent라면 리턴
			if (m_transfer_from == GetParentDirectory(m_transfer_to))
			{
				TRACE(_T("대상 폴더가 원본 폴더와 같습니다. skip.\n"));
				return;
			}

			//폴더가 다르다면 move로 동작한다.
		}
	}
	else
	{
		pListFile = &m_list_remote;
		pulDiskSpace = &m_ulServerDiskSpace;
		m_transfer_from = convert_special_folder_to_real_path(m_remoteCurrentPath, m_list_remote.get_shell_imagelist(), 1);

		//remote에서 remote로 전송할 경우
		if (m_srcSide == m_dstSide)
		{
			//from이 to와 같거나
			if (m_transfer_from == m_transfer_to)
			{
				TRACE(_T("same folder. skip"));
				return;
			}

			//to의 바로 위 parent라면 리턴
			if (m_transfer_from == GetParentDirectory(m_transfer_to))
			{
				TRACE(_T("대상 폴더가 원본 폴더와 같습니다. skip"));
				return;
			}

			//폴더가 다르다면 move로 동작한다.
		}
	}

	TRACE(_T("src mtime = %s\n"), get_file_time_str(m_transfer_list[0].ftLastWriteTime));

	if (m_FileTransferDlg.FileTransferInitalize(&m_ServerManager, &m_transfer_list, pulDiskSpace,
												m_srcSide, m_dstSide, m_transfer_from, m_transfer_to, m_check_close_after_all.GetCheck()))
	{
		m_FileTransferDlg.DoModal();
		
		//전송이 모두 끝나면 해당 폴더를 refresh해준다.
		change_directory(m_transfer_to, m_dstSide);
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

void CnFTDServerDlg::add_transfered_file_to_list(int dstSide, WIN32_FIND_DATA data)
{
	CVtListCtrlEx* plist = (dstSide == SERVER_SIDE ? &m_list_local : &m_list_remote);
	plist->insert_item(-1, data, true, true);
}


void CnFTDServerDlg::OnNMRClickTreeLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CnFTDServerDlg::OnNMRClickTreeRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CnFTDServerDlg::OnNMRClickListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = pNMItemActivate->iItem;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_LIST_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	int index = favorite_cmd(favorite_find, SERVER_SIDE, m_list_local.get_selected_path());
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_FAVORITE, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_FAVORITE, (index >= 0 ? _S(IDS_FAVORITE_REMOVE) : _S(IDS_FAVORITE_ADD)) + _T("(&F)"));

	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_SEND, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_SEND, _S(IDS_TRANSFER_START) + _T("(&S)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_OPEN, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_OPEN, _S(IDS_OPEN) + _T("(&O)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_REFRESH, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_REFRESH, _S(IDS_REFRESH) + _T("\tF5"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_NEW_FOLDER, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_NEW_FOLDER, _S(IDS_NEW_FOLDER) + _T("(&N)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_DELETE, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_DELETE, _S(IDS_DELETE) + _T("(&D)") + _T("\tDel"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_RENAME, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_RENAME, _S(IDS_RENAME) + _T("(&M)") + _T("\tF2"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_SELECT_ALL, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_SELECT_ALL, _S(IDS_SELECT_ALL) + _T("(&A)") + _T("\tCtrl+A"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_PROPERTY, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_PROPERTY, _S(IDS_PROPERTY) + _T("(&R)"));

	//선택된 항목이 없을 경우
	if (item == -1)
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_SEND, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_OPEN, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_RENAME, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_PROPERTY, MF_DISABLED);
	}

	//보호된 파일/폴더일 경우
	if (item >= 0 && is_protected(m_list_local.get_path(item), m_list_local.get_shell_imagelist(), 0))
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_SEND, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_RENAME, MF_DISABLED);
	}

	//멀티 선택일 경우는 "열기" 명령을 수행하면 맨 첫 아이템에 대한 "열기"를 수행하지만 좀 애매하다. 우선 제외시킨다.
	if (m_list_local.GetSelectedCount() > 1)
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_OPEN, MF_DISABLED);
	}

	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CnFTDServerDlg::OnNMRClickListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = pNMItemActivate->iItem;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_LIST_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	int index = favorite_cmd(favorite_find, CLIENT_SIDE, m_list_remote.get_selected_path());
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_FAVORITE, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_FAVORITE, (index >= 0 ? _S(IDS_FAVORITE_REMOVE) : _S(IDS_FAVORITE_ADD)) + _T("(&F)"));

	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_SEND, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_SEND, _S(IDS_TRANSFER_START) + _T("(&S)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_OPEN, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_OPEN, _S(IDS_OPEN) + _T("(&O)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_REFRESH, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_REFRESH, _S(IDS_REFRESH) + _T("\tF5"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_NEW_FOLDER, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_NEW_FOLDER, _S(IDS_NEW_FOLDER) + _T("(&N)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_DELETE, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_DELETE, _S(IDS_DELETE) + _T("(&D)") + _T("\tDel"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_RENAME, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_RENAME, _S(IDS_RENAME) + _T("(&M)") + _T("\tF2"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_SELECT_ALL, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_SELECT_ALL, _S(IDS_SELECT_ALL) + _T("(&A)") + _T("\tCtrl+A"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_PROPERTY, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_PROPERTY, _S(IDS_PROPERTY) + _T("(&R)"));

	//선택된 항목이 없을 경우
	if (item == -1)
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_SEND, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_RENAME, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_OPEN, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_PROPERTY, MF_DISABLED);
	}

	//보호된 파일/폴더일 경우
	if (item >= 0 && is_protected(m_list_remote.get_path(item), m_list_remote.get_shell_imagelist(), 1))
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_SEND, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_RENAME, MF_DISABLED);
	}

	//멀티 선택일 경우는 "열기" 명령을 수행하면 맨 첫 아이템에 대한 "열기"를 수행하지만 좀 애매하다. 우선 제외시킨다.
	if (m_list_remote.GetSelectedCount() > 1)
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_OPEN, MF_DISABLED);
	}

	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CnFTDServerDlg::OnListContextMenuSend()
{
	m_srcSide = (GetFocus() == &m_list_remote);
	m_dstSide = !m_srcSide;

	if (m_srcSide == SERVER_SIDE)
	{
		m_list_local.get_selected_items(&m_transfer_list, true);
		m_transfer_from = m_list_local.get_path();
		m_transfer_to = m_list_remote.get_path();
	}
	else
	{
		m_list_remote.get_selected_items(&m_transfer_list, true);
		m_transfer_from = m_list_remote.get_path();
		m_transfer_to = m_list_local.get_path();
	}

	TRACE(_T("m_srcSide = %d, send %d items from %s to %s\n"), m_srcSide, m_transfer_list.size(), m_transfer_from, m_transfer_to);
	file_transfer();
}


void CnFTDServerDlg::OnListContextMenuNewFolder()
{
	file_command(file_cmd_new_folder);
}


void CnFTDServerDlg::OnListContextMenuRename()
{
	file_command(file_cmd_rename);
}


void CnFTDServerDlg::OnListContextMenuDelete()
{
	file_command(file_cmd_delete);
}


void CnFTDServerDlg::OnListContextMenuRefresh()
{
	CVtListCtrlEx* plist = ((GetFocus() == &m_list_local ? &m_list_local : &m_list_remote));
	plist->refresh_list();
}


void CnFTDServerDlg::OnListContextMenuSelectAll()
{
	CVtListCtrlEx* plist = ((GetFocus() == &m_list_local ? &m_list_local : &m_list_remote));
	plist->select_item(-1);
}


void CnFTDServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_init_remote_controls)
	{
		if (m_ServerManager.is_connected() == false)
			return;

		KillTimer(timer_init_remote_controls);

		initialize();
		Invalidate();

		m_show_main_ui = true;
		m_progressDlg.ShowWindow(SW_HIDE);
	}
	else if (nIDEvent == timer_check_favorites)
	{
		KillTimer(timer_check_favorites);

		int i;
		CString path;

		for (i = 0; i < m_list_local_favorite.size(); i++)
		{
			path = m_list_local_favorite.get_text(i, 1);
			if (!PathFileExists(path))
				m_list_local_favorite.set_text_color(i, -1, Gdiplus::Color::Gray);
		}

		for (i = 0; i < m_list_remote_favorite.size(); i++)
		{
			path = m_list_remote_favorite.get_text(i, 1);

			std::deque<WIN32_FIND_DATA> dq;
			if (!m_ServerManager.get_folderlist(path, &dq, false))
				m_list_remote_favorite.set_text_color(i, -1, Gdiplus::Color::Firebrick);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

bool CnFTDServerDlg::file_command(int cmd, CString param0, CString param1)
{
	bool res = false;

	CVtListCtrlEx* plist;
	
	if (GetFocus() == &m_list_local)
		plist = &m_list_local;
	else if (GetFocus() == &m_list_remote)
		plist = &m_list_remote;
	else
		return false;

	std::deque<int> dq;
	plist->get_selected_items(&dq);
	//CString file;

	if (param0.IsEmpty())
	{
		if (dq.size() == 0)
			param0 = plist->get_path();
		else
			param0.Format(_T("%s\\%s"), plist->get_path(), plist->get_text(dq[0], CVtListCtrlEx::col_filename));
	}

	//이러한 처리를 기존처럼 m_ServerManager의 open_file()함수에서 일괄 처리하도록 구현할 수도 있으나
	//m_ServerManager에서는 CnFTDServerDlg에 있는 tree, list 등을 접근하자면 별도의 처리가 필요하므로
	//local은 그냥 이 클래스에서 처리하고 remote인 경우에만 m_ServerManager의 함수를 호출하여 처리한다.
	if (GetFocus() == &m_list_local)
	{
		switch (cmd)
		{
			case file_cmd_open :
				//폴더인 경우
				if (PathIsDirectory(param0))
				{
					res = change_directory(param0, SERVER_SIDE);
				}
				else
				{
					ShellExecute(NULL, _T("open"), param0, 0, 0, SW_SHOWNORMAL);
					res = true;
				}
				break;
			case file_cmd_open_explorer :
				param0 = m_list_local.get_path();
				ShellExecute(NULL, _T("explore"), param0, 0, 0, SW_SHOWNORMAL);
				break;
			case file_cmd_new_folder :
				//list의 현재 폴더에 새 폴더를 생성한다.
				res = m_list_local.new_folder(_S(IDS_NEW_FOLDER));
				break;
			case file_cmd_rename :
				m_list_local.edit_item(dq[0], CVtListCtrlEx::col_filename);
				res = true;
				break;
			case file_cmd_delete :
				{
					int deleted_count = 0;

					for (auto item : dq)
					{
						//item 위치의 항목을 제거하면 그 뒤 item들의 index는 당겨져야 한다.
						item -= deleted_count;

						res = m_list_local.delete_item(item, true);
						if (res)
						{
							deleted_count++;
						}
					}
				}
				break;
			case file_cmd_property :
				res = show_file_property_window(param0);
				break;
			case file_cmd_favorite :
				{
					//존재하는 항목이면 제거, 없던 항목이면 추가한다.
					int index = favorite_cmd(favorite_find, SERVER_SIDE, m_list_local.get_selected_path());
					if (index < 0)
						favorite_cmd(favorite_add, SERVER_SIDE, m_list_local.get_selected_path());
					else
						favorite_cmd(favorite_delete, SERVER_SIDE, m_list_local.get_selected_path());
				}
				break;
		}
	}
	else
	{
		//논리 경로라면 실제 경로로 변환해준다.
		param0 = convert_special_folder_to_real_path(param0, m_list_remote.get_shell_imagelist(), CLIENT_SIDE);

		switch (cmd)
		{
			case file_cmd_open:
				//폴더인 경우
				if (m_list_remote.get_text(dq[0], CVtListCtrlEx::col_filesize).IsEmpty())
				{
					res = change_directory(param0, CLIENT_SIDE);
				}
				//파일인 경우
				else
				{
					m_ServerManager.m_socket.file_command(file_cmd_open, param0);
					res = true;
				}
				break;
			case file_cmd_open_explorer :
				param0 = m_list_remote.get_path();
				m_ServerManager.m_socket.file_command(file_cmd_open_explorer, param0);
				break;
			case file_cmd_rename:
				m_list_remote.edit_item(dq[0], CVtListCtrlEx::col_filename);
				res = true;
				break;
			case file_cmd_delete:
				{
					int deleted_count = 0;

					for (auto item : dq)
					{
						//item 위치의 항목을 제거하면 그 뒤 item들의 index는 당겨져야 한다.
						item -= deleted_count;

						//remote에서 정상 삭제된 경우에만 리스트에서도 삭제해야 한다.
						res = m_ServerManager.m_socket.file_command(file_cmd_delete, m_list_remote.get_path(item));
						if (res)
						{
							res = m_list_remote.delete_item(item);
							deleted_count++;
						}
					}

					//m_list_remote.refresh_list();
					refresh_disk_usage(true);
					refresh_selection_status(&m_list_remote);
				}
				break;
			case file_cmd_property:
				res = m_ServerManager.m_socket.file_command(file_cmd_property, param0);
				break;
			case file_cmd_favorite:
				{
					//존재하는 항목이면 제거, 없던 항목이면 추가한다.
					int index = favorite_cmd(favorite_find, CLIENT_SIDE, m_list_remote.get_selected_path());
					if (index < 0)
						favorite_cmd(favorite_add, CLIENT_SIDE, m_list_remote.get_selected_path());
					else
						favorite_cmd(favorite_delete, CLIENT_SIDE, m_list_remote.get_selected_path());
				}
				break;
		}
	}

	return res;
}

//목록, 선택 정보가 변경되면 상태표시줄을 갱신한다.
void CnFTDServerDlg::refresh_selection_status(CVtListCtrlEx* plist)
{
	CString str;

	std::deque<WIN32_FIND_DATA> dq;
	plist->get_selected_items(&dq);

	ULARGE_INTEGER total_size;
	total_size.QuadPart = 0;

	//멀티 선택 시 파일/폴더가 혼합되어 선택되면 파일크기를 표시하지 않는다.
	bool folder_included = false;

	for (auto item : dq)
	{
		if (item.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			folder_included = true;

		ULARGE_INTEGER file_size;
		file_size.LowPart = item.nFileSizeLow;
		file_size.HighPart = item.nFileSizeHigh;
		total_size.QuadPart += file_size.QuadPart;
	}

	if (dq.size())
	{
		if (folder_included)
			str.Format(_T("%d%s     %d%s"), plist->size(), _S(IDS_ITEM_COUNT), dq.size(), _S(IDS_ITEM_SELECTED_COUNT));
		else
			str.Format(_T("%d%s     %d%s  %s"), plist->size(), _S(IDS_ITEM_COUNT), dq.size(), _S(IDS_ITEM_SELECTED_COUNT), get_size_str(total_size.QuadPart, -1, 2));
	}
	else
	{
		str.Format(_T("%d%s"), plist->size(), _S(IDS_ITEM_COUNT));
	}

	if (plist == &m_list_local)
		m_static_count_local.set_text(str);
	else if (plist == &m_list_remote)
		m_static_count_remote.set_text(str);
}

void CnFTDServerDlg::refresh_disk_usage(bool is_remote_side)
{
	CVtListCtrlEx* plist = (is_remote_side ? &m_list_remote : &m_list_local);
	CSCSliderCtrl* pslider = (is_remote_side ? &m_slider_remote_disk_space : &m_slider_local_disk_space);
	CSCStatic* pstatic = (is_remote_side ? &m_static_remote_disk_space : &m_static_local_disk_space);

	CString path = convert_special_folder_to_real_path(plist->get_path(), m_list_local.get_shell_imagelist(), is_remote_side);
	
	//내 PC를 선택한 경우는 용량표시는 숨긴다.
	if (!PathFileExists(path))
	{
		pslider->ShowWindow(SW_HIDE);
		pstatic->ShowWindow(SW_HIDE);
		//pslider->SetPos(0);
		//pslider->set_text(_T(""));
		return;
	}

	if (!pslider->IsWindowVisible())
	{
		pslider->ShowWindow(SW_SHOW);
		pstatic->ShowWindow(SW_SHOW);
	}

	CString drive = path.Left(2);

	ULARGE_INTEGER ulFree;// get_disk_free_size(drive);
	ULARGE_INTEGER ulTotal;// get_disk_total_size(drive);

	ulFree.QuadPart = 0;
	ulTotal.QuadPart = 0;

	if (is_remote_side)
	{
		m_ServerManager.m_socket.RemainSpace(&ulFree);
		m_ServerManager.m_socket.TotalSpace(&ulTotal);
		//m_ServerManager.KeepConnection();
		ulFree.QuadPart = ulFree.QuadPart;
		ulTotal.QuadPart = ulTotal.QuadPart;
	}
	else
	{
		ulFree.QuadPart = get_disk_free_size(drive);
		ulTotal.QuadPart = get_disk_total_size(drive);
	}

	double free_ratio = (((double)ulFree.QuadPart / (double)ulTotal.QuadPart)) * 100.0;

	TRACE(_T("%s drive free space ratio = %f\n"), drive, free_ratio);

	//progressbar에는 남은 용량만큼 채우는게 아니라 사용량만큼 채워서 보여준다.
	if (free_ratio < 50.0f)
		pslider->set_active_color(RGB(220 - free_ratio, 41, 42));
	else
		pslider->set_active_color(RGB(36, 160, 212));

	pslider->SetPos(100 - (int)free_ratio);
	pstatic->set_textf(Gdiplus::Color::DimGray, _T("%s 중 %s 사용 가능"), get_size_str(ulTotal.QuadPart, -1), get_size_str(ulFree.QuadPart, -1));
}

void CnFTDServerDlg::OnListContextMenuOpen()
{
	file_command(file_cmd_open);
}

void CnFTDServerDlg::OnListContextMenuOpenExplorer()
{
	file_command(file_cmd_open_explorer);
}

void CnFTDServerDlg::OnListContextMenuProperty()
{
	file_command(file_cmd_property);
}


void CnFTDServerDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	TRACE(_T("context menu\n"));
}


void CnFTDServerDlg::OnLvnItemChangedListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
		refresh_selection_status(&m_list_local);

	*pResult = 0;
}


void CnFTDServerDlg::OnLvnItemChangedListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
		refresh_selection_status(&m_list_remote);

	*pResult = 0;
}


void CnFTDServerDlg::OnBnClickedCheckCloseAfterAll()
{
	theApp.WriteProfileInt(_T("setting"), _T("auto close after all"), m_check_close_after_all.GetCheck());
}


void CnFTDServerDlg::OnBnClickedButtonLocalToRemote()
{
	m_srcSide = SERVER_SIDE;
	m_dstSide = CLIENT_SIDE;

	m_list_local.get_selected_items(&m_transfer_list, true);
	m_transfer_from = m_list_local.get_path();
	m_transfer_to = m_list_remote.get_path();

	TRACE(_T("m_srcSide = %d, send %d items from %s to %s\n"), m_srcSide, m_transfer_list.size(), m_transfer_from, m_transfer_to);
	file_transfer();
}


void CnFTDServerDlg::OnBnClickedButtonRemoteToLocal()
{
	m_srcSide = CLIENT_SIDE;
	m_dstSide = SERVER_SIDE;

	m_list_remote.get_selected_items(&m_transfer_list, true);
	m_transfer_from = m_list_remote.get_path();
	m_transfer_to = m_list_local.get_path();

	TRACE(_T("m_srcSide = %d, send %d items from %s to %s\n"), m_srcSide, m_transfer_list.size(), m_transfer_from, m_transfer_to);
	file_transfer();
}


void CnFTDServerDlg::OnNMDblclkListLocalFavorite(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = pNMItemActivate->iItem;

	if (item >= 0)
	{
		if (!PathFileExists(m_list_local_favorite.get_text(item, 1)))
		{
			m_list_local_favorite.set_text_color(item, -1, Gdiplus::Color::Firebrick);
		}
		else
		{
			change_directory(m_list_local_favorite.get_text(item, 1), SERVER_SIDE);
		}
	}

	*pResult = 0;
}


void CnFTDServerDlg::OnNMDblclkListRemoteFavorite(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = pNMItemActivate->iItem;

	if (item >= 0)
	{
		if (m_ServerManager.is_connected())
		{
			std::deque<WIN32_FIND_DATA> dq;
			bool res = m_ServerManager.get_folderlist(m_list_remote_favorite.get_text(item, 1), &dq, false);
			if (!res)
				m_list_remote_favorite.set_text_color(item, -1, Gdiplus::Color::Firebrick);
			else
				change_directory(m_list_remote_favorite.get_text(item, 1), CLIENT_SIDE);
		}
	}

	*pResult = 0;
}

int CnFTDServerDlg::favorite_cmd(int cmd, int side, CString fullpath)
{
	int i;
	int index;
	CVtListCtrlEx* plist = (side == SERVER_SIDE ? &m_list_local : &m_list_remote);
	CVtListCtrlEx* pfavoritelist = (side == SERVER_SIDE ? &m_list_local_favorite : &m_list_remote_favorite);

	if (cmd == favorite_add)
	{
		if (fullpath.IsEmpty())
			fullpath = plist->get_path();

		fullpath = convert_special_folder_to_real_path(fullpath, &theApp.m_shell_imagelist, (side ? 0 : 1));

		if (fullpath.Right(1) != '\\')
			fullpath += '\\';

		index = pfavoritelist->insert_item(-1, get_part(fullpath, fn_leaf_folder));
		pfavoritelist->set_text(index, 1, fullpath);

		//존재하지 않는 폴더일 경우는 붉은색으로 표시
		bool is_exist = false;
		if (side == SERVER_SIDE)
		{
			is_exist = PathFileExists(fullpath);
		}
		else
		{
			if (m_ServerManager.is_connected())
			{
				std::deque<WIN32_FIND_DATA> dq;
				is_exist = m_ServerManager.get_folderlist(fullpath, &dq, false);
			}
			else
			{
				is_exist = true;
			}
		}

		if (!is_exist)
			pfavoritelist->set_text_color(index, -1, Gdiplus::Color::Red);
	}
	else if (cmd == favorite_delete)
	{
		std::deque<int> dq;

		//즐겨찾기 리스트에서 온 명령이라면 fullpath는 공백이므로 선택항목 인덱스를 구하고
		if (fullpath.IsEmpty())
		{
			pfavoritelist->get_selected_items(&dq);
		}
		//리스트에서 팝업메뉴에 의해 삭제한 경우라면 fullpath가 넘어오므로 즐겨찾기 리스트에서 인덱스를 구한다.
		else
		{
			fullpath = convert_special_folder_to_real_path(fullpath, &theApp.m_shell_imagelist, (side ? 0 : 1));

			if (fullpath.Right(1) != '\\')
				fullpath += '\\';

			index = favorite_cmd(favorite_find, side, fullpath);
			dq.push_back(index);
		}

		for (int i = dq.size() - 1; i >= 0; i--)
		{
			if (dq[i] < 0)
				continue;

			pfavoritelist->delete_item(dq[i]);
		}
	}
	else if (cmd == favorite_find)
	{
		fullpath = convert_special_folder_to_real_path(fullpath, &theApp.m_shell_imagelist, (side ? 0 : 1));

		if (fullpath.Right(1) != '\\')
			fullpath += '\\';

		for (i = 0; i < pfavoritelist->size(); i++)
		{
			if (fullpath.CompareNoCase(pfavoritelist->get_text(i, 1)) == 0)
				return i;
		}
	}

	return -1;
}


void CnFTDServerDlg::OnListContextMenuFavorite()
{
	file_command(file_cmd_favorite);
}


void CnFTDServerDlg::OnFavoriteContextMenuDelete()
{
	int dwSide = (GetFocus() == &m_list_remote_favorite);
	favorite_cmd(favorite_delete, dwSide);
}


void CnFTDServerDlg::OnNMRClickListLocalFavorite(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = pNMItemActivate->iItem;

	//이 메뉴는 선택된 항목이 없을 경우 그냥 리턴한다.
	if (item == -1)
	{
		*pResult = 0;
		return;
	}

	CMenu menu;
	menu.LoadMenu(IDR_MENU_FAVORITE_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	pMenu->ModifyMenu(ID_FAVORITE_CONTEXT_MENU_DELETE, MF_BYCOMMAND, ID_FAVORITE_CONTEXT_MENU_DELETE, _S(IDS_FAVORITE_REMOVE) + _T("(&D)\tDel"));

	//선택된 항목이 없을 경우
	if (item == -1)
		pMenu->EnableMenuItem(ID_FAVORITE_CONTEXT_MENU_DELETE, MF_DISABLED);

	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CnFTDServerDlg::OnNMRClickListRemoteFavorite(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = pNMItemActivate->iItem;

	//이 메뉴는 선택된 항목이 없을 경우 그냥 리턴한다.
	if (item == -1)
	{
		*pResult = 0;
		return;
	}

	CMenu menu;
	menu.LoadMenu(IDR_MENU_FAVORITE_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	pMenu->ModifyMenu(ID_FAVORITE_CONTEXT_MENU_DELETE, MF_BYCOMMAND, ID_FAVORITE_CONTEXT_MENU_DELETE, _S(IDS_FAVORITE_REMOVE) + _T("(&D)\tDel"));

	//선택된 항목이 없을 경우
	if (item == -1)
		pMenu->EnableMenuItem(ID_FAVORITE_CONTEXT_MENU_DELETE, MF_DISABLED);

	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CnFTDServerDlg::OnLvnEndlabelEditListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CnFTDServerDlg::OnLvnEndlabelEditListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int item = pDispInfo->item.iItem;
	int sub_item = pDispInfo->item.iSubItem;

	//remote rename일 경우 실제 파일/폴더를 rename 성공한 후 변경해줘야 한다.
	CString path;
	CString old_name;
	CString new_name;

	path = convert_special_folder_to_real_path(m_list_remote.get_path(), &theApp.m_shell_imagelist, CLIENT_SIDE);
	if (path.GetLength() > 1 && path.Right(1) != '\\')
		path += '\\';

	old_name.Format(_T("%s%s"), path, m_list_remote.get_old_text());
	new_name.Format(_T("%s%s"), path, m_list_remote.get_text(item, sub_item));

	if (!m_ServerManager.m_socket.Rename(old_name, new_name))
	{
		m_list_remote.undo_edit_label();
	}

	*pResult = 0;
}
