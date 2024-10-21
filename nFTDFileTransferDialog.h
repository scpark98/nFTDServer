#pragma once

#include "nFTDServerManager.h"

#include "../../Common/colors.h"
#include "../../Common/CStatic/SCStatic/SCStatic.h"
#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "../../Common/CProgressCtrl/MacProgressCtrl/MacProgressCtrl.h"

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
		col_src,
		col_dst,
	};
	void	init_list();

	BOOL	FileTransferInitalize(CnFTDServerManager* pServerManager, CVtListCtrlEx* pShellListCtrl,
								  ULARGE_INTEGER* pulRemainDiskSpace, DWORD dwSide, BOOL isAutoClose, LPCTSTR lpszStartPath = NULL);

	void	thread_transfer();

	DWORD				m_dwSide;
	CVtListCtrlEx*		m_pFileList;
	CnFTDServerManager* m_pServerManager;
	LPTSTR				m_lpInitPath;
	CString				m_strStartPath;

	ProgressData		m_ProgressData;

	CString				m_ReceivedSize, m_TotalSize;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILETRANSFER };
#endif

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
	CVtListCtrlEx m_XFileList;
	virtual BOOL OnInitDialog();
	CListCtrl m_ListQueue;
};
