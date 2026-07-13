#pragma once

#include <vector>

#include "nFTDServerManager.h"

#include "Common/colors.h"
#include "Common/CStatic/SCStatic/SCStatic.h"
#include "Common/CListCtrl/CSCListCtrl/SCListCtrl.h"
#include "Common/CSliderCtrl/SCSliderCtrl/SCSliderCtrl.h"
#include "Common/ResizeCtrl.h"
#include "Common/CDialog/SCThemeDlg/SCThemeDlg.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"

// CnFTDFileTransferDialog 대화 상자

class CnFTDFileTransferDialog : public CSCThemeDlg
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
	enum TIMER_ID
	{
		timer_check_thread_transfer = 0,
	};

	CWnd				*m_parent = NULL;
	CResizeCtrl			m_resize;
	CSCMessageBox		m_messagebox;

	bool				m_thread_transfer_started = false;
	void				thread_transfer();
	std::thread			m_thread_transfer;

	int					m_srcSide;
	int					m_dstSide;
	CnFTDServerManager* m_pServerManager = NULL;
	LPTSTR				m_lpInitPath;
	CString				m_transfer_from;
	CString				m_transfer_to;
	bool				m_auto_close = false;		//전송 완료 후 창을 닫을 것인지

	ProgressData		m_ProgressData;

	CString				m_ReceivedSize, m_TotalSize;

	std::deque<WIN32_FIND_DATA>	m_filelist;

	//20260713 by claude. 전송 중 실패한 항목들의 m_filelist 인덱스. 새 전송 시작 시 clear, 실패마다 push_back.
	//완료 후 요약("N 성공, M 실패") 표시·자동닫기 억제·첫 실패 항목 자동 스크롤에 사용한다.
	std::vector<int>			m_fail_items;
	//요약 문구 클릭 시 순환 표시할 현재 실패 항목 위치(m_fail_items 인덱스). 클릭마다 +1, 끝에서 0으로 순환.
	int							m_fail_view_index = 0;

	void				KeepConnection();
	void				KeepDataConnection();

	//LRESULT		on_message_server_socket(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CSCStatic m_static_message;
	CSCSliderCtrl m_progress;
	CSCListCtrl m_list;
	virtual BOOL OnInitDialog();
	//타이틀바/테두리 처리는 base CSCThemeDlg(borderless dialog.md 정석 패턴)에 위임한다.
	//OnWindowPosChanged 만 윈도우 위치 저장을 위해 남긴다.
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	CSCStatic m_static_index_bytes;
	CSCStatic m_static_remain_speed;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CSCStatic m_static_copy;
	afx_msg void OnDestroy();
	//최신 Common 의 CSCSystemButtons(min/max/close)는 Message_CSCSystemButtons 를 parent 로 보내므로 여기서 처리한다.
	afx_msg LRESULT on_message_CSCSystemButtons(WPARAM wParam, LPARAM lParam);
	//전송 완료 요약 문구(m_static_message) 클릭 → 다음 실패 항목으로 순환 스크롤.
	afx_msg void OnStnClickedStaticMessage();
};
