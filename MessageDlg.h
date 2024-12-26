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
	CMessageDlg(CString message = _T(""), UINT type = MB_OK, int timeout_ms = 0, int width = 300, int height = 180);   // 표준 생성자입니다.
	virtual ~CMessageDlg();

	//메시지박스가 표시된 상태에서도 set()을 호출하여 메시지 내용등을 변경할 수 있지만
	//width, height를 변경하지 않는 것이 일반적이므로 0일 경우는 NO_RESIZE.
	void		set(CString message, UINT type = MB_OK, int timeout_ms = 0, int width = 0, int height = 0);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGE };
#endif

protected:
	CString		m_message;
	UINT		m_type = MB_OK;
	int			m_timeout = 0;	//단위 : ms. 0 이하는 timeout 적용 안함.
	int			m_width = 300;
	int			m_height = 180;

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
