// nFTDFileTransferDialog.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "nFTDFileTransferDialog.h"
#include "afxdialogex.h"

#include <thread>


// CnFTDFileTransferDialog 대화 상자

IMPLEMENT_DYNAMIC(CnFTDFileTransferDialog, CDialogEx)

CnFTDFileTransferDialog::CnFTDFileTransferDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILETRANSFER, pParent)
{

}

CnFTDFileTransferDialog::~CnFTDFileTransferDialog()
{
}

void CnFTDFileTransferDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_static_message);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_STATIC_SPEED, m_static_speed);
	DDX_Control(pDX, IDC_STATIC_INDEX, m_static_index);
	DDX_Control(pDX, IDC_LIST, m_XFileList);
	DDX_Control(pDX, IDC_LIST_QUEUE, m_ListQueue);
}


BEGIN_MESSAGE_MAP(CnFTDFileTransferDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CnFTDFileTransferDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CnFTDFileTransferDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CnFTDFileTransferDialog 메시지 처리기


void CnFTDFileTransferDialog::OnBnClickedOk()
{
}


void CnFTDFileTransferDialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

BOOL CnFTDFileTransferDialog::FileTransferInitalize(CnFTDServerManager* pServerManager, CVtListCtrlEx* pShellListCtrl,
			ULARGE_INTEGER* pulRemainDiskSpace,	DWORD dwSide, BOOL isAutoClose, LPCTSTR lpszStartPath)
{
	if (pShellListCtrl->get_selected_items() == 0)
		return FALSE;

	m_dwSide = dwSide;
	m_pFileList = pShellListCtrl;
	m_pServerManager = pServerManager;


	return TRUE;
}

void CnFTDFileTransferDialog::init_list()
{
	m_XFileList.set_headings(_T("파일명,100;크기,100;상태,60;원본 위치,100;대상 위치,100"));
	//m_list.set_color_theme(CVtListCtrlEx::color_theme_dark_gray);
	//m_list.set_line_height(theApp.GetProfileInt(_T("list name"), _T("line height"), 80));

	m_XFileList.set_font_size(theApp.GetProfileInt(_T("list"), _T("font size"), 9));
	m_XFileList.set_font_name(theApp.GetProfileString(_T("list"), _T("font name"), _T("맑은 고딕")));

	m_XFileList.load_column_width(&theApp, _T("CnFTDFileTransferDialog list"));
	m_XFileList.set_header_height(24);

	//
	m_XFileList.set_use_own_imagelist(false);
	m_XFileList.set_line_height(20);

	//plist->set_column_text_align(0, HDF_CENTER);
	//plist->set_column_text_align(0, HDF_CENTER);
	//plist->set_column_text_align(1, HDF_CENTER);
	//plist->set_column_text_align(2, HDF_CENTER);
	/*
	plist->set_header_text_align(0, HDF_CENTER);
	plist->set_header_text_align(1, HDF_CENTER);
	plist->set_header_text_align(2, HDF_CENTER);
	plist->set_header_text_align(3, HDF_LEFT);
	*/
	//m_list.set_column_data_type(list_score, CVtListCtrlEx::column_data_type_percentage_grid);
	m_XFileList.set_column_data_type(col_status, CVtListCtrlEx::column_data_type_progress);
}


BOOL CnFTDFileTransferDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	init_list();

	std::thread th(&CnFTDFileTransferDialog::thread_transfer, this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CnFTDFileTransferDialog::thread_transfer()
{
	if (!m_pServerManager->DataConnect())
	{
		AfxMessageBox(_T("m_pServerManager->DataConnect() fail."));
		return;
	}

	// 전체 파일 리스트 얻어오기
	if (!m_pServerManager->FileTransferInitalize(m_pFileList, &m_XFileList, &m_ListQueue, m_ProgressData.ulTotalSize, m_dwSide, m_strStartPath))
	{
		return;
	}

}
