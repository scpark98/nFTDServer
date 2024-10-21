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
	bool	get_filelist(LPCTSTR path, std::deque<WIN32_FIND_DATA> *dq);

	//���� �ڵ� �Լ��� : GetFileList(CShellTreeCtrl* pShellTreeCtrl)
	//tree���� �ش� path folder�� sub folder ����� refresh�Ѵ�.
	//������ ���� �ƴ϶� ������ ó�� ��ĥ ��, ���ΰ�ħ �� �� ���ȴ�.
	void	refresh_tree_folder(CSCTreeCtrl* pShellTreeCtrl, CString path);

	bool	ChangeDirectory(LPCTSTR lpPath, DWORD dwSide, bool bDataSocket = false);
	bool	CreateDirectory(LPCTSTR lpPath, DWORD dwSide, bool bDataSocket = false);
	//void	ChangeDrvComboBox(CDrvComboBox* pDrvComboBox, DWORD dwSide);
	bool	Rename(LPCTSTR lpOldName, LPCTSTR lpNewName, DWORD dwSide);
	bool	Delete(CVtListCtrlEx* pShellListCtrl, DWORD dwSide);
	bool	CurrentPath(DWORD dwPathLength, LPTSTR lpPath, DWORD dwSide);

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

private :
	void KeepConnection();
	void KeepDataConnection();

	CnFTDServerSocket m_socket;
	CnFTDServerSocket m_DataSocket;
	CnFTDFileManager m_FileManager;

	BOOL m_bDataClose;
	BOOL m_isStatisticsMode;

};
