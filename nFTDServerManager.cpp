#include "pch.h"
#include "nFTDServer.h"
#include "nFTDServerManager.h"
#include "../../Common/Functions.h"

CnFTDServerManager::CnFTDServerManager()
{
	InitializeCriticalSection(&m_CS);

	m_strServerIP = _T("");
	m_strDeviceID = _T("");
	m_strDeviceName = _T("");
	m_isP2PConnection = FALSE;

	//SetConfigPath();

	//m_nClientOSType = OS_WIN7;
}

CnFTDServerManager::~CnFTDServerManager()
{
	m_socket.Close();
	m_DataSocket.Close();
	DeleteCriticalSection(&m_CS);
}

BOOL CnFTDServerManager::SetConnectionService()
{
	DWORD dwConnectionMode = 0;
	ULONG ulAP2PAddress = 0;
	USHORT ushAP2PPort = 0;
	int nServerNum = 0;

	if (__argc < 4)
		return false;

	CString strConnectType = __targv[1];
	CString strPopupMode = __targv[2];
	CString strStatisticsMode = __targv[3];

	if (strConnectType == _T("-l"))
	{
		if (__argc < 5)
			return false;

		g_FT_mode = FT_MODE_P2P_S;
		dwConnectionMode = CONNECTION_LISTEN;
		ushAP2PPort = (USHORT)_ttoi(__targv[4]);
	}
	else if (strConnectType == _T("-c"))
	{
		if (__argc < 15)
			return false;

		g_FT_mode = FT_MODE_P2P_C;
		dwConnectionMode = CONNECTION_CONNECT;
		m_isP2PConnection = TRUE;

		m_strServerIP = __targv[4];
		//g_serverIp = m_strServerIP;
		ulAP2PAddress = inet_addr(unicodeToMultibyte(__targv[4]).c_str());
		ushAP2PPort = (USHORT)_ttoi(__targv[5]);
		//nServerNum = _ttoi(__wargv[6]);
		m_strDeviceID = __targv[7];
		m_strDeviceName = __targv[8];
		m_strDeviceIP = m_strServerIP;
		m_strManagerID = __targv[9];
		m_strCompanyKey = __targv[10];
		m_strToken = __targv[11];
		m_strViewerPublicIP = __targv[12];
		m_strViewerPrivateIP = __targv[13];
		m_nClientOSType = _ttoi(__targv[14]);

		m_strStatusbarTitle.Format(_S(IDS_TRANSFER), m_strDeviceName);
		m_strStatusbarTitle += _T(" (고속전송 모드)");
		m_strTitle = m_strDeviceName + _T(" (고속전송 모드)");

		//neturoService::SetServiceMode(TRUE);
	}
	else if (strConnectType == _T("-p"))
	{
		if (__argc < 15)
			return false;

		g_FT_mode = FT_MODE_AP2P;
		dwConnectionMode = CONNECTION_CONNECT;
		m_isP2PConnection = FALSE;

		m_strServerIP = __targv[4];
		//g_serverIp = m_strServerIP;
		ulAP2PAddress = inet_addr(unicodeToMultibyte(__targv[4]).c_str());
		ushAP2PPort = (USHORT)_ttoi(__targv[5]);
		nServerNum = _ttoi(__targv[6]);
		m_strDeviceID = __targv[7];
		m_strDeviceName = __targv[8];
		m_strManagerID = __targv[9];
		m_strCompanyKey = __targv[10];
		m_strToken = __targv[11];
		m_strViewerPublicIP = __targv[12];
		m_strViewerPrivateIP = __targv[13];
		m_nClientOSType = _ttoi(__targv[14]);

		m_strStatusbarTitle.Format(_S(IDS_TRANSFER), m_strDeviceName);
		//m_strStatusbarTitle += _T(" (AP2P)");
		m_strTitle = m_strDeviceName;

		//neturoService::SetServiceMode(TRUE);
	}

	//if (strStatisticsMode == _T("-statisticsmode"))
	//{
	//	m_isStatisticsMode = TRUE;
	//}
	//else
	//{
	//	m_isStatisticsMode = FALSE;
	//}

	m_socket.SetConnection(dwConnectionMode);
	m_socket.SetSockAddr(ulAP2PAddress, ushAP2PPort, nServerNum, FALSE);
	m_DataSocket.SetConnection(dwConnectionMode);
	m_DataSocket.SetSockAddr(ulAP2PAddress, ushAP2PPort, nServerNum, FALSE);

	return TRUE;
}


BOOL CnFTDServerManager::SetConnection(LPTSTR lptCmdLine)
{
	LPSTR lpCmdOpt;
	DWORD dwConnectionMode = 0;
	ULONG ulAP2PAddress = 0;
	USHORT ushAP2PPort = 0;
	int nServerNum = 0;
	BOOL isStandAlone;

	LPSTR lpCmdLine = new CHAR[MAX_PATH];
	ZeroMemory(lpCmdLine, MAX_PATH);
	int nLen = WideCharToMultiByte(CP_ACP, 0, lptCmdLine, -1, NULL, NULL, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, lptCmdLine, -1, lpCmdLine, nLen, NULL, NULL);

	if (strlen(lpCmdLine) < 2)
	{
		return FALSE;
	}

	lpCmdOpt = strtok(lpCmdLine, " ");
	if (!strcmp(lpCmdOpt, "-l"))
	{
		dwConnectionMode = CONNECTION_LISTEN;
		ushAP2PPort = (USHORT)atoi(strtok(NULL, " "));
	}
	else if (!strcmp(lpCmdOpt, "-c")) // connect
	{
		dwConnectionMode = CONNECTION_CONNECT;
		ulAP2PAddress = inet_addr(strtok(NULL, " "));
		ushAP2PPort = (USHORT)atoi(strtok(NULL, " "));

		m_isP2PConnection = TRUE;
		m_strServerIP = _T("P2P");
	}
	else if (!strcmp(lpCmdOpt, "-p")) // pat to pat . NMS 에 접속
	{
		dwConnectionMode = CONNECTION_CONNECT;

		LPCSTR lpcServerAddress = strtok(NULL, " ");
		TCHAR tchServerAddress[30] = { 0, };
		MultiByteToWideChar(CP_ACP, 0, lpcServerAddress, -1, tchServerAddress, 30);
		m_strServerIP.Format(_T("%s"), tchServerAddress);
		//g_serverIp.Format(_T("%s"), tchServerAddress);

		ulAP2PAddress = inet_addr(lpcServerAddress);
		ushAP2PPort = (USHORT)atoi(strtok(NULL, " "));
		nServerNum = atoi(strtok(NULL, " "));
		m_isP2PConnection = FALSE;
	}
	else
	{
		return FALSE;
	}

	lpCmdOpt = strtok(NULL, " ");
#ifdef ANYSUPPORT
	if (lpCmdOpt != NULL)
	{
		if (!strcmp(lpCmdOpt, "-viewermode"))
		{
			lpCmdOpt = strtok(NULL, " ");
			if (lpCmdOpt != NULL)
				m_nClientOSType = atoi(lpCmdOpt);
		}
		else
		{
			m_nClientOSType = atoi(lpCmdOpt);
		}
	}
#elif LMM_SERVICE
	if (lpCmdOpt != NULL)
	{
		if (!strcmp(lpCmdOpt, "-viewermode"))
		{
			//m_nClientOSType = OS_WIN7;
		}
		else
		{
			TCHAR tchManagerName[50] = { 0, };
			MultiByteToWideChar(CP_ACP, 0, lpCmdOpt, -1, tchManagerName, 50);

			m_strDeviceName.Format(_T("%s"), tchManagerName);
			m_strDeviceName.Replace(_T("&nbsp;"), _T(" "));

			//m_nClientOSType = OS_MAC;
			logWrite(_T("Do OS_MAC1"));
		}
	}
	else
	{
		//m_nClientOSType = OS_MAC;
		logWrite(_T("Do OS_MAC2"));
	}
#endif // ANYSUPPORT

	// standalone 인지 여부
	if (lpCmdOpt != NULL && !strcmp(lpCmdOpt, "-standalone"))
	{
		//neturoService::SetServiceMode(FALSE);
		isStandAlone = TRUE;
	}
	else
	{
		//neturoService::SetServiceMode(TRUE);
		isStandAlone = FALSE;
	}

	//m_isStatisticsMode = FALSE;
	if (!isStandAlone)
	{
		// pjh
		if (lpCmdOpt != NULL && !strcmp(lpCmdOpt, "-s"))
		{
			lpCmdOpt = strtok(NULL, " "); // hSocket (not used)
		}

		lpCmdOpt = strtok(NULL, " ");
		if (lpCmdOpt != NULL && !strcmp(lpCmdOpt, "-statisticsmode"))
		{
			deviceID = strtok(NULL, " ");
			deviceIP = strtok(NULL, " ");
			m_strManagerID = strtok(NULL, " ");
			managerIP = strtok(NULL, " ");
#ifdef LMM_SERVICE
			m_strCompanyKey = strtok(NULL, " ");
			m_strToken = strtok(NULL, " ");
			LPSTR deviceName = strtok(NULL, " ");
			m_nClientOSType = atoi(strtok(NULL, " "));


			TCHAR wDeviceName[50];
			MultiByteToWideChar(CP_ACP, 0, deviceName, -1, wDeviceName, 50);

			m_strDeviceName.Format(_T("%s"), wDeviceName);
			m_strDeviceName.Replace(_T("&nbsp;"), _T(" "));
#endif
			//m_isStatisticsMode = TRUE;

			TCHAR wDeviceID[50];
			MultiByteToWideChar(CP_ACP, 0, deviceID, -1, wDeviceID, 50);

			m_strDeviceID.Format(_T("%s"), wDeviceID);
			m_strDeviceID.Replace(_T("&nbsp;"), _T(" "));

#ifdef LMM_SERVICE
			if (m_strDeviceName != _T(""))
			{
				//m_strTitle.Format(IDS_DEVICE_NAME, m_strDeviceName);
				//m_strStatusbarTitle.Format(IDS_TRANSFER, m_strDeviceName);
			}
#elif ANYSUPPORT
			m_strTitle.Format(IDS_DEVICE_NAME, m_strDeviceName);
			m_strStatusbarTitle.Format(IDS_TRANSFER, m_strDeviceName);
#else
			m_strTitle.Format(IDS_DEVICE_NAME, m_strDeviceID);
			m_strStatusbarTitle.Format(IDS_TRANSFER, m_strDeviceID);
#endif // LMM_SERVICE
		}
		else {
#ifdef LMM_SERVICE
			//m_strStatusbarTitle.Format(IDS_TRANSFER, m_strDeviceName);
			m_strTitle = m_strDeviceName;
#endif
		}
	}

	m_socket.SetConnection(dwConnectionMode);
	m_socket.SetSockAddr(ulAP2PAddress, ushAP2PPort, nServerNum, isStandAlone);
	m_DataSocket.SetConnection(dwConnectionMode);
	m_DataSocket.SetSockAddr(ulAP2PAddress, ushAP2PPort, nServerNum, FALSE);

	return TRUE;
}

BOOL CnFTDServerManager::Connection()
{
	m_is_connected = m_socket.Connection();
	return m_is_connected;
}

BOOL CnFTDServerManager::DataConnect()
{
	m_bDataClose = TRUE;
	m_socket.ConnectionRequest();
	return m_DataSocket.Connection();
}

BOOL CnFTDServerManager::DataClose()
{
	m_bDataClose = FALSE;
	return m_DataSocket.Close();
}

void CnFTDServerManager::close_socket()
{
	m_socket.Close();
	DataClose();
}

void CnFTDServerManager::DriveList(std::deque<CString>* dq)
{
	UINT DriveType;
	TCHAR DriveName[MAX_PATH] = { 0, };

	if (!m_socket.DriveList(&DriveType, DriveName))
	{
		KeepConnection();
		return;
	}

	while (1)
	{
		//pDrvComboBox->InsertDrive(DriveName);
		TRACE(_T("DriveName = %s\n"), DriveName);
		dq->push_back(DriveName);
		//pShellTreeCtrl->insert_drive(DriveName);
		if (DriveName[_tcslen(DriveName) - 3] == 'C')
		{
			//pDrvComboBox->SetCurSelection(pDrvComboBox->GetCount() - 1);
		}

		ZeroMemory(DriveName, MAX_PATH);
		/*
		if (dwSide == CnFTDServerManager::SERVERSIDE)
		{
			if (!m_FileManager.NextDriveList(&DriveType, DriveName))
				break;
		}
		else
		*/
		{
			if (!m_socket.NextDriveList(&DriveType, DriveName))
			{
				KeepConnection();
				return;
			}
			if (DriveType == 0)
				break;
		}
	}
}

void CnFTDServerManager::refresh_list(std::deque<WIN32_FIND_DATA> *dq, bool is_server_side)
{
	WIN32_FIND_DATA FindFileData;
	dq->clear();
	//pListCtrl->delete_all_items(true);

	if (is_server_side)
	{
		//pListCtrl->refresh_list();
		/*
		m_FileManager.FileList(&FindFileData);

		do {
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0) // 숨김파일 출력 X
				continue;
			//if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일 출력
			pShellListCtrl->ShowFile(&FindFileData);
		} while (m_FileManager.NextFileList(&FindFileData));
		*/
	}
	else
	{
		m_socket.FileList(&FindFileData);

		do
		{
			if (FindFileData.dwFileAttributes == 0)
				break;

			if (_tcscmp(FindFileData.cFileName, _T(".")) == 0 ||
				_tcscmp(FindFileData.cFileName, _T("..")) == 0)
				continue;


			//숨김파일 출력 X
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0)
				continue;

			//TRACE(_T("%s\n"), FindFileData.cFileName);
			//pListCtrl->add_file(&FindFileData);
			dq->push_back(FindFileData);

			/*
			// for optimal
			if (TRUE == bRoot && _tcscmp(FindFileData.cFileName, _T("..")) == 0)
			{
				continue;
			}

			//if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일 출력
			pShellListCtrl->ShowFile(&FindFileData);
			*/
		} while (m_socket.NextFileList(&FindFileData));

		//pListCtrl->display_list();
	}
}

void CnFTDServerManager::refresh_tree(CSCTreeCtrl* pTreeCtrl, bool is_server_side)
{
	UINT DriveType;
	TCHAR DriveName[MAX_PATH] = { 0, };

	pTreeCtrl->refresh();

	if (is_server_side)
	{
		return;
		//pTreeCtrl->
	}
	else
	{
		if (!m_socket.DriveList(&DriveType, DriveName))
		{
			KeepConnection();
			return;
		}
	}

	while (1)
	{
		//pDrvComboBox->InsertDrive(DriveName);
		//pTreeCtrl->InsertDrive(DriveName);
		TRACE(_T("DriveName = %s\n"), DriveName);
		pTreeCtrl->insert_drive(DriveName);

		if (DriveName[_tcslen(DriveName) - 3] == 'C')
		{
			//pDrvComboBox->SetCurSelection(pDrvComboBox->GetCount() - 1);
		}

		ZeroMemory(DriveName, MAX_PATH);
		if (is_server_side)
		{
			return;
			//if (!m_FileManager.NextDriveList(&DriveType, DriveName))
			//	break;
		}
		else
		{
			if (!m_socket.NextDriveList(&DriveType, DriveName))
			{
				KeepConnection();
				return;
			}

			if (DriveType == 0)
				break;
		}
	}
}

//이전 코드 함수명 : GetFileList(CShellTreeCtrl* pShellTreeCtrl)
//tree에서 해당 path folder의 sub folder 목록을 refresh한다.
//선택할 때가 아니라 폴더를 처음 펼칠 때, 새로고침 할 때 사용된다.
void CnFTDServerManager::refresh_tree_folder(CSCTreeCtrl* pShellTreeCtrl, CString path)
{
	WIN32_FIND_DATA FindFileData;

	if (!m_socket.FileList2(&FindFileData, path))
	{
		logWriteE(_T("FileList2 Fail"));
	}

	do {
		if (FindFileData.dwFileAttributes == 0)
		{
			break;
		}

		// for optimal
		if (_tcscmp(FindFileData.cFileName, _T(".")) == 0 ||
			_tcscmp(FindFileData.cFileName, _T("..")) == 0)
		{
			continue;
		}

		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0) // 숨김파일 출력 X
		{
			continue;
		}

		//폴더이면 추가
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			pShellTreeCtrl->insert_folder(&FindFileData);
		}

	} while (m_socket.NextFileList(&FindFileData));
}

void CnFTDServerManager::KeepConnection()
{
	if (!m_socket.IsOpen())
	{
		TRACE(_T("!m_socket.IsOpen()"));

		//CString strMessage;
		//strMessage.LoadString(IDS_DISCONNECT);

		//CMessageDlg dlgMessage;
		//dlgMessage.SetMessage(MsgType::TypeOK, strMessage);
		//dlgMessage.DoModal();

		ExitProcess(0);
	}
}

void CnFTDServerManager::KeepDataConnection()
{
	if (!m_DataSocket.IsOpen() && m_bDataClose)
	{
		TRACE(_T("!m_DataSocket.IsOpen() && m_bDataClose"));
		//CString strMessage;
		//strMessage.LoadString(IDS_DISCONNECT);

		//CMessageDlg dlgMessage;
		//dlgMessage.SetMessage(MsgType::TypeOK, strMessage);
		//dlgMessage.DoModal();

		ExitProcess(0);
	}
}

bool CnFTDServerManager::change_directory(LPCTSTR lpPath, DWORD dwSide, bool bDataSocket)
{
	logWrite(_T("Path : %s"), lpPath);

	bool bRet;

	if (dwSide == SERVER_SIDE)
	{
		if (_tchdir(lpPath) == 0)
			return true;
		else
			return false;
	}
	else
	{
		if (!bDataSocket)
		{
			if (!(bRet = m_socket.change_directory(lpPath)))
				KeepConnection();
		}
		else
		{
			if (!(bRet = m_DataSocket.change_directory(lpPath)))
				KeepDataConnection();
		}
	}

	/*
	if (pDrvComboBox == NULL || !bRet)
		return bRet;

	if (_tcslen(lpPath) == 2 && !_tcsncmp(lpPath, _T(".."), 2))
	{
		if (m_nClientOSType != OS_MAC)
			pDrvComboBox->DeleteFolder();
	}
	else
	{
		LPTSTR temp = _tcsdup(lpPath);
		if (bRet)
		{
			if (m_nClientOSType != OS_MAC)
				pDrvComboBox->InsertFolder(temp);
		}
		delete[] temp;
	}
	*/
	return bRet;
}
/*
void CnFTDServerManager::ChangeDrvComboBox(CDrvComboBox* pDrvComboBox, DWORD dwSide)
{
	if (pDrvComboBox->GetDepth(pDrvComboBox->GetCurSel()) == 0)
	{	// Drive
		CString LBText, Drive;
		pDrvComboBox->GetLBText(pDrvComboBox->GetCurSel(), LBText);

		if (dwSide == CnFTDServerManager::SERVERSIDE)
		{
			if (LBText.Compare(_T("바탕화면")) == 0)
			{
				TCHAR path[MAX_PATH];
				neturoService::GetSpectialPath(path, GUID_FOLDER_DESKTOP);

				Drive.Format(_T("%s"), path);
			}
			else if (LBText.Compare(_T("내문서")) == 0)
			{
				TCHAR path[MAX_PATH];
				neturoService::GetSpectialPath(path, GUID_FOLDER_MYDOC);
				Drive.Format(_T("%s"), path);
			}
			else
			{
				Drive.Format(_T("%c:\\"), LBText[LBText.ReverseFind(':') - 1]);
			}
		}
		else
		{
			if (LBText.Compare(_T("바탕화면")) == 0)
			{
				Drive.Format(_T("%s"), LBText);
			}
			else if (LBText.Compare(_T("내문서")) == 0)
			{
				Drive.Format(_T("%s"), LBText);
			}
			else
			{
				Drive.Format(_T("%c:\\"), LBText[LBText.ReverseFind(':') - 1]);
			}
		}

		change_directory(Drive, NULL, dwSide);
		while (pDrvComboBox->DeleteFolder(0)) {}
		pDrvComboBox->SetCurSelection(pDrvComboBox->GetCurSel());
	}
	else
	{	// Folder
		while (pDrvComboBox->DeleteFolder(pDrvComboBox->GetDepth(pDrvComboBox->GetCurSel())))
		{
			change_directory(_T(".."), NULL, dwSide);
		}
	}
}
*/
bool CnFTDServerManager::CurrentPath(DWORD dwPathLength, LPTSTR lpPath, DWORD dwSide)
{
	if (dwSide == SERVER_SIDE)
		return true;// m_FileManager.CurrentPath(dwPathLength, lpPath);

	if (m_socket.CurrentPath(dwPathLength, lpPath))
		return true;

	KeepConnection();
	return false;
}

bool CnFTDServerManager::create_directory(LPCTSTR lpPath, DWORD dwSide, bool bDataSocket)
{
	if (dwSide == SERVER_SIDE)
	{
		return make_full_directory(lpPath);
	}
	else
	{
		if (!bDataSocket)
		{
			if (m_socket.create_directory(lpPath))
			{
				return true;
			}
			else
			{
				KeepConnection();
				return false;
			}
		}
		else
		{
			if (m_DataSocket.create_directory(lpPath))
			{
				return true;
			}
			else
			{
				KeepDataConnection();
				return false;
			}
		}
	}

	return false;
}


// 실질적인 화일전송부분. recursive 하게 구현되었다
// depth : recursive 의 깊이. 하위폴더인지판별하기위해사용
BOOL CnFTDServerManager::FileTransferInitalize(CVtListCtrlEx* pShellListCtrl, CVtListCtrlEx* pXList, CListCtrl* pDepthList, ULARGE_INTEGER& ulTotalSize, DWORD dwSide, CString& strStartPath)
{
	INT CurrentDepth = 0, iIndex;
	WIN32_FIND_DATA FindFileData;
	TCHAR From[1024];
	TCHAR temp[1024];
	CString To;
	CString Ext, File;
	BOOL bRet;
	ULARGE_INTEGER ulFileSize;

	m_DataSocket.SetFileWriteMode(WRITE_UNKNOWN);

	ulTotalSize.QuadPart = 0;


	// pShellList 꺼를 복제한다
	POSITION pos = pShellListCtrl->GetFirstSelectedItemPosition();


	if (!strStartPath.IsEmpty())
		change_directory(strStartPath, dwSide, TRUE);

	TCHAR temp2[20];

	ZeroMemory(From, 1024);  CurrentPath(1024, From, dwSide);
	ZeroMemory(temp, 1024); CurrentPath(1024, temp, !dwSide);

	To.Format(_T("%s"), temp);

	while (pos)
	{
		//if (WaitForSingleObject(*pThreadTerminateEvent, 0) == WAIT_OBJECT_0) // termination flag signaled
		//	return FALSE;

		int wparam = 0;

		if (pShellListCtrl->GetItemText((int)pos - 1, 1).GetLength() != 0)
		{	// file size
			File.Format(_T("%s"), pShellListCtrl->GetItemText((int)pos - 1, 0));
			int pExt = File.ReverseFind('.');
			if (pExt == -1)
				Ext.Format(_T("."));
			else
				Ext = File.Mid(pExt + 1, File.GetLength());

			iIndex = pXList->InsertItem(0, pShellListCtrl->GetItemText((int)pos - 1, 0), pShellListCtrl->m_pShellImageList->GetVirtualImageListIcon(Ext));

			pXList->SetItemText(iIndex, 2, _T("0%|"));
			ZeroMemory(temp, 1024);
			_stprintf(temp, _T("%s"), pShellListCtrl->GetItemText((int)pos - 1, 0));

			ulFileSize.QuadPart = 0;
			if (dwSide == SERVER_SIDE)
				m_FileManager.FileSize(temp, &ulFileSize);
			else
				m_socket.FileSize(temp, &ulFileSize);

			ulTotalSize.QuadPart += ulFileSize.QuadPart;

			ZeroMemory(temp, 1024);
			_ui64tot(ulFileSize.QuadPart, temp, 10);
			pXList->SetItemText(iIndex, 1, temp);
		}
		else
		{
			iIndex = pXList->InsertItem(0, pShellListCtrl->GetItemText((int)pos - 1, 0), pShellListCtrl->m_pShellImageList->GetVirtualImageListIcon(_T("\\")));
		}
		pXList->SetItemText(iIndex, 3, From);
		pXList->SetItemText(iIndex, 4, To);
		pDepthList->InsertItem(0, _T("0"));

		pShellListCtrl->GetNextSelectedItem(pos);
	}

	CurrentDepth = 0;
	for (int i = 0; i < pXList->GetItemCount(); i++)
	{
		// 상위폴더로
		while (CurrentDepth != _tstoi(pDepthList->GetItemText(i, 0)))
		{
			change_directory(_T(".."), dwSide, TRUE);
			CurrentDepth--;

			for (int j = To.GetLength() - 1; j > To.Find(_T("\\")); j--)
			{
				if (To[j] == '\\')
				{
					To.Delete(j); break;
				}
				To.Delete(j);
			}
		}

		if (pXList->GetItemText(i, 1).GetLength() == 0)
		{
			// change directory
			if (!change_directory(pXList->GetItemText(i, 0), dwSide, TRUE))
				continue;

			// get path
			ZeroMemory(From, 1024);
			CurrentPath(1024, From, dwSide);
			if (To[To.GetLength() - 1] != '\\')
				To += _T("\\");
			To += pXList->GetItemText(i, 0);

			// add depth
			CurrentDepth++;

			// file list update
			if (dwSide == SERVER_SIDE)
				m_FileManager.FileList(&FindFileData);
			else
				m_DataSocket.FileList(&FindFileData);

			do
			{
				//if (WaitForSingleObject(*pThreadTerminateEvent, 0) == WAIT_OBJECT_0) // termination flag signaled
				//	return FALSE;

				if (!(_tcslen(FindFileData.cFileName) == 1 && !_tcsncmp(FindFileData.cFileName, _T("."), 1)) && !(_tcslen(FindFileData.cFileName) == 2 && !_tcsncmp(FindFileData.cFileName, _T(".."), 2)))
				{
					LVITEM lvItem; memset(&lvItem, 0, sizeof(LVITEM));
					lvItem.mask = LVIF_IMAGE | LVIF_INDENT | LVIF_TEXT;
					lvItem.iItem = i + 1;
					lvItem.iIndent = CurrentDepth;
					lvItem.pszText = FindFileData.cFileName;

					if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
					{	// file size
						File.Format(_T("%s"), FindFileData.cFileName);
						int pExt = File.ReverseFind('.');
						if (pExt == -1) Ext.Format(_T("."));
						else Ext = File.Mid(pExt + 1, File.GetLength());

						lvItem.iImage = pShellListCtrl->m_pShellImageList->GetVirtualImageListIcon(Ext);
						pXList->InsertItem(&lvItem);
						pXList->SetItemText(i + 1, 2, _T("0%|"));
						ulFileSize.LowPart = FindFileData.nFileSizeLow;
						ulFileSize.HighPart = FindFileData.nFileSizeHigh;

						ulTotalSize.QuadPart += ulFileSize.QuadPart;
						ZeroMemory(temp, 1024);
						_ui64tot(ulFileSize.QuadPart, temp, 10);
						pXList->SetItemText(i + 1, 1, temp);
					}
					else
					{
						lvItem.iImage = pShellListCtrl->m_pShellImageList->GetVirtualImageListIcon(_T("\\"));
						pXList->InsertItem(&lvItem);
					}
					pXList->SetItemText(i + 1, 3, From);
					pXList->SetItemText(i + 1, 4, To);
					_stprintf(temp, _T("%d\0"), CurrentDepth);
					pDepthList->InsertItem(i + 1, temp);

				}

				if (dwSide == SERVER_SIDE)
					bRet = m_FileManager.NextFileList(&FindFileData);
				else
					bRet = m_DataSocket.NextFileList(&FindFileData);

			} while (bRet && ((dwSide == SERVER_SIDE) || (dwSide == CLIENT_SIDE && FindFileData.dwFileAttributes != 0)));
		}
	}

	while (CurrentDepth != 0)
	{
		change_directory(_T(".."), dwSide, TRUE);
		CurrentDepth--;
	}
	return TRUE;
}

//scpark add
bool CnFTDServerManager::get_filelist(LPCTSTR path, std::deque<WIN32_FIND_DATA> *dq, bool recursive)
{
	msg ret;

	//명령 전송
	ret.type = nFTD_filelist_all;
	if (!m_socket.m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return false;
	}

	USHORT length = _tcslen(path) * 2;

	//path 길이 전송
	if (!m_socket.m_sock.SendExact((LPSTR)&length, sizeof(USHORT), BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return false;
	}

	//path 전송
	if (!m_socket.m_sock.SendExact((LPSTR)path, length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return false;
	}

	//recursive 여부 전송
	if (!m_socket.m_sock.SendExact((LPSTR)&recursive, sizeof(bool), BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return false;
	}

	LPTSTR file_path[MAX_PATH] = { 0, };
	WIN32_FIND_DATA data;

	while (true)
	{
		ZeroMemory(&data, sizeof(data));

		if (!m_socket.m_sock.RecvExact((LPSTR)&data, sizeof(WIN32_FIND_DATA), BLASTSOCK_BUFFER))
		{
			logWriteE(_T("CODE-1 : %d "), GetLastError());
			return false;
		}

		if (_tcslen(data.cFileName) == 0)
			break;

		dq->push_back(data);
		TRACE(_T("%3d = %s\n"), dq->size() - 1, dq->back().cFileName);
	}

	return true;
}

bool CnFTDServerManager::get_folderlist(LPCTSTR path, std::deque<WIN32_FIND_DATA>* dq, bool fullpath)
{
	msg ret;

	//명령 전송
	ret.type = nFTD_folderlist_all;
	if (!m_socket.m_sock.SendExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-1 : %d"), GetLastError());
		return false;
	}

	USHORT length = _tcslen(path) * 2;

	//path 길이 전송
	if (!m_socket.m_sock.SendExact((LPSTR)&length, sizeof(USHORT), BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-2 : %d"), GetLastError());
		return false;
	}

	//path 전송
	if (!m_socket.m_sock.SendExact((LPSTR)path, length, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-3 : %d"), GetLastError());
		return false;
	}

	//응답 및 준비 결과 확인
	if (!m_socket.m_sock.RecvExact((LPSTR)&ret, sz_msg, BLASTSOCK_BUFFER))
	{
		logWriteE(_T("CODE-4 : %d"), GetLastError());
		return false;
	}

	if (ret.type == nFTD_ERROR)
		return false;

	LPTSTR file_path[MAX_PATH] = { 0, };
	WIN32_FIND_DATA data;

	while (true)
	{
		ZeroMemory(&data, sizeof(data));

		if (!m_socket.m_sock.RecvExact((LPSTR)&data, sizeof(WIN32_FIND_DATA), BLASTSOCK_BUFFER))
		{
			logWriteE(_T("CODE-1 : %d "), GetLastError());
			return false;
		}

		if (_tcslen(data.cFileName) == 0)
			break;

		dq->push_back(data);
		TRACE(_T("%3d = %s\n"), dq->size() - 1, dq->back().cFileName);
	}

	//client에서 넘어올 때 폴더목록이 fullpath로 넘어오므로 path길이만큼 앞에서 잘라준다.
	if (dq->size() > 0 && !fullpath)
	{
		CString sPath = GetParentDirectory(CString(dq->at(0).cFileName));
		int folder_length = sPath.GetLength() + 1;

		for (int i = 0; i < dq->size(); i++)
		{
			TCHAR* p = (data.cFileName);
			p += folder_length;
			_tcscpy(data.cFileName, p);
		}
	}

	return true;
}

bool CnFTDServerManager::get_remote_system_label(std::map<int, CString> *map)
{
	if (m_socket.get_remote_system_label(map))
		return true;

	return false;
}

bool CnFTDServerManager::get_remote_system_path(std::map<int, CString>* map)
{
	if (m_socket.get_remote_system_path(map))
		return true;

	return false;
}

CString CnFTDServerManager::GetRemoteDesktopPath()
{
	WIN32_FIND_DATA data;
	if (m_socket.GetDesktopPath(&data))
	{
		return data.cFileName;
	}
	
	return _T("");
}


CString CnFTDServerManager::GetRemoteDocumentPath()
{
	WIN32_FIND_DATA data;
	if (m_socket.GetDocumentPath(&data))
	{
		return data.cFileName;
	}

	return _T("");
}

