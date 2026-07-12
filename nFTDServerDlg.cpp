
// nFTDServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "nFTDServer.h"
#include "nFTDServerDlg.h"
#include "afxdialogex.h"

#include <thread>
#include <memory>

#include "Common/Functions.h"
#include "Common/MemoryDC.h"
#include "Common/ini/EZIni/EZIni.h"

#include "nFTDFileTransferDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//20260705 by claude. 트리/리스트 populate 시 항목 수가 이 값 이하면 progress(로딩바)를 아예 표시하지 않는다. 대부분의 폴더는
//항목이 적어 progress 가 순식간에 나타났다 사라지는 깜빡임만 유발하므로, 시작 시 알려지는 총 개수로 표시 여부를 판단한다.
static const int PROGRESS_SHOW_MIN_COUNT = 200;



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
	DDX_Control(pDX, IDC_SPLITTER_LEFT, m_splitter_left);
	DDX_Control(pDX, IDC_SPLITTER_RIGHT, m_splitter_right);
	DDX_Control(pDX, IDC_STATIC_COUNT_LOCAL, m_static_count_local);
	DDX_Control(pDX, IDC_STATIC_COUNT_REMOTE, m_static_count_remote);
	DDX_Control(pDX, IDC_STATIC_LOCAL, m_static_local);
	DDX_Control(pDX, IDC_STATIC_REMOTE, m_static_remote);
	DDX_Control(pDX, IDC_CHECK_CLOSE_AFTER_ALL, m_check_close_after_all);
	DDX_Control(pDX, IDC_BUTTON_LOCAL_TO_REMOTE, m_button_local_to_remote);
	DDX_Control(pDX, IDC_BUTTON_REMOTE_TO_LOCAL, m_button_remote_to_local);
	DDX_Control(pDX, IDC_LIST_LOCAL_FAVORITE, m_list_local_favorite);
	DDX_Control(pDX, IDC_LIST_REMOTE_FAVORITE, m_list_remote_favorite);
	DDX_Control(pDX, IDC_SPLITTER_LOCAL_FAVORITE, m_splitter_local_favorite);
	DDX_Control(pDX, IDC_SPLITTER_REMOTE_FAVORITE, m_splitter_remote_favorite);
	DDX_Control(pDX, IDC_SLIDER_LOCAL_DISK_SPACE, m_slider_local_disk_space);
	DDX_Control(pDX, IDC_SLIDER_REMOTE_DISK_SPACE, m_slider_remote_disk_space);
	DDX_Control(pDX, IDC_STATIC_LOCAL_DISK_SPACE, m_static_local_disk_space);
	DDX_Control(pDX, IDC_STATIC_REMOTE_DISK_SPACE, m_static_remote_disk_space);
	DDX_Control(pDX, IDC_PROGRESS_LOCAL, m_progress_local);
	DDX_Control(pDX, IDC_PROGRESS_REMOTE, m_progress_remote);
}

BEGIN_MESSAGE_MAP(CnFTDServerDlg, CSCThemeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDOK, &CnFTDServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CnFTDServerDlg::OnBnClickedCancel)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_ENTERSIZEMOVE()
	ON_WM_EXITSIZEMOVE()
	ON_REGISTERED_MESSAGE(Message_CPathCtrl, &CnFTDServerDlg::on_message_CPathCtrl)
	ON_REGISTERED_MESSAGE(Message_CSCListCtrl, &CnFTDServerDlg::on_message_CSCListCtrl)
	ON_REGISTERED_MESSAGE(Message_CSCTreeCtrl, &CnFTDServerDlg::on_message_CSCTreeCtrl)
	ON_REGISTERED_MESSAGE(Message_CControlSplitter, &CnFTDServerDlg::on_message_CControlSplitter)
	ON_REGISTERED_MESSAGE(Message_CSCSystemButtons, &CnFTDServerDlg::on_message_CSCSystemButtons)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LOCAL, &CnFTDServerDlg::OnTvnSelchangedTreeLocal)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_REMOTE, &CnFTDServerDlg::OnTvnSelchangedTreeRemote)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LIST_CONTEXT_MENU_SEND, &CnFTDServerDlg::OnListContextMenuSend)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_NEW_FOLDER, &CnFTDServerDlg::OnListContextMenuNewFolder)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_RENAME, &CnFTDServerDlg::OnListContextMenuRename)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_DELETE, &CnFTDServerDlg::OnListContextMenuDelete)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_REFRESH, &CnFTDServerDlg::OnListContextMenuRefresh)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_SELECT_ALL, &CnFTDServerDlg::OnListContextMenuSelectAll)
	ON_WM_TIMER()
	ON_COMMAND(ID_LIST_CONTEXT_MENU_OPEN, &CnFTDServerDlg::OnListContextMenuOpen)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_PROPERTY, &CnFTDServerDlg::OnListContextMenuProperty)
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
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_LOCAL, &CnFTDServerDlg::OnLvnEndlabelEditListLocal)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_REMOTE, &CnFTDServerDlg::OnLvnEndlabelEditListRemote)
	ON_MESSAGE(MESSAGE_CONNECT_FAIL, &CnFTDServerDlg::on_message)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_FAVORITE, &CnFTDServerDlg::OnTreeContextMenuFavorite)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_REFRESH, &CnFTDServerDlg::OnTreeContextMenuRefresh)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_NEW_FOLDER, &CnFTDServerDlg::OnTreeContextMenuNewFolder)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_PROPERTY, &CnFTDServerDlg::OnTreeContextMenuProperty)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_OPEN_EXPLORER, &CnFTDServerDlg::OnTreeContextMenuOpenExplorer)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_SEND, &CnFTDServerDlg::OnTreeContextMenuSend)
	ON_REGISTERED_MESSAGE(Message_CSCDirWatcher, &CnFTDServerDlg::on_message_CSCDirWatcher)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_DELETE, &CnFTDServerDlg::OnTreeContextMenuDelete)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_RENAME, &CnFTDServerDlg::OnTreeContextMenuRename)
	ON_COMMAND(ID_TREE_CONTEXT_MENU_PATH_TO_CLIPBOARD, &CnFTDServerDlg::OnTreeContextMenuPathToClipboard)
	ON_COMMAND(ID_LIST_CONTEXT_MENU_PATH_TO_CLIPBOARD, &CnFTDServerDlg::OnListContextMenuPathToClipboard)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_LOCAL, &CnFTDServerDlg::OnNMRClickTreeLocal)
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
		//ASSERT(bNameValid);
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

	m_resize.Create(this);
	m_resize.SetMinimumTrackingSize(CSize(900, 600));
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

	set_color_theme(theApp.m_color_theme);
	set_system_buttons(this, SC_MINIMIZE, SC_MAXIMIZE, SC_CLOSE);
	set_titlebar_icon(IDR_MAINFRAME);// , 20, 20);
	//set_draw_border(true, 1, get_color(m_theme.cr_title_back_active, 16));

	m_dir_watcher.init(this);

	m_messagebox.create(this, _T("nFTDServer"));
	m_messagebox.set_color_theme(m_theme.get_color_theme());

	//init_splitter();
	int min_size = 120;

	//왼쪽 트리와 리스트 스플리터
	m_splitter_left.set_type(CControlSplitter::CS_VERT, true, Gdiplus::Color::LightGray);
	m_splitter_left.set_back_color(m_theme.cr_back);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_STATIC_LOCAL, min_size);			//static은 cx만 제한하면 되지만
	m_splitter_left.AddToTopOrLeftCtrls(IDC_TREE_LOCAL, min_size, min_size);	//tree는 cx, cy 둘 다 제한시켜야 한다.
	m_splitter_left.AddToTopOrLeftCtrls(IDC_SPLITTER_LOCAL_FAVORITE);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_LIST_LOCAL_FAVORITE);
	m_splitter_left.AddToTopOrLeftCtrls(IDC_SLIDER_LOCAL_DISK_SPACE);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_PATH_LOCAL);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_LIST_LOCAL, min_size, min_size);
	m_splitter_left.AddToBottomOrRightCtrls(IDC_STATIC_COUNT_LOCAL);

	//왼쪽 트리와 즐겨찾기 리스트 스플리터
	m_splitter_local_favorite.set_type(CControlSplitter::CS_HORZ, true, Gdiplus::Color::LightGray);
	m_splitter_local_favorite.set_back_color(m_theme.cr_back);
	m_splitter_local_favorite.AddToTopOrLeftCtrls(IDC_TREE_LOCAL, min_size, min_size);
	m_splitter_local_favorite.AddToBottomOrRightCtrls(IDC_LIST_LOCAL_FAVORITE, min_size, min_size);


	//오른쪽 트리와 리스트 스플리터
	m_splitter_right.set_type(CControlSplitter::CS_VERT, true, Gdiplus::Color::LightGray);
	m_splitter_right.set_back_color(m_theme.cr_back);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_STATIC_REMOTE, min_size);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_TREE_REMOTE, min_size, min_size);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_SPLITTER_REMOTE_FAVORITE);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_LIST_REMOTE_FAVORITE);
	m_splitter_right.AddToTopOrLeftCtrls(IDC_SLIDER_REMOTE_DISK_SPACE);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_PATH_REMOTE);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_LIST_REMOTE, min_size, min_size);
	m_splitter_right.AddToBottomOrRightCtrls(IDC_STATIC_COUNT_REMOTE);

	//오른쪽 트리와 즐겨찾기 리스트 스플리터
	m_splitter_remote_favorite.set_type(CControlSplitter::CS_HORZ, true, Gdiplus::Color::LightGray);
	m_splitter_remote_favorite.set_back_color(m_theme.cr_back);
	m_splitter_remote_favorite.AddToTopOrLeftCtrls(IDC_TREE_REMOTE, min_size, min_size);
	m_splitter_remote_favorite.AddToBottomOrRightCtrls(IDC_LIST_REMOTE_FAVORITE, min_size, min_size);

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

	//borderless 윈도우 스타일 정규화(WS_CAPTION/WS_BORDER/WS_DLGFRAME 제거 + WS_THICKFRAME/WS_SYSMENU/
	//WS_MINIMIZEBOX/WS_MAXIMIZEBOX 부여 + SWP_FRAMECHANGED 로 NC 재계산)는 위에서 호출한
	//CSCThemeDlg::OnInitDialog() 이 이미 정석대로 처리한다(borderless dialog.md Step 4).
	//따라서 작업표시줄 minimize/restore·shadow·resize 도 base 가 보장한다.
	//여기서는 자식 컨트롤이 많아 깜빡임을 줄이기 위한 WS_CLIPCHILDREN 만 추가한다.
	//(예전엔 SetWindowLong 으로 스타일을 통째로 덮어써 base 의 정규화를 지웠으나, 이제 base 에 위임한다.)
	ModifyStyle(0, WS_CLIPCHILDREN);

	m_static_local.set_header_images(IDB_LOCAL_PC1);
	m_static_local.set_back_color(m_theme.cr_back);
	m_static_local.set_font_weight();
	m_static_local.set_text(_S(NFTD_IDS_AGENT));
	m_static_remote.set_header_images(IDB_REMOTE_PC1);
	m_static_remote.set_back_color(m_theme.cr_back);
	m_static_remote.set_font_weight();
	m_static_remote.set_text(_S(NFTD_IDS_REMOTE_PC));

	m_button_local_to_remote.add_image(IDB_ARROW_LEFT_TO_RIGHT);
	//투명 png 화살표 — add_image 가 m_cr_back 을 비우고 m_transparent=true 로 둔다. 따라서 고정 배경색을 칠하는
	//set_back_color 대신 set_parent_back_color 로 투명 배경 fill 색(부모 테마색)을 지정한다.
	//→ disable 상태에서도 m_cr_back[3](회색) 박스가 칠해지지 않고 이미지만 grayed 되어 표시된다.
	m_button_local_to_remote.set_parent_back_color(m_theme.cr_back);
	m_button_local_to_remote.fit_to_image(false);
	m_button_local_to_remote.draw_drop_shadow(true, 1.8f, 1.6f);
	m_button_local_to_remote.set_down_offset(1, 1);

	m_button_remote_to_local.add_image(IDB_ARROW_RIGHT_TO_LEFT);
	//투명 png 화살표 — 부모 테마색으로 투명 배경 fill (disable 시 회색 박스 방지). 위 local 버튼과 동일.
	m_button_remote_to_local.set_parent_back_color(m_theme.cr_back);
	m_button_remote_to_local.fit_to_image(false);
	m_button_remote_to_local.draw_drop_shadow(true, 1.8f, 1.6f);
	m_button_remote_to_local.set_down_offset(1, 1);

	//전송버튼이 보호 폴더 선택으로 disable 됐을 때 사유 툴팁. tool 텍스트는 refresh_selection_status 에서 동적 갱신.
	//disabled 컨트롤에도 표시되려면 이 툴팁을 parent 가 소유하고 PreTranslateMessage 에서 릴레이해야 한다.
	m_tooltip.Create(this);
	m_tooltip.SetDelayTime(TTDT_INITIAL, 400);
	m_tooltip.SetMaxTipWidth(400);
	m_tooltip.AddTool(&m_button_local_to_remote, _T(""));
	m_tooltip.AddTool(&m_button_remote_to_local, _T(""));
	m_tooltip.Activate(TRUE);

	m_progress_local.set_style(CSCSliderCtrl::style_progress);
	m_progress_local.set_track_height(4);
	m_progress_local.set_text_style(CSCSliderCtrl::text_style_none);
	//m_progress_local.set_inactive_color(GRAY(232));
	m_progress_local.set_track_color(gRGB(36, 160, 212), gRGB(230, 230, 230));

	m_progress_remote.set_style(CSCSliderCtrl::style_progress);
	m_progress_remote.set_track_height(4);
	m_progress_remote.set_text_style(CSCSliderCtrl::text_style_none);
	//m_progress_remote.set_inactive_color(GRAY(232));
	m_progress_remote.set_track_color(gRGB(36, 160, 212), gRGB(230, 230, 230));

	m_slider_local_disk_space.set_style(CSCSliderCtrl::style_progress);
	m_slider_local_disk_space.set_track_height(11);
	m_slider_local_disk_space.set_track_color(gRGB(36, 160, 212), gRGB(230, 230, 230));
	m_slider_local_disk_space.set_text_style(CSCSliderCtrl::text_style_none);
	m_slider_local_disk_space.draw_progress_border();
	m_slider_local_disk_space.SetPos(0);

	m_slider_remote_disk_space.set_style(CSCSliderCtrl::style_progress);
	m_slider_remote_disk_space.set_track_height(11);
	m_slider_remote_disk_space.set_track_color(gRGB(36, 160, 212), gRGB(230, 230, 230));
	m_slider_remote_disk_space.set_text_style(CSCSliderCtrl::text_style_none);
	m_slider_remote_disk_space.draw_progress_border();
	m_slider_remote_disk_space.SetPos(0);

	m_static_local_disk_space.set_color(Gdiplus::Color::DimGray, m_theme.cr_back);
	m_static_remote_disk_space.set_color(Gdiplus::Color::DimGray, m_theme.cr_back);

	m_static_count_local.set_color(Gdiplus::Color::DimGray, m_theme.cr_back);
	m_static_count_remote.set_color(Gdiplus::Color::DimGray, m_theme.cr_back);

	m_check_close_after_all.SetWindowText(_S(IDS_CLOSE_AFTER_TRANSFER));			//전송 완료 후 창 닫기
	//m_check_close_after_all.set_tooltip_text(_S(IDS_CLOSE_ALL_TRANSFER_SUCCESS));	//모든 전송이 성공해야만 창 닫기 옵션이 적용됨. 하나라도 실패하면 닫지 않음
	m_check_close_after_all.set_text_color(m_theme.cr_title_text);
	m_check_close_after_all.set_back_color(m_theme.cr_title_back_active, false);
	m_check_close_after_all.use_hover(false);
	m_check_close_after_all.set_font_weight();
	int auto_close = theApp.GetProfileInt(_T("setting"), _T("auto close after all"), BST_CHECKED);
	m_check_close_after_all.SetCheck(auto_close);
	CRect rz = m_check_close_after_all.calc_rect();
	CRect rc;
	GetClientRect(rc);
	m_check_close_after_all.SetWindowPos(NULL,
										rc.right - m_sys_buttons.get_size().cx - 12 - rz.Width(),
										(get_titlebar_height() - rz.Height()) / 2 - 1,
										rz.Width(),
										rz.Height(), SWP_NOZORDER);


	//for test. 한 PC에서 단독으로 테스트 할 경우
	if (true)//get_process_running_count(get_exe_directory() + _T("\\FileTransferTest.exe")) == 0)
	{
		CString my_ip = get_my_ip();
#ifdef _REMOTE_SDK
		//for Remote SDK test
		if (true)
#else
		//내 PC에서 P2P 모드로 테스트 할 경우
		if (my_ip == CString(__targv[4]))
#endif
		{
			//대상 IP가 my_ip와 같고 실행중인 nFTDClient.exe가 없다면 실행시켜준다.
			//만약 nFTDClient.exe도 trace mode로 돌려야한다면 nFTDClient.exe를 먼저 trace mode로 실행시켜서 테스트해야 한다.
			if (get_process_running_count(get_exe_directory() + _T("\\nFTDClient2.exe")) == 0)
			{
				#ifdef _REMOTE_SDK
					//for REMOTE_SDK AP2P connection test
					ShellExecute(NULL, _T("open"), get_exe_directory() + _T("\\nFTDClient2.exe"), _T("-p 192.168.0.48 7002 10000001"), 0, SW_SHOWNORMAL);
				#else
					//for P2P direct connection test
					ShellExecute(NULL, _T("open"), get_exe_directory() + _T("\\nFTDClient2.exe"), _T("-l 4567"), 0, SW_SHOWNORMAL);
				#endif
			}
		}
		//LinkMeMine 1.0 개발서버의 AP2P로 테스트 할 경우
		else if (CString(__targv[4]) == _T("3.35.127.253"))
		{
			if (get_process_running_count(get_exe_directory() + _T("\\nFTDClient2.exe")) == 0)
				ShellExecute(NULL, _T("open"), get_exe_directory() + _T("\\nFTDClient2.exe"), _T("-p 3.35.127.253 443 1234"), 0, SW_SHOWNORMAL);
		}
		//LinkMeMine 3.0 개발서버의 AP2P로 테스트 할 경우
		else if (CString(__targv[4]) == _T("13.125.4.150"))
		{
			if (get_process_running_count(get_exe_directory() + _T("\\nFTDClient2.exe")) == 0)
				ShellExecute(NULL, _T("open"), get_exe_directory() + _T("\\nFTDClient2.exe"), _T("-p 13.125.4.150 443 1234"), 0, SW_SHOWNORMAL);
		}
	}

	init_treectrl();
	init_listctrl();
	init_pathctrl();

	//20260712 by claude. 이 앱은 컨트롤이 많아 리사이즈 드래그가 느리므로(리스트당 sync_scrollbar 5~10ms) '바 숨김' 최적화를
	//opt-in 한다. 리사이즈 드래그 중엔 오버레이 스크롤바를 숨겨 window 조작 비용을 없애고, 놓으면 정확히 복원한다.
	//(CSCListCtrl/CSCTreeCtrl 기본값은 false — 바 유지. 여기서만 켠다.) 원격 컨트롤은 아직 생성 전이라도 멤버 bool 설정은 무해.
	//m_list_local.set_hide_scroll_when_resize(true);
	//m_list_remote.set_hide_scroll_when_resize(true);
	//m_list_local_favorite.set_hide_scroll_when_resize(true);
	//m_list_remote_favorite.set_hide_scroll_when_resize(true);
	//m_tree_local.set_hide_scroll_when_resize(true);
	//m_tree_remote.set_hide_scroll_when_resize(true);

	//로컬 경로를 복원시킨다. 이 작업은 연결 여부와 관계없이 먼저 진행시킨다.
	//real path로 변환하여 실제 존재하는 경로가 아니라면 내 PC를 선택하고
	//anysupport는 viewermode가 아니면 로컬이 고객컴퓨터가 된다. 따라서 특정 고객의 로컬 드라이브를 기억해놓는 것은 의미가 없다.
#if (defined(_ANYSUPPORT))
	m_path_local.set_path(_T("C:\\"));
#else
	CString path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(0, GetLocalLastPath());
	if (path.IsEmpty() || !PathFileExists(path))
		path = get_known_folder(CSIDL_DRIVES);

	//만약 real_path가 "C:\Users\scpark\Desktop"이라도 트리에서는 "바탕 화면" 항목이 선택되야 한다.
	logWrite(_T("local path = %s"), path);
	change_directory(path, SERVER_SIDE);
#endif

	//text 내용만 없을 뿐 폰트 크기 등을 미리 세팅해놓고 시작한다.
	//팝업 메시지가 필요할 경우 set_text(_T("text message"));와 같이 텍스트만 변경해주면 된다.
	m_toast_popup.set_text(this, _T(" "), 32, Gdiplus::FontStyleBold, 2, 2.0, _T("맑은 고딕"),
		Gdiplus::Color::RoyalBlue,
		Gdiplus::Color::White);
	m_toast_popup.use_control(false);

	RestoreWindowPosition(&theApp, this);

	//timer로 init_progressDlg()를 하는 이유는 일단 메인창이 화면에 표시된 후에 접속상태창을 센터에 표시하기 위함이다.
	//OnInitDialog()가 끝나기 전에 m_progressDlg.CenterWindow()를 해도 메인창의 중앙에 표시되지 않는다.
	SetTimer(timer_init_progress_and_connect, 100, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CnFTDServerDlg::init_treectrl()
{
	m_tree_local.set_use_drag_and_drop(true);
	m_tree_local.set_as_shell_treectrl(&theApp.m_shell_imagelist, true);	//shell 트리는 내부에서 라인 간격 30px 로 설정.
	m_tree_local.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
	//20260705 by claude. 드래그 중 대상에 따른 문구("+ …로 복사")를 계산해 드래그 이미지에 표시. 소스=로컬 트리.
	m_tree_local.set_drag_hint_provider([this](CWnd* pDropWnd, CPoint pt) { return compute_drag_hint(&m_tree_local, pDropWnd, pt); });
	//컨트롤 자체 메뉴 대신 이 앱이 제공하는 메뉴(OnContextMenu)를 쓰므로 false 로 위임한다.
	m_tree_local.set_use_own_context_menu(false);
	m_list_local.set_use_own_context_menu(false);

	//filetransfer.ini? favorite.ini?에서 읽어와서 마지막 열었던 폴더를 복원시키는데
	//anysupport는 linkmemine과 달리 favorite.ini를 활용하기 애매하다.
	//우선 C드라이브를 기본으로 열어준다.
#if (defined(_REMOTE_SDK) || defined(_ANYSUPPORT))
	m_tree_local.set_path(_T(""));
	Wait(100);
	m_tree_local.set_path(_T("C:\\"));
#endif
}

void CnFTDServerDlg::init_listctrl()
{
	m_list_local.set_as_shell_listctrl(&theApp.m_shell_imagelist, true);
	m_list_local.set_use_drag_and_drop(true);
	m_list_local.restore_column_width(&theApp, _T("list local"));
	m_list_local.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
	//20260705 by claude. 소스=로컬 리스트. 드래그 중 대상 드라이브에 따라 "+ …로 복사" 문구 표시.
	m_list_local.set_drag_hint_provider([this](CWnd* pDropWnd, CPoint pt) { return compute_drag_hint(&m_list_local, pDropWnd, pt); });

	auto drive_list = theApp.m_shell_imagelist.m_volume[0].get_drive_list();
	for (int i = 0; i < drive_list->size(); i++)
	{
		logWrite(_T("local drive[%d] = %s, %s"), i, drive_list->at(i).label, drive_list->at(i).path);
	}
}

void CnFTDServerDlg::init_pathctrl()
{
	m_path_local.set_shell_imagelist(&theApp.m_shell_imagelist, true);
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
	m_progressDlg.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	m_progressDlg.CenterWindow(this);
	m_progressDlg.ShowWindow(SW_SHOW);
}

void CnFTDServerDlg::init_favorite()
{
	int i;
	CString headings;
	headings.Format(_T("%s,100;%s,150"), _S(IDS_FAVORITE) + _T(" ") + _S(IDS_FOLDER), _S(IDS_PATH));

	m_list_local_favorite.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
	m_list_local_favorite.set_headings(headings);
	m_list_local_favorite.set_font_size(9);
	m_list_local_favorite.set_header_height(22);
	m_list_local_favorite.restore_column_width(&theApp, _T("list local favorite"));
	//컨트롤 자체 메뉴 대신 이 앱이 제공하는 즐겨찾기 메뉴(OnContextMenu)를 쓰므로 false 로 위임한다.
	m_list_local_favorite.set_use_own_context_menu(false);

	m_list_remote_favorite.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
	m_list_remote_favorite.set_headings(headings);
	m_list_remote_favorite.set_font_size(9);
	m_list_remote_favorite.set_header_height(22);
	m_list_remote_favorite.restore_column_width(&theApp, _T("list remote favorite"));
	m_list_remote_favorite.set_use_own_context_menu(false);

	CString favorite_ini;

	favorite_ini.Format(_T("%s\\favorite.ini"), get_exe_directory());
	EZIni ini(favorite_ini);

	//local
	CString str = ini[_T("LOCAL")][_T("FAVORITE")];

	int index;

	std::deque<CString> dq;
	get_token_str(str, dq, _T(";"), false);

	for (i = 0; i < dq.size(); i++)
	{
		//기존 즐겨찾기 폴더끝에 붙은 '\\' 제거. 단 드라이브 루트("X:\\", 길이 3)는 '\\'를 유지해야
		//신규 추가("D:\\")와 경로가 일치하여 중복검사/토글이 어긋나지 않는다.
		if (dq[i].GetLength() > 3 && dq[i].Right(1) == '\\')
			truncate(dq[i], 1);
		//드라이브 루트가 "X:"(역슬래시 없이) 저장돼 있었다면 정규 표기 "X:\\"로 승격 — 표시(드라이브 레이블)·저장·트리 이동 일관.
		//favorite_add 가 이 경로를 컬럼1에 저장하고 곧 save_favorite 하므로, 재저장 시 "X:\\"로 마이그레이션된다.
		if (dq[i].GetLength() == 2 && dq[i][1] == _T(':'))
			dq[i] += _T("\\");
		favorite_cmd(favorite_add, SERVER_SIDE, dq[i]);
	}

	//header text가 수직 중앙에 제대로 표시되지 않는 현상이 있어 강제로 Invalidate()을 호출함.
	//set_header_height()를 호출했으나 그 적용이 된 후에도 정상표시되지 않고 있음.
	m_list_local_favorite.Invalidate();


	//remote. __targv[4] = IP, __targv[7] = device_id
	if (__argc >= 8)
	{
		str = ini[__targv[4]][__targv[7]];

		dq.clear();
		get_token_str(str, dq, _T(";"), false);

		for (i = 0; i < dq.size(); i++)
		{
			//기존 즐겨찾기 폴더끝에 붙은 '\\' 제거.
			if (dq[i].GetLength() > 2 && dq[i].Right(1) == '\\')
				truncate(dq[i], 1);
			favorite_cmd(favorite_add, CLIENT_SIDE, dq[i]);
		}

		m_list_remote_favorite.Invalidate();
	}
}

void CnFTDServerDlg::save_favorite(int dwSide)
{
	CString favorite_ini;

	favorite_ini.Format(_T("%s\\favorite.ini"), get_exe_directory());
	EZIni ini(favorite_ini);

	int i;
	CString str;
	CSCListCtrl* plist = (dwSide == SERVER_SIDE ? &m_list_local_favorite : &m_list_remote_favorite);

	//local
	for (i = 0; i < plist->size(); i++)
		str = str + plist->get_text(i, 1) + _T(";");

	if (dwSide == SERVER_SIDE)
		ini[_T("LOCAL")][_T("FAVORITE")] = str;
	else if (__argc >= 8)
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
		//UINT id = (nID & 0xFFF0);

		CSCThemeDlg::OnSysCommand(nID, lParam);
	}
}

//CSCSystemButtons(타이틀바 시스템 버튼)은 클릭 시 WM_SYSCOMMAND 가 아닌 사용자 정의 메시지
//Message_CSCSystemButtons 를 부모로 보낸다(SCSystemButtons.cpp 참조 — WM_SYSCOMMAND 의 부가 처리 문제로 변경됨).
//따라서 parent 인 본 dlg 가 직접 cmd 를 처리해야 한다. 핸들러가 없으면 min/max/close 버튼이 무반응이다.
//(Endorphin2 의 on_message_CSCSystemButtons 패턴 참고.)
LRESULT CnFTDServerDlg::on_message_CSCSystemButtons(WPARAM wParam, LPARAM lParam)
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
			parent_maximized(IsZoomed());	//시스템 버튼의 maximize/restore 아이콘 모양 동기화.
			break;
		case SC_CLOSE:
			//종료 확정/정리(접속 종료, 경로·컬럼폭 저장)는 OnBnClickedCancel 에 일원화돼 있다.
			OnBnClickedCancel();
			break;
	}

	return 0;
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
		//타이틀바/배경/테두리 그리기는 CSCThemeDlg::OnPaint() 가 theme 색 기반으로 정석 처리한다.
		//(예전엔 여기서 아이콘/제목/테두리를 직접 그렸으나 base 로 일원화했다.)
		CSCThemeDlg::OnPaint();
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

	//특정 파일이나 폴더가 많을 경우 tree와 favorite 컨트롤 사이에 동적 progress로 표시하는데
	//윈도우 위치가 옮겨질 때 해당 컨트롤들도 따라와야 하므로 호출해준다.
	adjust_processing_progress_ctrl();

	//레지스트리에 현재 위치를 저장한 후 다시 프로그램이 시작될 때 위치 복원을 위해 저장
	SaveWindowPosition(&theApp, this);
}


void CnFTDServerDlg::OnBnClickedOk()
{
	file_command_on_list(file_cmd_open);
}


void CnFTDServerDlg::OnBnClickedCancel()
{
	//접속시도중인 상태에서 종료 버튼 클릭 시 noTopMost로 변경해줘야 한다.
	if (m_progressDlg.IsWindowVisible())
		m_progressDlg.SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	if (m_messagebox.DoModal(_S(IDS_CONFIRM_EXIT), MB_OKCANCEL) == IDCANCEL)
		return;

	kill_process_by_fullpath(get_exe_directory() + _T("\\nFTDClient.exe"));
	SaveLocalLastPath();

	m_list_local.save_column_width(&theApp, _T("list local"));
	m_list_local_favorite.save_column_width(&theApp, _T("list local favorite"));
	m_list_remote.save_column_width(&theApp, _T("list remote"));
	m_list_remote_favorite.save_column_width(&theApp, _T("list remote favorite"));

	CDialogEx::OnCancel();
}


//OnSetCursor / OnLButtonUp / OnLButtonDblClk 제거:
//예전엔 client 영역에서 get_corner_index() 로 가장자리를 직접 판정해 resize 커서를 SetCursor 하고
//OnLButtonDown 에서 SC_SIZE 를 DefWindowProc 으로 보내 수동 resize 하던 방식이었다.
//이제 CSCThemeDlg 의 WM_NCHITTEST 트랩(가장자리 8px → HTLEFT/HTTOP/... 반환)이 OS 표준 resize 와
//resize 커서를 처리하고, 타이틀바 더블클릭 maximize/restore 도 CSCThemeDlg::OnLButtonDblClk 가 처리한다.
//세 핸들러는 메시지 맵에도 등록돼 있지 않아 호출되지 않는 죽은 코드였으므로 정리한다.

void CnFTDServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_path_local.edit_end();
	m_path_remote.edit_end();

	//타이틀바 드래그 이동은 CSCThemeDlg::OnLButtonDown 이 처리한다(point.y < m_titlebar_height 이면 HTCAPTION 위임).
	CSCThemeDlg::OnLButtonDown(nFlags, point);
}


void CnFTDServerDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_path_local.edit_end();
	m_path_remote.edit_end();

	CSCThemeDlg::OnRButtonDown(nFlags, point);
}

BOOL CnFTDServerDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;

	return CDialogEx::OnEraseBkgnd(pDC);
}


void CnFTDServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CSCThemeDlg::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_hWnd == NULL || !m_splitter_local_favorite.m_hWnd)
		return;

	adjust_processing_progress_ctrl();
}

//20260712 by claude. 리사이즈 드래그 시작 — 오버레이 스크롤바 window 조작(리스트당 5~10ms, 형제 clip 재계산)을 건너뛰게 한다.
//드래그 중 바는 숨겨지고, 드래그 종료 시 OnExitSizeMove 가 정확 위치로 복원·표시한다.
void CnFTDServerDlg::OnEnterSizeMove()
{
	CSCListCtrl::set_live_resize(true);
	CSCTreeCtrl::set_live_resize(true);
	Default();
}

//20260712 by claude. 리사이즈 드래그 종료 — 플래그 해제 후 각 컨트롤 sync_scrollbar 로 바를 정확 위치로 복원·표시한다.
void CnFTDServerDlg::OnExitSizeMove()
{
	CSCListCtrl::set_live_resize(false);
	CSCTreeCtrl::set_live_resize(false);
	Default();

	m_list_local.sync_scrollbar();
	m_list_remote.sync_scrollbar();
	m_list_local_favorite.sync_scrollbar();
	m_list_remote_favorite.sync_scrollbar();
	m_tree_local.sync_scrollbar();
	m_tree_remote.sync_scrollbar();
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
	if (!m_ServerManager.SetConnection(GetCommandLine()))
#endif
	{
		AfxMessageBox(_T("m_ServerManager.SetConnection(lpCmdLine) failed."));
		CDialogEx::OnCancel();
		return 0;
	}

	logWrite(_T("Connection Begin. g_FT_mode = %d"), g_FT_mode);

	if (!m_ServerManager.Connection())
	{
		m_progressDlg.ShowWindow(SW_HIDE);

		//DoModal()로 실행했으나 modeless처럼 동작한다. thread내에서 호출해서 그런듯하다.
		//메인에 메시지로 보내서 에러창을 표시하고 종료시킨다.
		SendMessage(MESSAGE_CONNECT_FAIL);
		return 0;
	}

	//delete[] lpCmdLine;

	//m_treeClient.SetOsType(m_ServerManager.m_nClientOSType);

	//m_ServerManager.m_strStatusbarTitle;
#ifdef _REMOTE_SDK
	SetWindowText(_T("File Transfer"));
#elif _ANYSUPPORT
	SetWindowText(_T("AnySupport ") + _S(NFTD_IDS_FILETRANSFER));
	m_static_local.set_text(m_bViewerMode ? _S(IDS_SUPPORTER_PC) : _S(IDS_HOST_PC));
	m_static_remote.set_text(m_bViewerMode ? _S(IDS_HOST_PC) : _S(IDS_SUPPORTER_PC));
#else
	SetWindowText(m_ServerManager.m_strStatusbarTitle);
#endif

	logWrite(_T("Connection ok."));

	return true;
}

void CnFTDServerDlg::initialize()
{
	InitServerManager();		// Server Manager 설정

	m_tree_remote.set_as_shell_treectrl(&theApp.m_shell_imagelist, false, _T(""));	//shell 트리는 내부에서 라인 간격 30px 로 설정.
	//컨트롤 자체 메뉴 대신 이 앱이 제공하는 메뉴(OnContextMenu)를 쓰므로 false 로 위임한다.
	m_tree_remote.set_use_own_context_menu(false);
	m_list_remote.set_use_own_context_menu(false);
	m_tree_remote.set_use_drag_and_drop(true);
	m_tree_remote.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
	//20260705 by claude. 소스=리모트 트리. 리모트→리모트 같은 드라이브=이동(문구 없음), 다른 드라이브=복사, 로컬↔리모트=전송(문구 없음).
	m_tree_remote.set_drag_hint_provider([this](CWnd* pDropWnd, CPoint pt) { return compute_drag_hint(&m_tree_remote, pDropWnd, pt); });

	m_list_remote.set_as_shell_listctrl(&theApp.m_shell_imagelist, false, _T(""));
	m_list_remote.set_use_drag_and_drop(true);
	m_list_remote.add_drag_images(IDB_DRAG_SINGLE_FILE, IDB_DRAG_MULTI_FILES);
	//20260705 by claude. 소스=리모트 리스트. 리모트→리모트 다른 드라이브=복사, 로컬↔리모트=전송(문구 없음).
	m_list_remote.set_drag_hint_provider([this](CWnd* pDropWnd, CPoint pt) { return compute_drag_hint(&m_list_remote, pDropWnd, pt); });
	m_list_remote.restore_column_width(&theApp, _T("list remote"));

	m_path_remote.set_shell_imagelist(&theApp.m_shell_imagelist, false);
	m_path_remote.set_is_local_device(false);
	//m_path_remote.back_color(Gdiplus::Color::Turquoise);

	CString path = GetRemoteLastPath();
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

	//실행되면 remote의 "내 PC"가 펼쳐진 상태로 시작하도록 한다.
	HTREEITEM hItem = m_tree_remote.find_item(theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DRIVES));
	m_tree_remote.Expand(hItem, TVE_EXPAND);
	//std::deque<CString> drive_list;
	//m_ServerManager.DriveList(&drive_list);
	//theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].set_drive_list(&drive_list);
	//m_tree_remote.update_drive_list(path, &drive_list);


	//즐겨찾기 로딩 및 체크
#if (!defined(_REMOTE_SDK) && !defined(_ANYSUPPORT))
	SetTimer(timer_init_favorites, 1000, NULL);
#endif

	//filetransfer.ini? favorite.ini?에서 읽어와서 마지막 열었던 폴더를 복원시키는데
//anysupport는 linkmemine과 달리 favorite.ini를 활용하기 애매하다.
//우선 C드라이브를 기본으로 열어준다.
#if (defined(_REMOTE_SDK) || defined(_ANYSUPPORT))
	m_tree_remote.set_path(_T("C:\\"));
#endif

	SetForegroundWindowForce(m_hWnd);
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


	std::deque<CDiskDriveInfo> drive_list;
	m_ServerManager.get_remote_drive_list(&drive_list);
	theApp.m_shell_imagelist.set_drive_list(CLIENT_SIDE, &drive_list);

	drive_list = *(theApp.m_shell_imagelist.m_volume[1].get_drive_list());
	for (int i = 0; i < drive_list.size(); i++)
	{
		logWrite(_T("remote drive[%d] = %s, real_path = %s, total_space = %s, free_space = %s"),
				i, drive_list[i].label, drive_list[i].path, i2S(drive_list[i].total_space.QuadPart, true), i2S(drive_list[i].free_space.QuadPart, true));
	}

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

	//disabled 컨트롤(전송버튼)에도 툴팁이 표시되도록 parent 에서 마우스 메시지를 툴팁에 릴레이한다.
	//WS_DISABLED 윈도우는 마우스 메시지를 못 받으므로 컨트롤 자체 PreTranslateMessage 로는 불가 — 모든 MFC 컨트롤 공통.
	if (m_tooltip.m_hWnd)
	{
		MSG msg = *pMsg;
		msg.hwnd = (HWND)m_tooltip.SendMessage(TTM_WINDOWFROMPOINT, 0, (LPARAM) & (msg.pt));

		CPoint pt = msg.pt;
		if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
			::ScreenToClient(msg.hwnd, &pt);

		msg.lParam = MAKELONG(pt.x, pt.y);
		m_tooltip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)&msg);
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		TRACE(_T("keydown on CnFTDServerDlg\n"));
		switch (pMsg->wParam)
		{
			//case VK_RETURN :
			//	OnBnClickedOk();
			//	return TRUE;
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
				else if (GetFocus() == &m_tree_local || GetFocus() == &m_tree_remote)
				{
					//트리 항목 Del = 휴지통 삭제(확인창 포함). 컨텍스트 메뉴 삭제와 동일 경로(FO_DELETE + FOF_ALLOWUNDO).
					OnTreeContextMenuDelete();
					return TRUE;
				}

				//삭제 시 물어보고 지우거나 아예 키를 이용한 삭제는 방지
				//file_command_on_list(file_cmd_delete);
				break;
			case VK_F5 :
				//F5 = 새로고침. 포커스가 트리면 트리, 리스트면 리스트를 새로고침(메뉴의 'F5' 힌트와 일치).
				//기존엔 F5 케이스가 없어 메뉴엔 F5 라 적혀 있어도 실제 키는 무동작이었다.
				if (GetFocus() == &m_tree_local || GetFocus() == &m_tree_remote)
				{
					file_command_on_tree(file_cmd_refresh);
					return TRUE;
				}
				else if (GetFocus() == &m_list_local || GetFocus() == &m_list_remote)
				{
					file_command_on_list(file_cmd_refresh);
					return TRUE;
				}
				break;
			case '1' :
				if (IsCtrlPressed() && IsShiftPressed())
				{
					ShellExecute(m_hWnd, _T("open"), gLog.get_log_full_path(), 0, 0, SW_SHOWNORMAL);
					CString client_log;
					client_log.Format(_T("%s\\Log\\nFTDClient_%s.log"), get_exe_directory(), get_cur_datetime_str(0, false));
					ShellExecute(m_hWnd, _T("open"), client_log, 0, 0, SW_SHOWNORMAL);
					return TRUE;
				}
				break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT	CnFTDServerDlg::on_message_CPathCtrl(WPARAM wParam, LPARAM lParam)
{
	CPathCtrlMessage* pMsg = (CPathCtrlMessage*)wParam;

	if (pMsg->pThis == &m_path_local)
	{
		if (pMsg->message == CPathCtrl::message_pathctrl_path_changed)
		{
			TRACE(_T("message_pathctrl_path_changed from m_path_local. path = %s\n"), pMsg->cur_path);
			bool* res = (bool*)lParam;

			//내 PC, 바탕 화면 등과 같은 경로일 경우는 PathFileExists()로 검사가 안되므로 실제 경로로 변환한 후 검사해야 한다.
			/*
			CPathCtrl* pPath = (CPathCtrl*)(msg->pThis);
			CString real_path = pPath->get_shell_imagelist()->convert_special_folder_to_real_path((pPath->get_is_local() ? 0 : 1), msg->cur_path);

			if (PathFileExists(real_path))
			{
				if (res)
					*res = true;

				m_list_local.set_path(pMsg->cur_path);
				m_tree_local.set_path(pMsg->cur_path);
				m_dir_watcher.stop();
				m_dir_watcher.add(theApp.m_shell_imagelist.convert_special_folder_to_real_path(0, pMsg->cur_path));
			}
			*/
			if (change_directory(pMsg->cur_path, SERVER_SIDE))
			{
				if (res)
					*res = true;

				//tree에서 set_path()를 호출하면 list도 자동 갱신되므로
				//별도로 호출하지 않는다.
				m_tree_local.set_path(pMsg->cur_path);
			}
			else
			{
				m_messagebox.DoModal(_T("\'") + pMsg->cur_path + _T("\'") + _S(IDS_NOT_EXIST_PATH));
				if (res)
					*res = false;
			}
		}
	}
	else if (pMsg->pThis == &m_path_remote)
	{
		if (pMsg->message == CPathCtrl::message_pathctrl_path_changed)
		{
			TRACE(_T("message_pathctrl_path_changed from m_path_remote. path = %s\n"), pMsg->cur_path);
			bool* res = (bool*)lParam;

			if (change_directory(pMsg->cur_path, CLIENT_SIDE))
			{
				if (res)
					*res = true;

				//tree에서 set_path()를 호출하면 list도 자동 갱신되므로
				//별도로 호출하지 않는다.
				m_tree_remote.set_path(pMsg->cur_path);
			}
			else
			{
				m_messagebox.DoModal(_T("\'") + pMsg->cur_path + _T("\'") + _S(IDS_NOT_EXIST_PATH));

				if (res)
					*res = false;
			}
		}
		else if (pMsg->message == CPathCtrl::message_pathctrl_request_remote_subfolders)
		{
			std::deque<CString>* folder_list = (std::deque<CString>*)lParam;
			folder_list->clear();

			if (pMsg->cur_path.IsEmpty())
			{
				folder_list->push_back(theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DRIVES));
				folder_list->push_back(theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_MYDOCUMENTS));
				folder_list->push_back(theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DESKTOP));
			}
			else
			{
				std::deque<WIN32_FIND_DATA> dq;
				m_ServerManager.get_folderlist(pMsg->cur_path, &dq, true);

				for (int i = 0; i < dq.size(); i++)
				{
					folder_list->push_back(dq[i].cFileName);
				}
			}
		}
	}

	return 0;
}

LRESULT	CnFTDServerDlg::on_message_CSCListCtrl(WPARAM wParam, LPARAM lParam)
{
	CSCListCtrlMessage* msg = (CSCListCtrlMessage*)wParam;

	if (msg->message == CSCListCtrl::message_path_changed)
	{
		CString path = *(CString*)lParam;

		if (msg->pThis == &m_list_local)
		{
			m_tree_local.set_path(path);
			m_path_local.set_path(path);
			change_directory(path, SERVER_SIDE);

			m_dir_watcher.stop();
			rewatch_local();
		}
		else if (msg->pThis == &m_list_remote)
		{
			//m_tree_remote.set_path(path);
			//m_path_remote.set_path(path);
			change_directory(path, CLIENT_SIDE);
		}
	}
	else if (msg->message == CSCListCtrl::message_drag_and_drop)
	{
		m_transfer_list.clear();

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
		CSCListCtrl* pDragListCtrl = (CSCListCtrl*)msg->pThis;
		
		m_srcSide = (pDragListCtrl == &m_list_remote);

		if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		{
			CSCListCtrl* pDropListCtrl = (CSCListCtrl*)msg->pTarget;
			m_dstSide = (pDropListCtrl == &m_list_remote);

			int droppedIndex = pDragListCtrl->get_drop_index();

			if (droppedIndex >= 0)
				droppedItemText = pDropListCtrl->get_text(droppedIndex, CSCListCtrl::col_filename);

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
					dropped_path = concat_path(pDropListCtrl->get_path(), droppedItemText);
				}
				else if (pDropListCtrl == &m_list_remote)
				{
					dropped_path = concat_path(m_remoteCurrentPath, droppedItemText);
				}

				//만약 dropped_path가 폴더가 아닌 파일이라면 dropped_path는 폴더명까지만 취한다.
				if (pDropListCtrl->get_text(droppedIndex, CSCListCtrl::col_filesize).IsEmpty() == false)
				{
					dropped_path = get_parent_dir(dropped_path);
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
			m_dstSide = (pDropTreeCtrl == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;	//트리로 드롭 시 side 설정(누락 버그).
			HTREEITEM hItem = pDropTreeCtrl->GetDropHilightItem();

			if (hItem)
			{
				droppedItemText = pDropTreeCtrl->GetItemText(hItem);
				dropped_path = pDropTreeCtrl->get_path(hItem);
				TRACE(_T("dropped on = %s (%s)\n"), droppedItemText, dropped_path);

				//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
				//pDropTreeCtrl->SetItemState(hItem, 0, TVIS_DROPHILITED);	<= 이걸로는 해제 안된다.
				pDropTreeCtrl->SelectDropTarget(NULL);
			}
		}

		std::deque<int> dq;
		pDragListCtrl->get_selected_items(&dq);

		//[윈도우 탐색기 동일] 드래그한 폴더를 자기 자신 위에 드롭(같은 리스트에서 drop 대상이 드래그 항목 중 하나)한 경우는
		//아무 동작도 하지 않는다. 그대로 두면 file_transfer 의 change_directory 가 그 폴더로 진입하거나(=폴더 안으로 들어감)
		//자기 자신으로의 전송이 시도된다.
		if (msg->pTarget == (CWnd*)pDragListCtrl)
		{
			int dropped_index = pDragListCtrl->get_drop_index();
			for (int i = 0; i < (int)dq.size(); i++)
			{
				if (dq[i] == dropped_index)
				{
					if (dropped_index >= 0)
						pDragListCtrl->SetItemState(dropped_index, 0, LVIS_DROPHILITED);
					m_transfer_list.clear();
					return 0;
				}
			}
		}

		m_transfer_list.clear();
		m_transfer_from = pDragListCtrl->get_path();

		if (m_dstSide == SERVER_SIDE)
			m_transfer_to = theApp.m_shell_imagelist.convert_special_folder_to_real_path(0, dropped_path);
		else
			m_transfer_to = theApp.m_shell_imagelist.convert_special_folder_to_real_path(1, dropped_path);

		for (int i = 0; i < dq.size(); i++)
		{
			if (m_srcSide == SERVER_SIDE)
			{
				m_transfer_list.push_back(pDragListCtrl->get_win32_find_data(dq[i]));
			}
			else
			{
				m_transfer_list.push_back(pDragListCtrl->get_win32_find_data(dq[i]));
			}
			TRACE(_T("dragged src %d = %s (%s)\n"), i, pDragListCtrl->get_text(dq[i], CSCListCtrl::col_filename), m_transfer_list.back().cFileName);
		}

		file_transfer();
	}
	else if (msg->message == CSCListCtrl::message_get_remote_free_space)
	{
		ULARGE_INTEGER* ul_free_space = (ULARGE_INTEGER*)lParam;
		m_ServerManager.m_socket.RemainSpace(ul_free_space, msg->param0[0]);
	}
	else if (msg->message == CSCListCtrl::message_get_remote_total_space)
	{
		ULARGE_INTEGER* ul_total_space = (ULARGE_INTEGER*)lParam;
		m_ServerManager.m_socket.TotalSpace(ul_total_space, msg->param0[0]);
	}
	else if (msg->message == CSCListCtrl::message_request_new_folder)
	{
		bool* res = (bool*)lParam;
		*res = m_ServerManager.m_socket.create_directory(msg->param0);
	}
	else if (msg->message == CSCListCtrl::message_request_new_folder_index)
	{
		int* index = (int*)lParam;
		bool res = m_ServerManager.m_socket.get_new_folder_index(msg->param0, msg->param1, index);
	}
	else if (msg->message == CSCListCtrl::message_request_rename)
	{
		bool* res = (bool*)lParam;
		*res = m_ServerManager.m_socket.Rename(msg->param0, msg->param1);
		if (*res == false)
		{
			CString folder = get_part(msg->param1, fn_folder);
			CString name = get_part(msg->param1, fn_name);
			CString msg;

			msg.Format(_S(IDS_ALREADY_EXIST_SAME_NAME_ITEM), folder, name);
			m_messagebox.DoModal(msg, MB_OK);
		}
	}
	else if (msg->message == CSCListCtrl::message_rename_duplicated)
	{
		CString folder = get_part(msg->param1, fn_folder);
		CString name = get_part(msg->param1, fn_name);
		CString msg;

		msg.Format(_S(IDS_ALREADY_EXIST_SAME_NAME_ITEM), folder, name);
		m_messagebox.DoModal(msg, MB_OK);
	}
	else if (msg->message == CSCListCtrl::message_list_processing)
	{
		CSCSliderCtrl* slider = (msg->pThis == &m_list_local ? &m_progress_local : &m_progress_remote);
		if ((int)lParam < 0)
		{
			//20260705 by claude. 항목 수가 적으면 progress 를 아예 표시하지 않는다(폴더 변경마다 깜빡임 방지). 총 개수는 시작 신호(reserved)에 실려 온다.
			if ((int)msg->reserved > PROGRESS_SHOW_MIN_COUNT)
				slider->ShowWindow(SW_SHOW);
			slider->SetRange(0, msg->reserved);
		}
		else
		{
			//TRACE(_T("list total = %d, cur = %d\n"), msg->reserved, (int)lParam);
			slider->SetPos((int)lParam);
			if (msg->reserved == (int)lParam)
				slider->ShowWindow(SW_HIDE);
		}
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
		m_transfer_list.clear();

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

		//드래그 소스 side. (기존엔 미설정이라 이전 조작의 stale 값이 쓰이는 버그였음.)
		m_srcSide = (pDragTreeCtrl == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;

		m_transfer_from = pDragTreeCtrl->get_path(pDragTreeCtrl->m_DragItem);
		TRACE(_T("drag item = %s. m_transfer_from = %s\n"), pDragTreeCtrl->GetItemText(pDragTreeCtrl->m_DragItem), m_transfer_from);

		CSCTreeCtrl* pDstTree = NULL;		//대상이 트리일 때만 세팅(서피컬 노드 추가용).
		HTREEITEM   hDstTreeItem = NULL;

		if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CListCtrl)))
		{
			CSCListCtrl* pDropListCtrl = (CSCListCtrl*)msg->pTarget;
			m_dstSide = (pDropListCtrl == &m_list_remote) ? CLIENT_SIDE : SERVER_SIDE;

			if (pDragTreeCtrl->m_nDropIndex >= 0)
			{
				droppedItemText = pDropListCtrl->GetItemText(pDragTreeCtrl->m_nDropIndex, 0);
			}

			if (droppedItemText.IsEmpty())
			{
				m_transfer_to = pDropListCtrl->get_path();
				TRACE(_T("dropped on = %s\n"), m_transfer_to);
			}
			else
			{
				m_transfer_to = concat_path(pDropListCtrl->get_path(), droppedItemText);
				TRACE(_T("dropped on = %s\n"), m_transfer_to);
			}

			//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
			if (pDragTreeCtrl->m_nDropIndex >= 0)
				pDropListCtrl->SetItemState(pDragTreeCtrl->m_nDropIndex, 0, LVIS_DROPHILITED);
		}
		else if (msg->pTarget->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
		{
			CSCTreeCtrl* pDropTreeCtrl = (CSCTreeCtrl*)msg->pTarget;
			m_dstSide = (pDropTreeCtrl == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;

			m_transfer_to = pDropTreeCtrl->get_path(pDragTreeCtrl->m_DropItem);
			//TRACE(_T("drag item = %s\n"), pDragTreeCtrl->GetItemText(pDragTreeCtrl->m_DragItem));
			TRACE(_T("dropped on = %s. m_transfer_to = %s\n"), pDropTreeCtrl->GetItemText(pDragTreeCtrl->m_DropItem), m_transfer_to);

			pDstTree = pDropTreeCtrl;			//서피컬 대상(트리) 저장.
			hDstTreeItem = pDragTreeCtrl->m_DropItem;

			//필요한 모든 처리가 끝나면 drophilited 표시를 없애준다.
			//pDropTreeCtrl->SetItemState(hItem, 0, TVIS_DROPHILITED);	<= 이걸로는 해제 안된다.
			pDropTreeCtrl->SelectDropTarget(NULL);
		}

		file_transfer();

		//[로컬 이동] refresh 대신 서피컬로 갱신: 소스 노드는 트리에서 제거, 대상 노드 아래에 이동된 폴더 노드를 추가.
		//PathFileExists(from)==false 로 실제 이동 성공을 확인(from==to 스킵/실패면 트리 안 건드림).
		//20260704 by claude. 복사(!m_drag_copy)는 소스가 남으므로 이 서피컬(소스 제거) 대상이 아님 → 아래 else 에서 refresh.
		if (m_srcSide == SERVER_SIDE && m_dstSide == SERVER_SIDE && !m_drag_copy && !PathFileExists(m_transfer_from))
		{
			HTREEITEM hDrag = pDragTreeCtrl->m_DragItem;
			HTREEITEM hSrcParent = pDragTreeCtrl->GetParentItem(hDrag);

			//대상이 트리면 대상 노드 아래에 이동된 폴더 노드를 추가.
			if (pDstTree && hDstTreeItem)
			{
				if (pDstTree->GetChildItem(hDstTreeItem) != NULL)	//대상 자식이 이미 로드됨 → 이동된 폴더 1개만 정렬 위치에 삽입.
				{
					CString   moved_name = get_part(m_transfer_from, fn_name);
					HTREEITEM hExisting  = pDstTree->find_children_item(moved_name, hDstTreeItem);
					if (hExisting != NULL)
					{
						//[병합] 대상에 같은 이름 폴더가 이미 있음(Windows 병합 대화상자로 합쳐졌거나, from==to 위치). 중복 노드 추가 금지.
						//기존 노드가 펼쳐져 있으면 병합으로 하위가 바뀌었을 수 있으니 자식을 재열거해 최신 상태로 갱신.
						if ((pDstTree->GetItemState(hExisting, TVIS_EXPANDED) & TVIS_EXPANDED) && pDstTree->GetChildItem(hExisting) != NULL)
						{
							HTREEITEM hc = pDstTree->GetChildItem(hExisting);
							while (hc) { HTREEITEM hnext = pDstTree->GetNextSiblingItem(hc); pDstTree->DeleteItem(hc); hc = hnext; }
							pDstTree->insert_folder(hExisting, pDstTree->get_path(hExisting));
						}
					}
					else
					{
						WIN32_FIND_DATA fd; ZeroMemory(&fd, sizeof(fd));
						_tcscpy_s(fd.cFileName, _countof(fd.cFileName), moved_name);
						pDstTree->insert_folder_sorted(hDstTreeItem, &fd);	//오름차순(탐색기식) 정렬 위치에 삽입
					}
				}
				else												//자식 미로드 → 직접 전체 열거해서 로드(이동 폴더 포함, 정렬됨).
				{													//Expand 지연로딩에 의존하면, 대상이 '이미 펼쳐진' 상태일 때 Expand 가 no-op → OnTvnItemexpanding 미호출 → 열거 누락(로그 expanded=1 child_exists=0)이었음.
					pDstTree->insert_folder(hDstTreeItem, pDstTree->get_path(hDstTreeItem));
				}

				//이제 자식이 확실히 로드됨 → 접혀 있으면 실제로 펼치고, 이미 펼쳐져 있으면 방금 추가된 자식이 그대로 보인다.
				pDstTree->Expand(hDstTreeItem, TVE_EXPAND);
				pDstTree->SetRedraw(TRUE);		//OnTvnItemexpanding 의 SetRedraw(FALSE) 가 짝(expanded)을 못 만나 남는 경우 방어적 복구.
				pDstTree->Invalidate(FALSE);
			}

			//소스 노드 제거. 부모에 남은 자식이 없으면 [+] 도 제거.
			pDragTreeCtrl->DeleteItem(hDrag);
			if (hSrcParent && pDragTreeCtrl->GetChildItem(hSrcParent) == NULL)
			{
				TVITEM tv; ZeroMemory(&tv, sizeof(tv));
				tv.mask = TVIF_HANDLE | TVIF_CHILDREN;   tv.hItem = hSrcParent;   tv.cChildren = 0;
				pDragTreeCtrl->SetItem(&tv);
			}
		}
		//20260704 by claude. 같은 쪽 복사(로컬 FO_COPY) / 리모트 이동·복사(file_command)는 file_transfer 가 파일 작업만 하고
		//트리는 미갱신 → 소스·대상 트리를 refresh 로 반영(복사는 소스 유지, 이동은 소스에서 사라짐도 반영). cross-side 전송은 file_transfer 가 이미 트리 refresh.
		else if (m_srcSide == m_dstSide)
		{
			pDragTreeCtrl->refresh(pDragTreeCtrl->GetSelectedItem());
			if (pDstTree && hDstTreeItem)
				pDstTree->refresh(hDstTreeItem);
		}

		return 0;
	}
	else if (msg->message == CSCTreeCtrl::message_request_folder_list)
	{
		EnableWindow(FALSE);
		m_tree_remote.SetRedraw(FALSE);

		long t0 = clock();

		//TRACE(_T("received msg = CSCTreeCtrl::message_request_folder_list\n"));
		int i;
		CString path = *(CString*)lParam;

		EnterCriticalSection(&g_cs);

		//path가 "내 PC"이면 DriveList()를, 그렇지 않으면 refresh_tree_folder()를 호출한다.
		if (path == theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].get_label(CSIDL_DRIVES))
		{
			std::deque<CDiskDriveInfo> drive_list;
			m_ServerManager.get_remote_drive_list(&drive_list);
			theApp.m_shell_imagelist.m_volume[CLIENT_SIDE].set_drive_list(&drive_list);
			m_tree_remote.update_drive_list(path, &drive_list);
		}
		else
		{
			std::deque<WIN32_FIND_DATA> dq;
			m_ServerManager.get_folderlist(path, &dq, true);

			if (dq.size() > 0)
			{
				if (dq.size() > 20)
					::SetCursor(theApp.LoadStandardCursor(IDC_WAIT));

				path = get_parent_dir(CString(dq[0].cFileName));
				//"C:\\" 인 경우와 "C:\\Windows" 인 경우는 처리가 달라야 한다.
				int folder_length = path.GetLength() + (path.Right(1) == '\\' ? 0 : 1);

				m_progress_remote.SetRange(0, dq.size());
				m_progress_remote.SetPos(0);
				if ((int)dq.size() > PROGRESS_SHOW_MIN_COUNT)		//20260705 by claude. 항목 적으면 표시 안 함(깜빡임 방지). 리모트도 로컬과 동일 정책.
					m_progress_remote.ShowWindow(SW_SHOW);

				for (i = 0; i < dq.size(); i++)
				{
					m_progress_remote.SetPos(i + 1);

					//TRACE(_T("add folder %d = %s\n"), i, dq[i].cFileName);
					bool has_child = false;

					if (is_drive_root(dq[i].cFileName))
					{
						has_child = true;
					}
					else
					{
						//검색된 폴더의 sub folder 유무에 따라 확장버튼 유무도 정해진다.
						//has_child = true;// (m_ServerManager.get_subfolder_count(dq[i].cFileName) > 0);
						has_child = (dq[i].nFileSizeLow > 0);

						//임시적으로 nFileSizeLow에 subfolder count를 넘겨받은 것이므로 0으로 다시 리셋시켜준다.
						dq[i].nFileSizeLow = 0;
					}

					//dq 가 fullpath 로 넘어오므로 폴더명(leaf)만 남긴다. 같은 버퍼 내 겹침 복사(_tcscpy 로 dst<src, UB)를
					//피하려 CString 경유 bounded 복사. folder_length 가 길이를 넘으면 원본 유지(방어).
					CString full_name = dq[i].cFileName;
					CString leaf_name = (folder_length < full_name.GetLength()) ? full_name.Mid(folder_length) : full_name;
					_tcscpy_s(dq[i].cFileName, _countof(dq[i].cFileName), (LPCTSTR)leaf_name);
					m_tree_remote.insert_folder(m_tree_remote.get_expanding_item(), &dq[i], has_child);
				}

				m_progress_remote.ShowWindow(SW_HIDE);
			}
		}

		LeaveCriticalSection(&g_cs);
		//TRACE(_T("tree expanding elapsed for %s = %ld\n"), path, clock() - t0);

		m_tree_remote.SetRedraw(TRUE);
		EnableWindow(TRUE);
		::SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
	}
	else if (msg->message == CSCTreeCtrl::message_path_changed)
	{
		if (msg->pThis == &m_tree_local)
		{
			m_path_local.set_path(m_tree_local.get_path());
			m_list_local.set_path(m_tree_local.get_path(), true);
		}
		else if (msg->pThis == &m_tree_remote)
		{
			m_path_remote.set_path(m_tree_remote.get_path());
			m_list_remote.set_path(m_tree_remote.get_path(), true);
		}
	}
	else if (msg->message == CSCTreeCtrl::message_request_new_folder_index)
	{
		int* index = (int*)lParam;
		bool res = m_ServerManager.m_socket.get_new_folder_index(msg->param0, msg->param1, index);
	}
	else if (msg->message == CSCTreeCtrl::message_request_new_folder)
	{
		bool* res = (bool*)lParam;
		*res = m_ServerManager.m_socket.create_directory(msg->param0);
	}
	else if (msg->message == CSCTreeCtrl::message_request_rename)
	{
		bool* res = (bool*)lParam;
		*res = m_ServerManager.m_socket.Rename(msg->param0, msg->param1);
		if (*res == false)
		{
			CString folder = get_part(msg->param1, fn_folder);
			CString folder_name = get_part(msg->param1, fn_name);
			CString msg;

			msg.Format(_S(IDS_ALREADY_EXIST_SAME_NAME_FOLDER), folder, folder_name);
			m_messagebox.DoModal(msg, MB_OK);
		}
	}
	else if (msg->message == CSCTreeCtrl::message_rename_duplicated)
	{
		CString folder = get_part(msg->param1, fn_folder);
		CString folder_name = get_part(msg->param1, fn_name);
		CString msg;

		msg.Format(_S(IDS_ALREADY_EXIST_SAME_NAME_FOLDER), folder, folder_name);
		m_messagebox.DoModal(msg, MB_OK);
	}
	else if (msg->message == CSCTreeCtrl::message_edit_item)
	{
		//F2 등으로 편집 요청 — 선택 항목을 편집모드로 진입시킨다. 편집종료(edit_end) 시
		//로컬은 MoveFile, 원격은 message_request_rename → socket Rename 이 수행된다. (컨텍스트 메뉴 이름변경과 동일 경로.)
		CSCTreeCtrl* ptree = (CSCTreeCtrl*)msg->pThis;
		HTREEITEM hItem = ptree->GetSelectedItem();
		if (hItem)
		{
			int side = (ptree == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;
			CString path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, ptree->get_path(hItem));
			if (theApp.m_shell_imagelist.is_movable(side, path))	//보호 폴더는 이름변경 금지(메뉴 disable 과 일관)
				ptree->edit_item(hItem);
		}
	}
	else if (msg->message == CSCTreeCtrl::message_expand_changed)
	{
		//로컬 트리의 확장/축소로 '펼쳐진 폴더 집합'이 바뀌면 dir watcher 를 다시 설정한다.
		if (msg->pThis == &m_tree_local)
			rewatch_local();
	}
	else if (msg->message == CSCTreeCtrl::message_request_property)
	{
		std::deque<CString> dq_fullpath{ msg->param0 };
		
		bool res = m_ServerManager.m_socket.file_command(file_cmd_property, 0, 0, &dq_fullpath);

		if (res)// && !viewer_is_running)
		{
#ifdef _ANYSUPPORT
			m_toast_popup.set_text(m_bViewerMode ? _S(IDS_SHOW_ON_CLIENT) : _S(IDS_SHOW_ON_AGENT));
#else
			m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
#endif
			m_toast_popup.CenterWindow(this);
			m_toast_popup.fade_in(10, 2000, true);
		}
		else
		{
			m_messagebox.DoModal(msg->param0 + _T("\n") + _S(IDS_FAIL_TO_GET_PROPERTY));
		}
	}
	else if (msg->message == CSCTreeCtrl::message_tree_processing)
	{
		CSCSliderCtrl* slider = (msg->pThis == &m_tree_local ? &m_progress_local : &m_progress_remote);
		if ((int)lParam < 0)
		{
			//20260705 by claude. 항목 수가 적으면 progress 를 아예 표시하지 않는다(폴더 변경마다 깜빡임 방지). 총 개수는 시작 신호(reserved)에 실려 온다.
			if ((int)msg->reserved > PROGRESS_SHOW_MIN_COUNT)
				slider->ShowWindow(SW_SHOW);
			slider->SetRange(0, msg->reserved);
		}
		else
		{
			//TRACE(_T("total = %d, cur = %d\n"), msg->reserved, (int)lParam);
			slider->SetPos((int)lParam);
			if (msg->reserved == (int)lParam)
				slider->ShowWindow(SW_HIDE);
		}
	}

	return 0;
}

LRESULT	CnFTDServerDlg::on_message_CControlSplitter(WPARAM wParam, LPARAM lParam)
{
	adjust_processing_progress_ctrl();
	return 0;
}

void CnFTDServerDlg::adjust_processing_progress_ctrl()
{
	CRect r;
	m_splitter_local_favorite.GetWindowRect(r);
	ScreenToClient(r);
	m_progress_local.MoveWindow(r);

	m_splitter_remote_favorite.GetWindowRect(r);
	ScreenToClient(r);
	m_progress_remote.MoveWindow(r);
}

void CnFTDServerDlg::OnTvnSelchangedTreeLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//change_directory()를 써서 local이든 remote이든 공통으로 처리하면 좋지만
	//tree가 변경된 경우는 path와 listctrl만 refresh한다든지 여러가지 부가적인 처리가 필요하여
	//local인 경우는 직접 처리한다.
	CString path = m_tree_local.get_path();

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
	CString path = m_tree_remote.get_path(NULL);

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

#if 0
void CnFTDServerDlg::OnNMDblclkListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = pNMItemActivate->iItem;

	CString path = m_list_local.get_path();
	CString new_path;

	if (path == get_system_label(CSIDL_DRIVES))
	{
		//new_path = convert_special_folder_to_real_path(m_list_local.get_text(index, CSCListCtrl::col_filename));// , theApp.m_shell_imagelist.m_volume[SERVER_SIDE].get_label_map());
		//m_list_local.set_path(new_path);
		//m_path_local.set_path(new_path);
		//m_tree_local.set_path(new_path);
	}
	else
	{
		//빈 공간을 더블클릭한 경우는 상위 폴더로의 이동으로 동작한다.
		if (index < 0)
		{
			new_path = GetParentDirectory(path);
			change_directory(new_path, SERVER_SIDE);
		}
		else
		{
			new_path = concat_path(path, m_list_local.get_text(index, CSCListCtrl::col_filename));

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
		//내 PC가 선택된 상태도 아니고 파일크기 컬럼이 비어있지 않다면 파일인 경우임.
		if (m_list_remote.get_path() != theApp.m_shell_imagelist.m_volume[1].get_label(CSIDL_DRIVES) &&
			m_list_remote.get_text(index, CSCListCtrl::col_filesize).IsEmpty() == false)
			return;

		//내 PC가 선택된 상태에서 디스크 드라이브를 더블클릭하거나 파일을 더블클릭 한 경우 외에는 폴더이므로 폴더 이동시킨다.
		change_directory(concat_path(m_remoteCurrentPath, m_list_remote.get_text(index, CSCListCtrl::col_filename)), CLIENT_SIDE);
	}

	*pResult = 0;
}
#endif

BOOL CnFTDServerDlg::change_directory(CString path, DWORD dwSide)
{
	//logWrite(_T("path = %s"), path);

	int i;
	BOOL result = FALSE;

	EnableWindow(FALSE);

	if (dwSide == SERVER_SIDE)
	{
		m_path_local.set_path(path);
		m_tree_local.set_path(path, false);
		m_list_local.set_path(path);
		m_dir_watcher.stop();
		rewatch_local();

		refresh_selection_status(&m_list_local);
		refresh_disk_usage(false);
		result = true;
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

		//EnterCriticalSection(&g_cs);
		//std::lock_guard<mutex> lock(g_mutex);

		result = m_ServerManager.change_directory(path, dwSide, false);
		if (result)
		{
			//path가 내 PC, 바탕 화면, 문서, 로컬 디스크(C:) 등 모두 그대로 전달한다.
			m_remoteCurrentPath = path;
			SaveRemoteLastPath();

			std::deque<WIN32_FIND_DATA> dq;

			//리스트 파일목록 갱신
			//20260712 by claude. sync=false — clear 시점 total=0 sync 생략(폴더 전환 flicker 방지). 아래 add_file+display_filelist 가 최종 sync.
			m_list_remote.delete_all_items(true, false);
			//m_ServerManager.refresh_list(&dq, false);
			m_ServerManager.get_filelist(path, &dq, false);

			if (dq.size() > 20)
				::SetCursor(theApp.LoadStandardCursor(IDC_WAIT));

			m_progress_remote.SetRange(0, dq.size());
			m_progress_remote.SetPos(0);
			if ((int)dq.size() > PROGRESS_SHOW_MIN_COUNT)		//20260705 by claude. 항목 적으면 표시 안 함(깜빡임 방지). 리모트도 로컬과 동일 정책.
				m_progress_remote.ShowWindow(SW_SHOW);

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

			m_list_remote.display_filelist(path);

			//path ctrl 갱신. dq에서 파일들을 제외하고 폴더 목록을 넘겨준다.
			std::deque<CString> folder_list;
			for (i = 0; i < dq.size(); i++)
			{
				WIN32_FIND_DATA FindFileData = dq[i];
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
					folder_list.push_back(FindFileData.cFileName);
			}

			m_path_remote.set_path(path, &folder_list);
			m_tree_remote.set_path(path, false);

			refresh_selection_status(&m_list_remote);
			refresh_disk_usage(true);

			m_progress_remote.ShowWindow(SW_HIDE);
		}
	}

	EnableWindow(TRUE);
	::SetCursor(theApp.LoadStandardCursor(IDC_ARROW));

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
/*
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
*/
//보내기 버튼 or 받기 버튼 or drag&drop으로 전송을 시작한다.
//전송 조건으로 아래 값들이 채워진 후 호출되어야 한다.
//m_transfer_list	: fullpath 전송 목록. 목록이 없다면 m_transfer_from 이라는 1개의 폴더인 경우임.
//m_srcSide			: 송신측
//m_dstSide			: 수신측
//m_transfer_from	: 송신 폴더
//m_transfer_to		: 수신 폴더
//20260705 by claude. 드래그 중 대상에 따라 드래그 이미지 하단에 표시할 문구를 계산한다. DroppedHandler 의 타깃 해석과 같은
//기준(트리=HitTest 항목, 리스트=폴더 항목 위면 그 하위 폴더·아니면 현재 폴더)으로 src/dst 를 잡아, 실제 드롭 결과와 문구가
//일치하도록 한다. cross-side(로컬↔리모트)=파일전송이라 문구 없음, 같은 side·같은 드라이브=이동(문구 없음), 다른 드라이브=복사.
CString CnFTDServerDlg::compute_drag_hint(CWnd* pDragWnd, CWnd* pDropWnd, CPoint pt_screen)
{
	if (pDragWnd == NULL || pDropWnd == NULL)
		return _T("");

	//--- 소스 side + 소스 경로(드래그 항목/현재 폴더) ---
	int		src_side;
	CString	src_path;
	if (pDragWnd == &m_tree_local)			{ src_side = SERVER_SIDE; src_path = m_tree_local.get_path(m_tree_local.m_DragItem); }
	else if (pDragWnd == &m_tree_remote)	{ src_side = CLIENT_SIDE; src_path = m_tree_remote.get_path(m_tree_remote.m_DragItem); }
	else if (pDragWnd == &m_list_local)		{ src_side = SERVER_SIDE; src_path = m_list_local.get_path(); }
	else if (pDragWnd == &m_list_remote)	{ src_side = CLIENT_SIDE; src_path = m_remoteCurrentPath; }
	else
		return _T("");

	//--- 타깃 side + 타깃 폴더(커서 아래) + 표시명 ---
	int		dst_side;
	CString	dst_path;
	CString	dst_name;		//문구에 보일 폴더명 — 컨트롤 항목 텍스트(드라이브면 "로컬 디스크 (D:)" 같은 레이블)를 우선 사용.
	CPoint	cli = pt_screen;

	if (pDropWnd == &m_tree_local || pDropWnd == &m_tree_remote)
	{
		CSCTreeCtrl* pt = (CSCTreeCtrl*)pDropWnd;
		dst_side = (pDropWnd == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;
		pt->ScreenToClient(&cli);
		UINT flags = 0;
		HTREEITEM h = pt->HitTest(cli, &flags);
		if (h == NULL)
			return _T("");
		dst_path = pt->get_path(h);
		dst_name = pt->GetItemText(h);
	}
	else if (pDropWnd == &m_list_local || pDropWnd == &m_list_remote)
	{
		CSCListCtrl* pl = (CSCListCtrl*)pDropWnd;
		dst_side = (pDropWnd == &m_list_remote) ? CLIENT_SIDE : SERVER_SIDE;
		CString base_path = (pDropWnd == &m_list_remote) ? m_remoteCurrentPath : pl->get_path();
		pl->ScreenToClient(&cli);
		UINT flags = 0;
		int idx = pl->HitTest(cli, &flags);
		//폴더 항목(크기 컬럼 empty) 위면 그 하위 폴더로, 아니면 현재 폴더로 드롭.
		if (idx >= 0 && pl->get_text(idx, CSCListCtrl::col_filesize).IsEmpty())
		{
			dst_path = concat_path(base_path, pl->get_text(idx, CSCListCtrl::col_filename));
			dst_name = pl->get_text(idx, CSCListCtrl::col_filename);
		}
		else
			dst_path = base_path;
	}
	else
		return _T("");

	//cross-side = 파일전송(이 앱 기본 기능) → 문구 없음.
	if (src_side != dst_side)
		return _T("");

	//같은 side: 실제 경로로 변환 후 드라이브 비교.
	CString from = theApp.m_shell_imagelist.convert_special_folder_to_real_path(src_side, src_path);
	CString to   = theApp.m_shell_imagelist.convert_special_folder_to_real_path(dst_side, dst_path);
	if (from.GetLength() < 2 || to.GetLength() < 2)
		return _T("");

	bool	is_copy = is_drag_copy(from, to);	//Ctrl=복사 / Shift=이동 / 없으면 드라이브 기준(같은 드라이브=이동, 다른=복사)

	//같은 폴더(소스가 이미 들어있는 폴더)로의 '이동'은 무동작이라 문구를 표시하지 않는다(탐색기 동일). 단 복사는 같은 폴더에도
	//'복사본' 생성이 유효하므로 표시. 소스 컨테이너: 리스트=현재 폴더(from), 트리=드래그한 폴더의 부모(자기 자신에 드롭도 무동작).
	if (!is_copy)
	{
		auto same_path = [](CString a, CString b) -> bool
		{
			if (a.GetLength() > 3 && a.Right(1) == _T('\\')) a = a.Left(a.GetLength() - 1);
			if (b.GetLength() > 3 && b.Right(1) == _T('\\')) b = b.Left(b.GetLength() - 1);
			return a.CompareNoCase(b) == 0;
		};
		bool	is_list_src = (pDragWnd == &m_list_local || pDragWnd == &m_list_remote);
		CString	src_container = is_list_src ? from : get_parent_dir(from);
		if (same_path(to, src_container) || (!is_list_src && same_path(to, from)))
			return _T("");
	}

	//기호(+/→)와 동작어(복사/이동)를 bold + 강조색으로, 폴더명은 기본색으로 — CSCShapeDlg/CSCParagraph 의 tagged text(<b>/<cr=..>).
	//표시명: 컨트롤 항목 텍스트(dst_name) 우선 → 폴더 leaf → 실제 경로. 드라이브 루트는 leaf 가 비므로 항목 레이블/경로로 표시.
	CString folder_name = dst_name;
	if (folder_name.IsEmpty())
		folder_name = get_part(to, fn_name);
	if (folder_name.IsEmpty())
		folder_name = to;
	//기호(+/→)·동작어(복사/이동) 모두 <b> 로 강조. 굵기(faux-bold thickness)는 compose 에서 run 별로 — 기호는 강하게, 한글 단어는
	//semibold(약하게) 처리해 CJK 뭉개짐 없이 가독성 유지.
	CString text;
	if (is_copy)
		text.Format(_T("<b><cr=seagreen>+</cr></b> %s(으)로 <b><cr=seagreen>복사</cr></b>"), folder_name);
	else
		text.Format(_T("<b><cr=royalblue>→</cr></b> %s(으)로 <b><cr=royalblue>이동</cr></b>"), folder_name);
	return text;
}

void CnFTDServerDlg::file_transfer()
{
	m_transfer_from = theApp.m_shell_imagelist.convert_special_folder_to_real_path(m_srcSide, m_transfer_from);
	m_transfer_to = theApp.m_shell_imagelist.convert_special_folder_to_real_path(m_dstSide, m_transfer_to);

	//20260705 by claude. 드래그 이동/복사 판정(탐색기 기본, is_drag_copy): Ctrl=강제 복사, Shift=강제 이동, 키 없으면 드라이브
	//기준(같은 드라이브=이동, 다른 드라이브=복사). cross-side(로컬↔리모트)는 소켓 전송이라 m_drag_copy 미사용. 같은 폴더에 같은
	//이름으로 복사 시 이름충돌은 아래 SHFileOperation 에 FOF_RENAMEONCOLLISION 을 줘 탐색기처럼 "… - 복사본" 으로 자동 리네임.
	m_drag_copy = is_drag_copy(m_transfer_from, m_transfer_to);


	//목록이 없다면 m_transfer_from 이라는 1개의 폴더인 경우임.
	if (m_transfer_list.size() == 0)
	{
		WIN32_FIND_DATA data;
		HANDLE hFind = FindFirstFile(m_transfer_from, &data);
		FindClose(hFind);
		//cFileName(TCHAR[MAX_PATH])에 fullpath 를 넣되, 경로가 MAX_PATH 를 넘겨도 스택을 깨지 않도록 bounded 복사(초과 시 잘림).
		_sntprintf_s(data.cFileName, _countof(data.cFileName), _TRUNCATE, _T("%s"), (LPCTSTR)m_transfer_from);
		m_transfer_list.push_back(data);
	}

	//실행부 강제(방어선 이원화): 버튼/메뉴 UI 게이트는 드래그앤드롭·조작된 요청을 못 막으므로, 실제 전송 시작 지점에서
	//다시 검사한다. 소스에 보호 항목(드라이브 루트·시스템 폴더 등)이 하나라도 있으면 전송 금지.
	for (auto& fd : m_transfer_list)
	{
		//20260712 by claude. 소스 방어선 — 실제 이동(같은 side + 복사아님)이면 is_movable, 그 외(복사/크로스머신 전송=복사)면
		//is_copyable_from. move 는 시스템 경로에서 절대 불가(소스 제거). 현재 규칙은 균일하나 액션 의도를 코드에 명시.
		bool is_move = (m_srcSide == m_dstSide) && !m_drag_copy;
		bool src_ok = is_move
			? theApp.m_shell_imagelist.is_movable(m_srcSide, fd.cFileName)
			: theApp.m_shell_imagelist.is_copyable_from(m_srcSide, fd.cFileName);
		if (!src_ok)
		{
			m_messagebox.DoModal(_T("드라이브 루트와 주요 시스템 폴더(및 그 하위)는 시스템 손상을 막기 위해 전송할 수 없습니다."));
			m_transfer_list.clear();
			return;
		}
	}
	//목적지: 시스템 폴더 및 시스템(실행 OS) 드라이브 루트로는 수신 금지. 데이터 드라이브 루트(D:\ 등)는 허용.
	if (!theApp.m_shell_imagelist.is_writable_to(m_dstSide, m_transfer_to))
	{
		m_messagebox.DoModal(_T("주요 시스템 폴더나 시스템 드라이브 루트로는 파일을 받을 수 없습니다."));
		m_transfer_list.clear();
		return;
	}

	//m_transfer_from, m_transfer_to의 끝에 '\\'가 있을 경우의 보정.
	if (m_transfer_from.GetLength() > 3 && m_transfer_from.Right(1) == '\\')
		truncate(m_transfer_from, 1);
	if (m_transfer_to.GetLength() > 3 && m_transfer_to.Right(1) == '\\')
		truncate(m_transfer_to, 1);

	//트리로 drop된 경우 list가 해당 폴더를 표시하고 있지 않다면 change_directory()해준다.
	//20260704 by claude. 단 같은 쪽(로컬↔로컬·리모트↔리모트) 이동/복사는 뷰가 '소스'에 남아야 하므로 대상으로 change_directory 하지 않는다.
	//(특히 리모트는 이 change_directory 가 m_remoteCurrentPath 를 대상으로 바꿔, 뒤의 m_transfer_from 재계산(convert(1, m_remoteCurrentPath))이
	// 대상으로 덮어써져 from==to 로 조용히 skip 되던 버그의 원인 — cross-side 전송일 때만 대상 폴더를 표시한다.)
	if (m_srcSide != m_dstSide &&
		((m_dstSide == SERVER_SIDE && m_list_local.get_path() != m_transfer_to) ||
		 (m_dstSide == CLIENT_SIDE && m_list_remote.get_path() != m_transfer_to)))
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

	CSCListCtrl* pListFile;
	ULARGE_INTEGER* pulDiskSpace;

	if (m_srcSide == SERVER_SIDE)
	{
		pListFile = &m_list_local;
		pulDiskSpace = &m_ulClientDiskSpace;

		//서버에서 서버로 전송할 경우
		if (m_srcSide == m_dstSide)
		{
			//from 이 to 와 같으면(같은 폴더) 이동은 무동작. 단 복사(Ctrl)는 같은 폴더에 '복사본' 생성이 유효하므로 통과.
			//20260704 by claude. 기존엔 copy 여부와 무관하게 skip 해서 '같은 폴더 안 드롭' 시 복사가 아예 실행 안 됐음.
			if (!m_drag_copy && m_transfer_from == m_transfer_to)
			{
				TRACE(_T("same folder. skip\n"));
				return;
			}

			//[윈도우 탐색기 동일] 자식을 이미 들어있는 부모(=현재 위치)로 '이동' 드롭 = 변화 없음 → 조용히 무동작.
			//(FO_MOVE 는 이 경우 "대상 폴더가 원본 폴더와 같습니다" 중단 대화상자를 띄우므로 미리 차단.)
			//20260704 by claude. 복사(Ctrl)는 같은 부모로도 '복사본' 생성이 유효하므로 통과시킨다.
			if (!m_drag_copy && get_parent_dir(m_transfer_from) == m_transfer_to)
			{
				return;
			}

			//NOTE: 예전엔 'from == parent(to)' 면 skip 했으나, 그건 "현재 폴더의 하위 폴더로 드롭"(가장 흔한 이동)을
			//막아 로컬 d&d 가 무동작이 되는 버그였다. 순환 이동(폴더를 자기 하위로)은 SHFileOperation 이 안전하게 거부.

			//폴더가 다르다면 move(윈도우 탐색기와 동일). 로컬끼리는 소켓 전송이 아니라 SHFileOperation FO_MOVE —
			//이름충돌 시 탐색기 기본 대화상자, 크로스드라이브 자동 처리, FOF_ALLOWUNDO 로 되돌리기(휴지통) 가능.
			//소스 목록: SHFileOperation 규약대로 '\0' 구분 + 끝에 '\0''\0'. (임베디드 null 안전 위해 std::basic_string 사용.)
			std::basic_string<TCHAR> from_buf;
			for (auto& fd : m_transfer_list)
			{
				CString src = fd.cFileName;
				if (src.Find(_T(':')) < 0)		//리스트 d&d 는 이름만 저장 → 소스 폴더와 결합. 트리 d&d 는 이미 fullpath.
					src = concat_path(m_transfer_from, src);
				from_buf.append((LPCTSTR)src);
				from_buf.push_back(_T('\0'));
			}
			from_buf.push_back(_T('\0'));

			std::basic_string<TCHAR> to_buf = (LPCTSTR)m_transfer_to;	//대상 폴더.
			to_buf.push_back(_T('\0'));
			to_buf.push_back(_T('\0'));

			SHFILEOPSTRUCT op = { 0 };
			op.hwnd = GetSafeHwnd();
			op.wFunc = m_drag_copy ? FO_COPY : FO_MOVE;	//Ctrl=복사 / Shift=이동 / 드라이브 기준(is_drag_copy)
			op.pFrom = from_buf.c_str();
			op.pTo = to_buf.c_str();
			//20260705 by claude. 복사 시 대상에 같은 이름이 있으면 FOF_RENAMEONCOLLISION 으로 탐색기처럼 "… - 복사본"/"… (2)" 자동 리네임.
			op.fFlags = FOF_ALLOWUNDO | (m_drag_copy ? FOF_RENAMEONCOLLISION : 0);
			int move_rc = SHFileOperation(&op);


			//20260705 by claude. 복사는 소스 항목이 그대로 남으므로 refresh 로 사라진 선택을 원래 소스 항목으로 복원(사용자 기대: 복사
			//후에도 원본이 선택 유지). 이동은 소스가 사라져 복원 대상 없음.
			std::deque<CString> sel_names;
			if (m_drag_copy)
				for (auto& fd : m_transfer_list)
					sel_names.push_back(get_part(fd.cFileName, fn_name));

			//이동/복사 후 로컬 리스트만 새로고침. 트리는 호출부(트리 핸들러)가 갱신(이동=서피컬 소스제거/대상추가, 복사=대상 refresh).
			m_list_local.refresh_list(true, true);

			if (!sel_names.empty())
				m_list_local.select_items_by_names(sel_names);
			return;
		}
	}
	else
	{
		pListFile = &m_list_remote;
		pulDiskSpace = &m_ulServerDiskSpace;
		m_transfer_from = theApp.m_shell_imagelist.convert_special_folder_to_real_path(1, m_remoteCurrentPath);

		//remote에서 remote로 = 같은 client 머신 내 작업
		if (m_srcSide == m_dstSide)
		{
			//from 이 to 와 같으면 이동은 무동작. 복사(Ctrl)는 같은 폴더 '복사본' 이 유효하므로 통과.
			//20260704 by claude. 로컬과 동일 — copy 는 same-folder 도 허용.
			if (!m_drag_copy && m_transfer_from == m_transfer_to)
			{
				TRACE(_T("same folder. skip"));
				return;
			}

			//20260704 by claude. 리모트→리모트는 같은 client 머신 내 이동/복사 → 소켓 전송이 아니라 client 의 SHFileOperation 에 위임.
			//각 소스에 file_command(move/copy)를 보내 client 가 FO_MOVE/FO_COPY 로 처리(이름충돌 대화상자·크로스드라이브·되돌리기 네이티브).
			//Ctrl=복사, 기본=이동(기존 미구현이던 리모트 이동을 여기서 구현). 이동 시 자기 부모로 드롭(무동작)은 client SHFileOperation 이 안전 처리.
			int cmd = m_drag_copy ? file_cmd_copy : file_cmd_move;
			std::deque<CString> srcs;
			std::deque<CString> sel_names;	//20260705 by claude. 복사 후 원본 선택 복원용(로컬과 동일).
			for (auto& fd : m_transfer_list)
			{
				CString src = fd.cFileName;
				if (src.Find(_T(':')) < 0)		//리스트 d&d 는 이름만 저장 → 소스 폴더와 결합. 트리 d&d 는 이미 fullpath.
					src = concat_path(m_transfer_from, src);
				srcs.push_back(src);
				if (m_drag_copy)
					sel_names.push_back(get_part(fd.cFileName, fn_name));
			}
			m_ServerManager.m_socket.file_command(cmd, NULL, m_transfer_to, &srcs);	//배치: N개 소스 + 대상 폴더

			//대상/소스가 보이는 원격 리스트 갱신. 트리는 호출부(트리 핸들러)가 갱신.
			m_list_remote.refresh_list(true, true);

			if (!sel_names.empty())
				m_list_remote.select_items_by_names(sel_names);
			return;
		}
	}

	//[폴더 레벨 보존] 트리 드래그처럼 소스가 '단일 폴더이고 그 폴더 자신이 m_transfer_from'인 경우, 전송의 dst 는
	//각 파일 fullpath 에서 m_transfer_from → m_transfer_to 로 치환해 만든다(nFTDFileTransferDialog). from=A 면 A\child → to\child 가
	//되어 A 폴더 레벨이 사라진다. base 를 A 의 부모로 조정하면 A\child → to\A\child 로 폴더가 보존된다(리스트 드래그와 동일 모델).
	//로컬 move(same-side)는 위에서 이미 return 하므로 이 조정은 전송(cross-side)에만 적용된다.
	if (m_transfer_list.size() == 1 && m_transfer_from.CompareNoCase(m_transfer_list[0].cFileName) == 0)
		m_transfer_from = get_parent_dir(m_transfer_from);

	if (m_FileTransferDlg.FileTransferInitalize(&m_ServerManager, &m_transfer_list, pulDiskSpace,
												m_srcSide, m_dstSide, m_transfer_from, m_transfer_to, m_check_close_after_all.GetCheck()))
	{
		//20250319 scpark m_ServerManager.DataConnect(), m_ServerManager.DataClose()는 기존에는 m_FileTransferDlg 안에서 수행했으나
		//정상 전송 완료 후 또는 전송 취소 후에도 DataClose()를 수행해야 하는데 뭔가 중복처리되거나 오동작이 발생하여
		//m_FileTransferDlg.DoModal();을 하기 전에 연결하고 DoModal() 후에 닫는 방식으로 변경한다.
		//그래야만 끝까지 전송 완료되거나 도중에 취소한 경우에 무관하게 DataClose()가 수행된다.
		if (!m_ServerManager.DataConnect())
		{
			m_messagebox.DoModal(_S(NFTD_IDS_TRANSFER_INIT_FAIL));
			return;
		}

		m_transfered_names.clear();	//이번 전송분 이름 수집 초기화(전송 중 add_transfered_file_to_dst_list 가 채운다).
		m_FileTransferDlg.DoModal();
		m_ServerManager.DataClose();
		
		//전송이 모두 끝나면 해당 폴더를 refresh해준다.
		//change_directory(m_transfer_to, m_dstSide);
		if (m_dstSide == SERVER_SIDE)
		{
			m_tree_local.refresh(m_tree_local.GetSelectedItem());
			m_list_local.refresh_list(true, true);	//전송이 기존 파일을 덮어썼으면 제자리 쓰기(dir mtime 불변)라 캐시 우회 필요
			select_transfered_items(&m_list_local);	//리로드로 지워진 선택/스크롤을 전송된 항목 기준으로 복원
		}
		else if (m_dstSide == CLIENT_SIDE)
		{
			m_tree_remote.refresh(m_tree_remote.GetSelectedItem());
			m_list_remote.refresh_list(true, true);	//remote 는 캐시 미적용이라 무해하지만 일관성 위해 동일 처리
			select_transfered_items(&m_list_remote);	//리로드로 지워진 선택/스크롤을 전송된 항목 기준으로 복원
		}
	}
	else
	{
		CString message;
		message.LoadString(NFTD_IDS_SELECT_FILE);
	}
}

void CnFTDServerDlg::add_transfered_file_to_dst_list(int dstSide, WIN32_FIND_DATA data)
{
	CSCListCtrl* plist = (dstSide == SERVER_SIDE ? &m_list_local : &m_list_remote);
	plist->insert_item(-1, data, true, true);

	//전송 완료 후 refresh_list 로 리로드되면 이 삽입/선택이 지워지므로, 이름을 기록해뒀다가 refresh 뒤에 다시 선택한다.
	m_transfered_names.push_back(get_part(data.cFileName, fn_name));
}

//전송 완료 후(리스트가 refresh_list 로 리로드된 뒤) 이번에 전송된 항목들을 다시 선택하고 마지막 항목으로 스크롤한다.
//insert_item 은 선택을 하지 않고 refresh_list 가 그마저 리로드로 지우므로, 이름 매칭 선택 복원은 컨트롤의 select_items_by_names 에 위임한다.
void CnFTDServerDlg::select_transfered_items(CSCListCtrl* plist)
{
	plist->select_items_by_names(m_transfered_names);
}

//상황에 따라 송신, 수신이 불가능 할 경우의 처리를 위해.
bool CnFTDServerDlg::is_transfer_enable_for_list(int dwSide)
{
	CSCListCtrl* plist = (dwSide == SERVER_SIDE ? &m_list_local : &m_list_remote);

	//선택된 항목이 없다면 불가.
	std::deque<int> dq;
	plist->get_selected_items(&dq);

	if (dq.size() == 0)// && hItem == NULL)
		return false;

	for (int i = 0; i < dq.size(); i++)
	{
		CString path = plist->get_path(dq[i]);

		path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(dwSide, path);
		if (path.IsEmpty())
			return false;

		//상대가 내 PC를 열고 있다면 전송 불가
		if (dwSide == SERVER_SIDE && m_list_remote.get_path() == theApp.m_shell_imagelist.get_system_path(!dwSide, CSIDL_DRIVES))
			return false;

		//상대가 내 PC를 열고 있다면 전송 불가
		if (dwSide == CLIENT_SIDE && m_list_local.get_path() == theApp.m_shell_imagelist.get_system_path(!dwSide, CSIDL_DRIVES))
			return false;

		//드라이브 루트·주요 시스템 폴더(및 하위)는 전송 불가. 판정은 is_protected 단일 출처로 통합.
		if (!theApp.m_shell_imagelist.is_copyable_from(dwSide, path))
			return false;
	}

	return true;
}

//상황에 따라 송신, 수신이 불가능 할 경우의 처리를 위해.
bool CnFTDServerDlg::is_transfer_enable_for_tree(int dwSide)
{
	CSCTreeCtrl* ptree = (dwSide == SERVER_SIDE ? &m_tree_local : &m_tree_remote);
	CSCListCtrl* plist = (dwSide == SERVER_SIDE ? &m_list_local : &m_list_remote);

	//선택된 항목이 없다면 불가.
	CString path = ptree->get_path();

	path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(dwSide, path);
	if (path.IsEmpty())
		return false;

	//내 PC에서 우클릭 한 경우는 전송 불가
	if (path == theApp.m_shell_imagelist.get_system_path(dwSide, CSIDL_DRIVES))
		return false;

	//상대가 내 PC를 열고 있다면 전송 불가
	if (dwSide == SERVER_SIDE && m_list_remote.get_path() == theApp.m_shell_imagelist.get_system_path(!dwSide, CSIDL_DRIVES))
		return false;

	//상대가 내 PC를 열고 있다면 전송 불가
	if (dwSide == CLIENT_SIDE && m_list_local.get_path() == theApp.m_shell_imagelist.get_system_path(!dwSide, CSIDL_DRIVES))
		return false;

	//드라이브 루트·주요 시스템 폴더(및 하위)는 전송 불가. 판정은 is_protected 단일 출처로 통합.
	if (!theApp.m_shell_imagelist.is_copyable_from(dwSide, path))
		return false;

	return true;
}

//전송 대상은 리스트 선택 항목 또는(선택이 없으면) 트리에서 선택한 현재 폴더다.
//리스트에 선택이 있으면 그 항목들을, 없으면 트리의 현재 폴더를 기준으로 전송 가능 여부를 판정한다.
bool CnFTDServerDlg::is_transfer_enable(int dwSide)
{
	CSCListCtrl* plist = (dwSide == SERVER_SIDE ? &m_list_local : &m_list_remote);

	if (plist->GetSelectedCount() > 0)
		return is_transfer_enable_for_list(dwSide);

	return is_transfer_enable_for_tree(dwSide);
}

//리스트에서 선택된 항목 중 하나라도 보호(삭제/이름변경 금지) 대상이면 true. 다중선택 삭제/이름변경 방어.
//(우클릭한 단일 항목만 검사하면 정상+보호 항목을 함께 선택 후 정상 항목을 우클릭해 보호 항목까지 삭제되는 구멍이 있다.)
bool CnFTDServerDlg::any_selected_item_protected(int dwSide)
{
	CSCListCtrl* plist = (dwSide == SERVER_SIDE ? &m_list_local : &m_list_remote);

	std::deque<int> dq;
	plist->get_selected_items(&dq);

	for (int i = 0; i < dq.size(); i++)
		if (!theApp.m_shell_imagelist.is_movable(dwSide, plist->get_path(dq[i])))
			return true;

	return false;
}

//트리 우클릭 메뉴(로컬/원격 공통). WM_CONTEXTMENU 로 위임받아 호출된다. side: SERVER_SIDE/CLIENT_SIDE, point: 화면좌표.
void CnFTDServerDlg::show_tree_context_menu(int side, CPoint point)
{
	CSCTreeCtrl& tree = (side == SERVER_SIDE) ? m_tree_local : m_tree_remote;

	CPoint pt = point;
	tree.ScreenToClient(&pt);
	HTREEITEM hItem = tree.HitTest(pt);

	if (!hItem)
		return;

	//우클릭을 하면 일단 해당 노드를 선택상태로 만들어줘야 한다.
	tree.SelectItem(hItem);

	CMenu menu;
	menu.LoadMenu(IDR_MENU_TREE_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	CString path = tree.get_path();

	//즐겨찾기에 등록된 폴더인지 확인한다.
#if (!defined(_REMOTE_SDK) && !defined(_ANYSUPPORT))
	int favorite_index = favorite_cmd(favorite_find, side, path);
	pMenu->ModifyMenu(ID_TREE_CONTEXT_MENU_FAVORITE, MF_BYCOMMAND, ID_TREE_CONTEXT_MENU_FAVORITE, (favorite_index >= 0 ? _S(IDS_FAVORITE_REMOVE) : _S(IDS_FAVORITE_ADD)) + _T("(&F)"));
#else
	//pMenu->EnableMenuItem(ID_TREE_CONTEXT_MENU_FAVORITE, MF_DISABLED);
	pMenu->DeleteMenu(ID_TREE_CONTEXT_MENU_FAVORITE, MF_BYCOMMAND);
#endif

	pMenu->ModifyMenu(ID_TREE_CONTEXT_MENU_SEND, MF_BYCOMMAND, ID_TREE_CONTEXT_MENU_SEND, _S(IDS_TRANSFER_START) + _T("(&S)"));
	pMenu->ModifyMenu(ID_TREE_CONTEXT_MENU_OPEN_EXPLORER, MF_BYCOMMAND, ID_TREE_CONTEXT_MENU_OPEN_EXPLORER, _S(IDS_OPEN_WITH_EXPLORER) + _T("(&E)"));
	pMenu->ModifyMenu(ID_TREE_CONTEXT_MENU_REFRESH, MF_BYCOMMAND, ID_TREE_CONTEXT_MENU_REFRESH, _S(IDS_REFRESH) + _T("\tF5"));
	pMenu->ModifyMenu(ID_TREE_CONTEXT_MENU_NEW_FOLDER, MF_BYCOMMAND, ID_TREE_CONTEXT_MENU_NEW_FOLDER, _S(IDS_NEW_FOLDER) + _T("(&N)"));
	pMenu->ModifyMenu(ID_TREE_CONTEXT_MENU_PROPERTY, MF_BYCOMMAND, ID_TREE_CONTEXT_MENU_PROPERTY, _S(IDS_PROPERTY) + _T("(&R)"));

	//현재 경로가 "내 PC"인 경우는 즐겨찾기를 지원하지 않는다.
	if (path == theApp.m_shell_imagelist.m_volume[side].get_label(CSIDL_DRIVES))
		pMenu->EnableMenuItem(ID_TREE_CONTEXT_MENU_FAVORITE, MF_DISABLED);

	//tree에서는 빈 공간 클릭, 즉 선택된 항목이 없을 경우에 대한 처리는 불필요하다.

	//보호된 파일/폴더(드라이브 루트·주요 시스템 폴더)일 경우 삭제/이름변경 비활성.
	//is_protected 는 실제 경로 기준이므로 get_path()(특수폴더 표시형)를 real path 로 변환해 판정한다.
	CString real_path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, path);
	if (!theApp.m_shell_imagelist.is_movable(side, real_path))
	{
		pMenu->EnableMenuItem(ID_TREE_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_TREE_CONTEXT_MENU_RENAME, MF_DISABLED);
	}

	//전송 가능 상태가 아닐 경우(ex. 상대편이 "내 PC"를 열고 있다면 전송 불가 등)
	pMenu->EnableMenuItem(ID_TREE_CONTEXT_MENU_SEND, (is_transfer_enable_for_tree(side) ? MF_ENABLED : MF_DISABLED));

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


//리스트 우클릭 메뉴(로컬/원격 공통). WM_CONTEXTMENU 로 위임받아 호출된다. side: SERVER_SIDE/CLIENT_SIDE, point: 화면좌표.
void CnFTDServerDlg::show_list_context_menu(int side, CPoint point)
{
	CSCListCtrl& list = (side == SERVER_SIDE) ? m_list_local : m_list_remote;

	//우클릭 위치의 항목(-1 = 빈 영역). 기존 NM_RCLICK 의 iItem 을 HitTest 로 대체.
	CPoint pt = point;
	list.ScreenToClient(&pt);
	int item = list.HitTest(pt);

	CMenu menu;
	menu.LoadMenu(IDR_MENU_LIST_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	CString fullpath;

	std::deque<WIN32_FIND_DATA> dq;
	list.get_selected_items(&dq);

	//선택된 항목이 없으면 현재 경로를 취한다.
	//멀티 선택이라면 0번 항목을 대상으로 한다.
	if (dq.size() == 0)
		fullpath = list.get_path();
	else
		fullpath = dq[0].cFileName;

	//파일 또는 폴더에서 우클릭하므로 즐겨찾기에 등록된 폴더인지는 폴더 경로로 비교해야 한다.
#if (!defined(_REMOTE_SDK) && !defined(_ANYSUPPORT))
	int favorite_index;
	if (dq.size() == 0 || !(dq[0].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		favorite_index = favorite_cmd(favorite_find, side, list.get_path());
	else
		favorite_index = favorite_cmd(favorite_find, side, dq[0].cFileName);

	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_FAVORITE, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_FAVORITE, (favorite_index >= 0 ? _S(IDS_FAVORITE_REMOVE) : _S(IDS_FAVORITE_ADD)) + _T("(&F)"));
#else
	//pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_FAVORITE, MF_DISABLED);
	pMenu->DeleteMenu(ID_LIST_CONTEXT_MENU_FAVORITE, MF_BYCOMMAND);
#endif

	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_SEND, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_SEND, _S(IDS_TRANSFER_START) + _T("(&S)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_OPEN, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_OPEN, _S(IDS_OPEN) + _T("(&O)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_OPEN_EXPLORER, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_OPEN_EXPLORER, _S(IDS_OPEN_WITH_EXPLORER) + _T("(&E)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_REFRESH, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_REFRESH, _S(IDS_REFRESH) + _T("\tF5"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_NEW_FOLDER, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_NEW_FOLDER, _S(IDS_NEW_FOLDER) + _T("(&N)"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_DELETE, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_DELETE, _S(IDS_DELETE) + _T("(&D)") + _T("\tDel"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_RENAME, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_RENAME, _S(IDS_RENAME) + _T("(&M)") + _T("\tF2"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_SELECT_ALL, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_SELECT_ALL, _S(IDS_SELECT_ALL) + _T("(&A)") + _T("\tCtrl+A"));
	pMenu->ModifyMenu(ID_LIST_CONTEXT_MENU_PROPERTY, MF_BYCOMMAND, ID_LIST_CONTEXT_MENU_PROPERTY, _S(IDS_PROPERTY) + _T("(&R)"));

	//"내 PC"(드라이브 목록) 뷰에서 "내 PC" 자체는 즐겨찾기 대상이 아니다. 단 선택된 드라이브가 있으면 그 드라이브
	//루트를 즐겨찾기할 수 있으므로(트리와 동일), 선택이 없을 때만 비활성화한다.
	if (list.get_path() == theApp.m_shell_imagelist.m_volume[side].get_label(CSIDL_DRIVES) && dq.size() == 0)
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_FAVORITE, MF_DISABLED);

	//선택된 항목이 없을 경우
	if (item == -1)
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_SEND, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_OPEN, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_RENAME, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_PROPERTY, MF_DISABLED);
	}

	//전송 가능 상태가 아닐 경우(ex. 상대편이 "내 PC"를 열고 있다면 전송 불가 등)
	pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_SEND, (is_transfer_enable_for_list(side) ? MF_ENABLED : MF_DISABLED));

	//보호된 파일/폴더일 경우
	if (any_selected_item_protected(side))
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_DELETE, MF_DISABLED);
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_RENAME, MF_DISABLED);
	}

	//멀티 선택일 경우는 "열기" 명령을 수행하면 맨 첫 아이템에 대한 "열기"를 수행하지만 좀 애매하다. 우선 제외시킨다.
	if (list.GetSelectedCount() > 1)
	{
		pMenu->EnableMenuItem(ID_LIST_CONTEXT_MENU_OPEN, MF_DISABLED);
	}

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CnFTDServerDlg::OnListContextMenuSend()
{
	m_srcSide = (GetFocus() == &m_list_remote);
	m_dstSide = !m_srcSide;

	if (m_srcSide == SERVER_SIDE)
	{
		m_list_local.get_selected_items(&m_transfer_list);
		m_transfer_from = m_list_local.get_path();
		m_transfer_to = m_list_remote.get_path();
	}
	else
	{
		m_list_remote.get_selected_items(&m_transfer_list);
		m_transfer_from = m_list_remote.get_path();
		m_transfer_to = m_list_local.get_path();
	}

	TRACE(_T("m_srcSide = %d, send %d items from %s to %s\n"), m_srcSide, m_transfer_list.size(), m_transfer_from, m_transfer_to);
	file_transfer();
}


void CnFTDServerDlg::OnListContextMenuNewFolder()
{
	file_command_on_list(file_cmd_new_folder);
}


void CnFTDServerDlg::OnListContextMenuRename()
{
	file_command_on_list(file_cmd_rename);
}


void CnFTDServerDlg::OnListContextMenuDelete()
{
	file_command_on_list(file_cmd_delete);
}


void CnFTDServerDlg::OnListContextMenuRefresh()
{
	file_command_on_list(file_cmd_refresh);
}


void CnFTDServerDlg::OnListContextMenuSelectAll()
{
	CSCListCtrl* plist = ((GetFocus() == &m_list_local ? &m_list_local : &m_list_remote));
	plist->select_item(-1);
}


void CnFTDServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_init_remote_controls)
	{
		//TRACE(_T("timer0. start\n"));
		if (m_ServerManager.is_connected() == false)
			return;

		KillTimer(timer_init_remote_controls);
		//TRACE(_T("killtimer0.\n"));

		initialize();
		Invalidate();

		m_show_main_ui = true;
		m_progressDlg.ShowWindow(SW_HIDE);
	}
	else if (nIDEvent == timer_init_favorites)
	{
		KillTimer(timer_init_favorites);

		init_favorite();

		int i;
		CString path;

		for (i = 0; i < m_list_local_favorite.size(); i++)
		{
			path = m_list_local_favorite.get_text(i, 1);
			if (!PathFileExists(path))
				m_list_local_favorite.set_text_color(i, -1, Gdiplus::Color::Firebrick);
		}

		for (i = 0; i < m_list_remote_favorite.size(); i++)
		{
			path = m_list_remote_favorite.get_text(i, 1);

			//remote에 해당 경로가 존재하는지 검사하기 위해 해당 폴더내의 파일/폴더를 검색시켜서
			//그 결과값으로 판단한다.
			std::deque<WIN32_FIND_DATA> dq;
			if (!m_ServerManager.get_folderlist(path, &dq, false))
				m_list_remote_favorite.set_text_color(i, -1, Gdiplus::Color::Firebrick);
		}
	}
	else if (nIDEvent == timer_refresh_selection_status)
	{
		KillTimer(timer_refresh_selection_status);

		//변경을 발생시킨 리스트를 갱신한다. GetFocus() 로 추측하면 첫 실행 직후 포커스가 아직 리스트에 없는 순간
		//갱신이 스킵되어 전송버튼이 enable 되지 않는 버그가 있었다(맨 처음 1회만 재현).
		if (m_selection_status_target)
			refresh_selection_status(m_selection_status_target);
	}
	else if (nIDEvent == timer_init_progress_and_connect)
	{
		KillTimer(timer_init_progress_and_connect);

		//tree에서 폴더를 펼칠 때 특정 폴더들은 그 갯수가 몇천개를 넘는 경우도 있는데
		//이를 프로그래스로 알려주는 컨트롤은 CResizeCtrl로 arrange하지 않고 수동으로 adjust하도록 되어 있다.
		//트리와 즐겨찾기 컨트롤의 ControlSplitter의 위치를 공통으로 사용하는데
		//프로그램 시작 시 그 위치가 정렬되지 않은 상태이므로 여기서 그 위치를 맞춰주기 위해 호출한다.
		adjust_processing_progress_ctrl();


		init_progressDlg();

		//20241212 scpark thread_connect()에서 접속 시도 후 initialize()를 통해 remote의 CPathCtrl, CTreeCtrl, CListCtrl등을 표시하는데
		//MFC 관련 오류가 발생한다. std::thread에서도 컨트롤에 대한 일반적인 액션들은 대부분 가능하나 생성과 관련된 뭔가 문제를 일으키는 듯 하다.
		//따라서 connection만 thread로 돌리고 connected후에 컨트롤을 초기화하는 코드는 thread밖에서 처리한다.
		std::thread t(&CnFTDServerDlg::thread_connect, this);
		t.detach();
		//thread_connect();

		SetTimer(timer_init_remote_controls, 1000, NULL);
	}

	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CnFTDServerDlg::on_message(WPARAM wParam, LPARAM lParam)
{
	m_messagebox.DoModal(_S(IDS_CONNECT_FAIL_NO_RESPONSE), MB_OK);
	CDialogEx::OnCancel();

	return 0;
}

bool CnFTDServerDlg::file_command_on_list(int cmd, CString param0, CString param1)
{
	bool res = false;
	int dwSide = SERVER_SIDE;
	CSCListCtrl* plist;
	CSCTreeCtrl* ptree;
	
	if (GetFocus() == &m_list_local || GetFocus() == &m_tree_local)
	{
		dwSide = SERVER_SIDE;
		plist = &m_list_local;
		ptree = &m_tree_local;
	}
	else if (GetFocus() == &m_list_remote || GetFocus() == &m_tree_remote)
	{
		dwSide = CLIENT_SIDE;
		plist = &m_list_remote;
		ptree = &m_tree_remote;
	}
	else
	{
		return false;
	}

	std::deque<int> dq;
	plist->get_selected_items(&dq);
	//CString file;

	if (param0.IsEmpty())
	{
		if (dq.size() == 0)
			param0 = plist->get_path();
		else
			param0.Format(_T("%s\\%s"), plist->get_path(), plist->get_text(dq[0], CSCListCtrl::col_filename));
	}

	param0 = theApp.m_shell_imagelist.convert_special_folder_to_real_path(dwSide, param0);

	//이러한 처리를 기존처럼 m_ServerManager의 open_file()함수에서 일괄 처리하도록 구현할 수도 있으나
	//m_ServerManager에서는 CnFTDServerDlg에 있는 tree, list 등을 접근하자면 별도의 처리가 필요하므로
	//local은 그냥 이 클래스에서 처리하고 remote인 경우에만 m_ServerManager의 함수를 호출하여 처리한다.
	if (dwSide == SERVER_SIDE)
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
				param0 = plist->get_path();

				//내 PC가 선택된 경우 param0도 "내 PC"라는 값을 가지는데 그대로 열면 "문서" 폴더가 열린다.
				//empty로 만들어줘고 열어야 실제 "내 PC"가 탐색기로 열린다.
				if (param0 == theApp.m_shell_imagelist.m_volume[0].get_label(CSIDL_DRIVES))
					param0 = _T("");

				ShellExecute(NULL, _T("open"), _T("explorer"), param0, 0, SW_SHOWNORMAL);
				break;
			case file_cmd_refresh :
				plist->refresh_list(true, true);	//명시적 새로고침 — 폴더 캐시 우회(제자리 편집 stale 방지)
				{
					//선택 노드의 실제 폴더가 사라졌으면(외부 rename/삭제) 부모를 refresh 해서 stale 노드 제거 + 실제 형제 반영.
					//(부모를 안 건드리면 사라진 선택 노드만 refresh 되어 이름변경된 폴더가 계속 안 보이던 간헐 버그.)
					HTREEITEM hSel = ptree->GetSelectedItem();
					HTREEITEM hSelParent = ptree->GetParentItem(hSel);
					CString sel_real = theApp.m_shell_imagelist.convert_special_folder_to_real_path(SERVER_SIDE, ptree->get_path(hSel));
					if (hSelParent != NULL && !sel_real.IsEmpty() && !PathFileExists(sel_real))
						ptree->refresh(hSelParent);
					else
						ptree->refresh(hSel);
				}
				break;
			case file_cmd_new_folder :
				{
					//list의 현재 폴더에 새 폴더를 생성한다.
					CString new_folder = plist->new_folder(_S(IDS_NEW_FOLDER));
					if (!new_folder.IsEmpty())
					{
						res = true;
						//refresh하면 간단하지만 깜빡임이 발생한다.
						//m_tree_local.refresh(m_tree_local.GetSelectedItem());

						//새 항목이 추가되면 expand시켜준다.
						UINT state = m_tree_local.GetItemState(m_tree_local.GetSelectedItem(), TVIS_EXPANDED);
						if (!(state & TVIS_EXPANDED))
							m_tree_local.Expand(m_tree_local.GetSelectedItem(), TVE_EXPAND);
						
						//expand 상태가 아니거나 처음부터 child가 없던 node라면 노드를 추가시킨다.
						if ((state & TVIS_EXPANDED) || m_tree_local.GetChildItem(m_tree_local.GetSelectedItem()) == NULL)
							m_tree_local.add_new_item(m_tree_local.GetSelectedItem(), new_folder);
					}
					else
					{
						res = false;
					}
				}
				break;
			case file_cmd_rename :
				m_dir_watcher.stop();
				m_list_local.edit_item(dq[0], CSCListCtrl::col_filename);
				//여기서는 편집모드로만 들어가고 다시 watching은 편집이 종료된 시점에 재개해야 한다.
				res = true;
				break;
			case file_cmd_delete :
				{
					//20260705 by claude. 삭제 중 워처 정지. delete_file(SHFileOperation)은 내부 메시지 펌프를 돌리는데, 그 사이 디렉터리
					//워처가 refresh 를 재진입시켜 m_list_db 를 재구성하면 아래 루프의 index/delete_item 의 erase 가 어긋나 크래시한다.
					//(리스트는 LVS_OWNERDATA 라 m_list_db 가 곧 데이터. 재구성되면 stale index 로 erase → end() 초과.) 끝에 rewatch_local() 로 재개.
					m_dir_watcher.stop();

					int deleted_count = 0;

					EnableWindow(FALSE);

					m_toast_popup.load(this, _T("GIF"), IDR_GIF_DELETE);
					m_toast_popup.CenterWindow(this);
					m_toast_popup.ShowWindow(SW_SHOW);

					for (auto item : dq)
					{
						//item 위치의 항목을 제거하면 그 뒤 item들의 index는 당겨져야 한다.
						//또는 제거 목록을 역순으로 제거해야 한다.
						item -= deleted_count;

						res = m_list_local.delete_item(item, true);

						if (res)
							deleted_count++;

						Wait(1);
					}

					m_toast_popup.ShowWindow(SW_HIDE);
					EnableWindow(TRUE);

					m_tree_local.refresh(m_tree_local.GetSelectedItem());

					refresh_disk_usage(false);
					refresh_selection_status(&m_list_local);
					
					rewatch_local();
				}
				break;
			case file_cmd_property :
			{
				std::deque<CString> dq_fullpath;
				m_list_local.get_selected_items(&dq_fullpath, true);
				logWrite(_T("multiple file property. %d files"), dq_fullpath.size());
				//20260712 by claude. 중괄호 없는 stray for 제거 — 선택 항목 수(N)만큼 show_property_window 가 반복 호출돼
				//속성창을 닫아도 다음 것이 또 뜨던 버그(회귀). show_property_window 는 dq_fullpath 전체를 받아 1회로 다중 속성 표시.
				res = show_property_window(dq_fullpath);
			}
			break;
			case file_cmd_favorite :
				{
					//존재하는 항목이면 제거, 없던 항목이면 추가한다.
					int index = favorite_cmd(favorite_find, SERVER_SIDE, param0);
					if (index < 0)
						favorite_cmd(favorite_add, SERVER_SIDE, param0);
					else
						favorite_cmd(favorite_delete, SERVER_SIDE, param0);
				}
				break;
		}
	}
	else
	{
		//bool viewer_is_running = (get_process_running_count(_T("C:\\Users\\Public\\Documents\\LinkMeMineSE\\LMMViewer\\LMMLauncher\\LMMViewer.exe")) > 0);

		switch (cmd)
		{
			case file_cmd_open:
				//폴더인 경우
				if (m_list_remote.get_text(dq[0], CSCListCtrl::col_filesize).IsEmpty())
				{
					res = change_directory(param0, CLIENT_SIDE);
				}
				//파일인 경우
				else
				{
					res = m_ServerManager.m_socket.file_command(file_cmd_open, param0);
					if (res)// && !viewer_is_running)
					{
						m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
						m_toast_popup.CenterWindow(this);
						m_toast_popup.fade_in(10, 2000, true);
					}
				}
				break;
			case file_cmd_open_explorer :
				param0 = m_list_remote.get_path();
				res = m_ServerManager.m_socket.file_command(file_cmd_open_explorer, param0);
				if (res)// && !viewer_is_running)
				{
					m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
					m_toast_popup.CenterWindow(this);
					m_toast_popup.fade_in(10, 2000, true);
				}
				break;
			case file_cmd_refresh:
				plist->refresh_list(true, true);	//명시적 새로고침 — 폴더 캐시 우회(제자리 편집 stale 방지)
				ptree->refresh(ptree->GetSelectedItem());
				break;
			case file_cmd_new_folder:
			{
				CString new_folder = plist->new_folder(_S(IDS_NEW_FOLDER));

				if (!new_folder.IsEmpty())
				{
					//새 항목이 추가되면 expand시켜준다.
					UINT state = m_tree_remote.GetItemState(m_tree_remote.GetSelectedItem(), TVIS_EXPANDED);
					if (!(state & TVIS_EXPANDED))
						m_tree_remote.Expand(m_tree_remote.GetSelectedItem(), TVE_EXPAND);

					//expand 상태가 아니거나 처음부터 child가 없던 node라면 노드를 추가시킨다.
					if ((state & TVIS_EXPANDED) || m_tree_remote.GetChildItem(m_tree_remote.GetSelectedItem()) == NULL)
						m_tree_remote.add_new_item(m_tree_remote.GetSelectedItem(), new_folder);
				}
			}
			break;
			case file_cmd_rename:
				m_list_remote.edit_item(dq[0], CSCListCtrl::col_filename);
				res = true;
				break;
			case file_cmd_delete:
				{
					int deleted_count = 0;

					EnableWindow(FALSE);

					m_toast_popup.load(this, _T("GIF"), IDR_GIF_DELETE);
					m_toast_popup.CenterWindow(this);
					m_toast_popup.ShowWindow(SW_SHOW);

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

						Wait(1);
					}

					m_toast_popup.ShowWindow(SW_HIDE);
					EnableWindow(TRUE);

					m_tree_remote.refresh(m_tree_remote.GetSelectedItem());

					refresh_disk_usage(true);
					refresh_selection_status(&m_list_remote);
				}
				break;
			case file_cmd_property:
			{
				std::deque<CString> dq_fullpath;
				m_list_remote.get_selected_items(&dq_fullpath, true);

				logWrite(_T("multiple file property. %d files"), dq_fullpath.size());
				//20260712 by claude. 중괄호 없는 stray for 제거 — 선택 항목 수(N)만큼 file_command(property)가 반복 전송돼
				//원격 속성창을 닫아도 다음 것이 또 뜨던 버그(회귀, 68160293). 다중 선택은 dq_fullpath 를 한 번에 넘겨 1회 전송.
				if (dq_fullpath.size())
					res = m_ServerManager.m_socket.file_command(file_cmd_property, 0, 0, &dq_fullpath);
				else
					res = true;

				if (res)// && !viewer_is_running)
				{
					m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
					m_toast_popup.CenterWindow(this);
					m_toast_popup.fade_in(10, 2000, true);
				}
			}
			break;
			case file_cmd_favorite:
				{
					//존재하는 항목이면 제거, 없던 항목이면 추가한다.
					int index = favorite_cmd(favorite_find, CLIENT_SIDE, param0);
					if (index < 0)
						favorite_cmd(favorite_add, CLIENT_SIDE, param0);
					else
						favorite_cmd(favorite_delete, CLIENT_SIDE, param0);
				}
				break;
		}
	}

	return res;
}

bool CnFTDServerDlg::file_command_on_tree(int cmd, CString param0, CString param1)
{
	bool res = false;
	int dwSide = SERVER_SIDE;
	CSCTreeCtrl* ptree;
	CSCListCtrl* plist;
	HTREEITEM hItem = NULL;

	if (GetFocus() == &m_tree_local)
	{
		dwSide = SERVER_SIDE;
		ptree = &m_tree_local;
		plist = &m_list_local;
	}
	else if (GetFocus() == &m_tree_remote)
	{
		dwSide = CLIENT_SIDE;
		ptree = &m_tree_remote;
		plist = &m_list_remote;
	}
	else
	{
		return false;
	}


	if (param0.IsEmpty())
		param0 = ptree->get_path();

	param0 = theApp.m_shell_imagelist.convert_special_folder_to_real_path(dwSide, param0);

	//이러한 처리를 기존처럼 m_ServerManager의 open_file()함수에서 일괄 처리하도록 구현할 수도 있으나
	//m_ServerManager에서는 CnFTDServerDlg에 있는 tree, list 등을 접근하자면 별도의 처리가 필요하므로
	//local은 그냥 이 클래스에서 처리하고 remote인 경우에만 m_ServerManager의 함수를 호출하여 처리한다.
	if (dwSide == SERVER_SIDE)
	{
		switch (cmd)
		{
			case file_cmd_open_explorer :
				//내 PC가 선택된 경우 param0도 "내 PC"라는 값을 가지는데 그대로 열면 "문서" 폴더가 열린다.
				//empty로 만들어줘고 열어야 실제 "내 PC"가 탐색기로 열린다.
				if (param0 == theApp.m_shell_imagelist.m_volume[0].get_label(CSIDL_DRIVES))
					param0 = _T("");

				ShellExecute(NULL, _T("open"), _T("explorer"), param0, 0, SW_SHOWNORMAL);
				break;
			case file_cmd_new_folder :
				//20260712 by claude. 새 폴더 생성 중 워처 정지 — add_new_item 이 노드를 직접 넣는데, 내부 CreateDirectory 가 워처
				//ADDED 를 유발해 여기서 또 삽입되면 "새 폴더 (2)"가 중복됐다(워처 live ADD 도입 6e5df8fc 2026-07-03 이후 회귀).
				//삭제 경로와 동일 패턴(stop → 작업 → rewatch). stop 은 감시 핸들을 닫아 CreateDirectory 의 ADD 가 캡처조차 안 돼 확실.
				m_dir_watcher.stop();
				hItem = ptree->add_new_item(NULL, _S(IDS_NEW_FOLDER), true, true);
				plist->insert_folder(-1, ptree->GetItemText(hItem), false);
				rewatch_local();
				break;
			case file_cmd_refresh :
				{
					//선택 노드의 실제 폴더가 사라졌으면(외부에서 rename/삭제됨) 그 노드만 refresh 해선 stale 노드가 남고
					//실제 폴더(새 이름)가 안 보인다 → 부모를 refresh 해서 stale 제거 + 실제 형제 반영.
					HTREEITEM hSel = ptree->GetSelectedItem();
					HTREEITEM hSelParent = ptree->GetParentItem(hSel);
					CString sel_real = theApp.m_shell_imagelist.convert_special_folder_to_real_path(SERVER_SIDE, ptree->get_path(hSel));
					if (hSelParent != NULL && !sel_real.IsEmpty() && !PathFileExists(sel_real))
						ptree->refresh(hSelParent);
					else
						ptree->refresh(hSel);
				}
				plist->refresh_list(true, true);	//명시적 새로고침 — 폴더 캐시 우회(제자리 편집 stale 방지)
				break;
			case file_cmd_property :
				show_property_window(std::deque<CString> {param0});
				break;
			case file_cmd_favorite :
			{
				//존재하는 항목이면 제거, 없던 항목이면 추가한다.
				int index = favorite_cmd(favorite_find, SERVER_SIDE, param0);
				if (index < 0)
					favorite_cmd(favorite_add, SERVER_SIDE, param0);
				else
					favorite_cmd(favorite_delete, SERVER_SIDE, param0);
			}
			break;
		}
	}
	else
	{
		switch (cmd)
		{
			case file_cmd_open_explorer:
				res = m_ServerManager.m_socket.file_command(file_cmd_open_explorer, param0);
				if (res)// && !viewer_is_running)
				{
					m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
					m_toast_popup.CenterWindow(this);
					m_toast_popup.fade_in(10, 2000, true);
				}
				break;
			case file_cmd_new_folder:
				logWrite(_T("[newfolder] REMOTE start -> add_new_item"));	//20260712 by claude. [diag temp] 테스트 중 유지 — 최종 push 시 제거.
				hItem = ptree->add_new_item(NULL, _S(IDS_NEW_FOLDER), true, true);
				logWrite(_T("[newfolder] REMOTE add_new_item DONE -> insert_folder"));
				plist->insert_folder(-1, ptree->GetItemText(hItem), true);
				logWrite(_T("[newfolder] REMOTE insert_folder DONE"));
				break;
			case file_cmd_refresh:
				ptree->refresh(ptree->GetSelectedItem());
				plist->refresh_list(true, true);	//명시적 새로고침 — 폴더 캐시 우회(제자리 편집 stale 방지)
				break;
			case file_cmd_property:
			{
				std::deque<CString> dq_fullpath;
				dq_fullpath.push_back(param0);
				bool res = m_ServerManager.m_socket.file_command(file_cmd_property, 0, 0, &dq_fullpath);

				if (res)// && !viewer_is_running)
				{
					m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
					m_toast_popup.CenterWindow(this);
					m_toast_popup.fade_in(10, 2000, true);
				}
				else
				{
				}
				/*
				std::deque<CString> dq_fullpath;
				m_list_remote.get_selected_items(&dq_fullpath, true);

				logWrite(_T("multiple file property. %d files"), dq_fullpath.size());
				for (auto item : dq_fullpath)

				if (dq_fullpath.size())
					res = m_ServerManager.m_socket.file_command(file_cmd_property, 0, 0, &dq_fullpath);
				else
					res = true;

				if (res)// && !viewer_is_running)
				{
					m_toast_popup.set_text(_S(IDS_SHOW_ON_REMOTE));
					m_toast_popup.CenterWindow(this);
					m_toast_popup.fade_in(10, 2000, true);
				}
				*/
			}
			break;
			case file_cmd_favorite:
			{
				//존재하는 항목이면 제거, 없던 항목이면 추가한다.
				int index = favorite_cmd(favorite_find, CLIENT_SIDE, param0);
				if (index < 0)
					favorite_cmd(favorite_add, CLIENT_SIDE, param0);
				else
					favorite_cmd(favorite_delete, CLIENT_SIDE, param0);
			}
			break;
		}
	}

	return res;
}

void CnFTDServerDlg::refresh_selection_status(bool local)
{
	if (local)
		refresh_selection_status(&m_list_local);
	else
		refresh_selection_status(&m_list_remote);
}

//목록, 선택 정보가 변경되면 상태표시줄을 갱신한다.
void CnFTDServerDlg::refresh_selection_status(CSCListCtrl* plist)
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
		{
			folder_included = true;
		}
		else
		{
			ULARGE_INTEGER file_size;
			file_size.LowPart = item.nFileSizeLow;
			file_size.HighPart = item.nFileSizeHigh;
			total_size.QuadPart += file_size.QuadPart;
		}
	}

	if (dq.size())
	{
		if (folder_included)
			str.Format(_T("%s%s     %s%s"), i2S(plist->size(), true), _S(IDS_ITEM_COUNT), i2S(dq.size(), true), _S(IDS_ITEM_SELECTED_COUNT));
		else
			str.Format(_T("%s%s     %s%s  %s"), i2S(plist->size(), true), _S(IDS_ITEM_COUNT), i2S(dq.size(), true), _S(IDS_ITEM_SELECTED_COUNT), get_size_str(total_size.QuadPart, -1, 2));
	}
	else
	{
		str.Format(_T("%s%s"), i2S(plist->size(), true), _S(IDS_ITEM_COUNT));
	}

	if (plist == &m_list_local)
	{
		m_static_count_local.set_text(str);

		//선택여부에 따라 송수신 버튼 상태도 변한다. 전송 대상은 리스트 선택 또는 트리 현재 폴더이므로 통합 판정한다.
		bool enable = is_transfer_enable(SERVER_SIDE);
		m_button_local_to_remote.EnableWindow(enable);
		//disable 사유가 "보호 폴더/드라이브 루트 선택"이면 그 사유를 hover 툴팁으로 안내. enable 이면 툴팁 비움.
		m_tooltip.UpdateTipText(enable ? _T("") : get_transfer_block_reason(SERVER_SIDE), &m_button_local_to_remote);
	}
	else if (plist == &m_list_remote)
	{
		m_static_count_remote.set_text(str);

		//선택여부에 따라 송수신 버튼 상태도 변한다. 전송 대상은 리스트 선택 또는 트리 현재 폴더이므로 통합 판정한다.
		bool enable = is_transfer_enable(CLIENT_SIDE);
		m_button_remote_to_local.EnableWindow(enable);
		m_tooltip.UpdateTipText(enable ? _T("") : get_transfer_block_reason(CLIENT_SIDE), &m_button_remote_to_local);
	}
}

//리스트에서 선택한 전송 대상 중 보호(전송 금지) 대상이 있으면 그 사유 문자열을, 없으면 빈 문자열을 리턴한다.
//선택 항목이 없으면(=전송할 파일 미선택) 버튼이 비활성이어도 "보호 폴더" 사유가 아니므로 빈 문자열을 리턴한다.
//즉 이 툴팁은 사용자가 실제로 보호 대상을 "선택"했을 때만 안내한다. (트리 현재 폴더 자체는 사유 판정에서 제외 —
//선택 없이 폴더에 머무는 것만으로 오해를 주는 보호 툴팁을 띄우지 않기 위함.)
CString CnFTDServerDlg::get_transfer_block_reason(int dwSide)
{
	CSCListCtrl* plist = (dwSide == SERVER_SIDE ? &m_list_local : &m_list_remote);

	std::deque<int> dq;
	plist->get_selected_items(&dq);

	if (dq.size() == 0)
		return _T("");

	//리스트 선택 항목 중 하나라도 보호 대상이면 그 사유를 안내.
	for (int i = 0; i < dq.size(); i++)
	{
		CString path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(dwSide, plist->get_path(dq[i]));
		if (!theApp.m_shell_imagelist.is_copyable_from(dwSide, path))
			return _S(IDS_PROTECTED_FOLDER_FILE);	//TODO: 다국어 — 이미 리소스 문자열 사용 중
	}

	return _T("");
}

void CnFTDServerDlg::refresh_disk_usage(bool is_remote_side)
{
	static TCHAR last_local_drive = 0;
	static TCHAR last_remote_drive = 0;

	CSCListCtrl* plist = (is_remote_side ? &m_list_remote : &m_list_local);
	CSCSliderCtrl* pslider = (is_remote_side ? &m_slider_remote_disk_space : &m_slider_local_disk_space);
	CSCStatic* pstatic = (is_remote_side ? &m_static_remote_disk_space : &m_static_local_disk_space);

	CString path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(is_remote_side, plist->get_path());
	
	//내 PC를 선택한 경우는 용량표시는 숨긴다.
	if (path == theApp.m_shell_imagelist.m_volume[is_remote_side].get_path(CSIDL_DRIVES))
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
		if (drive[0] == last_remote_drive)
			return;

		theApp.m_shell_imagelist.m_volume[1].get_drive_space(drive, &ulTotal, &ulFree);
		last_remote_drive = drive[0];
	}
	else
	{
		if (drive[0] == last_local_drive)
			return;

		ulFree.QuadPart = get_disk_free_size(drive);
		ulTotal.QuadPart = get_disk_total_size(drive);
		last_local_drive = drive[0];
	}

	double free_ratio = (((double)ulFree.QuadPart / (double)ulTotal.QuadPart)) * 100.0;

	//TRACE(_T("%s drive free space ratio = %f\n"), drive, free_ratio);

	//progressbar에는 남은 용량만큼 채우는게 아니라 사용량만큼 채워서 보여준다.
	if (free_ratio < 10.0f)
		pslider->set_active_color(gRGB(220 - free_ratio, 41, 42));
	else
		pslider->set_active_color(gRGB(36, 160, 212));

	pslider->SetPos(100 - (int)free_ratio);

	if (GetUserDefaultUILanguage() == 1042)
		pstatic->set_textf(_T("%s 중 %s 사용 가능"), get_size_str(ulTotal.QuadPart, -1), get_size_str(ulFree.QuadPart, -1));
	else
		pstatic->set_textf(_T("%s free of %s"), get_size_str(ulFree.QuadPart, -1), get_size_str(ulTotal.QuadPart, -1));
}

void CnFTDServerDlg::OnListContextMenuOpen()
{
	file_command_on_list(file_cmd_open);
}

void CnFTDServerDlg::OnListContextMenuOpenExplorer()
{
	file_command_on_list(file_cmd_open_explorer);
}

void CnFTDServerDlg::OnListContextMenuProperty()
{
	file_command_on_list(file_cmd_property);
}


//우클릭 컨텍스트 메뉴는 WM_CONTEXTMENU 단일 경로로 처리한다(정석). 공유 컨트롤(CSCTreeCtrl/CSCListCtrl)이
//set_use_own_context_menu(false) 상태에서 우클릭 시 WM_CONTEXTMENU 를 부모로 forward 하며, wParam(=소스 hwnd)로
//어느 컨트롤인지 구분한다. point 는 화면 좌표(키보드 메뉴키의 (-1,-1)은 컨트롤 쪽에서 이미 정규화됨).
void CnFTDServerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	switch (pWnd && pWnd->GetSafeHwnd() ? ::GetDlgCtrlID(pWnd->GetSafeHwnd()) : 0)
	{
	case IDC_TREE_LOCAL:			show_tree_context_menu(SERVER_SIDE, point);		break;
	case IDC_TREE_REMOTE:			show_tree_context_menu(CLIENT_SIDE, point);		break;
	case IDC_LIST_LOCAL:			show_list_context_menu(SERVER_SIDE, point);		break;
	case IDC_LIST_REMOTE:			show_list_context_menu(CLIENT_SIDE, point);		break;
	case IDC_LIST_LOCAL_FAVORITE:	show_favorite_context_menu(&m_list_local_favorite, point);	break;
	case IDC_LIST_REMOTE_FAVORITE:	show_favorite_context_menu(&m_list_remote_favorite, point);	break;
	}
}


void CnFTDServerDlg::OnLvnItemChangedListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	//선택 이벤트도 처리하지만 빈 공간을 클릭하여 선택이 안된 경우에도 status는 갱신되어야 한다.
	//if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	
	//ItemChanged가 발생할때마다 바로 status를 변경하면 딜레이가 심하다. 타이머로 처리한다.
	TRACE(_T("OnLvnItemChangedListLocal\n"));
	m_selection_status_target = &m_list_local;
	SetTimer(timer_refresh_selection_status, 50, NULL);
	//refresh_selection_status(&m_list_local);

	*pResult = 0;
}


void CnFTDServerDlg::OnLvnItemChangedListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	//선택 이벤트도 처리하지만 빈 공간을 클릭하여 선택이 안된 경우에도 status는 갱신되어야 한다.
	//if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
		
	//ItemChanged가 발생할때마다 바로 status를 변경하면 딜레이가 심하다. 타이머로 처리한다.
	TRACE(_T("OnLvnItemChangedListRemote\n"));
	m_selection_status_target = &m_list_remote;
	SetTimer(timer_refresh_selection_status, 50, NULL);
	//refresh_selection_status(&m_list_remote);

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

	m_list_local.get_selected_items(&m_transfer_list);
	m_transfer_from = m_list_local.get_path();
	m_transfer_to = m_list_remote.get_path();

	TRACE(_T("m_srcSide = %d, send %d items from %s to %s\n"), m_srcSide, m_transfer_list.size(), m_transfer_from, m_transfer_to);
	file_transfer();
}


void CnFTDServerDlg::OnBnClickedButtonRemoteToLocal()
{
	m_srcSide = CLIENT_SIDE;
	m_dstSide = SERVER_SIDE;

	m_list_remote.get_selected_items(&m_transfer_list);
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
			//std::deque<WIN32_FIND_DATA> dq;
			//bool res = m_ServerManager.get_folderlist(m_list_remote_favorite.get_text(item, 1), &dq, false);

			//remote에 해당 경로가 존재하는지만 검사하면 된다.
			CString path = m_list_remote_favorite.get_text(item, 1);
			bool res = m_ServerManager.m_socket.file_command(file_cmd_check_exist, path);

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
	CSCListCtrl* plist = (side == SERVER_SIDE ? &m_list_local : &m_list_remote);
	CSCListCtrl* pfavoritelist = (side == SERVER_SIDE ? &m_list_local_favorite : &m_list_remote_favorite);

	if (cmd == favorite_add)
	{
		if (fullpath.IsEmpty())
			fullpath = plist->get_path();

		fullpath = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, fullpath);

		//"즐겨찾기 폴더" 컬럼은 폴더명(fn_name). 단 드라이브 루트("D:\" · "D:")는 폴더명이 비므로
		//드라이브 볼륨 레이블(예: "작업 디스크 (D:)")을 대신 표시한다.
		CString display_name = get_part(fullpath, fn_name);
		if (display_name.IsEmpty())
			display_name = theApp.m_shell_imagelist.m_volume[side].get_drive_volume(fullpath);

		index = pfavoritelist->insert_item(-1, display_name);
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

		save_favorite(side);
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
			fullpath = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, fullpath);
			index = favorite_cmd(favorite_find, side, fullpath);
			dq.push_back(index);
		}

		for (int i = dq.size() - 1; i >= 0; i--)
		{
			if (dq[i] < 0)
				continue;

			pfavoritelist->delete_item(dq[i]);
		}

		save_favorite(side);
	}
	else if (cmd == favorite_find)
	{
		fullpath = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, fullpath);

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
	int side;
	CSCListCtrl* plist;
	
	if (GetFocus() == &m_list_local)
	{
		side = SERVER_SIDE;
		plist = &m_list_local;
	}
	else if (GetFocus() == &m_list_remote)
	{
		side = CLIENT_SIDE;
		plist = &m_list_remote;
	}
	else
	{
		return;
	}

	CString fullpath;

	std::deque<WIN32_FIND_DATA> dq;
	plist->get_selected_items(&dq);

	//멀티 선택이라면 0번 항목을 대상으로 한다.
	//선택된 항목이 없으면 현재 경로를 취한다.
	//파일 또는 폴더에서 우클릭하므로 즐겨찾기에 등록된 폴더인지는 폴더 경로로 비교해야 한다.
	if (dq.size() == 0 || !(dq[0].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		file_command_on_list(file_cmd_favorite, plist->get_path());
	}
	else
	{
		file_command_on_list(file_cmd_favorite, dq[0].cFileName);
	}
}


void CnFTDServerDlg::OnFavoriteContextMenuDelete()
{
	int dwSide = (GetFocus() == &m_list_remote_favorite);
	favorite_cmd(favorite_delete, dwSide);
}


//즐겨찾기 리스트 우클릭 메뉴(로컬/원격 공통). WM_CONTEXTMENU 로 위임받아 호출된다. point: 화면좌표.
void CnFTDServerDlg::show_favorite_context_menu(CSCListCtrl* plist, CPoint point)
{
	CPoint pt = point;
	plist->ScreenToClient(&pt);
	int item = plist->HitTest(pt);

	//이 메뉴는 선택된 항목이 없을 경우 그냥 리턴한다.
	if (item == -1)
		return;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_FAVORITE_CONTEXT);

	CMenu* pMenu = menu.GetSubMenu(0);

	pMenu->ModifyMenu(ID_FAVORITE_CONTEXT_MENU_DELETE, MF_BYCOMMAND, ID_FAVORITE_CONTEXT_MENU_DELETE, _S(IDS_FAVORITE_REMOVE) + _T("(&D)\tDel"));

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CnFTDServerDlg::OnLvnEndlabelEditListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int item = m_list_local.get_recent_edit_item();
	int subitem = m_list_local.get_recent_edit_subitem();

	//폴더인 경우의 편집이 완료되면 트리에도 적용시켜줘야 한다.
	//폴더인 경우의 편집이 완료되면 트리에도 적용시켜줘야 한다.
	//편집중에 트리의 한 폴더를 클릭하여 편집모드가 종료되면 새로 클릭된 폴더의 파일/폴더목록이 표시될테고
	//이 때 위에서 구한 item, subItem은 이전 리스트에서 기억된 값이므로 의미가 없어진다.
	//우선 인덱스를 검사하여 범위밖이면 그냥 리턴한다.
	if (item < 0 || item >= m_list_local.size())
		return;


	//만약 리스트에서 폴더의 이름을 변경했다면 트리에서도 동일하게 변경해줘야 한다.
	if (m_list_local.get_text(item, CSCListCtrl::col_filesize).GetLength() == 0)
	{
		m_tree_local.rename_child_item(m_tree_local.GetSelectedItem(), m_list_local.get_edit_old_text(), m_list_local.get_edit_new_text());
	}

	//이름을 변경했다면 m_cur_folder 또는 m_cur_file 목록에서도 이름을 변경시켜줘야 한다.
	auto item_data = (WIN32_FIND_DATA)m_list_local.get_win32_find_data(item);
	CString folder = get_part(item_data.cFileName, fn_folder);
	_sntprintf_s(item_data.cFileName, _countof(item_data.cFileName), _TRUNCATE, _T("%s\\%s"), (LPCTSTR)folder, (LPCTSTR)m_list_local.get_edit_new_text());
	m_list_local.set_win32_find_data(item, item_data);

	rewatch_local();
}


void CnFTDServerDlg::OnLvnEndlabelEditListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int item = m_list_remote.get_recent_edit_item();
	int subitem = m_list_remote.get_recent_edit_subitem();

	//폴더인 경우의 편집이 완료되면 트리에도 적용시켜줘야 한다.
	//편집중에 트리의 한 폴더를 클릭하여 편집모드가 종료되면 새로 클릭된 폴더의 파일/폴더목록이 표시될테고
	//이 때 위에서 구한 item, subItem은 이전 리스트에서 기억된 값이므로 의미가 없어진다.
	//우선 인덱스를 검사하여 범위밖이면 그냥 리턴한다.
	if (item < 0 || item >= m_list_remote.size())
		return;

	//만약 리스트에서 폴더의 이름을 변경했다면 트리에서도 동일하게 변경해줘야 한다.
	if (m_list_remote.get_text(item, CSCListCtrl::col_filesize).GetLength() == 0)
	{
		m_tree_remote.rename_child_item(m_tree_remote.GetSelectedItem(), m_list_remote.get_edit_old_text(), m_list_remote.get_edit_new_text());
	}

	//이름을 변경했다면 m_cur_folder 또는 m_cur_file 목록에서도 이름을 변경시켜줘야 한다.
	auto item_data = (WIN32_FIND_DATA)m_list_remote.get_win32_find_data(item);
	CString folder = get_part(item_data.cFileName, fn_folder);
	_sntprintf_s(item_data.cFileName, _countof(item_data.cFileName), _TRUNCATE, _T("%s\\%s"), (LPCTSTR)folder, (LPCTSTR)m_list_remote.get_edit_new_text());
	m_list_remote.set_win32_find_data(item, item_data);
}

//OnActivate / OnActivateApp 제거:
//deactivate 시 타이틀바 잔상(흰 막대) 보정을 위해 timer_refresh_title_area 를 띄우던 핸들러였으나,
//이제 CSCThemeDlg 의 WindowProc 가 WM_NCACTIVATE 를 TRUE 로 완전 차단해 NC 흰 막대 자체가 생기지 않는다.
//(Endorphin2 는 해당 타이머가 없고 ASee 는 비활성화 — 두 프로젝트 모두 정석 NC 처리로 해결됨.)

void CnFTDServerDlg::OnTreeContextMenuSend()
{
	m_srcSide = (GetFocus() == &m_tree_remote);
	m_dstSide = !m_srcSide;

	CSCTreeCtrl* ptree = (m_srcSide == SERVER_SIDE ? &m_tree_local : &m_tree_remote);

	CString path = ptree->get_path();
	path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(m_srcSide, path);

	WIN32_FIND_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	_sntprintf_s(data.cFileName, _countof(data.cFileName), _TRUNCATE, _T("%s"), (LPCTSTR)path);

	m_transfer_list.clear();
	m_transfer_list.push_back(data);

	//m_transfer_from은 전송하고자 하는 폴더의 parent folder로 지정해줘야 한다.
	m_transfer_from = get_parent_dir(path);

	if (m_srcSide == SERVER_SIDE)
	{
		m_transfer_to = m_list_remote.get_path();
	}
	else
	{
		m_transfer_to = m_list_local.get_path();
	}

	TRACE(_T("m_srcSide = %d, send %d items from %s to %s\n"), m_srcSide, m_transfer_list.size(), m_transfer_from, m_transfer_to);
	file_transfer();
}

void CnFTDServerDlg::OnTreeContextMenuFavorite()
{
	file_command_on_tree(file_cmd_favorite);
}

void CnFTDServerDlg::OnTreeContextMenuOpenExplorer()
{
	file_command_on_tree(file_cmd_open_explorer);
}

void CnFTDServerDlg::OnTreeContextMenuRefresh()
{
	file_command_on_tree(file_cmd_refresh);
}

void CnFTDServerDlg::OnTreeContextMenuNewFolder()
{
	file_command_on_tree(file_cmd_new_folder);
}

void CnFTDServerDlg::OnTreeContextMenuProperty()
{
	file_command_on_tree(file_cmd_property);
}

//file system의 이벤트에 대한 처리함수를 추가했으나 검토해야 할 부분이 많다.
//이 프로그램을 통해 생성, 삭제, 이름변경을 할 때에는 모든 처리를 알아서 하게 작성되었으므로
//이 이벤트를 여기서 또 처리하게되면 중복처리된다.
//윈도우 탐색기와 같은 외부 프로그램에 의한 변경에 대해서만 이 이벤트들을 처리해야 한다.
//로컬 dir watcher 를 현재 상태 기준으로 재설정: 리스트 현재 폴더 + 트리에서 펼쳐진 폴더들을 각각 '비재귀' 로 감시.
//- 리스트 현재 폴더: 그 안 파일/폴더의 rename/삭제/추가(파일 변경 포함)를 잡는다. 트리에서 안 펼쳐져 있어도 반드시 포함.
//- 펼쳐진 트리 폴더: 그 '직속 자식'(=화면에 보이는 폴더 노드)의 rename/추가/삭제를 잡는다. (자식 폴더 rename 은 부모에서 감지되므로)
//재귀 감시를 쓰지 않으므로 거대 폴더 진입/드라이브 루트여도 그 폴더 '직속' 변경만 와서 볼륨이 낮다.
void CnFTDServerDlg::rewatch_local()
{
	m_dir_watcher.stop();

	int count = 0;

	CString cur = theApp.m_shell_imagelist.convert_special_folder_to_real_path(0, m_list_local.get_path());
	if (!cur.IsEmpty() && PathIsDirectory(cur))
	{
		m_dir_watcher.add(cur, false);
		count++;
		logWrite(_T("[rewatch] +list %s"), (LPCTSTR)cur);	//20260712 by claude. [diag temp]
	}

	for (HTREEITEM h = m_tree_local.GetFirstVisibleItem(); h != NULL; h = m_tree_local.GetNextVisibleItem(h))
	{
		if (!(m_tree_local.GetItemState(h, TVIS_EXPANDED) & TVIS_EXPANDED))
			continue;
		CString p = theApp.m_shell_imagelist.convert_special_folder_to_real_path(0, m_tree_local.get_path(h));
		if (!p.IsEmpty() && PathIsDirectory(p))
		{
			m_dir_watcher.add(p, false);	//CSCDirWatcher::add 는 is_watching 으로 중복 skip
			count++;
			logWrite(_T("[rewatch] +tree %s"), (LPCTSTR)p);	//20260712 by claude. [diag temp]
		}
	}

}

LRESULT CnFTDServerDlg::on_message_CSCDirWatcher(WPARAM wParam, LPARAM lParam)
{
	//FILE_ACTION_ADDED(1), FILE_ACTION_REMOVED(2), FILE_ACTION_RENAMED_NEW_NAME(4)/NEW_NAME(5)
	CSCDirWatcherMessage* msg = (CSCDirWatcherMessage*)wParam;
	logWrite(_T("[watcher] action=%d path0='%s'"), msg->action, msg->path0);	//20260712 by claude. [diag temp] 테스트 중 유지 — 최종 push 시 제거.

	//변경된 항목의 '부모 폴더'(=내용이 바뀐 폴더)와 현재 리스트 폴더(둘 다 real path).
	CString changed_parent = get_part(msg->path0, fn_folder);
	CString cur_list       = theApp.m_shell_imagelist.convert_special_folder_to_real_path(SERVER_SIDE, m_list_local.get_path());

	//── [리스트] 리스트는 '현재 폴더의 내용'만 표시 → 변경의 부모가 현재 폴더일 때만 갱신(파일/폴더 무관).
	if (changed_parent.CompareNoCase(cur_list) == 0)
	{
		m_list_local.invalidate_folder_cache(m_list_local.get_path());
		if (msg->action == FILE_ACTION_REMOVED)
			m_list_local.delete_item(msg->path0);
		else if (msg->action == FILE_ACTION_RENAMED_NEW_NAME)
			m_list_local.rename(msg->path1, msg->path0);
		else if (msg->action == FILE_ACTION_ADDED)
			m_list_local.refresh_list(true, true);	//추가는 정렬 위치 계산이 필요 → 현재 폴더만 새로고침
	}

	//── [트리] 트리는 '폴더 계층'만 표시(파일 무관) → 변경의 부모 폴더에 해당하는 노드가 트리에 로드돼 있을 때만 그 노드를 서피컬 갱신.
	if (msg->action == FILE_ACTION_REMOVED || msg->action == FILE_ACTION_RENAMED_NEW_NAME || msg->action == FILE_ACTION_ADDED)
	{
		HTREEITEM hParentNode = m_tree_local.get_item_by_fullpath(changed_parent);
		logWrite(_T("[chevron] action=%d changed_parent='%s' hParent=%p childLoaded=%d"), msg->action, (LPCTSTR)changed_parent, hParentNode, hParentNode ? (int)(m_tree_local.GetChildItem(hParentNode) != NULL) : -1);	//20260712 by claude. [diag temp] chevron ADD 조사 — 테스트 후 정리.
		if (hParentNode)
		{
			if (msg->action == FILE_ACTION_REMOVED)
			{
				HTREEITEM hChild = m_tree_local.find_children_item(get_part(msg->path0, fn_name), hParentNode);
				if (hChild) m_tree_local.DeleteItem(hChild);	//폴더였으면 자식 노드 존재 → 제거. 파일이면 매칭 노드 없어 no-op.
					//20260712 by claude. 자식 폴더 삭제 후 부모의 실제 하위폴더가 0 이면 확장버튼(cChildren) 제거 → chevron 잔존 방지.
					//디스크(has_sub_folders) 기준 — 부모가 collapsed 라 자식 노드가 로드 안 됐어도 정확(로드된 노드 유무만 보면 collapsed 시 오판).
					if (!has_sub_folders(changed_parent))
					{
						TVITEM tv = { 0 };
						tv.mask = TVIF_HANDLE | TVIF_CHILDREN;
						tv.hItem = hParentNode;
						tv.cChildren = 0;
						m_tree_local.SetItem(&tv);
					}
			}
			else if (msg->action == FILE_ACTION_RENAMED_NEW_NAME)
			{
				CString old_name, new_name;	//존재하는 쪽이 new(필드 순서 무관).
				if (PathFileExists(msg->path0)) { new_name = get_part(msg->path0, fn_name); old_name = get_part(msg->path1, fn_name); }
				else                            { old_name = get_part(msg->path0, fn_name); new_name = get_part(msg->path1, fn_name); }
				m_tree_local.rename_child_item(hParentNode, old_name, new_name);	//폴더 자식만 매칭 → 파일 rename 은 no-op.
			}
			else if (msg->action == FILE_ACTION_ADDED)
			{
				if (PathIsDirectory(msg->path0))
				{
					if (m_tree_local.GetChildItem(hParentNode) != NULL)	//자식 로드됨 → 정렬 위치에 노드 삽입(확장버튼 자동 표시)
					{
						WIN32_FIND_DATA fd; ZeroMemory(&fd, sizeof(fd));
						_tcscpy_s(fd.cFileName, _countof(fd.cFileName), get_part(msg->path0, fn_name));
						m_tree_local.insert_folder_sorted(hParentNode, &fd);
					}
					else
					{
						//20260712 by claude. 자식 미로드(접힘/하위폴더 없던 상태)면 노드는 확장 시 지연 로드하되, 하위폴더가
						//생겼으니 확장버튼(cChildren)을 켠다 — 안 그러면 새로고침 전까지 chevron 이 안 나타남(REMOVED 의 chevron 끄기와 대칭).
						TVITEM tv = { 0 };
						tv.mask = TVIF_HANDLE | TVIF_CHILDREN;
						tv.hItem = hParentNode;
						tv.cChildren = 1;
						m_tree_local.SetItem(&tv);
						logWrite(_T("[chevron] set cChildren=1 -> ItemHasChildren=%d"), (int)m_tree_local.ItemHasChildren(hParentNode));	//20260712 by claude. [diag temp]
					}
				}
			}
		}
	}

	//20260712 by claude. [트리 chevron — 접힌 폴더] 감시된 상위 폴더가 준 MODIFIED(자식 디렉토리 내용 변경)로, 접혀서 직접
	//감시되지 않는 그 디렉토리의 확장버튼을 has_sub_folders 로 재평가한다. 목적지 폴더가 접혀 있으면 하위폴더 이동 시 ADDED 가
	//그 폴더로 안 오고(상위 감시가 '자식 dir modified' 만 통지) 이 MODIFIED 가 유일한 신호다. 자식 로드된 노드는 ADDED/REMOVED 가 처리.
	if (msg->action == FILE_ACTION_MODIFIED && PathIsDirectory(msg->path0))
	{
		HTREEITEM hDirNode = m_tree_local.get_item_by_fullpath(msg->path0);
		if (hDirNode && m_tree_local.GetChildItem(hDirNode) == NULL)
		{
			TVITEM tv = { 0 };
			tv.mask = TVIF_HANDLE | TVIF_CHILDREN;
			tv.hItem = hDirNode;
			tv.cChildren = has_sub_folders(msg->path0) ? 1 : 0;
			m_tree_local.SetItem(&tv);
			logWrite(_T("[chevron] MODIFIED dir='%s' -> cChildren=%d hasChildren=%d"), (LPCTSTR)msg->path0, tv.cChildren, (int)m_tree_local.ItemHasChildren(hDirNode));	//20260712 by claude. [diag temp]
		}
	}

	//── [재감시] 감시 중이던 '현재 폴더 자신'이 외부에서 rename 되면 감시 경로가 무효 → 새 경로로 이동(내부에서 재감시).
	if (msg->action == FILE_ACTION_RENAMED_NEW_NAME)
	{
		CString renamed_old = PathFileExists(msg->path0) ? msg->path1 : msg->path0;
		CString renamed_new = PathFileExists(msg->path0) ? msg->path0 : msg->path1;
		if (renamed_old.CompareNoCase(cur_list) == 0)
			change_directory(renamed_new, SERVER_SIDE);
	}

	return 0;
}

void CnFTDServerDlg::OnTreeContextMenuDelete()
{
	//우클릭한 트리(로컬/원격)를 focus 기준으로 판별.
	CSCTreeCtrl* ptree = (GetFocus() == &m_tree_remote) ? &m_tree_remote : &m_tree_local;
	int side = (ptree == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;
	HTREEITEM hItem = ptree->GetSelectedItem();
	if (hItem == NULL)
		return;

	CString path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, ptree->get_path(hItem));

	//실행부 방어선: 보호 폴더(드라이브 루트·시스템 폴더)는 삭제 금지 — 메뉴 disable 과 이원화(조작된 경로 방지).
	if (!theApp.m_shell_imagelist.is_movable(side, path))
	{
		m_messagebox.DoModal(_T("드라이브 루트와 주요 시스템 폴더는 삭제할 수 없습니다."));
		return;
	}

	//20260704 by claude. 삭제 수행. 로컬은 공통 헬퍼 delete_file(휴지통, FO_DELETE + FOF_ALLOWUNDO)로 직접, 원격은 소켓
	//file_command(file_cmd_delete)로 client 에 위임(client 가 자기 FS 에서 휴지통 삭제 — 리스트 원격 삭제와 동일 경로).
	//성공 판정: 로컬은 실제 삭제 확인(PathFileExists), 원격은 소켓 응답값.
	bool ok = false;
	if (side == SERVER_SIDE)
		ok = delete_file(path, true) && !PathFileExists(path);
	else
		ok = (m_ServerManager.m_socket.file_command(file_cmd_delete, path) != FALSE);

	if (!ok)
		return;

	//트리 노드 제거 + 부모 [+] 정리 (로컬/원격 공통).
	HTREEITEM hParent = ptree->GetParentItem(hItem);
	ptree->DeleteItem(hItem);
	if (hParent && ptree->GetChildItem(hParent) == NULL)
	{
		TVITEM tv; ZeroMemory(&tv, sizeof(tv));
		tv.mask = TVIF_HANDLE | TVIF_CHILDREN;   tv.hItem = hParent;   tv.cChildren = 0;
		ptree->SetItem(&tv);
	}

	//해당 쪽 리스트(삭제된 폴더/그 부모를 보고 있으면)와 디스크 사용량 갱신.
	CSCListCtrl* plist = (side == SERVER_SIDE) ? &m_list_local : &m_list_remote;
	plist->refresh_list(true, true);
	refresh_disk_usage(side == CLIENT_SIDE);
}

void CnFTDServerDlg::OnTreeContextMenuRename()
{
	//우클릭한 트리를 focus 기준으로 판별. 편집모드 진입만 하면 CSCTreeCtrl::edit_end 가
	//로컬은 MoveFile 로 실제 폴더명 변경, 원격은 message_request_rename → socket Rename 을 수행한다.
	CSCTreeCtrl* ptree = (GetFocus() == &m_tree_remote) ? &m_tree_remote : &m_tree_local;
	HTREEITEM hItem = ptree->GetSelectedItem();
	if (hItem == NULL)
		return;
	ptree->edit_item(hItem);
}

void CnFTDServerDlg::OnTreeContextMenuPathToClipboard()
{
	CSCTreeCtrl* ptree = (GetFocus() == &m_tree_remote) ? &m_tree_remote : &m_tree_local;
	int side = (ptree == &m_tree_remote) ? CLIENT_SIDE : SERVER_SIDE;

	//20260704 by claude. get_path() 는 표시형("내 PC\로컬 디스크 (C:)\...")이라 탐색기 등에 붙여넣을 실제 경로로 변환해서 넣는다.
	CString path = theApp.m_shell_imagelist.convert_special_folder_to_real_path(side, ptree->get_path());
	copy_to_clipboard(m_hWnd, path);
}

void CnFTDServerDlg::OnListContextMenuPathToClipboard()
{
	CSCListCtrl* plist = (GetFocus() == &m_list_remote) ? &m_list_remote : &m_list_local;

	//20260704 by claude. 선택 항목이 있으면 그 항목(파일/폴더)의 fullpath, 없으면 현재 폴더 경로. get_path(index) 는 내부에서 실경로로 변환됨.
	std::deque<int> dq;
	plist->get_selected_items(&dq);
	CString path = (dq.size() > 0) ? plist->get_path(dq[0]) : plist->get_path();
	copy_to_clipboard(m_hWnd, path);
}

void CnFTDServerDlg::OnNMRClickTreeLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
