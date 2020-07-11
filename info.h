#include <stdio.h>
#include <stdlib.h>
#define LUCKY 1
#define UNLUCKY 0

typedef struct celulag
{
	void *info;
	struct celulag *next;
} Cell, *TList;

typedef struct PCell
{
	char *name;
	int rounds;
} *Player;

typedef struct TCell
{
	char *name_table;
	TList players;
	int curr_players;
	int max_players;
} *Table;

typedef struct RCell
{
	TList table;
	int nr_tables;
	int curr_places;
	int max_places;
} *Room;

/* alloc and free functions */
TList create_cell(TList l, void *element, size_t size);
struct RCell create_room(TList tables, int nr_tables, int curr_places, int max_places);
struct TCell create_table(TList players, char *name, int curr_players, int max_players);
struct PCell create_player(char *name, int rounds);
void free_player(TList *l);
void free_table(TList *l);
void free_room(TList *l);
void free_list(TList *l);

/* events */
void print_room(FILE *out, TList rooms);
TList good_luck(FILE *out, TList rooms, char *table_name, char *player_name, int luck, int mode);
TList turn(FILE *out, TList rooms, char *table_name);
TList complete_turn(FILE *out, TList rooms);
void print_list(TList list);
TList order_insert(TList list, void *element, size_t size, int nr_of_tables);
TList ranking(FILE *out, TList rooms, char *table_name);
TList close_table(FILE *out, TList rooms, char *table_name);