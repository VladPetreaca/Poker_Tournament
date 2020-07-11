#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

/* print the tables and the players from a room */
void print_room(FILE *out, TList rooms)
{
	TList park = NULL;
	park = rooms;

	/* modify to english */
	if (!park) {	
		fprintf(out, "Sala este inchisa!\n");
	}

	while(park) {
		
		struct RCell room = *(Room)park->info;
		
		while(room.table) {
			struct TCell table = *(Table)room.table->info;

			fprintf(out, "%s: ", table.name_table);

			if (table.curr_players <= 1) {
				fprintf(out, "\n");
			} else {
				TList crossing = table.players;
				for(crossing = crossing->next; 
				strcmp((*(Player)crossing->info).name, "sentinel") != 0; 
				crossing = crossing->next)
				{
					Player pl = (Player)crossing->info;
					if(strcmp((*(Player)crossing->next->info).name, "sentinel") != 0)
						fprintf(out, "%s - %d; ", (*pl).name, (*pl).rounds);
					else
						fprintf(out, "%s - %d.\n", (*pl).name,(*pl).rounds);
				}
			}

			room.table = room.table->next;
		}

		park = park->next;
	}
}

/* increase or decrease the number of rounds of a player */
TList good_luck(FILE *out, TList rooms, char *table_name, char *player_name, int luck, int mode)
{
	TList crossing_rooms = NULL;
	crossing_rooms = rooms;
	int check = 0;

	while(crossing_rooms) {
		TList crossing_tables = NULL;
		Room room = ((Room)crossing_rooms->info);

		crossing_tables = ((Room)(crossing_rooms->info))->table;
		check = 0;

		while(crossing_tables) {
			Table table = ((Table)crossing_tables->info);

			/* search the table */
			if (strcmp(table->name_table, table_name) != 0) {
				check++;

				/* the table doesn't exist */
				if (check == room->nr_tables) {
					fprintf(out, "Masa %s nu exista!\n", table_name);
					return rooms;
				}
				
			} else {
				TList crossing_players = NULL;
				crossing_players = table->players;
				check = 0;

				while(crossing_players) {
					Player player = ((Player)crossing_players->info);

					if (strcmp(player->name, player_name) != 0) {
						check++;

						if (check == table->curr_players + 1) {
							fprintf(out, "Jucatorul %s nu exista la masa %s!\n", player_name, table_name);
							return rooms;
						}

						/* found the player at the table */
					} else {

						if (mode == LUCKY) {
							player->rounds += luck;
						}
						else {
							player->rounds -= luck;

							/* remove the player from the table */
							/* if the table hasn't players, remove it from the room */ 
							if (player->rounds <= 0) {
								TList prev = NULL, cross = NULL;
								cross = table->players;
								prev = cross;
								cross = cross->next;

								/* find the player and remove him */
								while(strcmp(((Player)cross->info)->name, "sentinel") != 0) {
									if (strcmp(((Player)cross->info)->name, player_name) == 0) {
										TList tmp = cross, prev = cross;
										int ok = 0;

										while(ok != table->curr_players - 1) {
											ok++;
											prev = prev->next;
										}
																				
										prev->next = cross->next;

										free(((Player)tmp->info)->name);
										free(tmp->info);
										free(tmp);
										table->curr_players--;

										break;
									}

									prev = cross;
									cross = cross->next;
								}

								/* remove the table if it hasn't players */
								if (table->curr_players == 1) {

									/* free the sentinel from the table */
									cross = table->players;
									cross->next = NULL;
									free(((Player)cross->info)->name);
									free(cross->info);
									free(cross);

									prev = NULL;
									cross = room->table;

									/* search the table for removing and free the memory */
									while(cross) {
										if (strcmp(((Table)cross->info)->name_table, table_name) == 0) {
											
											if (!prev) {
												TList tmp;
												tmp = room->table;
												room->table = room->table->next;
												free(((Table)tmp->info)->name_table);
												free(tmp->info);
												free(tmp);

											} else {
												TList tmp;
												tmp = cross;
												prev->next = tmp->next;
												free(((Table)tmp->info)->name_table);
												free(tmp->info);
												free(tmp);
											}
												
											room->nr_tables--;
											break;
										}

										prev = cross;
										cross = cross->next;
									}

									/* close the room if it hasn't tables */
									if (room->nr_tables == 0) {
										cross = rooms;
										free_room(&rooms);
										fprintf(out, "Sala este inchisa!\n");
									}
								}
								
							}
						}
										
						check = -1;
						break;
					}

					crossing_players = crossing_players->next;
				}
			}

			if (check == -1)
				break;

			crossing_tables = crossing_tables->next;
		}

		if (check == -1)
			break;

		crossing_rooms = crossing_rooms->next;
	}

	return rooms;
}

/* decrease the number of rounds for all players from a specified table */
TList turn(FILE *out, TList rooms, char *table_name)
{
	Room room = (Room)rooms->info;
	int check = 0;

	TList cross = room->table;

	while(cross) {
		Table table = ((Table)cross->info);

		if (strcmp(table->name_table, table_name) != 0) {
			check++;

			if (check == room->nr_tables) {
				fprintf(out, "Masa %s nu exista!\n", table_name);
				return rooms;
			}
		} else {
			TList sentinel = table->players;
			TList prev = sentinel;
			sentinel = sentinel->next;
			TList tmp = sentinel;
			check = 0;

			/* create a new player */
			struct PCell new_player = create_player(((Player)sentinel->info)->name, ((Player)sentinel->info)->rounds);
		
			/* delete the node which will be moved to the end */
			prev->next = sentinel->next;
			free(((Player)tmp->info)->name);
			free(tmp->info);
			free(tmp);

			prev = table->players;

			while(check != table->curr_players - 2) {
				prev = prev->next;
				check++;
			}

			prev->next = NULL;

			/* create a new node and put it to the end */
			table->players = create_cell(table->players, &new_player, sizeof(struct PCell));

			prev = table->players;
			check = 0;

			/* for every player, decrement the rounds */
			while(check != table->curr_players - 1) {
				check++;
				prev = prev->next;

				Player player = (Player)prev->info;
				player->rounds--;

				if (player->rounds <= 0) {
					rooms = good_luck(out, rooms, table->name_table, player->name, 0, UNLUCKY);
				}
			}

			prev->next = table->players;
			break;
		}

		cross = cross->next;
	}

	return rooms;
}

/* make a complete turn for every table */
TList complete_turn(FILE *out, TList rooms)
{
	Room room = (Room)rooms->info;
	TList croosing_room = room->table;

	while(croosing_room) {
		Table table = ((Table)croosing_room->info);
		rooms = turn(out, rooms, table->name_table);

		croosing_room = croosing_room->next;
	}

	return rooms;
	
}

void print_list(TList list)
{
	while(list) {
		Player player = (Player)list->info;

		printf("name = %s && rounds = %d\n", player->name, player->rounds);

		list = list->next;
	}

	printf("\n");
}

/* create a list with players in descending order by number of rounds */
TList order_insert(TList list, void *element, size_t size, int nr_of_players)
{
	if (!list) {
		list = create_cell(list, element, size);
	} else {
		/* create a new cell for order list */
		TList new_cell = (TList)malloc(sizeof(Cell));

		if (!new_cell) {
			printf("Error to create a new cell in insert!\n");
			return NULL;
		}

		new_cell->info = malloc(size);

		if (!new_cell->info) {
			printf("Error to alloc info in insert!\n ");
			return NULL;
		}

		/* copy the element in the new cell */
		memcpy(new_cell->info, element, size);

		struct PCell player = *(Player)element;
		TList crossing_list = NULL, prev = NULL;
		crossing_list = list;
		int ok = 0;

		/* insert the element in order */
		struct PCell player_list = (*(Player)crossing_list->info);

		if (player.rounds > player_list.rounds) {
			new_cell->next = list;
			list = new_cell;
		} else {
			prev = crossing_list;
			crossing_list = crossing_list->next;

			if (!crossing_list)
				prev->next = new_cell;

			ok = 0;

			while(crossing_list) {
				player_list = *(Player)crossing_list->info;
				struct PCell prev_player = *(Player)prev->info;

				if (ok == nr_of_players - 2 &&
						player_list.rounds > player.rounds) {
						crossing_list->next = new_cell;
						return list;
				} else if (ok == nr_of_players - 2 &&
						player_list.rounds < player.rounds) {
						new_cell->next = crossing_list;
						prev->next = new_cell;
						return list;
				} else if (ok == nr_of_players - 2 &&
						player_list.rounds == player.rounds) {
							if (strcmp(player_list.name, player.name) > 0) {
								crossing_list->next = new_cell;
							} else {
								new_cell->next = crossing_list;
								prev->next = new_cell;
							}

							return list;
						}

				if (player.rounds > player_list.rounds &&
					player.rounds < prev_player.rounds) {
						new_cell->next = crossing_list;
						prev->next = new_cell;
						return list;
				} else if (player.rounds == player_list.rounds &&
						prev_player.rounds > player.rounds) {
							if (strcmp (player.name, player_list.name) > 0) {
								new_cell->next = crossing_list;
								prev->next = new_cell;
							} else {
								prev = crossing_list;
								crossing_list = crossing_list->next;

								new_cell->next = crossing_list;
								prev->next = new_cell;
							}

							return list;
				}

				ok++;
				prev = crossing_list;
				crossing_list = crossing_list->next;
			}
		}

	}

	return list;
}

/* print the table's players in descending order by rounds */
TList ranking(FILE *out, TList rooms, char *table_name)
{
	Room room = (Room)rooms->info;
	TList crossing_tables = room->table;
	int check = 0;

	/* search the table */
	while(crossing_tables) {
		Table table = (Table)crossing_tables->info;

		if (strcmp(table->name_table, table_name) != 0) {
			check++;

			/* if the table wasn't found, print the message and return */
			if (check == room->nr_tables) {
				fprintf(out, "Masa %s nu exista!\n", table_name);
				return rooms;
			}
		} else {	
			TList crossing_players = table->players;
			TList sorted_list = NULL;

			crossing_players = crossing_players->next;

			/* create a ordered list with players for the specified table */
			while(strcmp(((Player)crossing_players->info)->name, "sentinel") != 0) {
				struct PCell p = (*(Player)crossing_players->info);
				sorted_list = order_insert(sorted_list, &p, sizeof(struct PCell), table->curr_players);

				crossing_players = crossing_players->next;
			}

			/* display the rank */
			fprintf(out, "Clasament %s:\n", table->name_table);

			TList tmp;
			check = 0;

			while(check != table->curr_players - 1) {
				struct PCell p = (*(Player)sorted_list->info);
				fprintf(out, "%s %d\n", p.name, p.rounds);
				free(sorted_list->info);
				tmp = sorted_list;
				sorted_list = tmp->next;
				free(tmp);
				check++;
			}

			break;
		}

		crossing_tables = crossing_tables->next;
	}

	return rooms;
}

/* close a table and distribute the players to the other tables */
TList close_table(FILE *out, TList rooms, char *table_name)
{
	Room room = (Room)rooms->info;
	int check = 0;
	TList crossing_tables = NULL;
	TList removed_players = NULL;

	crossing_tables = room->table;

	/* iterates in tables */ 
	while(crossing_tables) {
		Table table = (Table)crossing_tables->info;

		if (strcmp(table->name_table, table_name) != 0) {
			check++;

			/* the table wasn't found */
			if (check != room->nr_tables) {
				fprintf(out, "Masa %s nu exista!\n", table_name);
				return rooms;
			}
		} else {
			check = 0;
			/* remove the space occupied by table */
			room->max_places -= table->max_players;

			if (room->max_places < room->curr_places) {
				fprintf(out, "Nu exista suficiente locuri in sala!\n");
				room->max_places += table->max_players;
				return rooms;
			}

			TList crossing_players = NULL;
			
			crossing_players = table->players;
			crossing_players = crossing_players->next;

			while(strcmp(((Player)crossing_players->info)->name, "sentinel") != 0) {
				struct PCell player = (*(Player)crossing_players->info);

				removed_players = create_cell(removed_players, &player, sizeof(struct PCell));

				crossing_players = crossing_players->next;
				check++;
			}

			break;
		}

		crossing_tables = crossing_tables->next;
	}

	/* add the removed players at other tables */	
	crossing_tables = room->table;
	int ok = 0;

	/* add the players at the tables where the seats are available */
	while(crossing_tables) {
		Table table = ((Table)crossing_tables->info);

		if (strcmp(table->name_table, table_name) != 0) {
			/* remove sentinel and add the players */
			TList crossing_players = NULL;
			crossing_players = table->players;

			crossing_players = crossing_players->next;

			/* remove the circular chain */
			while(strcmp(((Player)crossing_players->next->info)->name, "sentinel") != 0)
				crossing_players = crossing_players->next;

			crossing_players->next = NULL;

			crossing_players = table->players;
			TList crossing_removed = NULL;
			crossing_removed = removed_players;

			while((ok != check) && (table->curr_players - 1 <= table->max_players)) {
				struct PCell player = *(Player)crossing_removed->info;
				struct PCell new_player = create_player(player.name, player.rounds);
				crossing_players = create_cell(crossing_players, &new_player, sizeof(struct PCell));

				crossing_removed = crossing_removed->next;
				table->curr_players++;
				ok++;
			}

			/* create the circular chain */
			crossing_players = table->players;
			while(crossing_players->next)
				crossing_players = crossing_players->next;

			crossing_players->next = table->players;
		}

		crossing_tables = crossing_tables->next;
	}

	/* free the list with the removed players */
	free_list(&removed_players);
	crossing_tables = room->table;
	TList tmp, prev = NULL;

	/* find the table and close it */
	while(crossing_tables) {
		Table table = (Table)crossing_tables->info;
		if (strcmp(table->name_table, table_name) == 0) {
			if (!prev) {
				tmp = room->table;
				room->table = room->table->next;
				free_player(&table->players);
				free(table->name_table);
				free(tmp->info);
				free(tmp);

			} else {
				tmp = crossing_tables; 	
				prev->next = tmp->next;
				free_player(&table->players);
				free(table->name_table);
				free(tmp->info);
				free(tmp);
			}

			break;
		}

		prev = crossing_tables;
		crossing_tables = crossing_tables->next;
	}

	return rooms;
}