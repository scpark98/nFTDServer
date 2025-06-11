
// nFTDServer.h: PROJECT_NAME 애플리케이션에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

#include "../../Common/colors.h"
#include "../../Common/system/ShellImageList/ShellImageList.h"
#include "../../Common/ini/EZIni/EZIni.h"

//메인창을 제외한 메시지박스, 전송창 등의 타이틀바는 메인창의 타이틀바보다는 작게 표시한다.
#define TOOLBAR_TITLE_HEIGHT			24
#define TOOLBAR_TITLE_BUTTON_WIDTH		26

// CnFTDServerApp:
// 이 클래스의 구현에 대해서는 nFTDServer.cpp을(를) 참조하세요.
//

class CnFTDServerApp : public CWinApp
{
public:
	CnFTDServerApp();

	//Shell의 imagelist 및 shell과 관계된 멤버 제공
	CShellImageList		m_shell_imagelist;

	EZIni				m_config_ini;
	int					m_color_theme = CSCColorTheme::color_theme_anysupport;

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CnFTDServerApp theApp;
