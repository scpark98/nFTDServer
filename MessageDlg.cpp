// MessageDlg.cpp: 구현 파일
//

#include "pch.h"
#include "nFTDServer.h"
#include "MessageDlg.h"
#include "afxdialogex.h"


// CMessageDlg 대화 상자

IMPLEMENT_DYNAMIC(CMessageDlg, CDialogEx)

CMessageDlg::CMessageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MESSAGE, pParent)
{

}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialogEx)
END_MESSAGE_MAP()


// CMessageDlg 메시지 처리기
