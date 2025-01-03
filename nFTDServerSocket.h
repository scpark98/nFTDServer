#pragma once

#include "nFTDProtocol.h"
#include "blastsock.h"

//#include "../../Common/CStatic/SCStatic/SCStatic.h"
//#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
//#include "../../Common/CProgressCtrl/MacProgressCtrl/MacProgressCtrl.h"
//#include "../../Common/CSliderCtrl/SCSliderCtrl/SCSliderCtrl.h"

//전송상황을 nFTDFileTransferDialog로 보내기 위한 메시지. 속도 저하되므로 사용 안함.
//static const UINT Message_CnFTDServerSocket = ::RegisterWindowMessage(_T("MessageString_CnFTDServerSocket"));

#define CONNECTION_CONNECT	0	
#define CONNECTION_LISTEN	1

#define SERVER_SIDE			0
#define CLIENT_SIDE			1

#define WRITE_UNKNOWN		0x00000000
#define WRITE_CONTINUE		0x00000100
#define WRITE_OVERWRITE		0x00000200
#define WRITE_IGNORE		0x00000400
#define WRITE_CANCEL		0x00000800
#define WRITE_ALL			0x00001000

enum TRANSFER_RESULT
{
	transfer_result_cancel = -1,
	transfer_result_fail,
	transfer_result_success,
	transfer_result_overwrite,
	transfer_result_skip,
};

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


	BOOL create_directory(LPCTSTR lpPathName);

	//열기(open), 이름변경(rename), 삭제(delete), 속성보기(property), 새 폴더 생성(new folder) 등의 파일명령은 파라미터만 다를 뿐이므로 하나의 함수로 통일한다.
	bool file_command(int cmd, LPCTSTR param0, LPCTSTR param1 = NULL, std::deque<CString>* dqlist = NULL);

	BOOL ExecuteFile(LPCTSTR lpDirName);
	BOOL Rename(LPCTSTR lpOldName, LPCTSTR lpNewName);
	BOOL delete_directory(LPCTSTR lpPath);
	BOOL delete_file(LPCTSTR lpPathName);
	BOOL change_directory(LPCTSTR lpDirName);
	BOOL TotalSpace(PULARGE_INTEGER lpTotalNumberOfFreeBytes);
	BOOL RemainSpace(PULARGE_INTEGER lpTotalNumberOfRemainBytes);
	BOOL CurrentPath(DWORD nBufferLength, LPTSTR lpCurrentPath);

	BOOL FileSize(LPTSTR lpPathName, ULARGE_INTEGER* ulFileSize);
	BOOL FileList(WIN32_FIND_DATA* pFileInfo);
	BOOL FileList2(WIN32_FIND_DATA* pFileInfo, LPCTSTR lpPath);
	BOOL NextFileList(WIN32_FIND_DATA* pFileInfo);
	BOOL DriveList(PUINT pDriveType, LPTSTR lpDriveName);
	BOOL NextDriveList(PUINT pDriveType, LPTSTR lpDriveName);

	//BOOL SendFile(LPCTSTR lpFromPathName, LPCTSTR lpToPathName, ULARGE_INTEGER& ulFileSize, ProgressData& Progress, CVtListCtrlEx& XList, INT iIndex, INT osType);
	//BOOL RecvFile(LPCTSTR lpFromPathName, LPCTSTR lpToPathName, ULARGE_INTEGER& ulFileSize, ProgressData& Progress, CVtListCtrlEx& XList, INT iIndex, INT osType);

	int send_file(CWnd* parent_dlg, int index, WIN32_FIND_DATA from, WIN32_FIND_DATA to, ProgressData& Progress);
	int recv_file(CWnd* parent_dlg, int index, WIN32_FIND_DATA from, WIN32_FIND_DATA to, ProgressData& Progress);
	void SetFileWriteMode(DWORD dwWrite);

	BOOL get_remote_system_label(std::map<int, CString> *map);
	BOOL get_remote_system_path(std::map<int, CString>* map);
	BOOL GetDesktopPath(WIN32_FIND_DATA* pFileInfo);
	BOOL GetDocumentPath(WIN32_FIND_DATA* pFileInfo);

	blastsock m_sock;
	void	set_transfer_pause(bool pause = true) { m_transfer_pause = pause; }
	void	set_transfer_stop() { m_transfer_pause = false; m_transfer_stop = true; }

protected :
	LONGLONG FileTime_to_POSIX(FILETIME ft);
	FILETIME POSIX_to_FileTime(LONGLONG data);

	DWORD m_dwWrite;
	DWORD m_dwConnection;
	ULONG m_addr;
	USHORT m_port;
	int m_iServerNum;
	BOOL m_bIsN2NConnectionTiming;

	bool	m_transfer_pause = false;
	bool	m_transfer_stop = false;
};

