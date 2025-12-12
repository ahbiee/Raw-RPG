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

const EnemyTemplate enemy_db[] = {
    {"", -1, -1, -1, -1, -1},     // type 0 是 placeholder, 不應該存取到這
    {"Slime", 20, 3, 4, 0, 0},    // type 1 是 Slime
    {"Skeleton", 15, 8, 7, 5, 5}, // type 2 是 Skeleton
    {"Zombie", 40, 5, 2, 1, 1},   // type 3 是 Zombie
    {"Goblin", 30, 6, 6, 3, 3},   // type 4 是 Goblin
    {"Ghost", 5, 15, 10, 30, 10}  // type 5 是 Ghost
};

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

int main()
{
    srand((unsigned)time(NULL)); // 用當前時間初始化隨機數(確保一定程度的偽隨機)

    int enemy_count = rand() % 3 + 3;  // randomly encounter 3~5 enemies, the player will fight them one by one
    int total_count = enemy_count + 1; // entity[0] is player

    printf("==========================================\n");
    printf("Welcome to Raw RPG\n");
    printf("Enter your name (accept spaces, limit 31 characters): ");
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

    printf("Welcome, %s\n", entity[0].name); // for test purpose

    entity[0].id = 0;
    entity[0].hp = entity[0].max_hp = 100;
    entity[0].atk = 20;
    entity[0].speed = 5;
    entity[0].dodge_rate = 10;
    entity[0].crit_rate = 5;
    entity[0].is_alive = 1;

    printf("==========================================\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("........................................\n");
    printf("Enemy list (3~5) for this game :\n");
    for (int i = 1; i <= enemy_count; ++i)
    {
        setupEnemy(i);
    }
    printf("==========================================\n");

    /*
    以下code是為了測試player backpack與heap sort能夠正常使用，可以先不管
    */
    backpack.item_count = 5;
    backpack.items[0] = (Item){5, "Potion", 10, 1};
    backpack.items[1] = (Item){1, "Sword", 100, 1};
    backpack.items[2] = (Item){3, "Key", 50, 1};
    backpack.items[3] = (Item){4, "Shield", 80, 1};
    backpack.items[4] = (Item){2, "Map", 5, 1};
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

    return 0;
}

// 設定敵人屬性
void setupEnemy(int i)
{
    int type = rand() % 5 + 1; // type依照 敵人清單.txt，敵人強度依照清單的順序

    // status from template
    strncpy(entity[i].name, enemy_db[type].name, sizeof(entity[i].name) - 1); // 存到destination的容量-1
    entity[i].name[sizeof(entity[i].name) - 1] = '\0';                        // 讓destination的最後一個位元變成 '\0'，保證字串安全

    entity[i].max_hp = enemy_db[type].hp;
    entity[i].atk = enemy_db[type].atk;
    entity[i].speed = enemy_db[type].speed;
    entity[i].dodge_rate = enemy_db[type].dodge_rate;
    entity[i].crit_rate = enemy_db[type].crit_rate;

    // status for common
    entity[i].id = i;
    entity[i].hp = entity[i].max_hp;
    entity[i].is_alive = 1;

    printf("ID: %d, Type: %-10s (HP: %d, ATK: %d, SPD: %d)\n", i, entity[i].name, entity[i].hp, entity[i].atk, entity[i].speed);
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

/*
代辦：確保大家都8964、shop 物品種類設置

考慮：武器系統

背包：金幣、不同功能藥水、
背包系統:總共16格，進入戰鬥選擇四個手持物品。
裝備系統：
終止條件：baby boss

輸入名稱


map set:地圖initialize 40*20（邊界另外設定）
        Boss固定出生右下角
        玩家生成左上角
        黑皮商店生成(maybe random)
        系統random分配enemies' x,y;
顯示:map
顯示：action
地圖模式：玩家選擇動作:移動：上下左右
                    吃藥水：可以在地圖使用的物品->列出來->再選擇;
黑皮商店模式：顯示四個物品

ㄉㄩ丟鬼模式：if(玩家位置==怪物位置）
            進入戰鬥模式（）
            {
            while(除非有人死掉）
            {每回合更新雙方condition
                if(速度快的先攻擊、速度一樣Random選擇）
                {
                攻擊回合()顯示 action
                防禦回合（）顯示action

                }
            }


祝大家活著，我說作者
*/