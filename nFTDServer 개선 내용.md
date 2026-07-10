# nFTDServer 개선 내용 (2026-06-30 ~ )

> 파일 전송 프로젝트(nFTDServer, GUI 메인 앱)의 개선 이력 상세 기술 정리.
> nFTDServer 자체 변경뿐 아니라, 이 프로젝트의 기능/버그가 의존하는 **Common 라이브러리**(트리·리스트·드래그·다이얼로그 컨트롤 등) 개선까지 함께 정리한다.
> 원격 서비스 측(수신/파일조작)은 `nFTDClient 개선 내용.md` 참조. 커밋 해시는 각 저장소 기준.
>
> ※ 아래 **요약**은 한 줄씩, 상세는 §0 이하 참조.

---

## 한눈에 보기 (요약)

- **d&d 이동/복사**: local→local 은 소켓전송이 아니라 `SHFileOperation` 이동(같은드라이브=이동·다른드라이브=복사, Ctrl=복사/Shift=이동, 복사본 자동네이밍).
- **로컬 d&d 무동작 버그**: '현재 폴더의 하위로 드롭'까지 막던 `from==parent(to)` 조기 return 가드 제거 + 드롭 방향(`m_dstSide`) stale 수정.
- **트리 이동 후 갱신**: 전체 refresh → 서피컬 갱신(정렬 삽입·중복노드 방지·폴더 레벨 보존·소스 부모 refresh).
- **드래그 이미지**: 실제 노드+선택항목 다행 렌더, 아이콘 알파 프린징(`draw_hicon_alpha`)·stale 캐시·메모리릭·use-after-free 수정.
- **드래그 자동스크롤**: 4방향 거리비례 속도 + 오버레이 스크롤바 위에서 오작동하던 것 근본 수정(`m_drag_scroll_target` 분리).
- **보호/취소 가드**: 보호폴더(드라이브루트·시스템폴더) 드래그 차단 + ESC/드래그중 우클릭 완전 취소.
- **트리 이름변경/삭제**: F2 편집 배선(`message_edit_item`) + Del=휴지통 삭제 배선(로컬/원격 분기).
- **메뉴/복사/새로고침**: 우클릭을 `WM_CONTEXTMENU` 정석 이관 + "경로 클립보드 복사" + F5 배선(stale 노드면 부모 refresh).
- **dir watcher**: '펼쳐진 폴더 집합' 비재귀 감시 + 실경로 감시 + rename 미반영(ACTION 4→5) 치명버그 수정.
- **ExistFileDlg**: 라운드 배너+상태 아이콘+부분강조 tagged text, 크기·날짜 함께 판정(상반 시 판단 보류), 다국어.
- **수신 날짜 '동일' 버그**: 대상 `to` 가 원본 memcpy 라 시각이 같던 것 → `GetFileTime` 으로 실제 로컬 시각 주입.
- **소켓 감사 Batch1~3**: ReadFile/WriteFile 검증 + 경로 length 가드 + 수신 `FILE_SHARE_READ`.
- **긴 경로(MAX_PATH) 크래시**: bounded 복사 4곳 + `has_sub_folders` FindFirstFile+`\\?\` + `cchTextMax` 문자수 수정.
- **정규화/UI**: 드라이브루트 즐겨찾기 `"X:\"` 정규화 + 파일명 `&` 리터럴 표시(`set_no_prefix`).
- **Common 연관**: SCTreeCtrl(정렬삽입·hot-track·이름변경)·CVtListCtrlEx(스크롤·drop색)·SCStatic(헤더아이콘·no_prefix)·SCThemeDlg/SCScrollbar/colors 다수 버그·기능.

---

## 0. 이 기간의 전체 방향

- **탐색기 수준의 파일 관리 UX** 를 목표로 로컬/원격 트리·리스트에 드래그앤드롭(이동/복사), 이름변경(F2), 삭제(Del/휴지통), 새로고침(F5), 우클릭 메뉴, 경로 클립보드 복사, 외부 변경 자동 반영(dir watcher) 등을 구현.
- **로컬 전용 기능을 로컬/원격 양쪽으로 확장**(삭제·이동·복사·이름변경).
- **소켓 송수신 계층의 보안·안정성 감사**(네트워크 length 미검증 버퍼 오버플로, 핸들 누수, 공유모드) 3배치 진행.
- **긴 경로(MAX_PATH 260 초과) 크래시** 계열 근본 수정.
- **동일이름 파일 처리 대화상자(ExistFileDlg)** 를 탐색기 충돌 대화상자 컨셉으로 전면 개선.
- 이 과정에서 드러난 **Common 컨트롤(SCTreeCtrl / CVtListCtrlEx / SCStatic 등)의 구조적 버그** 다수 수정.

> CVtListCtrlEx 는 세로 스크롤의 구조적 한계 때문에 추후 **CSCListCtrl** 로 대체 예정(별도 프로젝트에서 개발 중). 이 문서에는 현재 사용 중인 CVtListCtrlEx 관련 개선만 반영.

---

## 1. 드래그 앤 드롭 (이동/복사)

파일 전송 UX 의 핵심. 로컬↔로컬, 로컬↔원격, 원격↔원격, 트리↔리스트 조합을 탐색기와 동일한 규칙으로 동작시키는 작업이 이 기간의 가장 큰 비중.

### 1.1 이동/복사 정책 (탐색기 기본 규칙)
- **local→local 은 소켓 전송이 아니라 실제 이동(move)** 으로 처리 — `SHFileOperation FO_MOVE + FOF_ALLOWUNDO`(되돌리기 가능, 크로스드라이브·충돌 대화상자 OS 기본 제공). 소스 경로를 리스트(이름)·트리(fullpath) 양쪽에서 정규화. (`607cc94`)
- **같은 드라이브 = 이동, 다른 드라이브 = 복사** (탐색기 기본), **Ctrl=복사 / Shift=이동 토글**, 드래그 중 문구 즉시 반영. 복사 시 **복사본 자동 네이밍**, 복사 후 대상 항목 선택 유지. (`716ec3f`, `e87e67c`)
- 원격 이동/복사도 구현(로컬 전용 → 로컬/원격 분기). (`716ec3f`)
- remote→remote move 는 **보류** — undo 없음 + 원격 경로 모호성 + 런타임 미검증. 현재는 비파괴적 '전송'으로 동작. (`8ccb24a`)

### 1.2 로컬 이동 무동작 버그 (핵심 회귀)
- `file_transfer` same-side 의 `from==parent(to)` 조기 return 가드가 **현재 폴더의 하위 폴더로 드롭(가장 흔한 이동)** 까지 막아 로컬 d&d 가 전혀 동작 안 하던 원인 제거. 순환 이동은 SHFileOperation 이 안전하게 거부. (`bcebc07`)
- 리스트→트리 드롭 시 리스트 핸들러가 `m_dstSide` 를 설정 안 해 방향이 stale 이던 버그, 트리 드롭 핸들러의 `m_srcSide/m_dstSide` stale 값 사용 버그 수정. (`bcebc07`, `8ccb24a`)
- 이동 완료 후 **소스 폴더가 트리에 그대로 남던 문제** — 이동된 항목 자신이 아니라 소스의 **부모 노드**를 refresh 해 제거. (`ba0c0e8`)

### 1.3 트리 이동 후 노드 갱신 (서피컬 갱신으로 전환)
과한 전체 refresh 대신 트리 노드를 국소 갱신:
- 소스 노드는 `DeleteItem`, 대상엔 `insert_folder`(대상 자식 미로드면 `cChildren=1` 세팅 후 확장 시 전체 열거로 표시 → 중복 방지), 소스 부모에 자식 없으면 `[+]` 제거. (`34c8283`)
- 이동 폴더를 맨 끝이 아닌 **오름차순 정렬 위치**에 삽입(`insert_folder_sorted`). (`acf5614`)
- 대상 폴더는 드롭 후 항상 '확장 가능+펼침' 상태로 정규화(미로드면 `[+]`→Expand 로 열거, 로드면 정렬 삽입 후 Expand). (`f7f8dc7`, `5e91499`, `5407546`)
- **폴더 레벨(A) 유실 수정**: 트리 드래그는 `m_transfer_from` 이 드래그한 폴더 자신이라 dst 계산(from→to 치환)에서 `A\child → to\child` 로 A 레벨이 사라짐 → 전송 직전 base 를 A 의 부모로 조정해 `to\A\child` 로 폴더 보존. (`9309231`)
- 자식을 이미 들어있는 현재 부모로 드롭 시 SHFileOperation '대상=원본' 대화상자 대신 조용히 무동작(`get_parent_dir(from)==to` 사전 차단). (`b91ba1c`)
- 대상에 동일이름 폴더가 이미 있을 때 **트리에 폴더가 2개로 보이던 중복 노드 버그**: 무조건 `insert_folder_sorted` 하던 것 → `find_children_item` 으로 기존 노드 확인 후 있으면 삽입 금지(병합이면 자식 재열거로 갱신). (`371276e`)

### 1.4 드래그 이미지 (트리·리스트 공통, 로컬/원격 4개 컨트롤)
- 드래그 항목 + 펼쳐진 하위/선택 항목들을 **아이콘+텍스트 다행**으로 합성, 최대 높이 초과 시 하단 gradient alpha 페이드. 폭은 실제 폰트 폭 기준(줄바꿈 방지). (`728f123`, Common)
- **아이콘 배경 박스/까만 점(프린징) 계열 수정**: `FromHICON`(배경 불투명)·`DrawIconEx`(반투명 가장자리 프린징) 둘 다 결함 → `draw_hicon_alpha()`(Functions) 신설: `GetIconInfo` 의 32bpp straight-alpha 를 `GetDIBits` 로 직접 추출해 Gdiplus `DrawImage` 로 합성. 트리·리스트 아이콘 그리기 통일. (`54254e0`, `134ec17`, `ef01339`, `5e25261`, Common)
- **리스트 드래그 이미지 stale** — 항목이 바뀌어도 이전 이미지를 재사용하던 캐시 버그(‘선택과 다른 항목이 나오던’ 실제 원인) → 매 드래그 시작마다 파기 후 재생성. (`ef01339`, Common)
- 실제 노드/선택을 렌더한 드래그 이미지(`create_drag_image`)를 쓰도록 프리셋 이미지(`add_drag_images IDB_DRAG_*`) 비활성화(트리·리스트 로컬/원격 4곳). (`346b152`)
- 드래그 이미지 메모리 릭 수정(드롭/취소 시 `CImageList`/`m_pDragImage` 해제), use-after-free 크래시(`delete` 후 NULL 미설정) 수정. (`639fa5c`, `a991659`, `bfc2d51`, Common)

### 1.5 드래그 자동 스크롤 (가장자리 호버)
- 트리·리스트 모두 대상 컨트롤 가장자리 호버 시 4방향 자동 스크롤(거리 비례 속도 + 타이머 연속, MARGIN 48px, level 1~3, 70ms). 기존엔 리스트→트리만 동작. (`f68228b`, `fdcaff7`, `67a7da2`, Common)
- **가장자리 오작동 근본 수정**: 오버레이 스크롤바/헤더가 부모 다이얼로그 자식이라 커서가 그 위면 `WindowFromPoint` 가 리스트/트리가 아니라 스크롤바를 반환 → 자동스크롤/가로바 연동이 깨짐. 스크롤 대상을 `m_drag_scroll_target`(직전 유효 리스트/트리)으로 분리. 리스트 상단 컬럼 헤더는 트리거 존에서 제외. (`bc0e804`, `dfb86a6`, `d5bc3ab`, Common)

### 1.6 보호/취소/자기드롭 가드
- **보호 폴더(드라이브 루트·C:\Windows 등 시스템 폴더) 드래그 시작 차단** — 기존엔 C:\Windows 도 드래그됨. (`07e9ce9`, Common)
- **드래그 중 ESC 완전 취소**(`cancel_drag`: DroppedHandler 미호출 → 전송/이동 없이 캡처해제·타이머정지·이미지종료·하이라이트 해제). (`07e9ce9`, Common)
- 드래그 중 우클릭 = 취소 + 취소 후 팝업 메뉴가 뜨던 것 차단(`m_swallow_rbutton`). (`0e47085`, `134ec17`, Common)
- 드래그 self-drop 무동작 가드. (`ba62ba5`)
- shell 트리는 같은 트리 내 수동 이동(`move_tree_item`) 건너뜀 — 파일시스템 미러라 실제 파일 이동 후 refresh 로 재구성. `move_tree_item` 이 `m_DragItem` 핸들을 무효화해 d&d 소스 소실→무동작이던 버그 수정. (`1321c57`, Common)

---

## 2. 트리/리스트 파일 조작 (이름변경·삭제·메뉴·새로고침)

### 2.1 이름변경 (F2 / 컨텍스트 메뉴)
- 로컬 트리 컨텍스트 메뉴 Rename = `edit_item` 진입(로컬은 `CSCTreeCtrl::edit_end` 가 `MoveFile` 로 폴더명 변경). (`3dfc943`)
- **F2 배선**: 앱 `on_message_CSCTreeCtrl` 에 `message_edit_item` 핸들러 추가 — 기존엔 앱이 트리의 F2 편집요청 메시지를 안 받아 무동작. 보호 폴더 제외. (`0c08bde`)
- 드라이브 루트는 편집 박스에 **볼륨명만** 표시(드라이브 문자 ` (X:)` 는 편집 불가라 제거 후 커밋 시 재부착). (Common `fc91481` / 앱 연동)

### 2.2 삭제 (Del / 컨텍스트 메뉴, 휴지통)
- 트리 삭제를 앱 공통 헬퍼 `delete_file(fullpath, trash_can=true)` 로 통일(`FO_DELETE + FOF_ALLOWUNDO` = 휴지통, 완전삭제 아님. `FOF_NOCONFIRMATION` 이라 OS 확인창 없이 리스트 삭제와 동일 동작). (`a840c5e`, `3dfc943`)
- **Del 단축키 배선**: 포커스가 로컬/원격 트리면 Del → `OnTreeContextMenuDelete`. 기존엔 `PreTranslateMessage` 의 `VK_DELETE` 가 즐겨찾기 리스트만 처리하고 트리는 무시. (`8c5767a`)
- 원격 트리 폴더 삭제 구현(로컬 전용 → 로컬/원격 분기). (`c332fb7`)
- 보호 폴더(드라이브 루트·시스템 폴더)는 메뉴 disable + 실행부 방어선. (`3dfc943`)

### 2.3 우클릭 컨텍스트 메뉴 / 경로 복사 / 새로고침
- 우클릭 메뉴를 `WM_CONTEXTMENU` 정석 경로로 이관(`OnContextMenu` 분기), `NM_RCLICK` 통지와 분리. (`2bf3399`, Common `9610292`/`5b3dac9`)
- 트리/리스트 우클릭 **"경로 클립보드 복사"** 메뉴 구현. (`47e35b0`, `62d03a2`)
- **F5 새로고침 배선**: 메뉴엔 'F5' 힌트가 있으나 `PreTranslateMessage` 에 `VK_F5` 케이스가 없어 무동작이던 것 → 포커스가 트리/리스트에 맞춰 새로고침. 선택 노드가 stale(디스크에 없음)이면 부모 refresh. 트리 우클릭 새로고침(`file_command_on_tree`)에도 stale-node 처리 적용(기존엔 `file_command_on_list` 에만 있었음). (`aa9c702`)

### 2.4 전송 완료 후 UX
- 전송 완료 후 전송된 항목 자동 선택 + `ensure_visible` 스크롤. (`eb317c5`, `e87e67c`)
- 폴더 항목이 적으면(200개 이하) 로딩 progress 표시 안 함 — 폴더 변경마다 깜빡이던 현상 제거. (`2596c87`)

---

## 3. 디렉터리 변경 감시 (dir watcher) — 탐색기 외부 변경 반영

로컬에서 탐색기 등으로 파일/폴더를 바꾸면 리스트·트리에 즉시 반영되도록 `ReadDirectoryChangesW` 기반 감시를 재설계.

- **'펼쳐진 폴더 집합' 방식으로 전환**: `rewatch_local()` 이 (1) 리스트 현재 폴더 + (2) 트리에서 펼쳐진 로컬 폴더들을 각각 **비재귀**로 감시. 재귀 폐지로 볼륨↓(거대폴더/드라이브 루트도 직속만). 네비게이션 + 트리 확장/축소(`message_expand_changed`) 시 재설정. (`fb129ce`, `6e5df8f`)
- **실경로로 감시**(기존 일부 지점이 `get_path()`=특수폴더경로를 넘겨 `ReadDirectoryChangesW` 실패로 감시가 죽던 버그). (`6e5df8f`)
- 콜백: 리스트는 변경 부모==현재폴더일 때만 갱신, 트리는 '폴더' 변경만 `get_item_by_fullpath` 로 부모 노드 찾아 서피컬 갱신(`rename_child_item`/`DeleteItem`/`insert_folder_sorted`). 현재폴더 자신 rename 시 새 경로로 이동(재감시). (`6e5df8f`, `0c08bde`)
- **rename 미반영 치명 버그**: watcher 가 rename 을 `FILE_ACTION_RENAMED_NEW_NAME(5)` 로 전달(내부에서 `RENAMED_OLD_NAME(4)` 는 스킵하고 5 에 new/old 를 함께 실음)하는데 콜백이 4 를 검사해 rename 처리가 **한 번도 실행 안 됨**(기존부터 죽어있던 버그). 4→5 로 교체. (`10d1249`)
- 트리 확장/축소 통지용 `message_expand_changed` + `get_item_by_fullpath`(로드된 노드만 반환) Common 에 추가. (`33d00a1`, `a5dc19a`)

---

## 4. 동일이름 파일 처리 대화상자 (CExistFileDlg)

탐색기 파일 충돌 대화상자 컨셉으로 전면 개선.

### 4.1 시각화 / 상단 배너
- 상단 상태를 **라운드 배너 + 상태별 아이콘**으로 개선. verdict(권장 사유)를 배경 배너 + 크기/시각 **부분 강조 tagged text**(더 큼/더 최신 값만 녹색+bold, 완전 동일은 회색)로 표시. (`9f839b5`, `ba62ba5`)
- 크기/시각 비교 마커·구분기호·정렬 다듬기, 타이틀바 높이/여백 수정, 폰트 통일. (`f1bdcaf`, `054587d`, `f07b07a`)
- **다국어 리소스화**(한/영/일 STRINGTABLE). (`054587d`)

### 4.2 권장 로직 (크기 + 날짜 함께 판정)
- 초기엔 크기만 고려 → **크기·날짜를 함께 판정**하도록 개편. 크기가 작아도 더 최신일 수 있으므로:
  - `size_cmp * time_cmp < 0`(방향 상반) = **판단 보류** → 권장 없음, 라디오 전체 해제 + 확인버튼 비활성(사용자가 직접 선택할 때까지). 중립 회색 배너 + 물음표 아이콘.
  - 원본이 큼 = 이어서 전송(resume) 권장, 그 외 = 건너뛰기(skip) 권장. 대상이 큰데 무조건 덮어쓰기 권장하던 오동작 제거(덮어쓰기는 수동 선택으로만).
  - 크기 동일·날짜만 다름 = 건너뛰기 권장(날짜는 정보표시). (현행 커밋 `4180a23`, `a8c811c`)
- 잠재버그 정리: `OnBnClickedOk` 의 미초기화 `int Exist;` → `-1` 초기화 + 미선택 시 창 안 닫음.

### 4.3 물음표 배지 아이콘 (판단 보류용)
- 기존 verdict 아이콘(resume/skip/overwrite)과 **동일 규격**(20/24/28 PNG)·동일 스타일(파스텔 배지 + 진한 글리프)로 회색 원 + `?` 아이콘 생성. `IDB_QUESTION20/24/28`. (`4180a23` + 후속 미커밋 보정)
- 생성 시 겪은 문제와 교훈: 글리프 크기/중앙정렬을 눈대중하다 반복 수정 → **참조 아이콘 픽셀 실측 기반**으로 원지름=캔버스×(20/24), 글리프=원지름×50%, 잉크 bbox 중심 정렬로 확정. 특히 `Graphics.SmoothingMode=1`(=HighSpeed, AA off) 을 실수로 써서 원 가장자리 AA 가 빠졌던 것을 `::AntiAlias`(4)로 교정(참조 아이콘의 반투명 가장자리 픽셀 수와 대조해 검증).

### 4.4 수신 시 '대상 수정시각 = 원본과 동일' 버그
- **원인**: `nFTDFileTransferDialog.cpp` 에서 대상 정보 `to` 를 원본 `m_filelist[i]` 를 통째로 `memcpy` 하고 경로명만 바꿈 → `to.ftLastWriteTime == from.ftLastWriteTime`. 수신 경로(`recv_file`, `nFTDServerSocket.cpp`)가 `exist_file` 을 그 `to` 에서 다시 memcpy 하고 크기만 갱신해, 대화상자에서 원본/대상 수정시각이 **항상 동일**하게 표시됨(송신 방향은 `exist_file` 을 네트워크로 수신해 실제 시각이라 정상 → 방향에 따라 증상이 갈림).
- **수정**: 크기 설정 직후 `GetFileTime(hFile, ...)` 으로 실제 로컬 대상 파일 시각을 읽어 넣음. `hFile` 은 `GENERIC_WRITE`(FILE_GENERIC_WRITE 매핑에 `FILE_READ_ATTRIBUTES` 포함)라 재오픈 없이 가능. (`4180a23`)

---

## 5. 소켓 안정성 / 보안 감사 (Batch 1~3)

nFTDClient 와 대칭으로 진행. 서버 측 요점:

- **Batch1 (CRITICAL)**: `send_file` 의 ReadFile / `recv_file` 의 WriteFile 반환 검증, overwrite 재오픈 검증. (`ba62ba5`)
- **Batch2 (CRITICAL)**: 네트워크 length 미검증 버퍼 오버플로 방지 — 모든 경로 `RecvExact` 앞에 `length <= (MAX_PATH-1)*sizeof(TCHAR)` 가드(서버 7곳: NextFileList/NextDriveList/CurrentPath/get_remote_system_label,path/GetDesktop,DocumentPath). (`b96f546`)
- **Batch3 (HIGH-safe)**: `recv_file` 수신 `CreateFile` 공유모드 `FILE_SHARE_READ`(3곳). (`f3b609b`)
- 원격 폴더목록 응답의 leaf명 추출을 겹침 `_tcscpy(dst<src, UB)` → CString 경유 bounded 복사로 수정(`message_request_folder_list`). 원격 확장 경로 강화. (`93589ec`)

> 상세 항목·nFTDClient 측(length 가드 18곳, 핸들 누수 등)은 `nFTDClient 개선 내용.md` 참조.

---

## 6. 긴 경로 (MAX_PATH 260 초과) 크래시

MAX_PATH 근처/초과 경로에서 스택 손상·예외로 앱이 엉뚱한 지점에서 죽던 버그군.

- **버퍼 오버플로 4곳**: `WIN32_FIND_DATA::cFileName[260]` 에 무경계 `_stprintf`/`_tcscpy` 로 경로 복사하던 것 → `_sntprintf_s(_TRUNCATE)` bounded 로 교체(file_transfer 드롭, 리스트 이름변경 동기화 2곳, 트리 보내기). 초과 경로에서 스택 손상 → 나중에 `CInvalidArgException` 유발하던 클래스 버그. (`a1fdbd9`)
- **`has_sub_folders` 크래시(Common)**: `CFileFind` 사용 → 260 근처/초과면 내부에서 `CInvalidArgException` → '자식 폴더 유무' 판정 시 앱 사망. 예외 안 던지는 원시 `FindFirstFile` + `\\?\` 확장 프리픽스로 교체. (`ecf841c`)
- **`move_tree_item` 스택 오버플로(Common)**: `GetItem` 의 `TV.cchTextMax` 를 `sizeof(str)`(바이트=512) → `_countof(str)`(문자수=256)로 수정. 256칸 버퍼에 512자 허가되던 버그. (`01563c6`)

---

## 7. 즐겨찾기 / 드라이브 루트 정규화 / 기타 UI

- 드라이브 루트 즐겨찾기를 정규 표기 `"X:\"` 로 승격(표시/저장/트리 이동 일관, 레거시 `"X:"` 자동 마이그레이션). (`c3fa630`, `2bf3399`)
- 전송버튼 보호폴더 툴팁 오표시 수정. (`2bf3399`)
- 파일명의 `&` 가 상단 `m_static_message` 에서 니모닉 밑줄로 표시되던 것 → `set_no_prefix(true)`(DT_NOPREFIX 강제)로 리터럴 표시(SS_NOPREFIX 미지정 컨트롤이라 강제 필요). ExistFileDlg 의 src/dst 파일명 static 도 동일 처리. *(현재 미커밋)*
- 타이틀바 높이는 ctor 에서 `m_titlebar_height` 직접 대입으로 확정(OnInitDialog 의 레이아웃 보정이 참조하므로 create 전에 확정 필요). (`054587d`)

---

## 8. Common 라이브러리 개선 (파일전송 연관분)

이 프로젝트가 의존하는 Common 컨트롤의 버그/기능. (동영상 플레이어용 dshow/SCSlider/ffmpeg 등은 제외)

### SCTreeCtrl
- `insert_folder` 에 `hInsertAfter`(기본 TVI_LAST)+HTREEITEM 반환, `insert_folder_sorted`(탐색기식 숫자 인식 `is_greater_with_numeric` 오름차순 삽입). (`93a899f`)
- 드래그 자동 스크롤 추가(트리 자체 드래그 4방향). (`f68228b`)
- native hot-track 밑줄(TVS_TRACKSELECT) 제거 — 부분 invalidate 시 밑줄이 새어 보이던 것. hover 는 `m_hot_item` full-row 자체 처리. (`c9a9ec9`)
- 드래그 중 full-row hot-tracking 금지 + cross-control 드롭 하이라이트(다른 컨트롤이 소스여도 대상 트리에 표시). (`092a18e`)
- 우클릭 자동펼침·색상 깜빡임 + 드래그 이미지 메모리 릭, use-after-free 크래시 수정. (`a991659`, `bfc2d51`)
- 이름변경 자체 처리(폴더 rename·드라이브 볼륨 레이블) + 폰트명 변경 지원. (`fc91481`)
- `set_line_height` 추가 + shell 트리 라인 간격 28px. (`7ca5c7b`)
- 가로 스크롤 버그 4건(이름변경 위치·스크롤바 동기·드래그 자동스크롤). (`cdfa2d1`)

### CVtListCtrlEx
- 세로 스크롤 마지막 항목 도달 불가 → 팬텀 행으로 native 범위 확장(구조적 한계 조사·문서화 → 추후 CSCListCtrl 대체 배경). (`dbbb4e7`)
- 리사이즈 렌더링 정리(OnPaint 화면 되읽기 제거, sync_scrollbar 중복 제거). (`b54a5c8`)
- shell 리스트 라인 간격/헤더 높이 26px(탐색기 유사). (`4b8056d`)
- drop-hilight 색을 트리와 통일/항목색과 무관하게 적용. (`debaf83`, `4a7a800`)
- 우클릭 native `WM_CONTEXTMENU` 사용(브릿지 제거) + 우클릭 메뉴 신설. (`9610292`, `8466172`)
- 보조색(크기/날짜) 대비 비례 blend + 이동만 해도 스크롤바 sync. (`4768f3a`)

### SCStatic / SCStaticEdit / PathCtrl
- `set_no_prefix()` 추가(파일명 `&` 리터럴 표시, DT_NOPREFIX). (`9a98b3b`)
- 헤더 아이콘 + tagged text 합성(텍스트 겹침 수정, 간격/좌측여백 조절) — ExistFileDlg 배너용. (`b781ad2`, `057e3eb`)
- PathCtrl 세그먼트 폭 측정에 DT_NOPREFIX — `"R&D"` 가 `"RD"`·불필요 말줄임표로 나오던 것. (`f2df6d2`)
- `convert_real_path_to_special_folder`: 역슬래시 없는 드라이브 루트(`"D:"`)도 드라이브로 인식. (`ddc2a60`)
- SCStaticEdit disable 배경/테두리 통일, action 버튼 툴팁. (`8466172`, `c106d1f`)

### SCThemeDlg / SCScrollbar / CGdiButton / colors / SCMenu / Functions
- SCThemeDlg: borderless 비-resizable 창 우/하 여백 잘림 수정, 리사이즈 시 부분 무효화+부분 버퍼링. (`2e0b85c`, `6bcb040`)
- SCScrollbar: 썸/화살표 공통색 + 짧은 트랙 썸 길이 제한, 트랙 라인 제거. (`e44d0e4`, `8466172`)
- CGdiButton: 체크박스/라디오 indicator 간격 DPI 스케일, X 세로중앙, 정렬/테두리. (`a180488`, `76b48a8`, `8466172`)
- colors: `get_lightened_color`(HSL L 이동) 추가, 약화색 대비 비례 blend 계열. (`6ea4bae`, 06-30 배치)
- SCMenu: 우클릭 서브메뉴가 고DPI(150%)에서 멀리 뜨던 문제 수정. (`f8ddc5c`)
- Functions: `draw_hicon_alpha()`(아이콘 straight-alpha 직접 추출) 신설. (`54254e0`)
- 주석 작성자 마커 형식 통일(`//YYYYMMDD by claude.`). (`c90da9e`, `ece6156`)

---

## 9. 미커밋/진행 중

- 파일 전송창 상단 `m_static_message.set_no_prefix(true)`(파일명 `&` 리터럴) — 빌드 검증 완료, 커밋 대기.
- ExistFileDlg 물음표 아이콘 AA 보정본 + 관련 리소스 — exe 임베드까지 검증 완료.

> 다음 단계(별도 프로젝트): CVtListCtrlEx → **CSCListCtrl** 대체(세로 스크롤 구조 개선). 이 프로젝트 편입 시 관련 항목 별도 정리 예정.
