[수정할 내용-즐겨찾기]

[수정할 내용-drag&drop]
- 트리에서 drag할 경우 처리
- 트리에 drop할 경우 대상 폴더에 폴더 생성, 전송 완료 후 해당 트리 refresh
- drag할 때 상단/하단에 마우스가 위치하면 스크롤
- 리스트, 트리에 경로 복사 팝업메뉴 추가

[버그]
- 내 PC를 표시하는 상태에서 리스트 우클릭 메뉴 확인 필요.
ok. 이어받기 안됨, 상대편용량이 잘못표시됨
ok. 원격컴퓨터 새폴더 추가로 안됨

내컴퓨터 -> 원격컴퓨터로 드래그 시 파일 숫자가 잘못 표기됨
드라이브 여러개일때 아이콘 깨짐
리모트 원격드라이브 아이콘 오류(로컬 아이콘으로 표시됨)
즐겨찾기 파일 추가됨
원격컴퓨터 즐겨찾기 추가 오류
파일을 즐겨찾기 창, 파일목록트리로 드래그앤하면 파일전송이 됨
"파일목록을 작성하는 중입니다"에서 멈추는 버그
[개선사항]

파일전송창 크기가 좀더 크게 (파일이름항목을 좀 더 길게 표시)
파일전송 헤더 width 기억
Delete키방지 필요


- 멀티선택 후 속성창 표시

[수정한 내용]
*20241224
- 즐겨찾기 항목 더블클릭 시 tree, path, list 모두 변경
- 그 외 다수 버그 처리

*20241223
- 디스크 용량 표시 보완
- remote 파일명 변경 처리

*20241220
- 중앙에 전송 버튼 추가
- 즐겨찾기 폴더 표시


*20241219
- ok. "전송 완료 후 전송창 닫기" 추가 외 다수 수정.


[20241014 renewal 시작]

[20240930 scpark]
- SE Service 버전부터는 파일전송이 지원되어야 하므로 서비스 버전에 전처리기를 추가하여 사용하고자 함.
- LMMSE_SERVICE 전처리기를 추가하였는데 그렇다고 LMM_SERVICE 전처리기를 제거해선 안됨.
  LMM_SERVICE는 기능적인 부분까지 모두 관계되므로 이를 그대로 살려두고
  LMMSE_SERVICE 전처리기는 현재로서는 로그 경로를 구분하는 용도로만 사용됨.
  단, 비교할때는 반드시 LMMSE_SERVICE를 먼저 비교할 것!
- 따라서 서비스용으로 빌드할 경우는 LMM_SERVICE로, SE서비스용일 경우는 LMMSE_SERVICE로 빌드하면 됨.

[CryptInit()시의 CREATE_KEY, RECEIVE_KEY 관련 내부 협의] - by mwj 20241008
- 프로젝트명에 Host, Server붙은 경우는 CREATE_KEY로, Viewer, Client가 붙은 경우는 RECEIVE_KEY로 통일한다.
- AP2P, KMS와 같은 서버와의 연결시에는 항상 RECEIVE_KEY로 한다.

[AP2P 단독 테스트 시 파라미터]
nFTDClient.exe -p 13.124.64.192 7002 351

SCPARK-KOINO-LG	: nFTDServer.exe -p -viewermode -statisticsmode 13.124.64.192 7002 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1
SCPARK-KOINO	: nFTDServer.exe -p -viewermode -statisticsmode 13.124.64.192 7002 351 E0E1A9351FF9913636543560 "SCPARK-KOINO" "apple" "1" b4cf3f3509f42d5bb1e049bba39438e9b034435a "192.168.74.1" "192.168.74.1" 1

[P2P 단독 테스트 시 파라미터]
nFTDClient.exe -l 443
nFTDServer.exe -c -viewermode -statisticsmode 192.168.3.196 443 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1

[전송속도]
-P2P  send : 17.80 MB/s, recv : 15.45 MB/s
-AP2P send :  3.92 MB/s, recv : 15.45 MB/s
