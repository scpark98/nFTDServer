#include "pch.h"
#include "nFTDServerSocket.h"
#include <mstcpip.h>

#include <thread>

extern HMODULE g_hRes;
extern RSAKey g_rsakey;

CnFTDServerSocket::CnFTDServerSocket()
{
	//m_dwWrite = WRITE_UNKNOWN;
	m_iServerNum = 0;
}

CnFTDServerSocket::~CnFTDServerSocket()
{
}

BOOL CnFTDServerSocket::Connection()
{
	//proxy Server
	/*
	TCHAR strMyDir[MAX_PATH];
	ZeroMemory(strMyDir, MAX_PATH);

	int res = GetModuleFileName(NULL, strMyDir, MAX_PATH);
	while (res > 0)
	{
		if (strMyDir[--res] == '\\')
		{
			strMyDir[res] = '\0';
			break;
		}
	}
	*/

	if (m_dwConnection == CONNECTION_CONNECT)
	{
		in_addr Inaddr;
		Inaddr.S_un.S_addr = m_addr;

		BOOL bRet = FALSE;

		// 프록시 터널링 초기화
		// 서비스 모드인경우 레지스트리에서 얻어오고
		// 아닌 경우 IE에서 읽어온다
		if (true)//neturoService::RunningAsService() == TRUE)
		{
			char buffer[256] = "Software\\Optimal\\RCS";
			char buffer2[256] = "\\proxy";
			strcat(buffer, buffer2);


#ifdef LMM_SERVICE
			m_sock.TunnelingInit(BLASTSOCK_PROXYTUNNELING, NULL, false, false, false, HKEY_LOCAL_MACHINE, "nFTDServer");
#elif ANYSUPPORT
			ZeroMemory(buffer, 256);
#ifdef LINKVNC
			LoadStringA(g_hRes, IDS_REG_HKEY_LOCAL_MACH_LINKVNC, buffer, 256);
#elif LINKEIGHT
			LoadStringA(g_hRes, IDS_REG_HKEY_LOCAL_MACH_LINKEIGHT, buffer, 256);
#else
			LoadStringA(g_hRes, IDS_REG_HKEY_LOCAL_MACH_ANYSUPPORT, buffer, 256);
#endif // LINKVNC
			m_sock.TunnelingInit(BLASTSOCK_PROXYTUNNELING, NULL, false, false, false, HKEY_LOCAL_MACHINE, buffer);
#else // !ANYSUPPORT && !LMM_SERVICE
			m_sock.TunnelingInit(BLASTSOCK_PROXYTUNNELING, NULL, false, false, false, HKEY_LOCAL_MACHINE, buffer);
			//m_sock.TunnelingInit(BLASTSOCK_NO_PROXYTUNNELING, NULL, false, false, false, HKEY_LOCAL_MACHINE, buffer);
#endif // LMM_SERVICE
		}
		else
		{
#ifdef LMM_SERVICE
			m_sock.TunnelingInit(BLASTSOCK_PROXYTUNNELING, NULL, true, false, true, HKEY_CURRENT_USER, "nFTDServer");
#elif ANYSUPPORT
			m_sock.TunnelingInit(BLASTSOCK_PROXYTUNNELING, NULL, true, false, true, HKEY_CURRENT_USER, NULL);
#else
			m_sock.TunnelingInit(BLASTSOCK_PROXYTUNNELING, NULL, true, false, true, HKEY_CURRENT_USER, NULL);
			//m_sock.TunnelingInit(BLASTSOCK_NO_PROXYTUNNELING, NULL, true, false, true, HKEY_CURRENT_USER, NULL);
#endif
		}

		for (int i = 0; i < 10; i++)
		{
			if (!m_sock.Create())
				continue;

			bRet = m_sock.Connect(inet_ntoa(Inaddr), m_port);

			if (bRet)
				break;

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		if (!bRet)
		{
			return FALSE;
		}

		logWrite(_T("m_iServerNum = %d"), m_iServerNum);
		if (m_iServerNum != 0)
		{
			//scpark 20240411 timeout 추가. timeout은 .Create() 후, CryptInit()전에 설정해줘야 한다.
			m_sock.SetTimeout(100000);

			// N2N 일 경우에는 일단 N2N서버와 암호화를 한다
			logWrite(_T("before m_sock.CryptInit(BLASTSOCK_CRYPT_RECVAESKEY)"));
#ifdef _NO_CRYPT
			if (!m_sock.CryptInit(BLASTSOCK_NO_CRYPT, NULL, &g_rsakey)) return FALSE;
#else
			if (!m_sock.CryptInit(BLASTSOCK_CRYPT_RECVAESKEY, NULL, &g_rsakey)) return FALSE;
#endif
			logWrite(_T("after m_sock.CryptInit(BLASTSOCK_CRYPT_RECVAESKEY)"));

			msg_server_num server_num;
			// N2N과의 커넥션타이밍을 맞추기 위해
			// Neturo Viewer 가 처음 N2N과 접속할때 하는짓을 한다.
			if (m_bIsN2NConnectionTiming == TRUE)
			{
				server_num.command = 620;
			}
			// nFTD가 N2N과의 커넥션타이밍을 맞출때는
			// 그냥 700으로
			else
			{
				server_num.command = AP2P_NFTD_CS_SERVERNUM;
			}

			server_num.servernum = m_iServerNum;
			if (!m_sock.SendExact((LPSTR)&server_num, sizeof(msg_server_num)))
			{
				return FALSE;
			}
		}
#ifdef _NO_CRYPT
		m_sock.CryptInit(BLASTSOCK_NO_CRYPT);
#else
		//client와의 암호화 초기화
		logWrite(_T("before m_sock.CryptInit(BLASTSOCK_CRYPT_CREATEAESKEY)"));
		bool b = m_sock.CryptInit(BLASTSOCK_CRYPT_CREATEAESKEY);
		logWrite(_T("after m_sock.CryptInit(BLASTSOCK_CRYPT_CREATEAESKEY)"));
		TRACE(_T("b = %d\n"), b);
#endif

	}
	else
	{
		blastsock sock;
		if (!sock.Create()) return FALSE;
		int optval = 1;
		sock.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
		if (!sock.Bind(m_port)) return FALSE;
		if (!sock.Listen()) return FALSE;
		if (!sock.Accept(m_sock)) return FALSE;
		sock.CloseSocket();
#ifdef _NO_CRYPT
		m_sock.CryptInit(BLASTSOCK_NO_CRYPT);
#else
		m_sock.CryptInit(BLASTSOCK_CRYPT_CREATEAESKEY);
#endif
	}

	tcp_keepalive keepAlive = { TRUE, 60000, 1000 };
	DWORD dwTmp;

	// 킵얼라이브 옵션을 켠다. 
	// 컴파일하려면 mstcpip.h 헤더 파일이 필요하다(platform sdk 참조).
	WSAIoctl(m_sock.GetSocket(), SIO_KEEPALIVE_VALS, &keepAlive, sizeof(keepAlive), 0, 0, &dwTmp, NULL, NULL);

	return TRUE;
}

BOOL CnFTDServerSocket::ConnectionRequest()
{
	msg ret;
	ret.type = nFTD_OpenDataConnection;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CnFTDServerSocket::Close()
{
	return m_sock.CloseSocket();
}

void CnFTDServerSocket::SetSockAddr(ULONG addr, USHORT port, int iServerNum, BOOL bIsN2NConnectionTiming)
{
	m_addr = addr;
	m_port = port;
	m_iServerNum = iServerNum;
	m_bIsN2NConnectionTiming = bIsN2NConnectionTiming;
}

void CnFTDServerSocket::SetConnection(DWORD dwConnection)
{
	m_dwConnection = dwConnection;
}

BOOL CnFTDServerSocket::IsOpen()
{
	if (m_sock.GetSocket() != INVALID_SOCKET)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CnFTDServerSocket::FileSize(LPTSTR lpPathName, ULARGE_INTEGER* ulFileSize)
{
	msgString1 str1;
	msgFileSize MsgFileSize;

	str1.type = nFTD_FileSize;
	str1.length = _tcslen(lpPathName) * 2;

	if (!m_sock.SendExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpPathName, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&MsgFileSize, sz_msgFileSize, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}

	if (MsgFileSize.type == nFTD_ERROR)
	{
		return FALSE;
	}
#ifdef MOBILE_FILETRANSFER
	OutputDebugString(_T("MOBILE_FILETRANSFER"));
	ULARGE_INTEGER tempSize;
	tempSize.QuadPart = MsgFileSize.nFileSize;
	ulFileSize->LowPart = tempSize.LowPart;
	ulFileSize->HighPart = tempSize.HighPart;
#else
	ulFileSize->LowPart = MsgFileSize.nFileSizeLow;
	ulFileSize->HighPart = MsgFileSize.nFileSizeHigh;
#endif

	return TRUE;
}

BOOL CnFTDServerSocket::FileList(WIN32_FIND_DATA* pFileInfo)
{
	msg ret;

	ret.type = nFTD_FileList;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}

	return NextFileList(pFileInfo);
}

BOOL CnFTDServerSocket::FileList2(WIN32_FIND_DATA* pFileInfo, LPCTSTR lpPath)
{
	msg ret;

	ret.type = nFTD_FileList2;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}

	USHORT length = _tcslen(lpPath) * 2;

	if (!m_sock.SendExact((LPSTR)&length, sizeof(USHORT), BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpPath, length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}

	return NextFileList(pFileInfo);
}

BOOL CnFTDServerSocket::NextFileList(WIN32_FIND_DATA* pFileInfo)
{
	msgFileInfo msgFindFileData;

	if (!m_sock.RecvExact((LPSTR)&msgFindFileData, sz_msgFileInfo, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (msgFindFileData.type == nFTD_END)
	{
		pFileInfo->dwFileAttributes = 0;
		return TRUE;
	}

	if (msgFindFileData.type != nFTD_OK)
	{
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)pFileInfo->cFileName, msgFindFileData.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}

	pFileInfo->cFileName[msgFindFileData.length / 2] = '\0';
	pFileInfo->dwFileAttributes = msgFindFileData.dwFileAttributes;
#ifdef MOBILE_FILETRANSFER
	ULARGE_INTEGER tempSize;
	tempSize.QuadPart = msgFindFileData.nFileSize;
	pFileInfo->nFileSizeHigh = tempSize.HighPart;
	pFileInfo->nFileSizeLow = tempSize.LowPart;
	pFileInfo->ftLastWriteTime = POSIX_to_FileTime(msgFindFileData.ftLastWriteTime);
#else
	pFileInfo->nFileSizeHigh = msgFindFileData.nFileSizeHigh;
	pFileInfo->nFileSizeLow = msgFindFileData.nFileSizeLow;
	memcpy(&(pFileInfo->ftLastWriteTime), &(msgFindFileData.ftLastWriteTime), sizeof(FILETIME));
#endif

	return TRUE;
}

BOOL CnFTDServerSocket::DriveList(PUINT pDriveType, LPTSTR lpDriveName)
{
	msg ret;
	ret.type = nFTD_DriveList;

	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}

	return NextDriveList(pDriveType, lpDriveName);
}

BOOL CnFTDServerSocket::NextDriveList(PUINT pDriveType, LPTSTR lpDriveName)
{
	msgDriveInfo msgFindDriveData;

	if (!m_sock.RecvExact((LPSTR)&msgFindDriveData, sz_msgDriveInfo, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}

	if (msgFindDriveData.type == nFTD_END)
	{
		*pDriveType = 0;
		return TRUE;
	}

	if (!m_sock.RecvExact((LPSTR)lpDriveName, msgFindDriveData.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	*pDriveType = msgFindDriveData.driveType;
	return TRUE;
}

BOOL CnFTDServerSocket::CreateDirectory(LPCTSTR lpPathName)
{
	msgString1 str1;
	msg ret;

	str1.type = nFTD_CreateDirectory;
	str1.length = _tcslen(lpPathName) * 2;

	if (!m_sock.SendExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpPathName, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}
	if (ret.type == nFTD_OK)
	{
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::Rename(LPCTSTR lpOldName, LPCTSTR lpNewName)
{
	msgString2 str2;
	msg ret;

	str2.type = nFTD_Rename;
	str2.length1 = _tcslen(lpOldName) * 2;
	str2.length2 = _tcslen(lpNewName) * 2;

	if (!m_sock.SendExact((LPSTR)&str2, sz_msgString2, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpOldName, str2.length1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpNewName, str2.length2, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-4 : %d"), GetLastError());
		return FALSE;
	}
	if (ret.type == nFTD_OK)
	{
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::DeleteDirectory(LPCTSTR lpPath)
{
	msgString1 str1;
	msg ret;

	str1.type = nFTD_DeleteDirectory;
	str1.length = _tcslen(lpPath) * 2;

	if (!m_sock.SendExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpPath, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}
	if (ret.type == nFTD_OK)
	{
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::DeleteFile(LPCTSTR lpPathName)
{
	msgString1 str1;
	msg ret;

	str1.type = nFTD_DeleteFile;
	str1.length = _tcslen(lpPathName) * 2;

	if (!m_sock.SendExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpPathName, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}
	if (ret.type == nFTD_OK)
	{
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::ChangeDirectory(LPCTSTR lpDirName)
{
	msgString1 str1;
	msg ret;

	str1.type = nFTD_ChangeDirectory;
	str1.length = _tcslen(lpDirName) * 2;

	if (!m_sock.SendExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpDirName, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}
	if (ret.type == nFTD_OK)
	{
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::ExecuteFile(LPCTSTR lpDirName)
{
	msgString1 str1;
	msg ret;

	str1.type = nFTD_ExecuteFile;
	str1.length = _tcslen(lpDirName) * 2;

	if (!m_sock.SendExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.SendExact((LPSTR)lpDirName, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}
	if (ret.type == nFTD_OK)
	{
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::TotalSpace(PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	msg ret;
	msgDiskSpace msgTotalNumberOfFreeBytes;

	ret.type = nFTD_TotalSpace;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&msgTotalNumberOfFreeBytes, sz_msgDiskSpace, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (msgTotalNumberOfFreeBytes.type == nFTD_OK)
	{
		memcpy(lpTotalNumberOfFreeBytes, &(msgTotalNumberOfFreeBytes.space), sizeof(ULARGE_INTEGER));
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::RemainSpace(PULARGE_INTEGER lpTotalNumberOfRemainBytes)
{
	msg ret;
	msgDiskSpace msgTotalNumberOfRemainBytes;

	ret.type = nFTD_RemainSpace;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&msgTotalNumberOfRemainBytes, sz_msgDiskSpace, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}

	if (msgTotalNumberOfRemainBytes.type == nFTD_OK)
	{
		memcpy(lpTotalNumberOfRemainBytes, &(msgTotalNumberOfRemainBytes.space), sizeof(ULARGE_INTEGER));
		return TRUE;
	}
	else
	{
		logWriteE(_T("Receive Not OK"));
		return FALSE;
	}
}

BOOL CnFTDServerSocket::CurrentPath(DWORD nBufferLength, LPTSTR lpCurrentPath)
{
	msg ret;
	msgString1 str1;

	ret.type = nFTD_CurrentPath;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}

	if (!m_sock.RecvExact((LPSTR)&str1, sz_msgString1, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (str1.type != nFTD_OK)
	{
		return FALSE;
	}

	if (!m_sock.RecvExact((LPSTR)lpCurrentPath, str1.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL CnFTDServerSocket::GetDesktopPath(WIN32_FIND_DATA* pFileInfo)
{
	msg ret;
	msgFileInfo msgFindFileData;
	memset(&msgFindFileData, 0, sizeof(msgFileInfo));

	ret.type = nFTD_DesktopPath;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&msgFindFileData, sz_msgFileInfo, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)pFileInfo->cFileName, msgFindFileData.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}

	pFileInfo->cFileName[msgFindFileData.length / 2] = '\0';

	return TRUE;
}

BOOL CnFTDServerSocket::GetDocumentPath(WIN32_FIND_DATA* pFileInfo)
{
	msg ret;
	msgFileInfo msgFindFileData;
	memset(&msgFindFileData, 0, sizeof(msgFileInfo));

	ret.type = nFTD_DocumentPath;
	if (!m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)&msgFindFileData, sz_msgFileInfo, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return FALSE;
	}
	if (!m_sock.RecvExact((LPSTR)pFileInfo->cFileName, msgFindFileData.length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return FALSE;
	}

	pFileInfo->cFileName[msgFindFileData.length / 2] = '\0';

	return TRUE;
}

void CnFTDServerSocket::SetFileWriteMode(DWORD dwWrite)
{
	m_dwWrite = dwWrite;
}

