#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

TList create_cell(TList l, void *element, size_t size)
{
    TList res = (TList)malloc(sizeof(struct celulag));

    if (!res) {
        printf("Error to create a cell for list!\n");
        return NULL;
    }

    res->next = NULL;
    res->info = malloc(size);

    if (!res->info) {
        printf("Error to alloc memory for info!\n");
        free(res);
        return NULL;
    }

    memcpy(res->info, element, size);

    if (l == NULL) {
        l = res;

    } else {
        TList park = NULL;
        park = l;

        while(park->next)
            park = park->next;

        park->next = res;
    }

    return l;
}

struct RCell create_room(TList tables, int nr_tables, int curr_places, int max_places)
{
    struct RCell room;
    room.nr_tables = nr_tables;
    room.curr_places = curr_places;
    room.max_places = max_places;
    room.table = tables;
    return room;
}

struct TCell create_table(TList players, char *name, int curr_players, int max_players)
{
    struct TCell table;
    table.players = players;
    table.max_players = max_players;
    table.curr_players = curr_players;
    table.name_table = (char *)malloc(sizeof(char) * (strlen(name) + 1));

    if (!table.name_table) {
        printf("Error to alloc he table_name!\n");
    }

    strcpy(table.name_table, name);

    return table;
}

struct PCell create_player(char *name, int rounds)
{
    struct PCell player;
    player.rounds = rounds;

    player.name = (char *)malloc(sizeof(char) * (strlen(name) + 1));

    strcpy(player.name, name);

    return player;
}

void free_player(TList *l)
{
    TList tmp = NULL;
    tmp = (*l);

    while(strcmp(((Player)tmp->next->info)->name, "sentinel") != 0) 
        tmp = tmp->next;

    tmp->next = NULL;

    while((*l)) {
        tmp = (*l);
        free((*(Player)(*l)->info).name);
        free((*l)->info);

        (*l) = tmp->next;
        free(tmp);
    }

    (*l) = NULL;
}

void free_table(TList *l)
{
    TList tmp;

    while((*l)) {
        tmp = (*l);
        free_player(&((Table)(*l)->info)->players);
        free((*(Table)(*l)->info).name_table);
        free((*l)->info);

        (*l) = tmp->next;
        free(tmp);
    }

    (*l) = NULL;
}

void free_room(TList *l)
{
    TList tmp;

    while((*l)) {
        tmp = (*l);
        free_table(&((Room)(*l)->info)->table);
        free((*l)->info);

        (*l) = tmp->next;
        free(tmp);
    }

    (*l) = NULL;
}

void free_list(TList *l)
{
    TList tmp;

    while((*l)) {
        free((*l)->info);
        tmp = (*l);
        (*l) = tmp->next;
        free(tmp);
    }

    (*l) = NULL;
}