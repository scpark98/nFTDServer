#pragma once

#include <afxwin.h>
#include "nFTDServerSocket.h"
#include "nFTDFileManager.h"

#include "../../Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "../../Common/CTreeCtrl/SCTreeCtrl/SCTreeCtrl.h"

static CRITICAL_SECTION m_CS;

class CnFTDServerManager
{
public:
	CnFTDServerManager();
	virtual ~CnFTDServerManager();
	
	bool	is_connected() { return m_is_connected; }
	BOOL	SetConnection(LPTSTR lpCmdLine);
	BOOL	SetConnectionService();
	BOOL	Connection();
	BOOL	DataConnect();
	BOOL	DataClose();
	void	close_socket();

	void	DriveList(std::deque<CString> *dq);
	void	refresh_list(std::deque<WIN32_FIND_DATA> *dq, bool is_server_side);
	void	refresh_tree(CSCTreeCtrl* pTreeCtrl, bool is_server_side);

	//scpark add
	bool	get_filelist(LPCTSTR path, std::deque<WIN32_FIND_DATA> *dq, bool recursive);
	bool	get_folderlist(LPCTSTR path, std::deque<WIN32_FIND_DATA>* dq, bool fullpath);

	bool	get_disk_space(ULARGE_INTEGER* ulRemain, ULARGE_INTEGER* ulTotal);

	//이전 코드 함수명 : GetFileList(CShellTreeCtrl* pShellTreeCtrl)
	//tree에서 해당 path folder의 sub folder 목록을 refresh한다.
	//선택할 때가 아니라 폴더를 처음 펼칠 때, 새로고침 할 때 사용된다.
	void	refresh_tree_folder(CSCTreeCtrl* pShellTreeCtrl, CString path);

	bool	change_directory(LPCTSTR lpPath, DWORD dwSide, bool bDataSocket = false);
	bool	create_directory(LPCTSTR lpPath, DWORD dwSide, bool bDataSocket = false);
	//void	ChangeDrvComboBox(CDrvComboBox* pDrvComboBox, DWORD dwSide);
	bool	Rename(LPCTSTR lpOldName, LPCTSTR lpNewName, DWORD dwSide);
	bool	Delete(CVtListCtrlEx* pShellListCtrl, DWORD dwSide);
	bool	CurrentPath(DWORD dwPathLength, LPTSTR lpPath, DWORD dwSide);

	bool	get_remote_system_label(std::map<int, CString> *map);
	bool	get_remote_system_path(std::map<int, CString>* map);
	CString GetRemoteDesktopPath();
	CString GetRemoteDocumentPath();

	BOOL	FileTransferInitalize(CVtListCtrlEx* pShellListCtrl, CVtListCtrlEx* pXList, CListCtrl* pDepthList, ULARGE_INTEGER& ulTotalSize, DWORD dwSide, CString& strStartPath);
	BOOL	FileTransfer(HWND hWnd, CVtListCtrlEx& XList, INT iIndex, LPCTSTR lpFrom, LPCTSTR lpTo, ULARGE_INTEGER& ulFileSize, ProgressData& Progress, DWORD dwSide);

	int		m_is_server_side = SERVER_SIDE;

	CString m_strServerIP;
	CString m_strDeviceID;
	CString m_strDeviceIP;
	CString m_strDeviceName;
	CString m_strManagerID;
	CString m_strCompanyKey;
	CString m_strToken;
	CString m_strViewerPublicIP;
	CString m_strViewerPrivateIP;
	CString m_strStatusbarTitle;
	CString m_strTitle;
	LPSTR deviceID;
	LPSTR deviceIP;
	LPSTR managerIP;
	BOOL m_isP2PConnection;
	int m_nClientOSType;

	CnFTDServerSocket m_socket;
	CnFTDServerSocket m_DataSocket;
	CnFTDFileManager m_FileManager;

private :
	void KeepConnection();
	void KeepDataConnection();

	BOOL m_bDataClose;
	BOOL m_isStatisticsMode;
	bool m_is_connected = false;
};

