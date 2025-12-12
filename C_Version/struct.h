#ifndef STRUCT_H
#define STRUCT_H

#define MAX_ENTITIES 8
#define MAX_ITEMS 16

typedef struct{
    /*
        @Explanation 各資料解釋
        id: 0: player, 1~N: enemy
        name: entity's name
        hp: entity's current hp
        max_hp: entity's max hp
        atk: entity's attack damage
        speed: use to differentiate the attack sequence
        dodge_rate: entities have chance to dodge attack
        crit_rate: entities have chance to crit
        is_alive: 0: dead, 1: alive
    */
    int id;
    char name[24];
    int hp;
    int max_hp;
    int atk;
    int speed;
    int dodge_rate;
    int crit_rate;
    int is_alive;
    int pos_x;
    int pos_y;
} Entity; // Entity是實體，直接包含玩家(player)與敵人(enemy)

typedef struct{
    char name[32];
    int hp;
    int atk;
    int speed;
    int dodge_rate;
    int crit_rate;
} EnemyTemplate;

typedef struct{
    /*
        @Explanation
        id: item id
        name: item name
        value: the cost of item
        count: how many items player have
    */
    int id;
    char name[20];
    int value;
    int count;
} Item;

typedef struct{
    /*
        @Explanation
        items: put all items in one array
        item_count: the total amount of items
        gold: current gold, can be use to buy potions or maybe skip stage?
    */
    Item items[MAX_ITEMS];
    int item_count;
    int gold;
    // TODO: add more usages for gold
} Backpack; // player's backpack

void setupEnemy(int i); // 設定敵人屬性

void execute_attack(Entity* entity1, Entity* entity2); // 處理攻擊時的過程

int roll_defend(); // 骰防禦比例

// backpack heap sort
void sort_backpack();
void swap_items(int a, int b);
void heapify_item(Item arr[], int size, int i);

#endif