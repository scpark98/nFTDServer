[������ ����]
- ���ã�� ���� ǥ��
- ����Ʈ, Ʈ���� ��� ���� �˾��޴� �߰�
- remote ���ϸ� ���� ó��

[������ ����]
*20241220
- �߾ӿ� ���� ��ư �߰�

*20241219
- ok. "���� �Ϸ� �� ����â �ݱ�" �߰� �� �ټ� ����.


[20241014 renewal ����]

[20240930 scpark]
- SE Service �������ʹ� ���������� �����Ǿ�� �ϹǷ� ���� ������ ��ó���⸦ �߰��Ͽ� ����ϰ��� ��.
- LMMSE_SERVICE ��ó���⸦ �߰��Ͽ��µ� �׷��ٰ� LMM_SERVICE ��ó���⸦ �����ؼ� �ȵ�.
  LMM_SERVICE�� ������� �κб��� ��� ����ǹǷ� �̸� �״�� ����ΰ�
  LMMSE_SERVICE ��ó����� ����μ��� �α� ��θ� �����ϴ� �뵵�θ� ����.
  ��, ���Ҷ��� �ݵ�� LMMSE_SERVICE�� ���� ���� ��!
- ���� ���񽺿����� ������ ���� LMM_SERVICE��, SE���񽺿��� ���� LMMSE_SERVICE�� �����ϸ� ��.

[CryptInit()���� CREATE_KEY, RECEIVE_KEY ���� ���� ����] - by mwj 20241008
- ������Ʈ�� Host, Server���� ���� CREATE_KEY��, Viewer, Client�� ���� ���� RECEIVE_KEY�� �����Ѵ�.
- AP2P, KMS�� ���� �������� ����ÿ��� �׻� RECEIVE_KEY�� �Ѵ�.

[AP2P �ܵ� �׽�Ʈ �� �Ķ����]
nFTDClient.exe -p 13.124.64.192 7002 351

SCPARK-KOINO-LG	: nFTDServer.exe -p -viewermode -statisticsmode 13.124.64.192 7002 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1
SCPARK-KOINO	: nFTDServer.exe -p -viewermode -statisticsmode 13.124.64.192 7002 351 E0E1A9351FF9913636543560 "SCPARK-KOINO" "apple" "1" b4cf3f3509f42d5bb1e049bba39438e9b034435a "192.168.74.1" "192.168.74.1" 1

[P2P �ܵ� �׽�Ʈ �� �Ķ����]
nFTDClient.exe -l 443
nFTDServer.exe -c -viewermode -statisticsmode 192.168.3.196 443 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1

[���ۼӵ�]
-P2P  send : 17.80 MB/s, recv : 15.45 MB/s
-AP2P send :  3.92 MB/s, recv : 15.45 MB/s
