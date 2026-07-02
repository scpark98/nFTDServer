// ExistFileDlg.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "ExistFileDlg.h"
#include "afxdialogex.h"

#include "Common/Functions.h"

// CExistFileDlg 대화 상자

IMPLEMENT_DYNAMIC(CExistFileDlg, CSCThemeDlg)

CExistFileDlg::CExistFileDlg(CWnd* pParent, WIN32_FIND_DATA src_file, WIN32_FIND_DATA dst_file)
	: CSCThemeDlg(IDD_EXIST_FILE, pParent)
{
	m_src_file = src_file;
	m_dst_file = dst_file;
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
	DDX_Control(pDX, IDC_STATIC_SRC_MTIME, m_static_src_mtime);
	DDX_Control(pDX, IDC_STATIC_DST_MTIME, m_static_dst_mtime);
	DDX_Control(pDX, IDOK, m_button_ok);
}


BEGIN_MESSAGE_MAP(CExistFileDlg, CSCThemeDlg)
	ON_BN_CLICKED(IDOK, &CExistFileDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CExistFileDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_SUCCEED, &CExistFileDlg::OnBnClickedRadioSucceed)
	ON_BN_CLICKED(IDC_RADIO_OVERWRITE, &CExistFileDlg::OnBnClickedRadioOverwrite)
	ON_BN_CLICKED(IDC_RADIO_SKIP, &CExistFileDlg::OnBnClickedRadioSkip)
	ON_BN_CLICKED(IDC_CHECK_APPLY_ALL, &CExistFileDlg::OnBnClickedCheckApplyAll)
	//최신 Common 의 CSCSystemButtons 는 닫기/최소/최대를 Message_CSCSystemButtons 로 parent 에 보낸다.
	//핸들러가 없으면 닫기 버튼이 무반응(전송창과 동일 회귀).
	ON_REGISTERED_MESSAGE(Message_CSCSystemButtons, &CExistFileDlg::on_message_CSCSystemButtons)
END_MESSAGE_MAP()


// CExistFileDlg 메시지 처리기

BOOL CExistFileDlg::OnInitDialog()
{
	//이 창은 고정 크기다. base OnInitDialog 가 m_use_resizable 를 보고 WS_THICKFRAME 부여 여부를 정하므로 그 전에 꺼야 한다.
	//WS_THICKFRAME 이 붙으면 두꺼운 sizing 테두리 + 가장자리 resize 커서가 생긴다(실제 resize 는 원치 않음).
	set_use_resizable(false);

	CSCThemeDlg::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	set_color_theme(theApp.m_color_theme);
	set_titlebar_height(TOOLBAR_TITLE_HEIGHT);
	show_titlebar_logo(false);
	set_as_toolbar();
	//set_draw_border(true);	//DimGray 테두리가 진하게 보여 제거(전송창과 동일 처리). 필요 시 옅은 색으로 재지정.

#ifdef _REMOTE_SDK
	SetWindowText(_T("Remote SDK"));
#else
	SetWindowText(_S(NFTD_IDS_EXIST_TITLE));
#endif

	m_sys_buttons.set_button_width(TOOLBAR_TITLE_BUTTON_WIDTH);

	SetDlgItemText(IDOK, _S(NFTD_IDS_OK));

	m_button_ok.set_font_weight();
	m_button_ok.set_text_color(Gdiplus::Color::White);
	m_button_ok.set_back_color(gRGB(59, 70, 92));


	m_static_message.set_text_color(Gdiplus::Color(0, 51, 153));
	m_static_message.set_back_color(m_theme.cr_back);
	m_static_message.set_font_weight();
	m_static_message.set_text(_S(NFTD_IDS_EXIST_FILE));

	m_static_src_file_title.set_text_color(Gdiplus::Color(0, 51, 153));
	m_static_src_file_title.set_back_color(m_theme.cr_back);
	m_static_src_file_title.set_font_weight();
	m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE));

	m_static_dst_file_title.set_text_color(Gdiplus::Color(0, 51, 153));
	m_static_dst_file_title.set_back_color(m_theme.cr_back);
	m_static_dst_file_title.set_font_weight();
	m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE));

	m_static_src_file.set_back_color(m_theme.cr_back);
	m_static_src_filesize.set_back_color(m_theme.cr_back);
	m_static_src_mtime.set_back_color(m_theme.cr_back);

	m_static_dst_file.set_back_color(m_theme.cr_back);
	m_static_dst_filesize.set_back_color(m_theme.cr_back);
	m_static_dst_mtime.set_back_color(m_theme.cr_back);

	//파일명에 '&' 가 있어도 니모닉 접두로 먹히지 않고 글자 그대로 표시되도록.
	m_static_src_file.set_no_prefix(true);
	m_static_dst_file.set_no_prefix(true);


	m_radio_succeed.SetWindowText(_S(NFTD_IDS_CONTINUE));
	m_radio_succeed.set_back_color(m_theme.cr_back, false);
	m_radio_succeed.use_hover(false);

	m_radio_overwrite.SetWindowText(_S(NFTD_IDS_OVERWRITE));
	m_radio_overwrite.set_back_color(m_theme.cr_back, false);
	m_radio_overwrite.use_hover(false);

	m_radio_skip.SetWindowText(_S(NFTD_IDS_PASS));
	m_radio_skip.set_back_color(m_theme.cr_back, false);
	m_radio_skip.use_hover(false);

	m_check_apply_all.SetWindowText(_S(NFTD_IDS_CHECK_ALL));
	m_check_apply_all.SetCheck(theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("apply all"), BST_CHECKED));
	m_check_apply_all.set_back_color(m_theme.cr_back, false);
	m_check_apply_all.use_hover(false);


	if (theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. succeed"), BST_CHECKED) == BST_CHECKED)
		m_radio_succeed.SetCheck(BST_CHECKED);
	else if (theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. overwrite"), BST_CHECKED) == BST_CHECKED)
		m_radio_overwrite.SetCheck(BST_CHECKED);
	else if (theApp.GetProfileInt(_T("setting\\ExistFileDlg"), _T("exist file. skip"), BST_CHECKED) == BST_CHECKED)
		m_radio_skip.SetCheck(BST_CHECKED);

	m_tooltip.Create(this);
	m_tooltip.SetDelayTime(TTDT_INITIAL, 800);
	m_tooltip.SetMaxTipWidth(400);

	//── 원본/대상 비교 시각화 (윈도우 탐색기 충돌 대화상자와 동일 컨셉) ─────────────
	//크기·수정시각을 비교해 더 큰/더 최신 쪽을 강조색으로, 완전 동일하면 회색으로 표시하고
	//상단 안내문에 권장 처리 사유를 제시 + 권장 라디오를 자동 선택한다.
	ULARGE_INTEGER src_filesize, dst_filesize;
	src_filesize.LowPart = m_src_file.nFileSizeLow;   src_filesize.HighPart = m_src_file.nFileSizeHigh;
	dst_filesize.LowPart = m_dst_file.nFileSizeLow;   dst_filesize.HighPart = m_dst_file.nFileSizeHigh;

	int  size_cmp = (src_filesize.QuadPart == dst_filesize.QuadPart) ? 0 : (src_filesize.QuadPart > dst_filesize.QuadPart ? 1 : -1);	//+1: 원본이 큼
	int  time_cmp = CompareFileTime(&m_src_file.ftLastWriteTime, &m_dst_file.ftLastWriteTime);									//+1: 원본이 최신
	bool identical = (size_cmp == 0) && (time_cmp == 0);

	const Gdiplus::Color cr_same = Gdiplus::Color(150, 156, 164);	//완전 동일(회색)

	//크기/시각 비교 마커.
	CString src_size_tag, dst_size_tag, src_time_tag, dst_time_tag;
	if (identical)         { src_size_tag = dst_size_tag = _T("= 동일"); }
	else if (size_cmp > 0) { src_size_tag = _T("▲ 더 큼");   dst_size_tag = _T("▼ 더 작음"); }
	else if (size_cmp < 0) { src_size_tag = _T("▼ 더 작음"); dst_size_tag = _T("▲ 더 큼"); }
	else                   { src_size_tag = dst_size_tag = _T("= 같음"); }
	if (identical)         { src_time_tag = dst_time_tag = _T("= 동일"); }
	else if (time_cmp > 0) { src_time_tag = _T("▲ 더 최신");   dst_time_tag = _T("▼ 더 오래됨"); }
	else if (time_cmp < 0) { src_time_tag = _T("▼ 더 오래됨"); dst_time_tag = _T("▲ 더 최신"); }
	else                   { src_time_tag = dst_time_tag = _T("= 같음"); }

	//항목 상태: 3=완전동일(회색 전체), 2=강조(녹색+bold), 1=약화(amber), 0=같음(normal).
	int src_size_state = identical ? 3 : (size_cmp > 0 ? 2 : (size_cmp < 0 ? 1 : 0));
	int dst_size_state = identical ? 3 : (size_cmp < 0 ? 2 : (size_cmp > 0 ? 1 : 0));
	int src_time_state = identical ? 3 : (time_cmp > 0 ? 2 : (time_cmp < 0 ? 1 : 0));
	int dst_time_state = identical ? 3 : (time_cmp < 0 ? 2 : (time_cmp > 0 ? 1 : 0));

	//tagged text 빌더: "크기"/"수정한 날짜" 레이블은 normal, 값+마커만 상태별 색/굵기(목업과 동일한 부분 강조).
	auto cmp_line = [](const CString& label, const CString& value, const CString& tag, int state) -> CString
	{
		if (state == 3) return _T("<cr=150,156,164>") + label + _T(" : ") + value + _T("   ") + tag + _T("</cr>");	//완전동일: 전체 회색
		if (state == 2) return label + _T(" : <cr=60,175,95><b>") + value + _T("   ") + tag + _T("</b></cr>");		//강조: 녹색+bold
		if (state == 1) return label + _T(" : <cr=210,150,60>") + value + _T("   ") + tag + _T("</cr>");			//약화: amber
		return label + _T(" : ") + value + _T("   ") + tag;															//같음: normal
	};

	//제목의 크기 관계 표기.
	if (identical)         { m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE) + _T(" (완전 동일)")); m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE) + _T(" (완전 동일)")); }
	else if (size_cmp > 0) { m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE) + _T(" ") + _S(IDS_LARGER_FILE_SIZE)); }
	else if (size_cmp < 0) { m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE) + _T(" ") + _S(IDS_LARGER_FILE_SIZE)); }
	else                   { m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE) + _T(" ") + _S(IDS_EQUAL_FILE_SIZE)); m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE) + _T(" ") + _S(IDS_EQUAL_FILE_SIZE)); }

	//대상이 더 크면 "이어서 전송"은 물리적으로 불가 → 비활성.
	if (size_cmp < 0)
	{
		m_tooltip.AddTool(&m_radio_succeed, _S(IDS_DISABLE_SUCCEED_TRANSFER));
		m_radio_succeed.EnableWindow(FALSE);
	}

	//권장 처리 + 라디오 자동선택 + 상단 verdict 배너(배경색으로 강조). (기억된 라디오값보다 상황별 권장을 우선.)
	CString verdict;
	Gdiplus::Color cr_vtext, cr_vback;
	const Gdiplus::Color cr_green_t(96, 205, 130), cr_blue_t(96, 165, 245), cr_gray_t(176, 182, 190);
	const Gdiplus::Color bg_green(30, 52, 40), bg_blue(28, 44, 62), bg_gray(48, 52, 60);
	if (identical)         { verdict = _T("완전히 동일 — 건너뛰기 권장");                    cr_vtext = cr_gray_t;  cr_vback = bg_gray;  m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_CHECKED); }
	else if (size_cmp < 0) { verdict = _T("대상이 더 큼 — 이어서 전송 불가, 덮어쓰기 권장");            cr_vtext = cr_green_t; cr_vback = bg_green; m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_CHECKED);   m_radio_skip.SetCheck(BST_UNCHECKED); }
	else if (size_cmp > 0) { verdict = _T("대상이 더 작음(전송 중단 추정) — 이어서 전송 권장");          cr_vtext = cr_blue_t;  cr_vback = bg_blue;  m_radio_succeed.SetCheck(BST_CHECKED);   m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_UNCHECKED); }
	else if (time_cmp > 0) { verdict = _T("크기 동일·원본이 더 최신 — 덮어쓰기 권장");        cr_vtext = cr_green_t; cr_vback = bg_green; m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_CHECKED);   m_radio_skip.SetCheck(BST_UNCHECKED); }
	else                   { verdict = _T("크기 동일·대상이 더 최신 — 건너뛰기 권장");        cr_vtext = cr_gray_t;  cr_vback = bg_gray;  m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_CHECKED); }
	m_static_message.set_back_color(cr_vback);
	m_static_message.set_text_color(cr_vtext);
	m_static_message.set_text(verdict);

	//파일명(완전 동일 시 회색). '&' 리터럴 표시 위해 plain + no_prefix 유지.
	Gdiplus::Color cr_name = identical ? cr_same : m_theme.cr_text;
	m_static_src_file.set_text_color(cr_name);   m_static_src_file.set_textf(_T("%s"), m_src_file.cFileName);
	m_static_dst_file.set_text_color(cr_name);   m_static_dst_file.set_textf(_T("%s"), m_dst_file.cFileName);

	//크기·수정시각: tagged text 부분강조(레이블 normal + 값·마커만 색/bold). 단일 라인 세로중앙 정렬 유지.
	CString src_size_val, dst_size_val;
	src_size_val.Format(_T("%s (%s)"), get_size_str(src_filesize.QuadPart, 1), get_size_str(src_filesize.QuadPart, 0));
	dst_size_val.Format(_T("%s (%s)"), get_size_str(dst_filesize.QuadPart, 1), get_size_str(dst_filesize.QuadPart, 0));

	m_static_src_filesize.set_valign(DT_VCENTER);  m_static_src_filesize.set_tagged_text(cmp_line(_S(IDS_FILE_SIZE), src_size_val, src_size_tag, src_size_state));
	m_static_dst_filesize.set_valign(DT_VCENTER);  m_static_dst_filesize.set_tagged_text(cmp_line(_S(IDS_FILE_SIZE), dst_size_val, dst_size_tag, dst_size_state));
	m_static_src_mtime.set_valign(DT_VCENTER);     m_static_src_mtime.set_tagged_text(cmp_line(_S(IDS_FILE_LAST_MODIFIED_TIME), get_file_time_str(m_src_file.ftLastWriteTime), src_time_tag, src_time_state));
	m_static_dst_mtime.set_valign(DT_VCENTER);     m_static_dst_mtime.set_tagged_text(cmp_line(_S(IDS_FILE_LAST_MODIFIED_TIME), get_file_time_str(m_dst_file.ftLastWriteTime), dst_time_tag, dst_time_state));

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
	//X(우측 상단 닫기)/ESC = 전송 전체 취소(예전 동작). IDCANCEL 을 반환하면 caller(recv_file/send_file)가
	//transfer_result_cancel 로 해석해 전송 루프가 멈추고, 전송목록창(부모)도 닫힌다.
	CDialogEx::OnCancel();
}

//CSCSystemButtons(최소화/최대화/닫기)는 최신 Common 에서 Message_CSCSystemButtons 를 parent 로 보낸다.
//parent 인 본 dlg 가 직접 처리해야 버튼이 동작한다(핸들러 없으면 닫기 무반응 — 전송창과 동일 회귀).
LRESULT CExistFileDlg::on_message_CSCSystemButtons(WPARAM wParam, LPARAM lParam)
{
	CSCSystemButtonsMessage* msg = (CSCSystemButtonsMessage*)wParam;

	switch (msg->cmd)
	{
		case SC_MINIMIZE:
			ShowWindow(SW_MINIMIZE);
			break;
		case SC_RESTORE:
		case SC_MAXIMIZE:
			ShowWindow(IsZoomed() ? SW_RESTORE : SW_MAXIMIZE);
			break;
		case SC_CLOSE:
			OnBnClickedCancel();
			break;
	}

	return 0;
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

	if (pMsg->message == WM_KEYDOWN)
	{
		TRACE(_T("CExistFileDlg keydown\n"));
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}
