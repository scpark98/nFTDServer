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

	//20260706 by claude. 타이틀바 높이는 base OnInitDialog 안의 apply_client_titlebar_layout(WYSIWYG 레이아웃 보정)이
	//참조하므로 반드시 OnInitDialog 전(ctor)에 확정해야 한다(SCThemeDlg.h 주석 명시). set_titlebar_height()를 쓰면
	//아직 create 전인 m_sys_buttons.MoveWindow 에서 ASSERT 나므로, 여기선 window 조작 없는 멤버 직접 대입만 한다.
	//(이전엔 base 뒤에서 set_titlebar_height 호출 → 레이아웃이 기본값 32 로 예약되고 실제 타이틀바는 24 라 8px 어긋났다.
	// 이 창은 고정크기 + CResizeCtrl 없음이라 그 드리프트가 그대로 노출됨.)
	m_titlebar_height = TOOLBAR_TITLE_HEIGHT;
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
	if (identical)         { src_size_tag = dst_size_tag = _S(IDS_CMP_SAME); }
	else if (size_cmp > 0) { src_size_tag = _S(IDS_CMP_LARGER);  dst_size_tag = _S(IDS_CMP_SMALLER); }
	else if (size_cmp < 0) { src_size_tag = _S(IDS_CMP_SMALLER); dst_size_tag = _S(IDS_CMP_LARGER); }
	else                   { src_size_tag = dst_size_tag = _S(IDS_CMP_EQUAL); }
	if (identical)         { src_time_tag = dst_time_tag = _S(IDS_CMP_SAME); }
	else if (time_cmp > 0) { src_time_tag = _S(IDS_CMP_NEWER);   dst_time_tag = _S(IDS_CMP_OLDER); }
	else if (time_cmp < 0) { src_time_tag = _S(IDS_CMP_OLDER);   dst_time_tag = _S(IDS_CMP_NEWER); }
	else                   { src_time_tag = dst_time_tag = _S(IDS_CMP_EQUAL); }

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
	if (identical)         { m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE) + _T(" ") + _S(IDS_TITLE_IDENTICAL)); m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE) + _T(" ") + _S(IDS_TITLE_IDENTICAL)); }
	else if (size_cmp > 0) { m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE) + _T(" ") + _S(IDS_LARGER_FILE_SIZE)); }
	else if (size_cmp < 0) { m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE) + _T(" ") + _S(IDS_LARGER_FILE_SIZE)); }
	else                   { m_static_src_file_title.set_text(_S(IDS_EXIST_SRC_FILE_TITLE) + _T(" ") + _S(IDS_EQUAL_FILE_SIZE)); m_static_dst_file_title.set_text(_S(IDS_EXIST_DST_FILE_TITLE) + _T(" ") + _S(IDS_EQUAL_FILE_SIZE)); }

	//대상이 더 크면 "이어서 전송"은 물리적으로 불가 → 비활성.
	if (size_cmp < 0)
	{
		m_tooltip.AddTool(&m_radio_succeed, _S(IDS_DISABLE_SUCCEED_TRANSFER));
		m_radio_succeed.EnableWindow(FALSE);
	}

	//20260705 by claude. 권장 처리 + 라디오 자동선택 + 상단 verdict 배너를 '하이브리드' 스타일로 표시:
	//round border static + 솔리드 의미색 배경(컨트롤 전체 rect) + 상태글자=흰색 + 권장문구=의미색·bold(tagged text).
	//이전엔 flat 배경색 + 단색 텍스트라 상하 타이트·저가독이었음. 의미색 — 파랑=이어서 전송 / 주황=덮어쓰기 / 초록=건너뛰기.
	const Gdiplus::Color bg_resume(44, 71, 98), bd_resume(180, 121, 184, 255), rec_resume(121, 184, 255);
	const Gdiplus::Color bg_over  (90, 71, 42), bd_over  (180, 250, 190, 119), rec_over  (250, 190, 119);
	const Gdiplus::Color bg_skip  (44, 82, 65), bd_skip  (180, 127, 219, 170), rec_skip  (127, 219, 170);

	CString v_state, v_rec;
	Gdiplus::Color v_bg, v_bd, v_rec_cr;
	UINT v_icon;	//20260705 by claude. 상태별 좌측 아이콘(PNG 리소스, 타입 "PNG"). 파랑=이어서 / 주황=덮어쓰기 / 초록=건너뛰기.
	if (identical)         { v_state = _S(IDS_ST_IDENTICAL);                    v_rec = _S(IDS_REC_SKIP);   v_bg = bg_skip;   v_bd = bd_skip;   v_rec_cr = rec_skip;   v_icon = IDB_SKIP24;      m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_CHECKED); }
	else if (size_cmp < 0) { v_state = _S(IDS_ST_TARGET_LARGER); v_rec = _S(IDS_REC_OVERWRITE);   v_bg = bg_over;   v_bd = bd_over;   v_rec_cr = rec_over;   v_icon = IDB_OVERWRITE24; m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_CHECKED);   m_radio_skip.SetCheck(BST_UNCHECKED); }
	else if (size_cmp > 0) { v_state = _S(IDS_ST_TARGET_SMALLER); v_rec = _S(IDS_REC_RESUME); v_bg = bg_resume; v_bd = bd_resume; v_rec_cr = rec_resume; v_icon = IDB_RESUME24;    m_radio_succeed.SetCheck(BST_CHECKED);   m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_UNCHECKED); }
	else /* size_cmp == 0 · 크기 완전 동일하고 날짜만 다름 */
	{
		//20260706 by claude. 크기가 완전히 동일하면 같은 파일로 간주 → 날짜가 달라도 '건너뛰기 권장'.
		//날짜 방향(원본/대상 누가 더 최신)은 v_state 및 하단 수정시각 라인에 '정보'로만 표시하고,
		//이어서 전송/덮어쓰기 추천의 근거로는 쓰지 않는다(이전엔 원본이 더 최신이면 덮어쓰기를 권장했음).
		v_state = (time_cmp > 0) ? _S(IDS_ST_SRC_NEWER) : _S(IDS_ST_TGT_NEWER);
		v_rec = _S(IDS_REC_SKIP);   v_bg = bg_skip;   v_bd = bd_skip;   v_rec_cr = rec_skip;   v_icon = IDB_SKIP24;
		m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_CHECKED);
	}

	CString v_tagged;
	v_tagged.Format(_T("<cr=255,255,255>%s</cr>   <cr=150,160,170>›</cr>   <cr=%d,%d,%d><b>%s</b></cr>"),
		v_state.GetString(), v_rec_cr.GetR(), v_rec_cr.GetG(), v_rec_cr.GetB(), v_rec.GetString());

	m_static_message.set_font_size(10);						//약간 더 크게(기본보다 +1~2, 필요 시 조정). 상태=흰색, 권장=의미색+bold.
	m_static_message.set_font_weight(FW_NORMAL);			//상태글자는 normal, 권장문구만 <b> 로 bold
	m_static_message.set_back_color(v_bg);					//솔리드 의미색 배경(컨트롤 전체 rect → 상하 여백 확보)
	m_static_message.set_round(8, v_bd, m_theme.cr_back);	//round border(의미색). 코너는 다이얼로그 배경으로 블렌드
	m_static_message.set_margin(10, 0, 0, 0);				//20260705 by claude. 라운드 좌측 여백(아이콘 시작 위치를 모서리에서 띄움). 필요 시 조정.
	m_static_message.add_header_image(v_icon);				//상태별 좌측 아이콘(PNG 리소스). 텍스트와의 간격은 set_header_gap 으로 조정.
	m_static_message.set_header_gap(8);						//아이콘-텍스트 간격(px). 필요 시 조정.
	m_static_message.set_valign(DT_VCENTER);
	m_static_message.set_tagged_text(v_tagged);

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

	//20260706 by claude. [진단 임시·주석] 여백 비대칭 측정 — client 와 주요 컨트롤의 rect/각 변 여백(px). 재조사 시 해제. §2J
	//{
	//	CRect rcC;
	//	GetClientRect(&rcC);
	//	auto log_ctrl = [&](LPCTSTR name, CWnd& w)
	//	{
	//		if (!::IsWindow(w.GetSafeHwnd()))
	//			return;
	//		CRect r;
	//		w.GetWindowRect(&r);
	//		ScreenToClient(&r);
	//		logWrite(_T("[EXISTLAYOUT] %-10s rect=(%d,%d,%d,%d)  L=%d R=%d T=%d B=%d"),
	//			name, r.left, r.top, r.right, r.bottom,
	//			r.left - rcC.left, rcC.right - r.right, r.top - rcC.top, rcC.bottom - r.bottom);
	//	};
	//	logWrite(_T("[EXISTLAYOUT] client=%dx%d  titlebar=%d"), rcC.Width(), rcC.Height(), m_titlebar_height);
	//	log_ctrl(_T("banner"), m_static_message);
	//	log_ctrl(_T("src_title"), m_static_src_file_title);
	//	log_ctrl(_T("ok_btn"), m_button_ok);
	//	log_ctrl(_T("check_all"), m_check_apply_all);
	//}

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
