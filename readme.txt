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
nFTDServer.exe -p -viewermode -statisticsmode "13.124.64.192" 7002 351 "C4BDE5622FFF487532505376" "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1

[P2P 단독 테스트 시 파라미터]
nFTDClient.exe -l 443
nFTDServer.exe -c -viewermode -statisticsmode "192.168.3.196" 443 351 "C4BDE5622FFF487532505376" "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1

[전송속도]
-P2P  send : 17.80 MB/s, recv : 15.45 MB/s
-AP2P send :  3.92 MB/s, recv : 15.45 MB/s
