# 🧩 미로 게임 구현 (Dedicated Server 기반)

> 이 프로젝트는 **언리얼 엔진의 Dedicated Server 구조**를 활용하여, 자동으로 미로를 생성하고, 플레이어가 도착 지점에 도달하면 게임이 종료되는 방식으로 구성된 멀티플레이 게임입니다.

<br>

## 📌 프로젝트 개요

- **프로젝트 명**: Maze Escape Game
- **개발 환경**: Unreal Engine 5.54, C++
- **서버 구조**: Dedicated Server
- **핵심 기능**:
  - DFS 기반 자동 미로 생성 시스템
  - 가장 깊은 셀을 목표 지점으로 지정
  - 서버-클라이언트 구조에 맞춘 로직 및 클래스 설계
  - 플레이어 간 경쟁 구조 (먼저 도착한 사람이 승리)
 
<br>

## 🖥️ Dedicated Server 구조의 특징

본 프로젝트는 **Listen Server가 아닌 완전한 Dedicated Server** 환경을 기반으로 제작되었습니다.  
Dedicated Server는 클라이언트와 별개로 동작하는 독립 서버 프로세스로, 게임의 모든 상태와 로직을 서버에서 authoritative하게 처리합니다.

### ✅ 설계 의도 및 장점
- 서버가 모든 게임 로직을 소유하므로 **치트 및 동기화 오류 방지**
- **클라이언트는 단순 입력/출력 처리**, 로직 결정은 전적으로 서버 담당
- 추후 **매치메이킹, 로비 시스템 연계** 시 확장성 확보

### ✅ 관련 클래스 설계 구조
| 클래스 | 역할 |
|--------|------|
| `AEscapeFromMazeGameMode` | 서버 전용. 접속자 처리 및 게임 흐름, 승패 로직 제어 |
| `AEFM_GameState` | 서버/클라이언트 공용. 시작 위치, Maze 상태 등 공유 데이터 관리 |
| `AEFM_PlayerState` | 플레이어 고유 정보(닉네임 등)를 서버에서 관리하고 클라이언트로 복제 |
| `AEFM_PlayerController` | 클라이언트 입력 수집, 서버 RPC 호출, HUD 갱신 등 처리 |

<br>

<br>

## 🧱 전체 시스템 구조 및 클래스 흐름도

### 1. 미로 생성 구조
- `FMazeCell`: 셀 구조를 정의하는 구조체 (방문 여부, 벽 4방향)
- `MazeData`: 전체 미로를 저장하는 2D 배열
- `AMazeGenerator`: 미로 데이터를 생성하고 게임에 배치하는 클래스

### 2. 게임 진행 흐름
- 서버에서 게임 시작 시, 자동으로 미로를 생성
- 가장 깊은 셀을 목표 지점으로 설정
- 플레이어는 임의의 위치에서 같이 시작하여 목표 지점을 탐색 및 이동
- 최초 한 명이 목표 도달 시, 승자 판정 후 게임 종료

<br>

## 🧮 미로 생성 알고리즘

- DFS 기반 재귀 알고리즘으로 미로를 생성
- 방문하지 않은 셀을 랜덤하게 방문하며 경로 생성
- 가장 깊이 도달한 셀을 `GoalPoint`로 설정해 도착 지점 지정
- 각 셀은 네 방향 벽 상태를 저장하며, 인접 셀과 연결 시 해당 벽을 제거

<br>

## 🔄 데이터 통신 및 동기화

### 📌 게임 전체 흐름에 따른 클래스 역할 요약

**서버 시작 시 → 미로 생성기 동작**

```
AMazeGenerator::InitMazeData()
→ 미로 셀 초기화 (FMazeCell 2D 배열)

AMazeGenerator::GenerateMaze()
→ 재귀 백트래킹 알고리즘으로 미로 생성
→ 가장 깊게 도달한 셀을 GoalPoint로 저장

AMazeGenerator::SpawnMazeActors(), SpawnGoalActor()
→ 월드에 벽과 끝 점 Actor 생성
→ 시작 위치는 AEFM_GameState에 저장
```

**플레이어 접속 → 서버 처리**

```
AEscapeFromMazeGameMode::PostLogin()
→ 일정 시간 딜레이 후 AssignStartLocation() 호출
→ GameState에 저장된 StartLocation 기반으로 위치 계산
→ AEscapeFromMazeCharacter::SetActorLocation() 또는 Client_MoveToStart() 호출
```

**클라이언트에서 닉네임 입력**

```
UMG 위젯 → OnTextCommitted()
→ AEFM_PlayerController::Server_SetPlayerNickname()
→ AEFM_PlayerState::SetPlayerNickname() → Replicated
→ 클라이언트에 이름 동기화
```

**골 도달 시 처리**

```
AGoalPoint::NotifyActorBeginOverlap()
→ AEscapeFromMazeGameMode::EndGame() 호출
→ 승자 이름 모든 클라이언트에 전달
→ AEFM_PlayerController::Client_ReachedGoal() → HUD 업데이트
```

**게임 종료**

```
AEscapeFromMazeGameMode::ExitGame()
→ 모든 클라이언트의 AEFM_PlayerController::Client_ExitGame() 호출
→ 레벨 종료 또는 게임 종료 처리
```

<br>

## 🧭 구조 요약

### 🧩 미로 관련
| 구성 요소 | 설명 |
|-----------|------|
| `FMazeCell` | 셀 하나의 벽 정보 및 방문 여부 정의 |
| `MazeData` | `FMazeCell`을 기반으로 구성된 2차원 배열 미로 데이터 |
| `AMazeGenerator` | `MazeData` 초기화 및 DFS를 통한 미로 생성, 골 지점 결정 |
| `GoalPoint` | DFS로 생성된 최종 목적지 좌표, 해당 위치에 골인 액터 배치 |

### 🧍‍♂️ 플레이어 관련
| 구성 요소 | 설명 |
|-----------|------|
| `AEFM_PlayerController` | 입력 처리, 클라이언트 HUD 및 상태 제어 |
| `AEFM_PlayerState` | 플레이어 닉네임 등 고유 정보 저장, 승자 표시에 활용 |
| `AEscapeFromMazeCharacter` | 실제 월드에 존재하는 플레이어 캐릭터 Actor |

### 🎮 게임 흐름 관련
| 구성 요소 | 설명 |
|-----------|------|
| `AEscapeFromMazeGameMode` | 플레이어 접속 및 시작 위치 배치, 게임 승패 처리 담당 |
| `AEFM_GameState` | 모든 플레이어가 공유하는 시작 위치, 미로 상태 등 게임 데이터 저장 |

<br>

## 📎 관련 링크

- 🔗 [개발 블로그 원문 보기](https://dong-grae.tistory.com/206)
