
// nFTDServerDlg.h: 헤더 파일
//

#pragma once

#include "Common/colors.h"
#include "Common/ResizeCtrl.h"
#include "Common/CDialog/SCThemeDlg/SCThemeDlg.h"
#include "Common/CDialog/SCProgressDlg/SCProgressDlg.h"
#include "Common/CTreeCtrl/SCTreeCtrl/SCTreeCtrl.h"
#include "Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "Common/CStatic/PathCtrl/PathCtrl.h"
#include "Common/ControlSplitter.h"
#include "Common/CSliderCtrl/SCSliderCtrl/SCSliderCtrl.h"
#include "Common/CProgressCtrl/MacProgressCtrl/MacProgressCtrl.h"
#include "Common/CDialog/SCShapeDlg/SCShapeDlg.h"
#include "Common/file_system/SCDirWatcher/SCDirWatcher.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"

#include "nFTDServerManager.h"

#define MESSAGE_CONNECT_FAIL	WM_USER + 862

// CnFTDServerDlg 대화 상자
class CnFTDServerDlg : public CSCThemeDlg
{
// 생성입니다.
public:
	CnFTDServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	enum TIMER_ID
	{
		timer_init_remote_controls = 0,
		timer_init_progress_and_connect,
		timer_init_favorites,
		timer_refresh_selection_status,
	};

	CResizeCtrl			m_resize;
	CSCShapeDlg			m_toast_popup;
	CSCMessageBox		m_messagebox;

	CSCDirWatcher		m_dir_watcher;
	LRESULT				on_message_CSCDirWatcher(WPARAM wParam, LPARAM lParam);

	//20250212 scpark
	//파일전송 히스토리를 저장하려고 보니 기존 코드는 .NetAPI로 작성되어 있고
	//.Net 서버주소와 포트를 얻어오는 코드가 존재하지 않는다. 얻어와서 config.ini에 기록한다.
	//=> ManualLauncher.exe가 실행되면서 config.ini에 이 주소를 기록했는데
	//ManualLauncher.exe를 리뉴얼하면서 해당 코드가 누락되어 있었음. LIP, WIP모두 기록되도록 수정했으므로
	//아래 함수는 불필요함.
	//bool				get_server_info();

	void				init_treectrl();
	void				init_listctrl();
	void				init_pathctrl();
	void				init_progressDlg();
	void				init_splitter();

	void				init_favorite();
	void				save_favorite(int dwSide);

	LRESULT				on_message_CVtListCtrlEx(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CPathCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CSCTreeCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CControlSplitter(WPARAM wParam, LPARAM lParam);
	//타이틀바 시스템 버튼(min/max/close)은 WM_SYSCOMMAND 가 아닌 Message_CSCSystemButtons 를 부모로 보낸다.
	//parent 인 본 dlg 가 직접 처리해야 버튼이 동작한다.
	LRESULT				on_message_CSCSystemButtons(WPARAM wParam, LPARAM lParam);
	void				adjust_processing_progress_ctrl();

	LRESULT				on_message(WPARAM wParam, LPARAM lParam);

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
	bool				file_command_on_list(int cmd, CString param0 = _T(""), CString param1 = _T(""));
	bool				file_command_on_tree(int cmd, CString param0 = _T(""), CString param1 = _T(""));
	//bool				file_command_on_favorite_list()

	//목록, 선택 정보가 변경되면 상태표시줄을 갱신한다.
	void				refresh_selection_status(CVtListCtrlEx* plist);
	void				refresh_selection_status(bool local);
	void				refresh_disk_usage(bool is_remote_side);

	//즐겨찾기 관련 명령
	enum FAVORITE_CMD
	{
		favorite_delete = 0,
		favorite_add,
		favorite_find,
	};
	//favorite_delete(0), favorite_add, favorite_find
	int					favorite_cmd(int cmd, int side, CString fullpath = _T(""));

	//전송시작 버튼 또는 drag&drop으로 전송을 시작한다.
	std::deque<WIN32_FIND_DATA> m_transfer_list;	//전송할 파일/폴더 목록이며 .Filename은 반드시 fullpath로 기록되어 있어야 한다.
	CString				m_transfer_from;	//전송할 파일/폴더의 src 폴더
	CString				m_transfer_to;		//전송할 dst 폴더
	void				file_transfer();	//위의 값들에 따라 전송 시작
	void				add_transfered_file_to_dst_list(int dstSide, WIN32_FIND_DATA);
	//전송 중 대상 폴더 뷰에 실제로 들어간 파일/폴더 이름들. 전송 완료 후 refresh_list 로 폴더를 리로드하면 선택/스크롤이
	//날아가므로, 이 목록으로 전송된 항목을 다시 선택하고 마지막 항목으로 ensure_visible 해 사용자가 결과를 바로 보게 한다.
	std::deque<CString>	m_transfered_names;
	void				select_transfered_items(CVtListCtrlEx* plist);

	//상황에 따라 송신, 수신이 불가능 할 경우의 처리를 위해.
	bool				is_transfer_enable_for_list(int srcSide);
	bool				is_transfer_enable_for_tree(int srcSide);
	//전송 대상은 리스트 선택 항목 또는(선택이 없으면) 트리에서 선택한 현재 폴더다. 두 경우를 통합 판정한다.
	bool				is_transfer_enable(int srcSide);
	//리스트에서 선택된 항목 중 하나라도 보호(삭제/이름변경 금지) 대상이면 true. 다중선택 삭제 방어에 사용.
	bool				any_selected_item_protected(int dwSide);

	//선택 변경(LVN_ITEMCHANGED)은 딜레이 방지를 위해 timer_refresh_selection_status(50ms)로 지연 처리한다.
	//타이머가 발화할 때 어느 리스트를 갱신할지를 GetFocus()로 추측하면 첫 실행 직후처럼 포커스가 아직 리스트에
	//있지 않은 순간(연결 진행 다이얼로그 등)에 갱신이 통째로 스킵되어 전송버튼이 enable되지 않는다.
	//→ 변경을 발생시킨 리스트를 여기에 기록해두고 타이머는 그 리스트를 갱신한다(포커스 비의존).
	CVtListCtrlEx*		m_selection_status_target = NULL;

	//전송버튼이 "보호 폴더 선택"으로 disable 됐을 때 그 사유를 hover 툴팁으로 표시한다.
	//WS_DISABLED 윈도우는 마우스 메시지를 못 받아 컨트롤 자체로는 툴팁을 못 띄우므로(모든 MFC 컨트롤 공통 제약),
	//툴팁을 parent 에서 선언하고 PreTranslateMessage 에서 릴레이해야 disabled 상태에도 표시된다.
	CToolTipCtrl		m_tooltip;
	//선택 항목 중 보호(전송 금지) 항목이 있으면 그 사유 문자열을, 없으면 빈 문자열을 리턴한다.
	CString				get_transfer_block_reason(int dwSide);

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
	//타이틀바/테두리 처리는 base CSCThemeDlg(borderless dialog.md 정석 패턴)에 위임한다.
	//OnLButtonDown 은 path edit_end 처리를 위해 남기되, 드래그 이동은 base 가 처리한다.
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
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
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTreeContextMenuFavorite();
	afx_msg void OnTreeContextMenuOpenExplorer();
	afx_msg void OnTreeContextMenuRefresh();
	afx_msg void OnTreeContextMenuNewFolder();
	afx_msg void OnTreeContextMenuProperty();
	afx_msg void OnTreeContextMenuSend();
	CSCSliderCtrl m_progress_local;
	CSCSliderCtrl m_progress_remote;
};
