#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_ENTITIES 10

typedef struct{
    /*
        @Explanation
        id: 0: player, 1~N: enemy
        name: entity's name
        hp: entity's current hp
        max_hp: entity's max hp
        atk: entity's attack damage
        speed: use to differentiate the attack sequence
        is_alive: 0: dead, 1: alive
    */
    int id;
    char name[30];
    int hp;
    int max_hp;
    int atk;
    int speed;
    int is_alive;
} Entity;

typedef struct{
    /*
        @Explanation
        heal_potion: can heal a specific amount of hp
        gold: current gold, can be use to buy potions or maybe skip stage?
    */
    int heal_potion;
    // TODO: add different levels of potion
    int gold;
    // TODO: add more usages for gold
} Backpack; // player's backpack

void execute_attack(Entity* entity1, Entity* entity2){
    if(!entity1->is_alive || !entity2->is_alive) return; // if one of the entity is not alive anymore, don't do anything

    printf("%s �����F %s!\n", entity1->name, entity2->name);

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
        case 0.0:
            printf("perfect defend\n");
            break;
        case 0.5:
            printf("great defend\n");
            break;
        case 0.2:
            printf("nice defend\n");
            break;
        case 0.0:
            printf("defend unsuccessfully\n");
            break;
    }
*/
int defend(){
    // TODO: if percent == 100, we need to give entity some awards, for example healing
    int percent = rand() % 100;
    if(percent >= 50) return 0;
    if(percent >= 25) return 50;
    if(percent >= 10) return 80;
    return 100;
}

/*
every round

1. we need to print
===========CURRENT STATUS================
PLAYER:
    HP: xxx/max    ATK: xxx     SPD:xxx

ENEMY:
    HP: xxx/max    ATK: xxx     SPD:xxx
=========================================

2. asking attack or defend or use item

3. calculate corresponding choices

4. check entity status
*/

int main()
{
    srand((unsigned)time(NULL));
    Entity entity[MAX_ENTITIES];
    int enemy_count = rand()%3 + 1; // 1~3 enemies
    int total_count = enemy_count + 1; // entity[0] is player
    Backpack backpack; // player's backpack

    printf("�o�OCLI RPG�^�X���ԹC���I\n");
    printf("�п�J�A������W�١G");
    fgets(entity[0].name, sizeof(entity[0].name), stdin);
    entity[0].name[strcspn(entity[0].name, "\n")] = 0;
    entity[0].id = 0;
    entity[0].atk = -1; // set later
    entity[0].max_hp = -1; // set later
    entity[0].hp = entity[0].max_hp;
    entity[0].speed = -1; // set later
    entity[0].is_alive = -1; // set later

    return 0;
}
