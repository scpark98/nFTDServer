#pragma once

#include "../../Common/Functions.h"
#include "../../Common/CButton/GdiButton/GdiButton.h"
#include "../../Common/CDialog/SCThemeDlg/SCThemeDlg.h"
#include "../../Common/CStatic/SCStatic/SCStatic.h"

// CMessageDlg 대화 상자

class CMessageDlg : public CSCThemeDlg
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL, CString message = _T(""), UINT type = MB_OK, int timeout_ms = 0);   // 표준 생성자입니다.
	virtual ~CMessageDlg();

	void		set(CString message, UINT type = MB_OK, int timeout_ms = 0);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGE };
#endif

protected:
	CString		m_message;
	UINT		m_type;
	int			m_timeout;	//단위 : ms. 0 이하는 timeout 적용 안함.

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CGdiButton m_button_ok;
	CGdiButton m_button_cancel;
	CSCStatic m_static_message;
};
