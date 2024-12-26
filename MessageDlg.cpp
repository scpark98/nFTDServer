// MessageDlg.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "MessageDlg.h"
#include "afxdialogex.h"


// CMessageDlg 대화 상자

IMPLEMENT_DYNAMIC(CMessageDlg, CSCThemeDlg)

CMessageDlg::CMessageDlg(CString message, UINT type, int timeout_ms, int width, int height)
	: CSCThemeDlg(IDD_MESSAGE, NULL)
{
	set(message, type, timeout_ms, width, height);
}

void CMessageDlg::set(CString message, UINT type, int timeout_ms, int width, int height)
{
	m_message = message;
	m_type = type;
	m_timeout = timeout_ms;

	if (width > 0)
		m_width = width;

	if (height > 0)
		m_height = height;
}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CSCThemeDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_button_ok);
	DDX_Control(pDX, IDCANCEL, m_button_cancel);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_static_message);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CSCThemeDlg)
	ON_BN_CLICKED(IDOK, &CMessageDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMessageDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMessageDlg 메시지 처리기

BOOL CMessageDlg::OnInitDialog()
{
	CSCThemeDlg::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(NULL, 0, 0, m_width, m_height, SWP_NOMOVE | SWP_NOZORDER);

	set_color_theme(CSCThemeDlg::theme_linkmemine);
	set_titlebar_height(TOOLBAR_TITLE_HEIGHT);
	show_titlebar_logo(false);
	m_sys_buttons.set_button_width(TOOLBAR_TITLE_BUTTON_WIDTH);

	//set_back_color(Gdiplus::Color::White);
	//set_title(_T("LinkMeMine File Manager"));
	//set_title_bold(true);
	//set_font_size(10);
	//set_titlebar_text_color(Gdiplus::Color::White);
	//set_titlebar_back_color(gRGB(59, 70, 92));

	m_static_message.set_font_bold();
	m_static_message.set_font_size(10);
	m_static_message.set_text(m_message);
	m_static_message.set_back_color(m_cr_back);
	
	CRect rc;
	GetClientRect(rc);

	int button_width = 80;
	int button_height = 28;
	int button_count = 2;
	int button_gap = 8;
	int bottom_margin = 16;

	if (m_type == MB_OK)
	{
		m_button_cancel.ShowWindow(SW_HIDE);
		button_count = 1;
		button_gap = 0;
	}

	int bx = (rc.Width() - button_count * button_width - button_gap * (button_count - 1)) / 2;
	m_button_ok.MoveWindow(bx, rc.bottom - bottom_margin - button_height, button_width, button_height);
	if (button_count == 2)
	{
		bx += (button_width + button_gap);
		m_button_cancel.MoveWindow(bx, rc.bottom - bottom_margin - button_height, button_width, button_height);
	}

	m_static_message.MoveWindow(4, m_titlebar_height + 12, rc.Width() - 8, rc.Height() - m_titlebar_height - 12 - bottom_margin - button_height - 2);

	m_button_ok.set_font_bold();
	m_button_ok.set_font_size(10);
	m_button_ok.text_color(Gdiplus::Color::White);
	m_button_ok.back_color(gRGB(59, 70, 92));
	m_button_ok.set_round(10);
	m_button_cancel.set_font_bold();
	m_button_cancel.set_font_size(10);
	m_button_cancel.set_round(10);
	m_button_cancel.draw_border();
	m_button_cancel.draw_hover_rect();

	CenterWindow(AfxGetApp()->GetMainWnd());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CMessageDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void CMessageDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}



BOOL CMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}
