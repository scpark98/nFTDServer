﻿
// nFTDServer.h: PROJECT_NAME 애플리케이션에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

#include "../../Common/system/ShellImageList/ShellImageList.h"

#define	TITLEBAR_HEIGHT		26

// CnFTDServerApp:
// 이 클래스의 구현에 대해서는 nFTDServer.cpp을(를) 참조하세요.
//

class CnFTDServerApp : public CWinApp
{
public:
	CnFTDServerApp();

	//Shell의 imagelist 및 shell과 관계된 멤버 제공
	CShellImageList		m_shell_imagelist;

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CnFTDServerApp theApp;
