﻿// ExistFileDlg.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "ExistFileDlg.h"
#include "afxdialogex.h"

#include "../../Common/Functions.h"

// CExistFileDlg 대화 상자

IMPLEMENT_DYNAMIC(CExistFileDlg, CDialogEx)

CExistFileDlg::CExistFileDlg(CWnd* pParent, CString src_file, ULONGLONG src_size, CString dst_file, ULONGLONG dst_size)
	: CDialogEx(IDD_EXIST_FILE, pParent)
{
	m_src_file = src_file;
	m_src_filesize = src_size;
	m_dst_file = dst_file;
	m_dst_filesize = dst_size;
}

CExistFileDlg::~CExistFileDlg()
{
}

void CExistFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_SUCCEED, m_radio_succeed);
	DDX_Control(pDX, IDC_RADIO_OVERWRITE, m_radio_overwrite);
	DDX_Control(pDX, IDC_RADIO_SKIP, m_radio_skip);
	DDX_Control(pDX, IDC_CHECK_APPLY_ALL, m_check_apply_all);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_static_message);
	DDX_Control(pDX, IDC_STATIC_SRC_FILE_TITLE, m_static_src_file_title);
	DDX_Control(pDX, IDC_STATIC_DST_FILE_TITLE, m_static_dst_file_title);
	DDX_Control(pDX, IDC_STATIC_SRC_FILE, m_static_src_file);
	DDX_Control(pDX, IDC_STATIC_DST_FILE, m_static_dst_file);
	DDX_Control(pDX, IDC_STATIC_SRC_FILESIZE, m_static_src_filesize);
	DDX_Control(pDX, IDC_STATIC_DST_FILESIZE, m_static_dst_filesize);
}


BEGIN_MESSAGE_MAP(CExistFileDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CExistFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CExistFileDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_SUCCEED, &CExistFileDlg::OnBnClickedRadioSucceed)
	ON_BN_CLICKED(IDC_RADIO_OVERWRITE, &CExistFileDlg::OnBnClickedRadioOverwrite)
	ON_BN_CLICKED(IDC_RADIO_SKIP, &CExistFileDlg::OnBnClickedRadioSkip)
	ON_BN_CLICKED(IDC_CHECK_APPLY_ALL, &CExistFileDlg::OnBnClickedCheckApplyAll)
END_MESSAGE_MAP()


// CExistFileDlg 메시지 처리기

BOOL CExistFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetDlgItemText(IDOK, _S(NFTD_IDS_OK));
	m_static_message.set_text_color(Gdiplus::Color(0, 51, 153));
	m_static_message.set_font_bold();
	m_static_message.set_text(_S(NFTD_IDS_EXIST_FILE));

	m_static_src_file_title.set_text_color(Gdiplus::Color(0, 51, 153));
	m_static_src_file_title.set_font_bold();
	m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE));

	m_static_dst_file_title.set_text_color(Gdiplus::Color(0, 51, 153));
	m_static_dst_file_title.set_font_bold();
	m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE));

	m_radio_succeed.SetWindowText(_S(NFTD_IDS_CONTINUE));
	m_radio_overwrite.SetWindowText(_S(NFTD_IDS_OVERWRITE));
	m_radio_skip.SetWindowText(_S(NFTD_IDS_PASS));

	if (theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. succeed"), BST_CHECKED) == BST_CHECKED)
		m_radio_succeed.SetCheck(BST_CHECKED);
	else if (theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. overwrite"), BST_CHECKED) == BST_CHECKED)
		m_radio_overwrite.SetCheck(BST_CHECKED);
	else if (theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. skip"), BST_CHECKED) == BST_CHECKED)
		m_radio_skip.SetCheck(BST_CHECKED);

	m_tooltip.Create(this);

	//대상 파일이 원본 파일보다 크다면 "이어서 전송"은 불가하다. 그 경우는 덮어쓰기, 건너뛰기 또는 전송 취소를 선택할 수 밖에 없다.
	if (m_dst_filesize > m_src_filesize)
	{
		m_tooltip.AddTool(&m_radio_succeed, _S(IDS_DISABLE_SUCCEED_TRANSFER));
		m_radio_succeed.SetCheck(BST_UNCHECKED);
		m_radio_succeed.EnableWindow(FALSE);
		m_radio_overwrite.SetCheck(BST_CHECKED);
		m_radio_skip.SetCheck(BST_UNCHECKED);
	}

	m_check_apply_all.SetWindowText(_S(NFTD_IDS_CHECK_ALL));
	m_check_apply_all.SetCheck(theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("apply all"), BST_CHECKED));
	
	//src와 dst 파일크기가 동일하면 파란색으로 표시, 그렇지 않으면 기본색으로 표시.
	m_static_src_file.set_textf((m_src_filesize == m_dst_filesize ? Gdiplus::Color(0, 51, 200) : -1), _T("%s"), m_src_file);
	m_static_dst_file.set_textf((m_src_filesize == m_dst_filesize ? Gdiplus::Color(0, 51, 200) : -1), _T("%s"), m_dst_file);
	m_static_src_filesize.set_textf((m_src_filesize == m_dst_filesize ? Gdiplus::Color(0, 51, 200) : -1), _T("%s (%s)"), get_size_string(m_src_filesize, 1), get_size_string(m_src_filesize, 0));
	m_static_dst_filesize.set_textf((m_src_filesize == m_dst_filesize ? Gdiplus::Color(0, 51, 200) : -1), _T("%s (%s)"), get_size_string(m_dst_filesize, 1), get_size_string(m_dst_filesize, 0));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CExistFileDlg::OnBnClickedOk()
{
	theApp.WriteProfileInt(_T("setting\\ExistFileDlg"), _T("apply all"), m_check_apply_all.GetCheck());
	theApp.WriteProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. succeed"), m_radio_succeed.GetCheck());
	theApp.WriteProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. overwrite"), m_radio_overwrite.GetCheck());
	theApp.WriteProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. skip"), m_radio_skip.GetCheck());

	int Exist;

	if (m_radio_succeed.GetCheck())
	{
		Exist = WRITE_CONTINUE;
	}
	else if (m_radio_overwrite.GetCheck())
	{
		Exist = WRITE_OVERWRITE;
	}
	else if (m_radio_skip.GetCheck())
	{
		Exist = WRITE_IGNORE;
	}

	if (m_check_apply_all.GetCheck())
	{
		Exist = Exist | WRITE_ALL;
	}

	EndDialog(Exist);
}


void CExistFileDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CExistFileDlg::OnBnClickedRadioSucceed()
{
}


void CExistFileDlg::OnBnClickedRadioOverwrite()
{
}


void CExistFileDlg::OnBnClickedRadioSkip()
{
}

void CExistFileDlg::OnBnClickedCheckApplyAll()
{
}


BOOL CExistFileDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//이 코드를 넣어줘야 disabled 컨트롤에서도 툴팁이 동작한다.
	//단, 이 코드를 컨트롤 클래스의 PreTranslateMessage()에 넣어줘도 소용없다.
	//이 코드는 parent에 있어야 한다.
	//disabled가 아닌 경우는 컨트롤 클래스에 넣어도 잘 표시된다.
	if (m_tooltip.m_hWnd)
	{
		//msg를 따로 선언해서 사용하지 않고 *pMsg를 그대로 이용하면 이상한 현상이 발생한다.
		MSG msg = *pMsg;
		msg.hwnd = (HWND)m_tooltip.SendMessage(TTM_WINDOWFROMPOINT, 0, (LPARAM) & (msg.pt));

		CPoint pt = msg.pt;

		if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
			::ScreenToClient(msg.hwnd, &pt);

		msg.lParam = MAKELONG(pt.x, pt.y);

		// relay mouse event before deleting old tool
		m_tooltip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)&msg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}