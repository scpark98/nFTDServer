#pragma once

#include "nFTDServerManager.h"

#include "../../Common/colors.h"
#include "../../Common/CStatic/SCStatic/SCStatic.h"
#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "../../Common/CProgressCtrl/MacProgressCtrl/MacProgressCtrl.h"
#include "../../Common/ResizeCtrl.h"
#include "../../Common/CWnd/WndShadow/WndShadow.h"

// CnFTDFileTransferDialog 대화 상자

class CnFTDFileTransferDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CnFTDFileTransferDialog)

public:
	CnFTDFileTransferDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CnFTDFileTransferDialog();

	//m_list.set_headings(_T("파일명,100;크기,100;상태,60;원본 위치,100;대상 위치,100"));
	enum LIST_COLUMN
	{
		col_filename = 0,
		col_filesize,
		col_status,
	};
	void	init_list();

	BOOL	FileTransferInitalize(CnFTDServerManager* pServerManager, std::deque<WIN32_FIND_DATA> *filelist,
								  ULARGE_INTEGER* pulRemainDiskSpace, int srcSide, int dstSide, CString from, CString to, BOOL isAutoClose, LPCTSTR lpszStartPath = NULL);


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILETRANSFER };
#endif

protected:
	CResizeCtrl			m_resize;

	int					m_corner_index = -1;	//커서가 코너의 어느 영역에 있는지

	CWndShadow			m_shadow;
	void				init_shadow();

	bool				m_thread_transfer_started = false;
	void				thread_transfer();


	int					m_srcSide;
	int					m_dstSide;
	//CVtListCtrlEx*		m_pFileList;
	CnFTDServerManager* m_pServerManager;
	LPTSTR				m_lpInitPath;
	CString				m_transfer_from;
	CString				m_transfer_to;

	ProgressData		m_ProgressData;

	CString				m_ReceivedSize, m_TotalSize;

	std::deque<WIN32_FIND_DATA>	m_filelist;

	void		KeepConnection();
	void		KeepDataConnection();

	//LRESULT		on_message_server_socket(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CSCStatic m_static_message;
	CMacProgressCtrl m_progress;
	CSCStatic m_static_speed;
	CSCStatic m_static_index;
	CVtListCtrlEx m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CButton m_button_cancel;
};
