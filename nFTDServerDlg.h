
// nFTDServerDlg.h: 헤더 파일
//

#pragma once

#include "../../Common/colors.h"
#include "../../Common/ResizeCtrl.h"
#include "../../Common/CButton/SCSystemButtons/SCSystemButtons.h"
#include "../../Common/CDialog/SCProgressDlg/SCProgressDlg.h"
#include "../../Common/CTreeCtrl/SCTreeCtrl/SCTreeCtrl.h"
#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "../../Common/CStatic/PathCtrl/PathCtrl.h"
#include "../../Common/ControlSplitter.h"
#include "../../Common/CProgressCtrl/MacProgressCtrl/MacProgressCtrl.h"
#include "../../Common/CWnd/WndShadow/WndShadow.h"

#include "nFTDServerManager.h"

#define	TITLEBAR_HEIGHT		32

// CnFTDServerDlg 대화 상자
class CnFTDServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CnFTDServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	//ui 관련
	CSCColorTheme		m_theme = CSCColorTheme(this);
	void				set_color_theme(int theme);

	//shadow
	CWndShadow			m_shadow;
	void				init_shadow();

	CResizeCtrl			m_resize;
	CSCSystemButtons	m_sys_buttons;

	int					m_corner_index = -1;	//커서가 코너의 어느 영역에 있는지
	CString				m_title = _T("FileTransfer (not connected)");

	//Shell의 imagelist 및 shell과 관계된 멤버 제공
	CShellImageList		m_ShellImageList;
	void				init_treectrl();
	void				init_listctrl();
	void				init_pathctrl();
	void				init_progress();
	LRESULT				on_message_CVtListCtrlEx(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CPathCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT				on_message_CSCTreeCtrl(WPARAM wParam, LPARAM lParam);

	CVtListCtrlEx* m_pDropListCtrl;

	CnFTDServerManager	m_ServerManager;
	DWORD				m_dwSide = SERVER_SIDE;
	bool				m_bViewerMode;

	ULARGE_INTEGER		m_ulClientDiskSpace;
	ULARGE_INTEGER		m_ulServerDiskSpace;

	CString				m_remoteCurrentPath;
	BOOL				ChangeDirectory(CString path, DWORD dwSide);
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

	//전송시작 버튼 또는 drag&drop으로 전송을 시작한다.
	std::deque<CString> m_transfer_from;
	CString				m_transfer_to;
	void				FileTransfer(DWORD target);

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
	CControlSplitter m_splitter_left;
	CControlSplitter m_splitter_right;
	CSCStatic m_static_count_local;
	CSCStatic m_static_count_remote;
	CMacProgressCtrl m_progress_space_local;
	CMacProgressCtrl m_progress_space_remote;
};
