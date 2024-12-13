#pragma once

#include "nFTDServerSocket.h"
#include "../../Common/CStatic/SCStatic/SCStatic.h"

// CExistFileDlg 대화 상자

class CExistFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExistFileDlg)

public:
	CExistFileDlg(CWnd *pParent, CString src_file, ULONGLONG src_size, CString dst_file, ULONGLONG dst_size);   // 표준 생성자입니다.
	virtual ~CExistFileDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXIST_FILE };
#endif

protected:
	CString			m_src_file;
	ULONGLONG		m_src_filesize;
	CString			m_dst_file;
	ULONGLONG		m_dst_filesize;

	CToolTipCtrl	m_tooltip;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CButton m_radio_succeed;
	CButton m_radio_overwrite;
	CButton m_radio_skip;
	CButton m_check_apply_all;
	CSCStatic m_static_message;
	afx_msg void OnBnClickedRadioSucceed();
	afx_msg void OnBnClickedRadioOverwrite();
	afx_msg void OnBnClickedRadioSkip();
	virtual BOOL OnInitDialog();
	CSCStatic m_static_src_file_title;
	CSCStatic m_static_dst_file_title;
	CSCStatic m_static_src_file;
	CSCStatic m_static_dst_file;
	CSCStatic m_static_src_filesize;
	CSCStatic m_static_dst_filesize;
	afx_msg void OnBnClickedCheckApplyAll();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
