
// nFTDServerDlg.h: 헤더 파일
//

#pragma once

#include "../../Common/colors.h"
#include "../../Common/ResizeCtrl.h"
#include "../../Common/CDialog/SCThemeDlg/SCThemeDlg.h"
//#include "../../Common/CButton/SCSystemButtons/SCSystemButtons.h"
#include "../../Common/CDialog/SCProgressDlg/SCProgressDlg.h"
#include "../../Common/CTreeCtrl/SCTreeCtrl/SCTreeCtrl.h"
#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "../../Common/CStatic/PathCtrl/PathCtrl.h"
#include "../../Common/ControlSplitter.h"
#include "../../Common/CSliderCtrl/SCSliderCtrl/SCSliderCtrl.h"
#include "../../Common/CProgressCtrl/MacProgressCtrl/MacProgressCtrl.h"
#include "../../Common/CWnd/WndShadow/WndShadow.h"

#include "nFTDServerManager.h"


// CnFTDServerDlg 대화 상자
class CnFTDServerDlg : public CSCThemeDlg
{
// 생성입니다.
public:
	CnFTDServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	enum TIMER_ID
	{
		timer_init_remote_controls = 0,
		timer_check_favorites,
	};

	//ui 관련
	CSCColorTheme		m_theme = CSCColorTheme(this);
	//void				set_color_theme(int theme);

	//shadow
	//CWndShadow			m_shadow;
	//void				init_shadow();

	CResizeCtrl			m_resize;

	int					m_corner_index = -1;	//커서가 코너의 어느 영역에 있는지

	void				init_treectrl();
	void				init_listctrl();
	void				init_pathctrl();
	void				init_progressDlg();
	void				init_splitter();

	void				init_favorite();
	void				save_favorite();

	LRESULT				on_message_CVtListCtrlEx(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CPathCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CSCTreeCtrl(WPARAM wParam, LPARAM lParam);

	CVtListCtrlEx*		m_pDropListCtrl;

	CnFTDServerManager	m_ServerManager;
	//l to l, r to r도 처리해야 하므로 src, dst가 어디인지 구분해야 한다.
	int					m_srcSide = SERVER_SIDE;
	int					m_dstSide = CLIENT_SIDE;
	bool				m_bViewerMode;

	ULARGE_INTEGER		m_ulClientDiskSpace;
	ULARGE_INTEGER		m_ulServerDiskSpace;

	CString				m_remoteCurrentPath;
	BOOL				change_directory(CString path, DWORD dwSide);
	void				SetDefaultPathToDesktop(int type);

	CString				GetLocalLastPath();
	CString				GetRemoteLastPath();
	void				SaveLocalLastPath();
	void				SaveRemoteLastPath();

	int					connect();
	void				initialize();
	CSCProgressDlg		m_progressDlg;
	void				thread_connect();
	bool				m_show_main_ui = false;

	void				InitServerManager();

	//열기(open), 이름변경(rename), 삭제(delete), 속성보기(property), 새 폴더 생성(new folder) 등의 파일명령은 파라미터만 다를 뿐이므로 하나의 함수로 통일한다.
	//param0가 ""이면 현재 포커스를 가진 리스트에서 선택된 항목의 값으로 채워주고
	//param1은 rename일 경우에만 유효하다.
	bool				file_command(int cmd, CString param0 = _T(""), CString param1 = _T(""));
	//bool				file_command_on_favorite_list()

	//목록, 선택 정보가 변경되면 상태표시줄을 갱신한다.
	void				refresh_selection_status(CVtListCtrlEx* plist);
	void				refresh_disk_usage(bool is_remote_side);

	//즐겨찾기 목록
	enum FAVORITE_CMD
	{
		favorite_delete = 0,
		favorite_add,
		favorite_find,
	};
	int					favorite_cmd(int cmd, int side, CString fullpath = _T(""));

	//전송시작 버튼 또는 drag&drop으로 전송을 시작한다.
	std::deque<WIN32_FIND_DATA> m_transfer_list;	//전송할 파일/폴더 목록이며 .Filename은 반드시 fullpath로 기록되어 있어야 한다.
	CString				m_transfer_from;	//전송할 파일/폴더의 src 폴더
	CString				m_transfer_to;		//전송할 dst 폴더
	void				file_transfer();	//위의 값들에 따라 전송 시작
	void				add_transfered_file_to_list(int dstSide, WIN32_FIND_DATA);

	//상황에 따라 송신, 수신이 불가능 할 경우의 처리를 위해.
	bool				is_transfer_enable(int srcSide);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NFTDSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CSCTreeCtrl m_tree_local;
	CVtListCtrlEx m_list_local;
	CSCTreeCtrl m_tree_remote;
	CVtListCtrlEx m_list_remote;
	CPathCtrl m_path_local;
	CPathCtrl m_path_remote;
	afx_msg void OnTvnSelchangedTreeLocal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnSelchangedTreeRemote(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkListRemote(NMHDR* pNMHDR, LRESULT* pResult);
	CControlSplitter m_splitter_center;
	CControlSplitter m_splitter_center2;
	CControlSplitter m_splitter_left;
	CControlSplitter m_splitter_right;
	CControlSplitter m_splitter_local_favorite;
	CControlSplitter m_splitter_remote_favorite;
	CSCStatic m_static_count_local;
	CSCStatic m_static_count_remote;
	afx_msg void OnNMDblclkListLocal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickTreeLocal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickTreeRemote(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickListLocal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickListRemote(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListContextMenuSend();
	afx_msg void OnListContextMenuNewFolder();
	afx_msg void OnListContextMenuRename();
	afx_msg void OnListContextMenuDelete();
	afx_msg void OnListContextMenuRefresh();
	afx_msg void OnListContextMenuSelectAll();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnListContextMenuOpen();
	afx_msg void OnListContextMenuProperty();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLvnItemChangedListLocal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemChangedListRemote(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListContextMenuOpenExplorer();
	CSCStatic m_static_local;
	CSCStatic m_static_remote;
	CGdiButton m_check_close_after_all;
	afx_msg void OnBnClickedCheckCloseAfterAll();
	CGdiButton m_button_local_to_remote;
	CGdiButton m_button_remote_to_local;
	afx_msg void OnBnClickedButtonLocalToRemote();
	afx_msg void OnBnClickedButtonRemoteToLocal();
	CVtListCtrlEx m_list_local_favorite;
	CVtListCtrlEx m_list_remote_favorite;
	afx_msg void OnNMDblclkListLocalFavorite(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkListRemoteFavorite(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListContextMenuFavorite();
	afx_msg void OnFavoriteContextMenuDelete();
	afx_msg void OnNMRClickListLocalFavorite(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickListRemoteFavorite(NMHDR* pNMHDR, LRESULT* pResult);
	CSCSliderCtrl m_slider_local_disk_space;
	CSCSliderCtrl m_slider_remote_disk_space;
	CSCStatic m_static_local_disk_space;
	CSCStatic m_static_remote_disk_space;
	afx_msg void OnLvnEndlabelEditListLocal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnEndlabelEditListRemote(NMHDR* pNMHDR, LRESULT* pResult);
};
