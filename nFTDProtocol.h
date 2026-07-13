#pragma pack(push, 1)

#define BUFFER_SIZE							1024 * 1024//1024 * 1024 * 4

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

//scpark add 새로 추가되는 프로토콜 정의는 #define이 아닌 enum으로 추가한다.
enum FTD_PROTOCOL
{
	nFTD_get_system_label = 40,		//내 PC, 바탕 화면, 문서 등의 레이블 요청.
	nFTD_get_system_path,			//바탕 화면, 문서 등의 절대 경로 요청.
	nFTD_get_drive_list,
	nFTD_filelist_all,				//remote의 파일목록
	nFTD_folderlist_all,			//nFTD_folderlist_all을 이용할수도 있지만 파일이 많을 경우는 패킷 송수신 양이 커지므로 별도 정의.
	nFTD_file_command,				//열기(open), 이름변경(rename), 삭제(delete), 속성보기(property), 새 폴더(new folder) 등의 파일 명령은 파라미터만 다를 뿐이므로 하나의 명령으로 통일한다.
	nFTD_total_disk_space,
	nFTD_remain_disk_space,
	nFTD_new_folder_index,
	nFTD_get_subfolder_count,
};

enum FILE_COMMAND
{
	file_cmd_open = 0,
	file_cmd_open_explorer,
	file_cmd_new_folder,
	file_cmd_refresh,				//tree 또는 list를 refresh하면 둘은 항상 같이 refresh되어야 한다.
	file_cmd_delete,
	file_cmd_rename,
	file_cmd_property,
	file_cmd_favorite,
	file_cmd_check_exist,
	file_cmd_move,					//20260704 by claude. 같은 쪽(리모트→리모트) 드래그 이동 — client 가 SHFileOperation FO_MOVE (src=param0, dst폴더=param1).
	file_cmd_copy,					//20260704 by claude. Ctrl+드래그 복사 — client 가 SHFileOperation FO_COPY.
};

//20260713 by claude. 원격 rename 결과 응답 코드(1바이트 msg.type 로 회신). 성공은 기존 nFTD_OK(0) 그대로 사용.
//실패 원인을 카테고리로 구분해 서버가 정확한 메시지(중복/권한/사용중/기타)를 표시한다. 값은 기존 명령 코드(0~49)와 안 겹치게 210+.
//구버전 클라는 실패 시 nFTD_ERROR(1)만 보내므로 서버는 '기타/일반 실패'로 처리(하위호환, 프로토콜 크기 불변이라 desync 없음).
enum RENAME_RESULT
{
	rename_err_exists = 210,		//이미 같은 이름이 존재(ERROR_ALREADY_EXISTS/FILE_EXISTS)
	rename_err_access = 211,		//권한 없음(ERROR_ACCESS_DENIED)
	rename_err_sharing = 212,		//다른 프로그램이 사용 중(ERROR_SHARING_VIOLATION)
	rename_err_other = 213,			//그 외
};


//scpark mwj 20240508 양방향 파일전송 오류 수정을 위해 추가
#define		P2P_NFTD_SERVERNUM				700
#define		AP2P_NFTD_CS_SERVERNUM			701		//nFTDServer용
#define		AP2P_NFTD_C_SERVERNUM			702		//nFTDClient용

//20241010 scpark P2P 모드 및 AP2P 모드 명칭 정의
#define		FT_MODE_P2P_S					0		//P2P 서버모드
#define		FT_MODE_P2P_C					1		//P2P 클라이언트 모드
#define		FT_MODE_AP2P					2		//AP2P 서버경유 모드

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
	//20260710 by claude. 표시용 평균 전송속도의 세션 기준(전송 시작~). session_bytes=실제 송수신 누적 바이트(스킵 등 인위 가산 제외),
	//session_start_clock=세션 시작 clock()(0=미시작, 소켓 코드가 첫 청크에서 지연 초기화). 기존엔 파일마다 t0 를 리셋한 per-file
	//순간속도라, 작은 파일 다수 전송 시 t_elapsed 가 1ms 로 클램프되며 속도 표시가 심하게 들쭉날쭉했다.
	ULARGE_INTEGER session_bytes;
	long           session_start_clock;
	BOOL bBlocked;
	FLOAT fSpeed;
	ULARGE_INTEGER ulRemainTimeSecond;
	ULARGE_INTEGER ulRemainDiskSpace;
} ProgressData;

#pragma pack(pop)