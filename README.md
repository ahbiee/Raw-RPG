# Raw RPG - 多層次架構角色扮演遊戲實作

![C Version](https://img.shields.io/badge/Language-C-blue.svg)
![MIPS Version](https://img.shields.io/badge/Language-MIPS-orange.svg)
![Data Structure](https://img.shields.io/badge/Data_Structure-Max_Heap-green.svg)

## 📌 專案概述 Overview
本專案為「組合語言」課程的期末專題。我們實作出一個具備 2D 地圖探索、回合制戰鬥及商店系統的 RPG 遊戲。核心亮點在於結合高階 C 語言的演算法邏輯與低階 MIPS 指令集的硬體操作，並實作 **堆積排序 (Heap Sort)** 進行背包空間管理。

## 🛠 技術棧 Tech Stack

### 1. 應用之資料結構 Data Structures
本專案透過自定義結構體（struct）以管理遊戲實體：
- **Entity Struct**: 封裝角色與敵人的屬性（HP, ATK, SPD, Crit Rate, Position）。
- **Item Struct**: 實作道具資訊，包含類型識別碼（Type ID）與數值增益。
- **Backpack Manager**: 核心架構為建立一個 `Item陣列` ，透過Heap sort維持有序狀態。

### 2. 演算法：堆積排序 Heap Sort
為了達成高效的背包管理，本專案實作了Heap sort演算法，保證每次整理背包的時間複雜度為 O（n * log n）：
- **Heapify**: 建立與調整 Max Heap，確保 ID 最大的物品位於根節點。
- **In-place Sort**: 在不使用額外空間的情況下，對背包內物品依據 `Item ID` 進行排序，確保介面輸出的穩定性。

### 3. 有限狀態機 Finite State Machine, FSM
系統邏輯依據 `GameMode` 枚舉類型進行狀態切換：
- `MAP`: 處理座標更新、碰撞偵測（Collision Detection）與邊界檢查。
- `BATTLE`: 實作回合制邏輯（Turn-based logic），依據速度屬性決定指令執行優先順序。
- `SHOP`: 處理金幣與物品陣列的數值交換。

## 💻 跨語言實作比對 (C vs. MIPS)

| 功能模組 | C 語言實作方式 | MIPS 組合語言實作方式 |
| :--- | :--- | :--- |
| **邏輯判斷** | `switch-case` 與 `if-else` | `beq`, `bne`, `slt` 分支跳轉 |
| **函數呼叫** | 標準 Function Call | `jal` 與 Stack Frame 管理 ($ra, $sp) |
| **亂數生成** | `rand()` (stdlib) | `syscall 42` (MARS 隨機數生成) |
| **資料儲存** | 全域與區域結構體變數 | `.data` 區段記憶體配置與偏移量存取 |

## 🚀 開發與執行環境 Development/Execution Environment

### C 語言版本
- **編譯器**: GCC (Linux/WSL)
- **指令**:
```bash
gcc main.c -o RawRPG
./RawRPG
```

### MIPS 版本

* **模擬器**: MARS (MIPS Assembler and Runtime Simulator)
* **核心功能**: 實作地圖座標計算、基礎戰鬥邏輯與 Syscall 系統呼叫。

## 📂 檔案結構 Files

* `struct.h`: 定義所有資料結構與function prototype。
* `main.c`: 遊戲主迴圈、Heap Sort 演算法及戰鬥邏輯實作。
* `rpg_simple.asm`: 針對 MIPS 指令集優化的精簡版遊戲邏輯。

## 👥 開發小組 Developers

* **hermeswangggg**
* **chris456621**
* **ahbiee**

```