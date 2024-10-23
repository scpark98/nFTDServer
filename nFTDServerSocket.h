#pragma once

#include "nFTDProtocol.h"
#include "blastsock.h"

#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"

#define CONNECTION_CONNECT	0	
#define CONNECTION_LISTEN	1

#define SERVER_SIDE			0
#define CLIENT_SIDE			1

#define WRITE_UNKNOWN		0x00000000
#define WRITE_CONTINUE		0x00000100
#define WRITE_OVERWRITE		0x00000200
#define WRITE_IGNORE		0x00000400
#define WRITE_ALL			0x00001000


class CnFTDServerSocket
{
public:
	CnFTDServerSocket();
	virtual ~CnFTDServerSocket();

	BOOL Connection();			// 정상적인 connection
	BOOL ConnectionRequest();	// DataSocket Open 을 위한

	BOOL Close();
	BOOL IsOpen();

	void SetSockAddr(ULONG addr, USHORT port, int iServerNum, BOOL bIsN2NConnectionTiming);
	void SetConnection(DWORD dwConnection);


	BOOL CreateDirectory(LPCTSTR lpPathName);
	BOOL ExecuteFile(LPCTSTR lpDirName);
	BOOL Rename(LPCTSTR lpOldName, LPCTSTR lpNewName);
	BOOL DeleteDirectory(LPCTSTR lpPath);
	BOOL DeleteFile(LPCTSTR lpPathName);
	BOOL ChangeDirectory(LPCTSTR lpDirName);
	BOOL TotalSpace(PULARGE_INTEGER lpTotalNumberOfFreeBytes);
	BOOL RemainSpace(PULARGE_INTEGER lpTotalNumberOfRemainBytes);
	BOOL CurrentPath(DWORD nBufferLength, LPTSTR lpCurrentPath);

	BOOL FileSize(LPTSTR lpPathName, ULARGE_INTEGER* ulFileSize);
	BOOL FileList(WIN32_FIND_DATA* pFileInfo);
	BOOL FileList2(WIN32_FIND_DATA* pFileInfo, LPCTSTR lpPath);
	BOOL NextFileList(WIN32_FIND_DATA* pFileInfo);
	BOOL DriveList(PUINT pDriveType, LPTSTR lpDriveName);
	BOOL NextDriveList(PUINT pDriveType, LPTSTR lpDriveName);

	BOOL SendFile(LPCTSTR lpFromPathName, LPCTSTR lpToPathName, ULARGE_INTEGER& ulFileSize, ProgressData& Progress, CVtListCtrlEx& XList, INT iIndex, INT osType);
	BOOL RecvFile(LPCTSTR lpFromPathName, LPCTSTR lpToPathName, ULARGE_INTEGER& ulFileSize, ProgressData& Progress, CVtListCtrlEx& XList, INT iIndex, INT osType);
	void SetFileWriteMode(DWORD dwWrite);

	BOOL GetMyPCLabel(WIN32_FIND_DATA* pFileInfo);
	BOOL GetDesktopPath(WIN32_FIND_DATA* pFileInfo);
	BOOL GetDocumentPath(WIN32_FIND_DATA* pFileInfo);

	blastsock m_sock;

protected :
	LONGLONG FileTime_to_POSIX(FILETIME ft);
	FILETIME POSIX_to_FileTime(LONGLONG data);

	DWORD m_dwWrite;
	DWORD m_dwConnection;
	ULONG m_addr;
	USHORT m_port;
	int m_iServerNum;
	BOOL m_bIsN2NConnectionTiming;
};

