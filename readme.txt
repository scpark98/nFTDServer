[빌드환경]
- blastsock과 그 안에 cryptopp와의 빌드호환성 때문에 특정 조건에서는 빌드에 실패한다.
- nFTDServer2
	Debug	: winSDK 8.1, vs2022 ok, vs2019 ok, vs2015 ?
	Release	: winSDK 8.1, vs2022 fail, vs2019 fail, vs2015 ok
- nFTDClient
	Debug	: winSDK 8.1, vs2022 ok, vs2019 ok, vs2015 ok
	Release	: winSDK 8.1, vs2022 fail, vs2019 fail, vs2015 ok


- anysupport
  무조건 C 드라이브로 시작,
  즐겨찾기 메뉴 항목은 제거,
  파일전송 시 alternative_back_color 사용안함으로 수정.

[공용라이브러리 설정]
1. 신규로 프로젝트 빌드할 경우
1) CommonLib.props를 텍스트 편집기로 오픈
2) `<CommonLibRoot>` 값을 본인의 소스 루트 경로로 수정
3) Visual Studio 재시작

2. Common 소스 파일 추가 시
- vcxproj와 vcxproj.filters에서 경로를 매크로로 치환
- vcxproj와 vcxproj.filters의 `Include` 경로가 일치하지 않으면 솔루션 탐색기에 파일이 표시되지 않음

1) 솔루션 탐색기에서 Common 필터 우클릭 → **추가 → 기존 항목** (Shift+Alt+A)
2) `{공용라이브러리러 루트경로}\Common\...` 에서 파일 선택 → 추가
3) VS가 vcxproj에 절대경로로 기록하므로, 텍스트 편집기에서 매크로로 치환

**vcxproj:**
```xml
<!-- VS가 자동 생성 -->
<ClCompile Include="{공용라이브러리러 루트경로}\Common\NewModule\NewFile.cpp" />

<!-- 매크로로 치환 -->
<ClCompile Include="$(CommonLibDir)\NewModule\NewFile.cpp" />
```

**vcxproj.filters:** (솔루션 탐색기 표시를 위해 동일하게 치환)
```xml
<ClCompile Include="$(CommonLibDir)\NewModule\NewFile.cpp">
  <Filter>Common</Filter>
</ClCompile>
```


[수정할 내용-즐겨찾기]

[수정할 내용-drag&drop]
- 트리가 간혹 맨 밑까지 휠스크롤이 안됨
- 트리에도 이름 변경 메뉴 추가
- 트리에서 drag할 경우 처리
- 트리에 drop할 경우 대상 폴더에 폴더 생성, 전송 완료 후 해당 트리 refresh
- drag할 때 상단/하단에 마우스가 위치하면 스크롤
- 리스트, 트리에 경로 복사 팝업메뉴 추가

- 폴더목록 리스트 박스 테두리, 깜빡임

[버그]
- 이름 변경이 완료되지 않은 상태에서 스크롤하면? 동일한 이름이 존재할 경우 처리, 
- 전송 버튼에 불이 들어오지 않는 경우

- 내 PC를 표시하는 상태에서 리스트 우클릭 메뉴 확인 필요.
ok. 이어받기 안됨, 상대편 용량이 잘못표시됨
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



[수정할 내용]
- 파일전송 시 전송 진행율을 taskbar 버튼에 표시한다.
  단, 자동 숨김으로 세팅한 사용자에게는 거슬릴 수 있으니 자동 숨김일 경우는 테스트 후 포함할 지 결정.
- 전체 경로를 클립보드로 복사
- 현재 폴더 경로를 클립보드로 복사


- local에 존재하는 remote 파일을 다운받을 때 이미 존재하는 파일에 대한 처리창에서 취소를 한 경우
  local의 파일을 remote로 전송하려하면 바로 fail이 되는 오류.


[수정한 내용]
*20260702
- 파일명 앞에 &기호가 그대로 표시되도록 트리, 리스트에서는 수정되었으나 ExistFileDlg에서 사용하는 CSCStatic에서 이를 prefix로 처리하고 있다.
  무조건 막으면 안되고 플래그 변수로 세팅하여 처리되도록 해야 한다.

*20250318
- 이어받기 시 용량표시 오류 수정
- 파일목록창에서 파일명, 파일크기, 파일날짜 별로 정렬 오류 수정
- 오름차순, 내림차순 정렬 시 헤더의 중앙에 화살표로 표시되도록 수정

*20250304
- 영문윈도우에서 파일날짜가 3/4/2025 1:42:34로 표시되던 시간표시오류를 "1:42 PM"으로 표시되도록 수정

*20250227
- AP2P 전송인 경우 속도제한 오류 수정(송신시는 정확히 적용되나 수신시 적용되지 않던 오류 수정)
- 이름 변경 후 해당 항목을 바로 전송하면 에러 발생. rename시에 VtListCtrlEx에서 m_cur_files를 갱신하지 않은 것이 원인이며 수정 완료.

*20250220
- 멀티모니터에서 최대화 시 기본모니터의 최대 영역크기로 최대화되던 오류 수정(WM_GETMINMAXINFO)

*20250205
- list에서 backspace키 동작 오류 수정
- list에서 폴더 더블클릭 시 set_path()호출 관련 오류로 폴더 이동이 되지 않던 오류 수정
- 새 폴더 생성 후 해당 폴더 우클릭으로 즐겨찾기 등록 시 그 상위 폴더가 등록되던 오류 수정
- 트리 및 리스트에서 새 폴더 추가 시 리스트 및 트리에도 동일한 이름 폴더 추가 오류 수정

*20250204
- PathCtrl에서 대소문자 구분없이 경로 입력해도 실제 폴더명을 구하고 폴더 이동되도록 수정
- list에서 파일크기로 정렬 시 오류 수정

*20250203
- 폴더 트리의 팝업메뉴를 parent에서 처리하도록 이동 및 전송, 즐겨찾기 메뉴 추가
- 트리 항목을 바로 전송할 경우 대상 트리 및 리스트에도 바로 전송된 폴더가 표시되도록 수정
- 즐겨찾기 경로의 끝에 '\'가 붙지 않도록 관련 코드 수정
- "이어서 전송"이 불가할 경우 해당 옵션의 disable 처리를 좀 더 구분되도록 수정

*20250131
- 트리를 펼치는 등 폴더목록을 얻어올 때 해당 폴더의 child가 있는지도 다시 검사로 인해 느렸던 동작 개선

*20250120
- 트리 팝업메뉴에 "새로고침" 항목 추가
- 리스트에서 폴더의 생성, 삭제, 이름변경 시 트리에도 반영


*20250117
- 최대화 시 상단이 -2정도 잘려 표시되는 현상 수정
  (CSCThemeDlg의 OnGetMinMaxInfo()에서 lpMMI->ptMaxPosition.y = 0;으로 세팅)
- 모니터가 100%가 아닌 배율일 때 최대화 시 모니터 영역보다 크게 최대화 되던 오류 수정
- "전송 완료 후 전송창 닫기" 체크박스를 텍스트 길이와 유사하게 자동 resize되도록 수정
  영문윈도우에서는 텍스트 너비가 넓으므로 크게 잡았으나 그럴 경우 한국어일때는 불필요한 우측 공백이 남음
  그 공백은 사용자 입장에서 보면 타이틀바 영역으로 보이나 체크박스 영역임
- 트리에서 "내 PC"를 선택한 상태에서 리스트에서 "탐색기로 현재 경로 열기" 명령 수행 시 실행되지 않던 오류 수정
- 전송 시 local to remote인지, remote to local인지를 알 수 있도록 파일복사 gif 이미지로 방향 표시 추가


*20250114
- 전송 완료 후 창 닫기 버튼의 크기와 위치를 재조정하여 표시
- 트리에서 새 폴더, 속성 메뉴 추가
- drive_list를 기존에는 disk label만 구했으나 CDiskDriveInfo 타입으로 상세히 구해서 사용하도록 수정

*20250108
- 메인창을 resize할 때 시스템 버튼 UI가 깨지는 오류 수정
- 원격 컴퓨터의 PathCtrl이 정상 동작하지 않던 오류 수정
- 원격 컴퓨터의 드라이브 용량이 제대로 표시되지 않던 오류 수정
- 전체화면에서 타이틀바를 드래깅하면 일반 화면으로 restore되도록 수정
- 타이틀바의 아이콘을 더블클릭하면 종료확인창 표시
- 디스크 용량은 드라이브가 변했을 경우에만 갱신

*20250107
- 원격 컴퓨터의 PathCtrl이 정상 동작하지 않던 오류 수정
- 원격 컴퓨터의 파일/폴더를 열기 또는 속성 표시할 때 알림 메시지 코드 보완
- 원격 컴퓨터의 파일/폴더를 삭제할 때 삭제 animated gif 관련 코드 보완
- 드래그 시 대상 컨트롤이 자동 스크롤되도록 수정

*20250106
- CSCThemeDlg 수정
- CControlSplitter 수정 (splitter 이동시 해당 컨트롤의 최소 너비/높이 지정 가능)
- 파일 삭제 시 Wait()를 줘서 한 항목 삭제할때마다 리스트 갱신되도록 수정
- "내 PC" 경로를 표시하고 있다면 즐겨찾기 메뉴는 disable로 처리


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
nFTDClient.exe -p 13.124.64.192 443 1234
nFTDClient.exe -p 3.35.127.253 443 1234

SCPARK-KOINO-LG	: nFTDServer2.exe -p -viewermode -statisticsmode "13.125.4.150" 7002 406 "C4BDE5622FFF428745499944" "SCPARK-KOINO-LG" "apple@linkmemine.com" "17" "f5eccd82833e4bed1d189531cecf70561fe7c0b1" "" "192.168.74.1" 1
SCPARK-KOINO-LG	: nFTDServer.exe -p -viewermode -statisticsmode 13.124.64.192 7002 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1
SCPARK-KOINO-LG	: nFTDServer.exe -p -viewermode -statisticsmode "13.125.4.150" 7002 406 "C4BDE5622FFF428745499944" "SCPARK-KOINO-LG" "apple@linkmemine.com" "17" "de1ce65b3f163b6e89526b4232b75142909047d7" "" "192.168.74.1" 1
SCPARK-KOINO	: nFTDServer.exe -p -viewermode -statisticsmode 13.124.64.192 7002 351 E0E1A9351FF9913636543560 "SCPARK-KOINO" "apple" "1" b4cf3f3509f42d5bb1e049bba39438e9b034435a "192.168.74.1" "192.168.74.1" 1

1.0 FastAPI	dev	: -p -viewermode -statisticsmode "114.108.164.45" 80 299529 "047C1612FEA4531246127046" "SCPARK-KOINO" "apple" "2451" "b4cf3f3509f42d5bb1e049bba39438e9b034435a" "118.37.42.237" "192.168.238.1" 1

[P2P 단독 테스트 시 파라미터]
nFTDClient.exe -l 443
nFTDServer.exe -c 192.168.0.48 443 => 예전엔 가능했으나 현재는 아래와 같이 15개 모두 줘야한다.
nFTDServer.exe -c -viewermode -statisticsmode 192.168.3.196 443 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1
nFTDServer.exe -c -viewermode -statisticsmode 192.168.0.48  443 351 C4BDE5622FFF865173239111 "SCPARK-KOINO-LG" "apple" "1" "bdb4d55fbba98e29875f38018651492645520607" "192.168.74.1" "192.168.74.1" 1

[RemoteSDK]
nFTDClient.exe -p 192.168.0.48 7002 10000001
nFTDServer.exe -p 192.168.0.48 7002 10000001

[AnySupport]
nFTDClient.exe -p 43.202.85.142 443 1
nFTDServer.exe -p 43.202.85.142 443 1

[전송속도]
-P2P  send : 17.80 MB/s, recv : 15.45 MB/s
-AP2P send :  3.92 MB/s, recv : 15.45 MB/s

[20260703 작업내용]
  d&d (드래그&드롭)

  1. 트리 d&d 활성화 + 로컬 이동(move) ? 트리 드래그 켜고, local↔local은 SHFileOperation 이동(서피컬
  노드 갱신). [로컬전용: remote↔remote 이동 미구현]
  2. 폴더 전송 시 폴더 레벨 보존 ? A(자식들)를 C로 드롭 → C\A\자식으로 보존(트리 드래그 base 보정).
  3. 이동 대상 폴더 처리 ? 이동된 폴더를 오름차순 정렬 위치에 삽입 + 대상 항상 펼침 + 자기 부모로
  드롭 시 조용히 무동작.
  4. cross-control 드롭 하이라이트 ? local tree→remote tree 드롭 시 대상 트리 하이라이트 + 드래그 중
  잘못된 hover 제거.
  5. 드래그 use-after-free 크래시 수정 ? m_pDragImage delete 후 NULL 처리.

  트리 파일 조작

  6. 트리 이름변경(F2 + 우클릭 메뉴) ? 편집모드→로컬 MoveFile / 원격 socket Rename(양쪽 동작).
  7. 트리 삭제(우클릭 메뉴 + Del) ? delete_file로 휴지통 삭제(완전삭제 아님). [로컬전용: 원격 트리
  삭제 미구현]
  8. 보호 폴더 메뉴 disable ? 드라이브 루트·시스템 폴더는 삭제/이름변경 비활성 + 실행부 방어.

  새로고침 / 외부 변경 반영

  9. 트리 새로고침 stale-node 수정 + F5 배선 ? 선택 노드가 사라졌으면 부모 refresh, 미배선이던 F5
  연결.
  10. dir watcher 재설계 ? 리스트 현재 폴더 + 트리 펼쳐진 폴더들을 비재귀 감시(재귀 폐지로 볼륨↓),
  확장/축소·이동 시 재설정 → 탐색기 변경이 트리/리스트에 반영. [로컬전용: 원격 폴더 변경 감지 별도
  필요]
  11. watcher rename 치명 버그 수정 ? rename이 action=5(NEW_NAME)로 오는데 4를 검사해 죽어있던 것 →
  4→5.
  12. UI 결함 2건 ? hover 밑줄(TVS_TRACKSELECT) 제거, 트리 refresh 과다 방지.

  Common 라이브러리 유틸(재사용)

  13. CSCTreeCtrl 추가 ? insert_folder_sorted(탐색기식 정렬 삽입), get_item_by_fullpath(강제확장 없이
  경로→노드), message_expand_changed(확장/축소 통지).

  원격 확장 후보: 7(트리 삭제), 1(remote↔remote 이동), 10(원격 변경 감지) ? 원격은 소켓 명령/원격측
  처리가 필요해 로컬과 구조가 다릅니다. 원하시면 다음에 이들부터 원격 버전으로 진행하겠습니다.
