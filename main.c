#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

int main(int argc, char **argv)
{
    TList rooms = NULL;
    FILE *in, *out, *conf;
    char * operation;

    int nr_tables, i, j, max_room, current_room;

    conf = fopen(argv[1], "r");
    in = fopen(argv[2], "r");
    out = fopen(argv[3], "w+");

    if (!in) {
        printf("Error at in!\n");
        return -1;
    }

    if (!conf) {
        printf("Error at conf!\n");
        return -1;  
    }

    if (!out) {
        printf("Error at out!\n");
        return -1;
    }

    operation = (char *)malloc(sizeof(char) * 50);

    if (!operation) {
        printf("The operation wasn't allocated!\n");
        return -1;
    }

    fscanf(conf, "%d", &nr_tables);

    TList tables = NULL;
    max_room = 0;
    current_room = 0;

    for(i = 0;i < nr_tables; i++) {
        int nr_of_players, table_size;
        char *table_name = (char *)malloc(sizeof(char) * 30);

        fscanf(conf, "%s%d%d", table_name, &nr_of_players, &table_size);

        max_room += table_size;
        current_room += nr_of_players;

        TList players = NULL;

        /* create the sentinel */
        struct PCell sentinel = create_player("sentinel", -1);

        /* add the sentinel to the list */
        players = create_cell(players, &sentinel, sizeof(struct PCell));

        for(j = 0;j < nr_of_players; j++) {
            int rounds;
            char *player_name = (char *)malloc(sizeof(char) * 30);

            fscanf(conf, "%s%d", player_name, &rounds);

            /* create a new player and add him to the list */
            struct PCell new_player = create_player(player_name, rounds);

            players = create_cell(players, &new_player, sizeof(struct PCell));

            if (!players) {
                printf("Error to add a player in the list!\n");
                return -1;
            }

            free(player_name);
        }

        /* increase the number by adding the sentinel */
        nr_of_players++;

        /* make the players' list circular */
        TList crossing_players = NULL;
        TList circ = NULL;

        circ = players;
        crossing_players = players;

        while(crossing_players->next)
            crossing_players = crossing_players->next;

        crossing_players->next = circ;

        /* create a new table and add him to the list */
        struct TCell new_table = create_table(players, table_name, nr_of_players, table_size);

        tables = create_cell(tables, &new_table, sizeof(struct TCell));

        if (!tables) {
            printf("Error to add a table  in the list!\n");
            return -1;
        }

        free(table_name);
    }

    struct RCell new_room = create_room(tables, nr_tables, current_room, max_room);

    rooms = create_cell(rooms, &new_room, sizeof(struct RCell));

    if (!rooms) {
        printf("Error to add a room in the list!\n");
        return -1;
    }

    while(!feof(in)) {

        fscanf(in, "%s", operation);

        if (strcmp(operation, "print") == 0) {
            print_room(out, rooms);
        } else if(strcmp(operation, "noroc") == 0) {
            char *table_name = (char *)malloc(sizeof(char) * 30);
            char *player_name = (char *)malloc(sizeof(char) * 30);
            int luck;

            if (!table_name || !player_name) {
                printf("Error to allocate memory for a table name and player name!\n");
                return -1;
            }

            fscanf(in, "%s%s%d", table_name, player_name, &luck);

            rooms = good_luck(out, rooms, table_name, player_name, luck ,LUCKY);

            free(table_name);
            free(player_name);
        } else if (strcmp(operation, "ghinion") == 0) {
            char *table_name = (char *)malloc(sizeof(char) * 30);
            char *player_name = (char *)malloc(sizeof(char) * 30);
            int luck;

            if (!table_name || !player_name) {
                printf("Error to allocate memory for a table name and player name!\n");
                return -1;
            }

            fscanf(in, "%s%s%d", table_name, player_name, &luck);

            rooms = good_luck(out, rooms, table_name, player_name, luck ,UNLUCKY);

            free(table_name);
            free(player_name);
        } else if (strcmp(operation, "tura") == 0) {
            char *table_name = (char *)malloc(sizeof(char) * 30);

            if (!table_name) {
                printf("Error to allocate memory for a table name!\n");
                return -1;
            }

            fscanf(in, "%s", table_name);

            if (strcmp(table_name, "completa") == 0)
                rooms = complete_turn(out, rooms);
            else
                rooms = turn(out, rooms, table_name);
            

            free(table_name);
        } else if (strcmp(operation, "clasament") == 0) {
            char *table_name = (char *)malloc(sizeof(char) * 30);

            if (!table_name) {
                printf("Error to allocate memory for a table name!\n");
                return -1;
            }

            fscanf(in, "%s", table_name);

            rooms = ranking(out, rooms, table_name);

            free(table_name);
        } else if (strcmp(operation, "inchide") == 0) {
            char *table_name = (char *)malloc(sizeof(char) * 30);

            if (!table_name) {
                printf("Error to allocate memory for a table name!\n");
                return -1;
            }

            fscanf(in, "%s", table_name);

            rooms = close_table(out, rooms, table_name);

            free(table_name);
        }
    }

    free_room(&rooms);
    free(operation);

    fclose(in);
    fclose(out);
    fclose(conf);
    
    return 0;
}