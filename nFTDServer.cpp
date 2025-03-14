﻿
// nFTDServer.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "nFTDServer.h"
#include "nFTDServerDlg.h"

#include "blastsocklib.h"
#include "neturoCrypto.h"
#include "SocketsInitializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CnFTDServerApp

BEGIN_MESSAGE_MAP(CnFTDServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CnFTDServerApp 생성

CnFTDServerApp::CnFTDServerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CnFTDServerApp 개체입니다.

CnFTDServerApp theApp;
RSAKey g_rsakey;
HMODULE g_hRes;


// CnFTDServerApp 초기화

BOOL CnFTDServerApp::InitInstance()
{
	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("Koino"));

	//LMM Service와 LMMSE Service의 차이 :
	//- 로그파일 위치
#if defined(LMMSE_SERVICE)
	gLog.set(get_known_folder(CSIDL_COMMON_DOCUMENTS) + _T("\\LinkMeMineSE\\Log\\FileTransfer"), get_exe_file_title());
#else
	gLog.set(get_known_folder(CSIDL_COMMON_DOCUMENTS) + _T("\\LinkMeMine\\Log\\FileTransfer"), get_exe_file_title());
#endif
	gLog.write_start_log();

	//개발 단계에서만 필요하며 실제 배포시에는 주석처리 할 것!
	gLog.set_log_level(SCLOG_LEVEL_DEBUG);

	logWrite(_T("cmdline = %s"), GetCommandLine());

	g_hRes = AfxGetInstanceHandle();
	SocketsInitializer socketsInitializer;

	//shellimagelist를 초기화 한 후 트리, 리스트에서 공통으로 사용한다.
	//m_shell_imagelist.Initialize();

	// rsa 키 생성
	neturoCrypto crypt;
	crypt.GenerateRSAKey(1024, g_rsakey.pvk, g_rsakey.pbk);


	m_config_ini.SetFileName(get_exe_directory() + _T("\\config.ini"));

	CnFTDServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}



int CnFTDServerApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWinApp::ExitInstance();
}
