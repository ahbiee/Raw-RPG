#ifndef STRUCT_H
#define STRUCT_H

#define MAX_ENTITIES 8

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
    char name[32];
    int hp;
    int max_hp;
    int atk;
    int speed;
    int dodge_rate;
    int crit_rate;
    int is_alive;
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
        heal_potion: can heal a specific amount (or percentage, TBD) of hp
        gold: current gold, can be use to buy potions or maybe skip stage?
    */
    int heal_potion_lvl1;
    int heal_potion_lvl2;
    int heal_potion_lvl3;
    // TODO: the higher the level is, the more health the player is healed
    // maybe 25%, 50%, 100% of max health?
    int gold;
    // TODO: add more usages for gold
} Backpack; // player's backpack

void setupEnemy(int i); // 設定敵人屬性

void execute_attack(Entity* entity1, Entity* entity2); // 處理攻擊時的過程

int roll_defend(); // 骰防禦比例

#endif