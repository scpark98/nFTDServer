﻿// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

CSCLog	gLog;
int g_FT_mode;
CRITICAL_SECTION g_cs;
std::mutex g_mutex;
// 미리 컴파일된 헤더를 사용하는 경우 컴파일이 성공하려면 이 소스 파일이 필요합니다.
