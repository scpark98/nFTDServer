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
		//20260706 by claude. 마커(tag, 예: "↑ 더 큼")는 화살표가 얇아 보여 <sz=10><b> 로 약간 크고 굵게. state 2 는 이미 <b> 안이라 <sz> 만.
		//(값 라인 static 높이가 작아 13/11 은 잘림 → 10. 배너 → 는 static 이 커서 13 유지.) 크기 다른 마커는 set_line_align(-1,DT_VCENTER)로 세로중앙.
		if (state == 3) return _T("<cr=150,156,164>") + label + _T(" : ") + value + _T("   <sz=10><b>") + tag + _T("</b></sz></cr>");	//완전동일: 전체 회색
		if (state == 2) return label + _T(" : <cr=60,175,95><b>") + value + _T("   <sz=10>") + tag + _T("</sz></b></cr>");			//강조: 녹색+bold
		if (state == 1) return label + _T(" : <cr=210,150,60>") + value + _T("   <sz=10><b>") + tag + _T("</b></sz></cr>");			//약화: amber
		return label + _T(" : ") + value + _T("   <sz=10><b>") + tag + _T("</b></sz>");											//같음: normal
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
	const Gdiplus::Color bg_skip  (44, 82, 65), bd_skip  (180, 127, 219, 170), rec_skip  (127, 219, 170);
	//20260708 by claude. 크기·날짜가 상반될 때의 '판단 보류' 중립색(회색 계열). 초록/파랑 등 의미색을 피해 '추천 아님'을 시각적으로 구분.
	const Gdiplus::Color bg_conflict(64, 68, 74), bd_conflict(180, 150, 156, 164), rec_conflict(200, 205, 212);

	//20260708 by claude. 권장 로직 개편 — 크기와 날짜를 함께 본다(예전엔 크기가 다르면 날짜를 무시). 크기 방향과 날짜 방향이
	//상반(size_cmp*time_cmp<0: 원본이 큰데 더 오래됨 / 원본이 작은데 더 최신)이면 어느 쪽이 최신·완전본인지 불분명 →
	//아무 옵션도 권장하지 않는다(라디오 전체 해제 + 확인버튼 비활성, 사용자가 직접 선택). 상반이 아닐 때만 아래 규칙으로 권장:
	//  · 원본이 더 큼 & (더 최신 or 동일날짜) → 이어서 전송(대상=중단된 과거 부분전송으로 간주)
	//  · 그 외(완전 동일 · 크기 동일·날짜만 다름 · 대상이 더 크고 원본이 오래됨/동일) → 건너뛰기(기존 유지)
	bool conflict = (size_cmp * time_cmp) < 0;

	CString v_state, v_rec;
	Gdiplus::Color v_bg, v_bd, v_rec_cr;
	UINT v_icon = 0;		//상태별 좌측 아이콘(PNG 리소스). 모든 분기에서 지정된다.
	bool recommend = true;	//false = 권장 없음(판단 보류) → 라디오 자동선택 안 함 + 확인버튼 비활성.

	if (conflict)
	{
		//크기·날짜 상반 → 판단 보류. 상태문구는 크기 관계만 사실로 표시(날짜는 하단 수정시각 라인에 정보로 나옴).
		v_state = (size_cmp > 0) ? _S(IDS_ST_TARGET_SMALLER) : _S(IDS_ST_TARGET_LARGER);
		v_rec = _S(IDS_REC_MANUAL);   v_bg = bg_conflict; v_bd = bd_conflict; v_rec_cr = rec_conflict; v_icon = IDB_QUESTION24;
		recommend = false;
		m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_UNCHECKED);
	}
	else if (size_cmp > 0)
	{
		//원본이 더 크고 날짜가 상반이 아님(더 최신 or 동일) → 대상은 중단된 과거 부분전송 → 이어서 전송 권장.
		v_state = _S(IDS_ST_TARGET_SMALLER); v_rec = _S(IDS_REC_RESUME); v_bg = bg_resume; v_bd = bd_resume; v_rec_cr = rec_resume; v_icon = IDB_RESUME24;
		m_radio_succeed.SetCheck(BST_CHECKED);   m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_UNCHECKED);
	}
	else
	{
		//완전 동일 / 크기 동일·날짜만 다름 / 대상이 더 크고 원본이 오래됨(or 동일) → 기존 유지 = 건너뛰기 권장.
		if (identical)          v_state = _S(IDS_ST_IDENTICAL);
		else if (size_cmp == 0) v_state = (time_cmp > 0) ? _S(IDS_ST_SRC_NEWER) : _S(IDS_ST_TGT_NEWER);	//크기 동일 → 같은 파일 간주, 날짜는 정보로만
		else                    v_state = _S(IDS_ST_TARGET_LARGER);										//대상이 더 큼(원본이 오래됨/동일)
		v_rec = _S(IDS_REC_SKIP);   v_bg = bg_skip;   v_bd = bd_skip;   v_rec_cr = rec_skip;   v_icon = IDB_SKIP24;
		m_radio_succeed.SetCheck(BST_UNCHECKED); m_radio_overwrite.SetCheck(BST_UNCHECKED); m_radio_skip.SetCheck(BST_CHECKED);
	}

	CString v_tagged;
	v_tagged.Format(_T("<cr=255,255,255>%s</cr>   <cr=255,255,255><sz=13><b>→</b></sz></cr>   <cr=%d,%d,%d><b>%s</b></cr>"),
		v_state.GetString(), v_rec_cr.GetR(), v_rec_cr.GetG(), v_rec_cr.GetB(), v_rec.GetString());

	m_static_message.set_font_size(10);						//약간 더 크게(기본보다 +1~2, 필요 시 조정). 상태=흰색, 권장=의미색+bold.
	m_static_message.set_font_weight(FW_NORMAL);			//상태글자는 normal, 권장문구만 <b> 로 bold
	m_static_message.set_back_color(v_bg);					//솔리드 의미색 배경(컨트롤 전체 rect → 상하 여백 확보)
	m_static_message.set_round(8, v_bd, m_theme.cr_back);	//round border(의미색). 코너는 다이얼로그 배경으로 블렌드
	m_static_message.set_margin(4, 0, 0, 0);				//20260706 by claude. 아이콘 좌측 여백 — 상하 여백((배너높이-아이콘)/2≈5)과 균형 맞춰 10→4.
	m_static_message.add_header_image(v_icon);	//상태별 좌측 아이콘(PNG 리소스). 판단 보류는 물음표(IDB_QUESTION24).
	m_static_message.set_header_gap(8);						//아이콘-텍스트 간격(px). 필요 시 조정.
	m_static_message.set_valign(DT_VCENTER);
	m_static_message.set_tagged_text(v_tagged);
	//20260706 by claude. 배너는 단일 라인 — 큰 화살표(<sz=13>)와 작은 상태/권장 텍스트를 라인 안에서 세로중앙 정렬(set_tagged_text 뒤 호출).
	m_static_message.set_line_align(0, DT_VCENTER);

	//20260708 by claude. 판단 보류(크기·날짜 상반)면 권장 없음 → 확인버튼 비활성. 사용자가 라디오를 직접 클릭하면 활성(각 라디오 핸들러에서 EnableWindow(TRUE)).
	if (!recommend)
		m_button_ok.EnableWindow(FALSE);

	//파일명(완전 동일 시 회색). '&' 리터럴 표시 위해 plain + no_prefix 유지.
	Gdiplus::Color cr_name = identical ? cr_same : m_theme.cr_text;
	m_static_src_file.set_text_color(cr_name);   m_static_src_file.set_textf(_T("%s"), m_src_file.cFileName);
	m_static_dst_file.set_text_color(cr_name);   m_static_dst_file.set_textf(_T("%s"), m_dst_file.cFileName);

	//크기·수정시각: tagged text 부분강조(레이블 normal + 값·마커만 색/bold). 단일 라인 세로중앙 정렬 유지.
	CString src_size_val, dst_size_val;
	src_size_val.Format(_T("%s (%s)"), get_size_str(src_filesize.QuadPart, 1), get_size_str(src_filesize.QuadPart, 0));
	dst_size_val.Format(_T("%s (%s)"), get_size_str(dst_filesize.QuadPart, 1), get_size_str(dst_filesize.QuadPart, 0));

	//20260706 by claude. 마커(↑↓)를 <sz=13> 로 키웠으므로 값(작은 텍스트)과 크기가 달라진다 → 라인 안에서 세로중앙 정렬(set_line_align).
	m_static_src_filesize.set_valign(DT_VCENTER);  m_static_src_filesize.set_tagged_text(cmp_line(_S(IDS_FILE_SIZE), src_size_val, src_size_tag, src_size_state));   m_static_src_filesize.set_line_align(-1, DT_VCENTER);
	m_static_dst_filesize.set_valign(DT_VCENTER);  m_static_dst_filesize.set_tagged_text(cmp_line(_S(IDS_FILE_SIZE), dst_size_val, dst_size_tag, dst_size_state));   m_static_dst_filesize.set_line_align(-1, DT_VCENTER);
	m_static_src_mtime.set_valign(DT_VCENTER);     m_static_src_mtime.set_tagged_text(cmp_line(_S(IDS_FILE_LAST_MODIFIED_TIME), get_file_time_str(m_src_file.ftLastWriteTime), src_time_tag, src_time_state));   m_static_src_mtime.set_line_align(-1, DT_VCENTER);
	m_static_dst_mtime.set_valign(DT_VCENTER);     m_static_dst_mtime.set_tagged_text(cmp_line(_S(IDS_FILE_LAST_MODIFIED_TIME), get_file_time_str(m_dst_file.ftLastWriteTime), dst_time_tag, dst_time_state));   m_static_dst_mtime.set_line_align(-1, DT_VCENTER);

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

	int Exist = -1;

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

	//판단 보류로 아무 라디오도 선택되지 않은 상태 → 창을 닫지 않는다(확인버튼이 비활성이라 정상 흐름엔 도달 안 하나 방어적).
	if (Exist == -1)
		return;

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
	m_button_ok.EnableWindow(TRUE);		//판단 보류(크기·날짜 상반)로 비활성됐던 경우, 사용자가 직접 선택하면 확인버튼 활성.
}


void CExistFileDlg::OnBnClickedRadioOverwrite()
{
	m_button_ok.EnableWindow(TRUE);
}


void CExistFileDlg::OnBnClickedRadioSkip()
{
	m_button_ok.EnableWindow(TRUE);
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
