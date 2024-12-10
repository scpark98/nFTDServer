#pragma pack(push, 1)

#define		nFTD_OK							0
#define		nFTD_ERROR						1
#define		nFTD_END						2

#define		nFTD_CreateDirectory			3
#define		nFTD_Rename						4
#define		nFTD_DeleteDirectory			5
#define		nFTD_DeleteFile					6
#define		nFTD_ChangeDirectory			7
#define		nFTD_TotalSpace					8
#define		nFTD_RemainSpace				9
#define		nFTD_CurrentPath				10

#define		nFTD_FileList					11
#define		nFTD_DriveList					12
#define		nFTD_FileSize					13

#define		nFTD_OpenDataConnection			14
#define		nFTD_CloseDataConnection		15
#define		nFTD_FileTransfer				16
#define		nFTD_FileTransferReq			17
#define		nFTD_FileExist					18
#define		nFTD_FileContinue				19
#define		nFTD_FileOverWrite				20
#define		nFTD_FileIgnore 				21
#define		nFTD_EmptyDiskSpace				22

#define		nFTD_FileList2					23

#define		nFTD_DesktopPath				24
#define		nFTD_DocumentPath				25

// SDC SDK
#define		nFTD_ExecuteFile				26
#define		nFTD_FileInfo					27
#define		nFTD_FileList3					28

//scpark add 
enum FTD_PROTOCOL
{
	nFTD_get_system_label = 40,		//�� PC, ���� ȭ��, ���� ���� ���̺� ��û.
	nFTD_get_system_path,			//���� ȭ��, ���� ���� ���� ��� ��û.
	nFTD_FileList_All,		//remote�� ���ϸ��
	nFTD_FolderList_All,	//nFTD_FileList_All�� �̿��Ҽ��� ������ ������ ���� ���� ��Ŷ �ۼ��� ���� Ŀ���Ƿ� ���� ����.
};

//scpark mwj 20240508 ����� �������� ���� ������ ���� �߰�
#define		P2P_NFTD_SERVERNUM				700
#define		AP2P_NFTD_CS_SERVERNUM			701		//nFTDServer��
#define		AP2P_NFTD_C_SERVERNUM			702		//nFTDClient��

//20241010 scpark P2P ��� �� AP2P ��� ��Ī ����
#define		FT_MODE_P2P_S					0		//P2P �������
#define		FT_MODE_P2P_C					1		//P2P Ŭ���̾�Ʈ ���
#define		FT_MODE_AP2P					2		//AP2P �������� ���

typedef struct _msg {
	UCHAR type;
} msg;

#define sz_msg  1 

typedef struct _msgString1 {
	UCHAR type;		// msg type
	USHORT length;	// directory name length
} msgString1;

#define sz_msgString1  3 

typedef struct _msgString2 {
	UCHAR type;		// msg type
	USHORT length1;	// old file name length
	USHORT length2;	// new file name length
} msgString2;

#define sz_msgString2  5  

#ifdef MOBILE_FILETRANSFER
typedef struct _msgDiskSpace {
	UCHAR type;		// msg type
	LONGLONG space;
} msgDiskSpace;
#else
typedef struct _msgDiskSpace {
	UCHAR type;		// msg type
	ULARGE_INTEGER space;
} msgDiskSpace;
#endif

#define sz_msgDiskSpace  9 

#ifdef MOBILE_FILETRANSFER
typedef struct _msgFileInfo {
	UCHAR type;		
	DWORD dwFileAttributes; 
	LONGLONG nFileSize; 
	LONGLONG ftLastWriteTime; 
	USHORT length; 
} msgFileInfo;
#else
typedef struct _msgFileInfo {
	UCHAR type;		
	DWORD dwFileAttributes; 
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow;
	FILETIME ftLastWriteTime; 
	USHORT length; 
} msgFileInfo;
#endif

#define sz_msgFileInfo  23 

typedef struct _msgFileInfo2 {
	UCHAR type;		
	DWORD dwFileAttributes; 
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow;
	FILETIME ftLastWriteTime; 
	FILETIME ftCreateTime;
	USHORT length; 
} msgFileInfo2;


#ifdef MOBILE_FILETRANSFER
typedef struct _msgFileSize {
	UCHAR type;		
	LONGLONG nFileSize;
} msgFileSize;
#else
typedef struct _msgFileSize {
	UCHAR type;		
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow;
} msgFileSize;
#endif

#define sz_msgFileSize  9 

typedef struct _msgDriveInfo {
	UCHAR type;		
	UINT  driveType;
	USHORT length; 
} msgDriveInfo;

#define sz_msgDriveInfo  7

typedef struct _msg_server_num {
	short command;
	long servernum;
	char temp[46];
} msg_server_num;

typedef struct _ProgressData 
{
	DWORD nTime;
	DWORD total_count;
	DWORD nTransferedFile;
	ULARGE_INTEGER ulTotalSize;
	ULARGE_INTEGER ulReceivedSize;
	BOOL bBlocked;
	FLOAT fSpeed;
	ULARGE_INTEGER ulRemainTimeSecond;
	ULARGE_INTEGER ulRemainDiskSpace;
} ProgressData;

#pragma pack(pop)