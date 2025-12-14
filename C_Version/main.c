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
    // name, id, hp, maxhp, atk, speed, dodge_rate, crit_rate, is_alive, pos_x, pos_y
    {"", -1, -1, -1, -1, -1, -1, -1, -1, 1, -1},     // type 0 是 placeholder, 不應該存取到這
    {"Slime", -1, 20, 20, 3, 4, 0, 0, 1, -1, -1},    // type 1 是 Slime
    {"Skeleton", -1, 15, 15, 8, 7, 5, 5, 1, -1, -1}, // type 2 是 Skeleton
    {"Zombie", -1, 40, 40, 5, 2, 1, 1, 1, -1, -1},   // type 3 是 Zombie
    {"Goblin", -1, 30, 30, 6, 6, 3, 3, 1, -1, -1},   // type 4 是 Goblin
    {"Ghost", -1, 5, 5, 15, 10, 30, 10, 1, -1, -1}   // type 5 是 Ghost
};

const Item weapon_db[] = {
    //  id, name, cost, count, can_be_used_in_map
    {0, "Sword", 5, 1, 0},
    {1, "Axe", 10, 1, 0},
    {2, "Bow", 15, 1, 0},
};

const Item armor_db[] = {
    //  id, name, cost, count, can_be_used_in_map
    {0, "Helmet", 13, 1, 1},     // 頭盔: map模式可穿上
    {1, "Chestplace", 20, 1, 1}, // 胸甲: map模式可穿上
    {2, "Leggings", 13, 1, 1},   // 護腿: map模式可穿上
    {3, "Boost", 8, 1, 1},       // 靴子: map模式可穿上
};

const Item posion_db[] = {
    //  id, name, cost, count, can_be_used_in_map
    {0, "Hp_Posion", 10, 1, 1}, // 生命(health point)藥水: map模式可喝
    {1, "Pw_Posion", 10, 1, 1}, // 力量(power)藥水: map模式可喝
    {2, "Hm_Posion", 15, 1, 0}, // 傷害(harm)藥水
};

// 彙整所有物品的指標陣列，方便隨機抽取 (可以考慮放在同一個陣列，只是先列出)
const Item *item_random_arr[] = {
    &weapon_db[0], &weapon_db[1], &weapon_db[2],
    &armor_db[0], &armor_db[1], &armor_db[2], &armor_db[3],
    &posion_db[0], &posion_db[1], &posion_db[2]};
const int ITEM_RANDOM_COUNT = sizeof(item_random_arr) / sizeof(item_random_arr[0]);

// ---------------------------------------------------------------------------------------------------------------------------

/*
every round

1. we need to print
==============CURRENT STATUS==============
PLAYER:
    HP: xxx/max    ATK: xxx     SPD:xxx

ENEMY:
    HP: xxx/max    ATK: xxx     SPD:xxx
==========================================

2. asking attack or defend or use item

3. calculate corresponding choices

4. check entity status
*/

// 最多 玩家 + 5個敵人 = 6個實體，我開[8]的陣列確保不會出問題
Entity entity[MAX_ENTITIES];

// player's backpack
Backpack backpack;

// map
char map[MAP_HEIGHT][MAP_WIDTH];

int main()
{
    srand((unsigned)time(NULL)); // 用當前時間初始化隨機數(確保一定程度的偽隨機)

    int enemy_count = rand() % 6 + 10;  // randomly encounter 10~15 enemies, the player will fight them one by one
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
            while ((c = getchar()) != '\n' && c != EOF)
                ; // clear buffer
            // we don't need to manually replace the last character to '\n', it's automaticly done by fgets
        }
    }

    printf("Welcome, %s\n", entity[0].name);

    entity[0].id = 0;
    entity[0].hp = entity[0].max_hp = 100;
    entity[0].atk = 20;
    entity[0].speed = 5;
    entity[0].dodge_rate = 10;
    entity[0].crit_rate = 5;
    entity[0].is_alive = 1;

    printf("==========================================\n");
    printf("Enemy list (10~15) for this game :\n");
    for (int i = 1; i <= enemy_count; ++i)
    {
        setupEnemy(i);
    }
    printf("==========================================\n");

    /*
    以下code是為了測試player backpack與heap sort能夠正常使用，可以先不管
    */
    backpack.item_count = 5;
    backpack.items[0] = (Item){5, "Potion", 10, 1, 1};
    backpack.items[1] = (Item){1, "Sword", 100, 1, 0};
    backpack.items[2] = (Item){3, "Key", 50, 1, 0};
    backpack.items[3] = (Item){4, "Shield", 80, 1, 0};
    backpack.items[4] = (Item){2, "Map", 5, 1, 1};
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
    以下code是為了測試map能夠正常使用，可以先不管
    */
    initialize_map(enemy_count);
    for (int i = 0; i < MAP_HEIGHT; ++i)
    {
        for (int j = 0; j < MAP_WIDTH; ++j)
        {
            printf("%c", map[i][j]);
        }
        puts("");
    }

    return 0;
}

// 設定敵人屬性
void setupEnemy(int i)
{
    int type = rand() % 5 + 1; // type依照 敵人清單.txt

    // status from template
    strncpy(entity[i].name, enemy_db[type].name, sizeof(entity[i].name) - 1); // 存到destination的容量-1
    entity[i].name[sizeof(entity[i].name) - 1] = '\0';                        // 讓destination的最後一個位元變成 '\0'，保證字串安全

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
void execute_attack(Entity *entity1, Entity *entity2)
{
    if (!entity1->is_alive || !entity2->is_alive)
        return; // if one of the entity is not alive anymore, don't do anything

    printf("%s attacked %s!\n", entity1->name, entity2->name);

    int dmg = entity1->atk;
    /* TODO: before calling attack function and calculations, we need to ask player to defend or attack, and add randomized choice for enemies too
        for example:
        e1 attack, e2 defend -> e2 roll dice to check how many % to defend, then receive the damage from e1
        e1 defend, e2 attack -> reverse from the previous
        e1 attack, e2 attack -> the one with higher speed will attack first, then check status. if one is dead then return
        e1 defend, e2 defend -> both roll the dice and check if there are awards
    */
}

/*
if entity called defend, we roll the dice:
    if the dice is between 50~100, defend successfully
    between 25~50, defend half of the damage
    between 10~25, defend 20% of the damage
    below 10, defend unsuccessful

the function call will be like:
    int def = defend();
    attack_damage = (attack_damage * def) / 100;
    switch(def){
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
*/

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
    return 100;    // 格檔失敗 (受傷 100%)
}

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

void swap_items(int a, int b)
{
    Item tmp = backpack.items[a];
    backpack.items[a] = backpack.items[b];
    backpack.items[b] = tmp;
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
        1.map初始化(空地:'.'、牆:'#')
        2.生成 boss(顯示:'B')、player(顯示:'P')
        3.生成 enemy、shop: (random (x,y)) if (x,y)不是空地 then 重新random或取下一個值
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
    entity[0].pos_x = MAP_HEIGHT - 2;
    entity[0].pos_y = MAP_WIDTH - 2;
    map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = 'P';
    map[1][1] = 'B';

    // 生成敵人'M'
    // 從 id=1 開始 (0 是玩家)
    for (int i = 1; i <= enemies_count; i++) 
    {
        int rand_x, rand_y; // 存隨機出來的數值
        int attempts = 0; // 防止無限迴圈的保險機制

        do {
            // 隨機生成座標 (1 ~ 寬度-2)，避開牆壁
            rand_x = rand() % (MAP_WIDTH - 2) + 1;
            rand_y = rand() % (MAP_HEIGHT - 2) + 1;
            attempts++;
            
            // 如果骰太多次都沒位置，就強制跳出
            if(attempts > 1000) break;

        } while (map[rand_y][rand_x] != '.'); // 如果地圖上這一格不是空地(可能是牆、B、P、或已經生成的M)，就重骰

        map[rand_y][rand_x] = 'M';
        
        // 設定 entity 的資料
        entity[i].pos_x = rand_x;
        entity[i].pos_y = rand_y;
    }

    // 生成商店'$'
    int shop_x, shop_y;
    do {
        shop_x = rand() % (MAP_WIDTH - 2) + 1;
        shop_y = rand() % (MAP_HEIGHT - 2) + 1;
    } while (map[shop_y][shop_x] != '.'); // 確保不重疊

    map[shop_y][shop_x] = '$';
}

/*
未來考慮加入：武器系統、裝備系統

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