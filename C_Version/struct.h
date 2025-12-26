#ifndef STRUCT_H
#define STRUCT_H

#define MAX_ENTITIES 16
#define MAX_ITEMS 16

#define MAP_WIDTH 40
#define MAP_HEIGHT 20

typedef struct
{
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
        is_alive: 0->dead, 1->alive
    */
    char name[24];
    int id;
    int hp;
    int max_hp;
    int atk;
    int speed;
    int dodge_rate;
    int crit_rate;
    int damage_reduction;
    int is_alive;
    int pos_x;
    int pos_y;
    char type; // 'P'->player, 'E'->enemy, 'B'->boss , '$'->shop
} Entity;      // Entity是實體，直接包含玩家(player)與敵人(enemy)

typedef struct
{
    /*
        @Explanation
        id: item id
        name: item name
        cost: the cost of item
        count: how many items player have
        can_be_used_in_map: 0->no, 1->yes
    */
    int id;
    char name[16];
    int cost;
    int count;
    int can_be_used_in_map;
    char type; // 'W'->weapon, 'A'->armor, 'P'->posion
} Item;

typedef struct
{
    /*
        @Explanation
        items: put all items in one array
        item_count: the total amount of items
        gold: current gold, can be use to buy potions or maybe skip stage?
        armor_slots: 0 -> head, 1 -> chest, 2 -> legs, 3 -> boots
    */
    Item items[MAX_ITEMS];
    int item_count;
    int gold;
    Item armor_slots[4];
    // TODO: add more usages for gold
} Backpack; // player's backpack



void initialize_map(int enemies_count); // 初始化地圖
void initialize_Player();               // 初始化玩家屬性
void initialize_Backpack();             // 初始化背包
void setupEnemy(int i);                 // 設定敵人屬性

void print_map();                 // 印出地圖
void refresh_map();               // 更新+印出地圖
void print_action_prompt();       // 印出玩家行動提示
void print_backpack();            // 輸出所有背包物品
void print_shop_items(Item shop_items[], int selected_indices[]);

void action_in_map(char nextAction, Entity *player);         // 玩家在地圖模式的行動
void execute_attack(Entity *entity1, Entity *entity2, int def_rate); // 處理攻擊時的過程
int is_valid_action_in_battle_mode(char c); // 判斷戰鬥模式的輸入是否合法
int is_critical_hit(int crit_rate); // 判斷是否爆擊
int roll_defend(); // 骰防禦比例
int get_gold(); // 隨機獲得gold

void use_item(Item *item, int item_backpack_index); // 使用物品
void Put_on_Armor(Item *armor, int item_backpack_index); // 穿上裝備 

// backpack heap sort
void sort_backpack();
void swap_items(int a, int b);
void heapify_item(Item arr[], int size, int i);

void Battle_Mode(Entity *player, Entity *enemy); // 進入對戰模式
void Shop_Mode();                                // 進入商店模式
void Backpack_Mode();                            // 進入背包模式
#endif