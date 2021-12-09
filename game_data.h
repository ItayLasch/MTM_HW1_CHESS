#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <stdbool.h>
#include <stdlib.h>
#include "player_data.h"

#define ZERO_POINTS 0
#define ONE_POINT 1
#define TOW_POINTS 2
#define DELETE_PLAYER -1
/*
* The following functions are available:
*   gameCreate		     - Creates a new game
*   gameDestroy		     - Deletes an existing game
*   gameCopy	     	 - Copies an existing game
*   gameContains    	 - Returns weather or not a player plays in that game.
*   gameGetWinner        - Returns the winner of the game.
*   gameRemovePlayer     - Removes a player,identified by his id, from the game and sets the other
*                          player stutus accordingly.        
*   gameGetTime	-        - Returns the amount of time the game took.
*/

/** Type for defining the game_data */
typedef struct game_data_t *Game_Data;

/** Type for defining the player's ID. */
typedef int Player_Id;

/** Type for defining the gamews time. */
typedef int Time;

/**
* gameCreate: Allocates a new game.
*
* @param winner - Enum element that contains wich player is the winner or if it's a draw.
* @param player1 - Player #1 ID.
* @param player2 - Player #2 ID.
* @param time - How much time the game was played.
*
* @return
* 	NULL - if  allocations failed.
* 	A new game in case of success.
*/
Game_Data gameCreate(Winner winner, Player_Id player1, Player_Id player2, Time time);

/**
* gameDestroy: Deallocates an existing game.
*
* @param game_data - Target game to be deallocated. If game is NULL nothing will be done
*/
void gameDestroy(MapDataElement game_data);

/**
* gameCopy: Creates a copy of target game.
*
* @param game_data - Target game.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A new game containing the same elements as game_data otherwise.
*/
MapDataElement gameCopy(MapDataElement game_data);

/**
* gameContains: Checks if a certain player, identified by his ID, plays in the game.
*
* @param game_data - The game to search in.
* @param player - The player to look for. 
* @return
* 	false - if the input is null, or if the player was not found.
* 	true - if the player plays in the game.
*/
bool gameContains(Game_Data game_data, Player_Id player);

/**
* 	gameRemovePlayer: Removes a certain player from the game. The other player in the game
*  will be named as the winner,if he wasn't already, and the player list in the tournament
*  will be updated after the removal and with the new statistics on each player.
*
* @param game_data -The game to remove player elements from.
* @param player -The player to find and remove from the game.
* @param playerList -The player list to be updated according to the new statistics.
*
*/
void gameRemovePlayer(Game_Data game_data, Player_Id player, Map playerList);

/**
* gameGetWinner: Returns the winner of the game.
*
* @param game_data - The game we want it's winner.
* @return
* 	a value according to the enum Winner.
*/
Player_Id gameGetWinner(Game_Data game_data);

/**
* gameGetTime: Returns the amount of time the game was played.
*
* @param game_data - The game we want it's time.
* @return
* 	0 - if a NULL was sent as input.
*   The game actual play time otherwise.
*/
Time gameGetTime(Game_Data game_data);

#endif