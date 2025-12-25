/*

Project Name: Raw RPG
Purpose: 組合語言期末專題
Code language: C

Author:
413262380 王柏硯
413262457 白侑恆
413262574 張博鈞

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "struct.h"

const Entity enemy_db[] = {
    // name, id, hp, maxhp, atk, speed, dodge_rate, crit_rate, is_alive, pos_x, pos_y, type
    {"", -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, 'E'},     // type 0 是 placeholder, 不應該存取到這
    {"Slime", -1, 20, 20, 3, 4, 0, 0, 1, -1, -1, 'E'},    // type 1 是 Slime
    {"Skeleton", -1, 15, 15, 8, 7, 5, 5, 1, -1, -1, 'E'}, // type 2 是 Skeleton
    {"Zombie", -1, 40, 40, 5, 2, 1, 1, 1, -1, -1, 'E'},   // type 3 是 Zombie
    {"Goblin", -1, 30, 30, 6, 6, 3, 3, 1, -1, -1, 'E'},   // type 4 是 Goblin
    {"Ghost", -1, 5, 5, 15, 10, 30, 10, 1, -1, -1, 'E'}   // type 5 是 Ghost
};

const Item item_db[] = {
    //  id, name, cost, count, can_be_used_in_map, type
    {0, "Sword", 5, 1, 0, 'W'},
    {1, "Axe", 10, 1, 0, 'W'},
    {2, "Bow", 15, 1, 0, 'W'},
    {3, "Helmet", 13, 1, 1, 'A'},     // 頭盔: map模式可穿上
    {4, "Chestplace", 20, 1, 1, 'A'}, // 胸甲: map模式可穿上
    {5, "Leggings", 13, 1, 1, 'A'},   // 護腿: map模式可穿上
    {6, "Boost", 8, 1, 1, 'A'},       // 靴子: map模式可穿上
    {7, "Hp_Posion", 10, 1, 1, 'P'},  // 生命(health point)藥水: map模式可喝
    {8, "Pw_Posion", 10, 1, 1, 'P'},  // 力量(power)藥水: map模式可喝
    {9, "Hm_Posion", 15, 1, 0, 'P'},  // 傷害(harm)藥水
};

// ---------------------------------------------------------------------------------------------------------------------------

// 玩家+敵人最多16個實體
Entity entity[MAX_ENTITIES];

// player's backpack
Backpack backpack;

enum
{
    MAP,
    BATTLE,
    SHOP,
    BACKPACK
} GameMode;

// 暫時的屬性加成數值(使用武器時會用到)
int temp_bonus[5] = {0, 0, 0, 0, 0}; // attack, defense, dodge_rate, crit_rate, Harm

// map
char map[MAP_HEIGHT][MAP_WIDTH];

int main()
{
    srand((unsigned)time(NULL)); // 用當前時間初始化隨機數(確保一定程度的偽隨機)

    int enemy_count = rand() % 6 + 10; // randomly encounter 10~15 enemies, the player will fight them one by one
    int total_count = enemy_count + 1; // entity[0] is player

    printf("==========================================\n");
    printf("Welcome to Raw RPG\n");
    printf("Enter your name (accept spaces, limit 23 characters): ");
    if (fgets(entity[0].name, sizeof(entity[0].name), stdin) != NULL)
    {
        size_t len = strlen(entity[0].name);

        // case A: user's input is correct, then we replace '\n' with '\0'
        if (len > 0 && entity[0].name[len - 1] == '\n')
            entity[0].name[len - 1] = '\0';
        // case B: user's input is more thant 19 characters, we need to clear the input buffer
        else
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF); // clear buffer
            // we don't need to manually replace the last character to '\n', it's automaticly done by fgets
        }
    }

    printf("Welcome, %s\n", entity[0].name);

    // 初始化玩家屬性
    initialize_Player();

    // 初始化背包
    initialize_Backpack();

    /*
        以下測試背包輸出功能，可以先不管
    */
    backpack.gold = 100;
    backpack.item_count = 5;
    backpack.items[0] = item_db[7];
    backpack.items[1] = item_db[1];
    backpack.items[2] = item_db[2];
    backpack.items[3] = item_db[3];
    backpack.items[4] = item_db[4];
    /*
        以上測試背包輸出功能，可以先不管
    */

    // 設定所有敵人屬性
    printf("==========================================\n");
    printf("Enemy list (10~15) for this game :\n");
    for (int i = 1; i <= enemy_count; ++i)
    {
        setupEnemy(i);
    }
    printf("==========================================\n");

    // 初始化地圖
    initialize_map(enemy_count);

    // 印出地圖
    print_map();
    print_action_prompt();

    // 地圖模式主迴圈
    while (entity[0].is_alive == 1)
    {
        GameMode = MAP;
        char nextAction; // the upcoming action

        scanf("%c", &nextAction);
        if (nextAction == '\n')
        {
            print_action_prompt();
            continue; // avoid enter key input
        }
        action_map(nextAction, &entity[0]); // update player position

        switch (map[entity[0].pos_y][entity[0].pos_x])
        {
        case 'M':
            // 進入戰鬥模式
            GameMode = BATTLE;
            int ind;
            for (int i = 1; i < MAX_ENTITIES; i++)
            {
                if (entity[i].pos_x == entity[0].pos_x && entity[i].pos_y == entity[0].pos_y)
                {
                    ind = i;
                    break;
                }
            } // confirm which enemy to fight
            Battle_Mode(&entity[0], &entity[ind]);
            break;

        case '$':
            // 進入商店模式
            GameMode = SHOP;
            Shop_Mode();
            break;

        case 'B':
            // 進入Boss戰鬥模式
            printf("You have encountered the BOSS!\n");

        default:
            break;
        }
        refresh_map();
    }
    printf("You're DEAD.\n");

    /*
    以下code是為了測試player backpack與heap sort能夠正常使用，可以先不管
    */
    backpack.item_count = 5;
    backpack.items[0] = (Item){5, "Potion", 10, 1, 1, 'P'};
    backpack.items[1] = (Item){1, "Sword", 100, 1, 0, 'W'};
    backpack.items[2] = (Item){3, "Key", 50, 1, 0, 'W'};
    backpack.items[3] = (Item){4, "Shield", 80, 1, 0, 'A'};
    backpack.items[4] = (Item){2, "Map", 5, 1, 1, 'W'};
    backpack.gold = 0;

    printf("ID sequence before heap sort: ");
    for (int i = 0; i < backpack.item_count; i++)
        printf("%d ", backpack.items[i].id);
    printf("\n");

    sort_backpack(); // 執行排序

    printf("ID sequence after heap sort: ");
    for (int i = 0; i < backpack.item_count; i++)
        printf("%d ", backpack.items[i].id);
    printf("\n");
    /*
    以上code是為了測試map能夠正常使用，可以先不管
    */

    return 0;
}

// 印出玩家行動提示
void print_action_prompt()
{
    switch (GameMode)
    {
    case MAP:
        printf("What's your action?\n");
        printf("W: move up\n");
        printf("S: move down\n");
        printf("A: move left\n");
        printf("D: move right\n");
        printf("E: open backpack\n");
        printf("The action you're going to make is: ");
        return;
    case BATTLE:
        printf("Choose your action:\n");
        printf("A: attack\n");
        printf("D: defend\n");
        printf("I: check and use item\n");
        printf("The action you're going to make is: ");
        return;

    case SHOP:
        return;

    case BACKPACK:
        printf("Enter an item index to use (enter anything, -1 to exit): ");
        return;

    default:
        return;
    }
}

// 印出地圖
void print_map()
{
    for (int i = 0; i < MAP_HEIGHT; ++i)
    {
        for (int j = 0; j < MAP_WIDTH; ++j)
        {
            printf("%c", map[i][j]);
        }
        if (i < MAP_HEIGHT - 1)
            puts("");
        else
            printf(" Player position: (%d, %d)\n", entity[0].pos_x, entity[0].pos_y);
    }
}

// 初始化玩家屬性
void initialize_Player()
{
    entity[0].id = 0;
    entity[0].hp = entity[0].max_hp = 100;
    entity[0].atk = 20;
    entity[0].speed = 5;
    entity[0].dodge_rate = 10;
    entity[0].crit_rate = 5;
    entity[0].is_alive = 1;
    entity[0].type = 'P';
    entity[0].pos_x = MAP_WIDTH - 2;
    entity[0].pos_y = MAP_HEIGHT - 2;
}

// 初始化背包
void initialize_Backpack()
{
    backpack.item_count = 0;
    backpack.gold = 0;
    for (int i = 0; i < 4; i++)
    {
        backpack.armor_slots[i].id = -1; // 代表該盔甲欄位沒有裝備
    }
}

// 設定敵人屬性
void setupEnemy(int i)
{
    int type = rand() % 5 + 1; // type依照 敵人清單.txt

    // status from template
    strncpy(entity[i].name, enemy_db[type].name, sizeof(entity[i].name) - 1); // 存到destination的容量-1
    entity[i].name[sizeof(entity[i].name) - 1] = '\0';                        // 讓destination的最後一個位元變成 '\0'，保證字串安全
    entity[i].type = 'M';
    entity[i].hp = entity[i].max_hp = enemy_db[type].hp;
    entity[i].atk = enemy_db[type].atk;
    entity[i].speed = enemy_db[type].speed;
    entity[i].dodge_rate = enemy_db[type].dodge_rate;
    entity[i].crit_rate = enemy_db[type].crit_rate;
    entity[i].id = i;
    entity[i].is_alive = 1;

    printf("ID: %2d, Type: %-8s (HP: %2d, ATK: %2d, SPD: %2d)\n",
           i, entity[i].name, entity[i].hp, entity[i].atk, entity[i].speed);
}

// 處理攻擊時的過程
void execute_attack(Entity *entity1, Entity *entity2, int def_rate)
{
    if (!entity1->is_alive || !entity2->is_alive)
        return; // if one of the entity is not alive anymore, don't do anything

    int dmg = entity1->atk;
    dmg = (dmg * def_rate) / 100;
    int newhp = entity2->hp-dmg;
    if(newhp < 0) newhp = 0;
    printf("%s dealt %d damage to %s (HP: %d -> %d)\n", entity1->name, dmg, entity2->name, entity2->hp, newhp);
    entity2->hp = newhp;
    if(entity2->hp == 0) entity2->is_alive = 0;
}

// 骰防禦比例
int roll_defend()
{
    // TODO: if percent == 100, we need to give entity some awards, for example healing or attack bonus
    int percent = rand() % 100;
    if (percent >= 50)
        return 0; // 完美格檔 (受傷 0%)
    if (percent >= 25)
        return 50; // 成功格檔 (受傷 50%)
    if (percent >= 10)
        return 80; // 輕微格檔 (受傷 80%)

    return 100; // 格檔失敗 (受傷 100%)
}

// 進入戰鬥模式
void Battle_Mode(Entity *player, Entity *enemy)
{
    printf("\n\n\n\n\n╭∩╮( ͡⚆ ͜ʖ ͡⚆)╭∩╮    VS.    %s!\n", enemy->name);

    // TODO: 戰鬥的前置作業(例如: 計算雙方屬性加成、選四個物品進入戰鬥等等)
    // 如果player速度 >= 敵人速度，player先
    int player_first = player->speed >= enemy->speed ? 1 : 0;
    char nextAction = 0; // the upcoming action
    int backpack_index = -1;

    while (player->is_alive && enemy->is_alive){
        // 1. show current status
        printf("==============CURRENT STATUS=============\n");
        printf("PLAYER (%s):\n\tHP: %3d/%3d    ATK: %3d     SPD: %2d\n", player->name, player->hp, player->max_hp, player->atk, player->speed);
        printf("ENEMY (%s):\n\tHP: %3d/%3d    ATK: %3d     SPD: %2d\n", enemy->name, enemy->hp, enemy->max_hp, enemy->atk, enemy->speed);
        printf("==========================================\n");

        // 2. asking attack or defend or use item

        do{
            print_action_prompt();
            scanf(" %c", &nextAction);
        }while(is_valid_action_in_battle_mode(nextAction) == 0);

        // 3. calculate corresponding choices
        puts("");
        if(nextAction == 'A'){
            if(player_first){
                execute_attack(player, enemy, 100);
                execute_attack(enemy, player, 100);
            }
            else{
                execute_attack(enemy, player, 100);
                execute_attack(player, enemy, 100);
            }
        }
        else if(nextAction == 'D'){
            int def_rate = roll_defend();
            switch(def_rate){
            case 0:
                printf("perfect defend\n");
                break;
            case 50:
                printf("great defend\n");
                break;
            case 80:
                printf("nice defend\n");
                break;
            case 100:
                printf("defend unsuccessfully\n");
                break;
            }
            // 如果不是完美防禦，執行敵人的攻擊並扣除對應防禦比例
            if(def_rate != 0){
                if(def_rate != 100) printf("You defend enemy's attack by %d percent.\n", def_rate);
                execute_attack(enemy, player, def_rate);
            }
            else{
                // 如果骰到完美防禦，給予回復當前血量1.2倍的獎勵(20hp -> 24hp, 50hp -> 60hp)
                printf("You got no damage and healed 1.2* of your health. (HP: %d -> %d)\n", player->hp, (player->hp*1.2 > player->max_hp ? player->max_hp : (int)(player->hp*1.2)));
                player->hp = (player->hp*1.2 > player->max_hp ? player->max_hp : player->hp*1.2);
            }
        }
        else if(nextAction == 'I'){
            while(1){
                print_backpack();
                printf("Enter an item index to use (enter anything, -1 to exit): ");
                scanf("%d", &backpack_index);
                if (backpack_index < 0)
                {
                    printf("Leave your backpack.\n");
                    break;
                }
                else if (backpack_index >= backpack.item_count) printf("There are no items in this backpack slot.\n");
                else{
                    use_item(&backpack.items[backpack_index], backpack_index);
                    break;
                }
            }
        }
    }
    printf("==============Battle Finished=============\n");
    printf("PLAYER (%s):\n\tHP: %3d/%3d    ATK: %3d     SPD: %2d\n", player->name, player->hp, player->max_hp, player->atk, player->speed);
    printf("ENEMY (%s):\n\tHP: %3d/%3d    ATK: %3d     SPD: %2d\n", enemy->name, enemy->hp, enemy->max_hp, enemy->atk, enemy->speed);
    printf("==========================================\n\n\n\n\n\n");
}

int is_valid_action_in_battle_mode(char c){
    if(c == 'A' || c == 'D' || c == 'I') return 1;
    return 0;
}

// 進入商店模式
void Shop_Mode()
{
    // TODO: 隨機抽取四個物品供玩家選購、處理購買流程
    printf("Welcome to the shop!\n");
    Item shop_items[4];
    int selected_indices[4] = {-1, -1, -1, -1};

    /*  商店範例輸出:
        Welcome to the shop!
        =================== Shop ===================
        Gold: 100
        (0) Items_name: $10 | (物品功能介紹)
        (1) Items_name: $20 | (物品功能介紹)
        (2) Items_name: $30 | (物品功能介紹)
        (3) Items_name: $40 | (物品功能介紹)
    */
}

// 進入背包介面
void Backpack_Mode()
{
    // TODO: 處理使用物品流程
    /*
        1. 印出背包內容
        2. 讓玩家選擇要使用的物品 or 離開背包
        3. 判斷物品是否能在地圖模式使用
        4. 根據物品效果更新玩家狀態 or 穿上裝備
        5. 移除已使用的物品或更新數量
    */
    int backpack_index;
    while (1)
    {
        print_backpack();
        print_action_prompt();

        if (scanf("%d", &backpack_index) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // 清掉 buffer 裡的垃圾字元
            continue; // 回到 while(1) 重新來
        }

        if (backpack_index < 0)
        {
            printf("Leave your backpack.\n");
            return;
        }
        else if (backpack_index >= backpack.item_count)
        {
            printf("There are no items in this backpack slot.\n");
        }
        else if (backpack.items[backpack_index].can_be_used_in_map == 1)
        {
            use_item(&backpack.items[backpack_index], backpack_index);
        }
        else
        {
            printf("This item \"%s\" cannot be used in map mode.\n", backpack.items[backpack_index].name);
        }
    }
}

// 輸出背包物品
void print_backpack()
{
    printf("================== Backpack ==================\n");
    printf("Gold: %d\n", backpack.gold);
    printf("Armor_slots: [%s] [%s] [%s] [%s]\n",
           backpack.armor_slots[0].id == -1 ? " " : backpack.armor_slots[0].name,
           backpack.armor_slots[1].id == -1 ? " " : backpack.armor_slots[1].name,
           backpack.armor_slots[2].id == -1 ? " " : backpack.armor_slots[2].name,
           backpack.armor_slots[3].id == -1 ? " " : backpack.armor_slots[3].name);
    for (int i = 0; i < backpack.item_count; i++)
    {
        if (backpack.items[i].count > 0)
            printf("(%d) %s ×%d\n", i, backpack.items[i].name, backpack.items[i].count);
        else
            break;
    }
    printf("==============================================\n");

    /* 背包範例輸出:
    ----------------- Backpack -----------------
    Gold: 100
    Armor_slots: [] [] [] []
    (0) Items_name ×2
    (1) Items_name ×1
    (2) Items_name ×2
    (3) Items_name ×2
    (4) Items_name ×2
    (5) Items_name ×1
    (6) Items_name ×2
    (7) Items_name ×2
    (8) Items_name ×2
    (9) Items_name ×1
    (10) Items_name ×2
    (11) Items_name ×2
    (12) Items_name ×2
    (13) Items_name ×1
    (14) Items_name ×2
    (15) Items_name ×2
    */
}

// 整理背包
void sort_backpack()
{
    int n = backpack.item_count;
    for (int i = n / 2 - 1; i >= 0; --i)
    {
        heapify_item(backpack.items, n, i);
    }
    for (int i = n - 1; i >= 0; --i)
    {
        swap_items(0, i);
        heapify_item(backpack.items, i, 0);
    }
}

// 交換背包中的兩個物品
void swap_items(int a, int b)
{
    Item tmp = backpack.items[a];
    backpack.items[a] = backpack.items[b];
    backpack.items[b] = tmp;
}

// 減少(刪除)背包中的物品
void reduce_item(int index)
{
    /*
        Args: index: 要減少的物品在背包中的索引值
        功能: 物品數量-1，若數量為0則刪除該物品
    */
    backpack.items[index].count--;
    if (backpack.items[index].count < 1)
    {
        // 刪除該物品(往前移動覆蓋)
        for (int i = index; i < backpack.item_count - 1; i++)
        {
            backpack.items[i] = backpack.items[i + 1];
        }
        backpack.item_count--;
    }
}

// 使用物品
void use_item(Item *item, int item_backpack_index)
{
    /*
        # 兩種 Mode 下使用: Backpack、Battle
        Args:
            *item: 指向要使用的物品 (背包中 or 戰鬥中手持的物品)
        功能:
            根據物品效果更新玩家狀態 or 穿上裝備

    TODO:
        1. 判斷物品類型 (例如: Hp_Posion, Pw_Posion, Helmet, 等等)
        2. 根據物品效果更新玩家狀態 or 穿上裝備
        3. 移除已使用的物品或更新數量

    */

    // 武器(不減少數量)
    if (item->type == 'W')
    {
        printf("You use %s to attack.\n", item->name);
        switch (item->id)
        {
        case 0:                 // Sword
            temp_bonus[0] += 5; // attack +5
            break;
        case 1:                  // Axe
            temp_bonus[0] += 10; // attack +10
            temp_bonus[3] += 5;  // crit_rate +5
            break;
        case 2:                 // Bow
            temp_bonus[0] += 7; // attack +7
            break;
        default:
            break;
        }
        return;
    }

    // 防具(放到盔甲欄位)
    if (item->type == 'A')
    {
        printf("You put on %s.\n", item->name);
        Put_on_Armor(item, item_backpack_index);
        printf("Your current hp: %d.\n", entity[0].hp);
        return;
    }

    // 藥水(減少數量)
    if (item->type == 'P')
    {
        int used = 1;
        switch (item->id)
        {
        case 7: // Hp_Posion
            if(entity[0].hp == entity[0].max_hp){
                printf("You cannot use heal potion when full hp.\n");
                used = 0;
                break;
            }
            entity[0].hp += 30;
            if (entity[0].hp > entity[0].max_hp)
                entity[0].hp = entity[0].max_hp;
            printf("You used %s. HP +10.\n", item->name);
            printf("Current HP: %d/%d\n", entity[0].hp, entity[0].max_hp);
            break;
        case 8:                       // Pw_Posion
            entity[0].crit_rate += 5; // crit_rate +5
            printf("You used %s. CRI +5.\n", item->name);
            break;
        case 9:                  // Hm_Posion
            temp_bonus[4] += 20; // HARM +20
            printf("You used %s.\n", item->name);
            break;
        default:
            break;
        }
        if(used) reduce_item(item_backpack_index);
        return;
    }
}

// 穿上裝備
void Put_on_Armor(Item *armor, int item_backpack_index)
{
    /*
        Args: armor: 要穿上的裝備
        功能: 根據裝備類型放到對應的盔甲欄位 (若對應盔甲欄已有裝備，直接替換)
        (0 -> head, 1 -> chest, 2 -> legs, 3 -> boots)
    */

    int slot = armor->id - 3; // 將 id 調整為 0~3 方便 switch 判斷

    if (backpack.armor_slots[slot].count <= 0)
    {
        // 該欄位沒有裝備，直接放入
        backpack.armor_slots[slot] = *armor;
        reduce_item(item_backpack_index);
        return;
    }
    else
    {
        // 該欄位已有裝備，先卸下再放入
        Item old_armor = backpack.armor_slots[slot];
        backpack.armor_slots[slot] = *armor;
        // 將舊裝備放回背包
        backpack.items[item_backpack_index] = old_armor;
        return;
    }
}

// Max Heap, sort by id
void heapify_item(Item arr[], int size, int i)
{
    int l = i * 2 + 1; // left child, right child = l+1
    while (l < size)
    {
        int winner = (l + 1 < size) && (arr[l + 1].id > arr[l].id) ? l + 1 : l;
        winner = (arr[winner].id > arr[i].id) ? winner : i;
        if (winner == i)
            break;
        swap_items(i, winner);
        i = winner;
        l = i * 2 + 1;
    }
}

// 初始化地圖
void initialize_map(int enemies_count)
{
    /*
        Args:
            enemies_count: 敵人數量
        功能:
            map生成 空地('.')、牆('#')、boss('B')、player('P') ; 隨機生成 enemy('M')、shop('$')
    */

    // 生成空地'.' (1~38)
    for (int y = 1; y < MAP_HEIGHT - 1; y++)
        for (int x = 1; x < MAP_WIDTH - 1; x++)
            map[y][x] = '.';

    // 生成牆'#'
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        map[0][x] = '#';
        map[MAP_HEIGHT - 1][x] = '#';
    }
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        map[y][0] = '#';
        map[y][MAP_WIDTH - 1] = '#';
    }

    // 生成 boss(B) 、player(P)
    map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = 'P';
    map[1][1] = 'B';

    // 生成敵人'M'
    for (int i = 1; i <= enemies_count; i++) // 從 id=1 開始 (0 是玩家)
    {
        int rand_x, rand_y; // 存隨機出來的數值
        int attempts = 0;   // 防止無限迴圈的保險機制

        do
        {
            // 隨機生成座標 (1 ~ 寬度-2)，避開牆壁
            rand_x = rand() % (MAP_WIDTH - 2) + 1;
            rand_y = rand() % (MAP_HEIGHT - 2) + 1;
            attempts++;

            // 如果骰太多次都沒位置，就強制跳出
            if (attempts > 1000)
                break;

        } while (map[rand_y][rand_x] != '.'); // 如果地圖上這一格不是空地(可能是牆、B、P、或已經生成的M)，就重骰

        map[rand_y][rand_x] = 'M';

        // 設定 entity 的資料
        entity[i].pos_x = rand_x;
        entity[i].pos_y = rand_y;
    }

    // 生成商店'$'
    int shop_x, shop_y;
    do
    {
        shop_x = rand() % (MAP_WIDTH - 2) + 1;
        shop_y = rand() % (MAP_HEIGHT - 2) + 1;
    } while (map[shop_y][shop_x] != '.'); // 確保不重疊

    map[shop_y][shop_x] = '$';
}

// 玩家在地圖模式的行動
void action_map(char nextAction, Entity *player)
{
    /*
        Args:
            nextAction: user輸入的下一個動作
            *player: 指向玩家的指標
        功能:
            根據玩家輸入的動作，更新玩家在地圖上的位置
    */
    switch (nextAction)
    {
    case 'W':
        if (player->pos_y > 1) // 防止超出上邊界
            player->pos_y -= 1;
        break;
    case 'A':
        if (player->pos_x > 1) // 防止超出左邊界
            player->pos_x -= 1;
        break;
    case 'S':
        if (player->pos_y < MAP_HEIGHT - 2) // 防止超出下邊界
            player->pos_y += 1;
        break;
    case 'D':
        if (player->pos_x < MAP_WIDTH - 2) // 防止超出右邊界
            player->pos_x += 1;
        break;
    case 'E':
        GameMode = BACKPACK;
        Backpack_Mode(); // 進入背包模式
        break;
    default:
        break;
    }
}

// 更新+印出地圖
void refresh_map()
{
    // Reset the inner map with '.', keep the walls
    for (int y = 1; y < MAP_HEIGHT - 1; y++)
    {
        for (int x = 1; x < MAP_WIDTH - 1; x++)
        {
            if (map[y][x] == '$' || map[y][x] == 'B' || map[y][x] == 'M')
                continue; // keep the wall
            map[y][x] = '.';
        }
    }
    map[entity[0].pos_y][entity[0].pos_x] = 'P'; // player

    print_map();
}

// ---------------------------------------------------------------------------------------------------------------------------

/*
        /  )))
    ／ イ  　　　(((ヽ
    (　 ﾉ　　　　 ￣Ｙ＼
    |　(＼　(\  /) ｜ )
    ヽ　ヽ` ( ﾟ∀ﾟ)_ノ /
    　＼ |　⌒Ｙ⌒　/  /
    　 ｜ヽ　 ｜　 ﾉ ／
    　  ＼トー仝ーイ
    　　 ｜ ミ土彡/
          )\ °  //
         (  \ / \. \
         /  /  \ \  \
       / / /     \ \  \
      ((  ).       ) ). )
     (    ).       ( |  |
      |   /        \   |
   nnn.  ).         (  .nnn
未來考慮加入：武器系統、裝備系統 );//consider set a flag at refresh_map if change happen flag =1 之類的

背包：包含金幣、背包系統(例如不同效果的potion、不同數值的武器)
背包系統:總共16格，進入戰鬥選擇四個手持物品。
遊戲通關條件：Final Boss

map set:地圖initialize 40*20(包含邊界)
        Boss固定出生右下角
        玩家生成左上角
        商店隨機生成
        系統random分配enemies' x,y;

地圖模式：每個行動後顯示地圖與可執行動作(移動:上下左右、使用道具:可以在地圖使用的物品->列出來->再選擇)

商店模式：顯示四個可購買物品
TODO: shop_db物品種類要有哪些、地圖會有幾個商店、價格...

戰鬥模式：
if(玩家位置==怪物位置）進入戰鬥狀態：
{
    while(兩個Entity都還活著）
    {
        每回合更新雙方condition(例如hp)

        玩家可以選擇要進行：攻擊、防禦、使用道具
        敵人會攻擊、防禦 (依照狀態決定random範圍，這個之後再寫，會有點麻煩)

        if(速度快的先行動、速度一樣就隨機）
        {
            處理速度快者的行動
            處理速度慢者的行動

            ** 如果快和慢的都選擇attack，且快的把慢的打死了，那慢的就不會行動(skip)
        }
    }
}

祝大家活著，我說作者
*/