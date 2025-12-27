# =========================================
# Raw RPG - MIPS 簡化完整版
# 適合初學者學習的組合語言RPG遊戲
# 可直接在MARS模擬器上執行
# =========================================
# 作者: 413262380 王柏硯, 413262457 白侑恆, 413262574 張博鈞
# 說明: 這是原C語言版本的MIPS實作，保留核心功能
# =========================================

.data
    # ===== 遊戲訊息 =====
    welcome:        .asciiz "====================================================\n"
    title:          .asciiz "Welcome to Raw RPG (MIPS Version)\n"
    name_prompt:    .asciiz "Enter your name: "
    hello:          .asciiz "Welcome, "
    newline:        .asciiz "\n"
    
    # 地圖訊息
    map_prompt:     .asciiz "\nWhat's your action?\nW: up  S: down  A: left  D: right  E: backpack\n> "
    pos_msg:        .asciiz "Position: ("
    comma:          .asciiz ", "
    bracket:        .asciiz ")\n"
    
    # 戰鬥訊息
    enemy_meet:     .asciiz "\n=== Battle Start! ===\n"
    you:            .asciiz "YOU - HP: "
    enemy_label:    .asciiz "ENEMY - HP: "
    battle_prompt:  .asciiz "\nA: Attack  D: Defend\n> "
    you_attack:     .asciiz "You attack! Damage: "
    enemy_attack:   .asciiz "Enemy attacks! Damage: "
    you_defend:     .asciiz "You defend! Damage reduced!\n"
    
    # 結果訊息
    win_msg:        .asciiz "\n*** Victory! ***\n"
    lose_msg:       .asciiz "\n*** Game Over ***\n"
    continue_msg:   .asciiz "Press 1 to continue: "
    
    # 背包訊息
    backpack_msg:   .asciiz "\n=== Backpack ===\n"
    gold_label:     .asciiz "Gold: "
    potion_label:   .asciiz "HP Potions: "
    use_potion:     .asciiz "1: Use Potion  0: Back\n> "
    potion_used:    .asciiz "HP +30!\n"
    no_potion:      .asciiz "No potions!\n"
    
    # ===== 遊戲資料 =====
    # 玩家資料
    player_name:    .space 24
    player_hp:      .word 100
    player_max_hp:  .word 100
    player_atk:     .word 15
    player_x:       .word 5
    player_y:       .word 5
    player_gold:    .word 50
    player_potions: .word 3
    player_alive:   .word 1
    
    # 敵人資料
    enemy_hp:       .word 50
    enemy_max_hp:   .word 50
    enemy_atk:      .word 10
    enemy_x:        .word 8
    enemy_y:        .word 8
    enemy_alive:    .word 1
    enemy_name:     .asciiz "Slime"
    
    # Boss資料
    boss_hp:        .word 100
    boss_atk:       .word 20
    boss_x:         .word 1
    boss_y:         .word 1
    boss_alive:     .word 1
    boss_name:      .asciiz "Lion King"
    
    # 遊戲狀態
    game_mode:      .word 0     # 0=探索, 1=戰鬥, 2=背包
    in_battle_with: .word 0     # 0=無, 1=敵人, 2=Boss
    
    # 簡化地圖 (10x10)
    map_size:       .word 10
    map:            .space 100  # 10x10的地圖

.text
.globl main

# =========================================
# 主程式
# =========================================
main:
    # 初始化隨機數
    li $v0, 30
    syscall
    move $a1, $a0
    li $v0, 40
    li $a0, 1
    syscall
    
    # 歡迎訊息
    li $v0, 4
    la $a0, welcome
    syscall
    
    li $v0, 4
    la $a0, title
    syscall
    
    li $v0, 4
    la $a0, welcome
    syscall
    
    # 輸入玩家名字
    li $v0, 4
    la $a0, name_prompt
    syscall
    
    li $v0, 8
    la $a0, player_name
    li $a1, 24
    syscall
    
    # 移除換行符
    la $t0, player_name
remove_newline:
    lb $t1, 0($t0)
    beq $t1, 10, found_newline
    beqz $t1, newline_done
    addi $t0, $t0, 1
    j remove_newline
found_newline:
    sb $zero, 0($t0)
newline_done:
    
    # 歡迎玩家
    li $v0, 4
    la $a0, hello
    syscall
    
    li $v0, 4
    la $a0, player_name
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    # 初始化地圖
    jal init_map
    
# ===== 主遊戲迴圈 =====
game_loop:
    # 檢查遊戲是否結束
    lw $t0, player_alive
    beqz $t0, game_over
    
    lw $t0, boss_alive
    beqz $t0, game_won
    
    # 顯示當前狀態
    jal show_status
    
    # 顯示提示並讀取輸入
    li $v0, 4
    la $a0, map_prompt
    syscall
    
    li $v0, 12      # 讀取字元
    syscall
    move $t0, $v0
    
    # 印換行
    li $v0, 4
    la $a0, newline
    syscall
    
    # 處理輸入
    li $t1, 'W'
    beq $t0, $t1, move_up
    li $t1, 'w'
    beq $t0, $t1, move_up
    
    li $t1, 'S'
    beq $t0, $t1, move_down
    li $t1, 's'
    beq $t0, $t1, move_down
    
    li $t1, 'A'
    beq $t0, $t1, move_left
    li $t1, 'a'
    beq $t0, $t1, move_left
    
    li $t1, 'D'
    beq $t0, $t1, move_right
    li $t1, 'd'
    beq $t0, $t1, move_right
    
    li $t1, 'E'
    beq $t0, $t1, open_backpack
    li $t1, 'e'
    beq $t0, $t1, open_backpack
    
    j game_loop

# ===== 移動處理 =====
move_up:
    lw $t0, player_y
    ble $t0, 0, game_loop
    addi $t0, $t0, -1
    sw $t0, player_y
    jal check_collision
    j game_loop

move_down:
    lw $t0, player_y
    lw $t1, map_size
    addi $t1, $t1, -1
    bge $t0, $t1, game_loop
    addi $t0, $t0, 1
    sw $t0, player_y
    jal check_collision
    j game_loop

move_left:
    lw $t0, player_x
    ble $t0, 0, game_loop
    addi $t0, $t0, -1
    sw $t0, player_x
    jal check_collision
    j game_loop

move_right:
    lw $t0, player_x
    lw $t1, map_size
    addi $t1, $t1, -1
    bge $t0, $t1, game_loop
    addi $t0, $t0, 1
    sw $t0, player_x
    jal check_collision
    j game_loop

# ===== 背包 =====
open_backpack:
    # 顯示背包
    li $v0, 4
    la $a0, backpack_msg
    syscall
    
    li $v0, 4
    la $a0, gold_label
    syscall
    lw $a0, player_gold
    li $v0, 1
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    li $v0, 4
    la $a0, potion_label
    syscall
    lw $a0, player_potions
    li $v0, 1
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    # 使用藥水
    li $v0, 4
    la $a0, use_potion
    syscall
    
    li $v0, 5
    syscall
    
    beqz $v0, game_loop
    
    # 使用藥水
    lw $t0, player_potions
    blez $t0, no_potions
    
    addi $t0, $t0, -1
    sw $t0, player_potions
    
    lw $t0, player_hp
    addi $t0, $t0, 30
    lw $t1, player_max_hp
    ble $t0, $t1, hp_ok
    move $t0, $t1
hp_ok:
    sw $t0, player_hp
    
    li $v0, 4
    la $a0, potion_used
    syscall
    j game_loop

no_potions:
    li $v0, 4
    la $a0, no_potion
    syscall
    j game_loop

# ===== 顯示狀態 =====
show_status:
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    
    # 顯示地圖
    jal show_map
    
    li $v0, 4
    la $a0, newline
    syscall
    
    li $v0, 4
    la $a0, you
    syscall
    
    lw $a0, player_hp
    li $v0, 1
    syscall
    
    li $v0, 4
    la $a0, pos_msg
    syscall
    
    lw $a0, player_x
    li $v0, 1
    syscall
    
    li $v0, 4
    la $a0, comma
    syscall
    
    lw $a0, player_y
    li $v0, 1
    syscall
    
    li $v0, 4
    la $a0, bracket
    syscall
    
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

# ===== 檢查碰撞 =====
check_collision:
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    
    # 檢查是否碰到敵人
    lw $t0, player_x
    lw $t1, player_y
    lw $t2, enemy_x
    lw $t3, enemy_y
    lw $t4, enemy_alive
    
    beqz $t4, check_boss
    bne $t0, $t2, check_boss
    bne $t1, $t3, check_boss
    
    # 碰到敵人！
    li $t0, 1
    sw $t0, in_battle_with
    jal battle
    j check_done
    
check_boss:
    # 檢查是否碰到Boss
    lw $t0, player_x
    lw $t1, player_y
    lw $t2, boss_x
    lw $t3, boss_y
    lw $t4, boss_alive
    
    beqz $t4, check_done
    bne $t0, $t2, check_done
    bne $t1, $t3, check_done
    
    # 碰到Boss！
    li $t0, 2
    sw $t0, in_battle_with
    jal battle
    
check_done:
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

# ===== 戰鬥系統 =====
battle:
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    
    # 顯示戰鬥開始
    li $v0, 4
    la $a0, enemy_meet
    syscall
    
battle_loop:
    # 顯示狀態
    li $v0, 4
    la $a0, you
    syscall
    lw $a0, player_hp
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    
    li $v0, 4
    la $a0, enemy_label
    syscall
    
    # 根據對手顯示HP
    lw $t0, in_battle_with
    li $t1, 2
    beq $t0, $t1, show_boss_hp
    
    lw $a0, enemy_hp
    j show_hp_done
show_boss_hp:
    lw $a0, boss_hp
show_hp_done:
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    
    # 玩家行動
    li $v0, 4
    la $a0, battle_prompt
    syscall
    
    li $v0, 12
    syscall
    move $s0, $v0
    
    li $v0, 4
    la $a0, newline
    syscall
    
    # 檢查行動
    li $t0, 'A'
    beq $s0, $t0, player_attack_action
    li $t0, 'a'
    beq $s0, $t0, player_attack_action
    li $t0, 'D'
    beq $s0, $t0, player_defend_action
    li $t0, 'd'
    beq $s0, $t0, player_defend_action
    j battle_loop

player_attack_action:
    # 玩家攻擊
    li $v0, 4
    la $a0, you_attack
    syscall
    
    lw $t0, player_atk
    # 隨機傷害 (atk ± 3)
    li $v0, 42
    li $a0, 0
    li $a1, 7
    syscall
    add $t0, $t0, $a0
    addi $t0, $t0, -3
    
    li $v0, 1
    move $a0, $t0
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    # 扣除敵人HP
    lw $t1, in_battle_with
    li $t2, 2
    beq $t1, $t2, damage_boss
    
    lw $t1, enemy_hp
    sub $t1, $t1, $t0
    blez $t1, enemy_defeated
    sw $t1, enemy_hp
    j enemy_turn
    
damage_boss:
    lw $t1, boss_hp
    sub $t1, $t1, $t0
    blez $t1, boss_defeated
    sw $t1, boss_hp
    j enemy_turn

player_defend_action:
    li $v0, 4
    la $a0, you_defend
    syscall
    
    # 敵人攻擊(減半)
    j enemy_turn_defend

enemy_turn:
    # 敵人攻擊
    li $v0, 4
    la $a0, enemy_attack
    syscall
    
    lw $t0, in_battle_with
    li $t1, 2
    beq $t0, $t1, boss_attack
    
    lw $t0, enemy_atk
    j calc_enemy_damage
boss_attack:
    lw $t0, boss_atk
    
calc_enemy_damage:
    # 隨機傷害
    li $v0, 42
    li $a0, 0
    li $a1, 5
    syscall
    add $t0, $t0, $a0
    addi $t0, $t0, -2
    
    li $v0, 1
    move $a0, $t0
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    lw $t1, player_hp
    sub $t1, $t1, $t0
    blez $t1, player_dead
    sw $t1, player_hp
    
    j battle_loop

enemy_turn_defend:
    # 敵人攻擊(減半)
    li $v0, 4
    la $a0, enemy_attack
    syscall
    
    lw $t0, in_battle_with
    li $t1, 2
    beq $t0, $t1, boss_attack_def
    
    lw $t0, enemy_atk
    j calc_enemy_damage_def
boss_attack_def:
    lw $t0, boss_atk
    
calc_enemy_damage_def:
    srl $t0, $t0, 1     # 除以2
    
    li $v0, 1
    move $a0, $t0
    syscall
    
    li $v0, 4
    la $a0, newline
    syscall
    
    lw $t1, player_hp
    sub $t1, $t1, $t0
    blez $t1, player_dead
    sw $t1, player_hp
    
    j battle_loop

enemy_defeated:
    sw $zero, enemy_alive
    li $v0, 4
    la $a0, win_msg
    syscall
    
    # 獲得金幣
    lw $t0, player_gold
    addi $t0, $t0, 20
    sw $t0, player_gold
    
    jal wait_continue
    
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

boss_defeated:
    sw $zero, boss_alive
    li $v0, 4
    la $a0, win_msg
    syscall
    
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

player_dead:
    sw $zero, player_alive
    
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

# ===== 等待繼續 =====
wait_continue:
    li $v0, 4
    la $a0, continue_msg
    syscall
    
    li $v0, 5
    syscall
    
    jr $ra

# ===== 初始化地圖 =====
init_map:
    # 填充地圖為空地 '.'
    la $t0, map
    lw $t1, map_size
    mult $t1, $t1
    mflo $t1          # total = size * size
    li $t2, 0
init_loop:
    bge $t2, $t1, init_done
    li $t3, '.'
    sb $t3, 0($t0)
    addi $t0, $t0, 1
    addi $t2, $t2, 1
    j init_loop
init_done:
    jr $ra

# ===== 顯示地圖 =====
show_map:
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    
    li $v0, 4
    la $a0, newline
    syscall
    
    # 繪製地圖
    lw $t9, map_size
    li $t0, 0         # y counter
map_y_loop:
    bge $t0, $t9, map_done
    li $t1, 0         # x counter
map_x_loop:
    bge $t1, $t9, map_y_next
    
    # 檢查是否是玩家位置
    lw $t2, player_x
    lw $t3, player_y
    bne $t1, $t2, check_enemy_pos
    bne $t0, $t3, check_enemy_pos
    li $v0, 11
    li $a0, 'P'
    syscall
    j map_x_next
    
check_enemy_pos:
    # 檢查是否是敵人位置
    lw $t2, enemy_x
    lw $t3, enemy_y
    lw $t4, enemy_alive
    beqz $t4, check_boss_pos
    bne $t1, $t2, check_boss_pos
    bne $t0, $t3, check_boss_pos
    li $v0, 11
    li $a0, 'M'
    syscall
    j map_x_next
    
check_boss_pos:
    # 檢查是否是Boss位置
    lw $t2, boss_x
    lw $t3, boss_y
    lw $t4, boss_alive
    beqz $t4, show_terrain
    bne $t1, $t2, show_terrain
    bne $t0, $t3, show_terrain
    li $v0, 11
    li $a0, 'B'
    syscall
    j map_x_next
    
show_terrain:
    # 顯示地形
    li $v0, 11
    li $a0, '.'
    syscall
    
map_x_next:
    li $v0, 11
    li $a0, ' '
    syscall
    addi $t1, $t1, 1
    j map_x_loop
    
map_y_next:
    li $v0, 4
    la $a0, newline
    syscall
    addi $t0, $t0, 1
    j map_y_loop
    
map_done:
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    jr $ra

# ===== 遊戲結束 =====
game_over:
    li $v0, 4
    la $a0, newline
    syscall
    
    li $v0, 4
    la $a0, lose_msg
    syscall
    j exit

game_won:
    li $v0, 4
    la $a0, newline
    syscall
    
    li $v0, 4
    la $a0, win_msg
    syscall

exit:
    li $v0, 10
    syscall
