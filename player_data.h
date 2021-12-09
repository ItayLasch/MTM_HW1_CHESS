#ifndef PLAYER_MAP_H
#define PLAYER_MAP_H
#include "chessSystem.h"
#include "./mtm_map/map.h"
/*
* The following functions are available:
*   PlayerDataCreate		- Creates a new player
*   playerDataDestroy		- Deletes an existing player.
*   playerDataCopy		    - Copies an existing player.
*   getPoints		        - Returns the amount of points a certain player has.
*   setPoints	            - Adds points to a certain player.
*   getWins		            - Returns the amount of wins a certain player has.
*   setWins  	            - Adds wins to a certain player.
*   getLosses		        - Returns the amount of losses a certain player has.
*   setLosses	            - Adds losses to a certain player.
*   getDraws		        - Returns the amount of draws a certain player has.
*	setDraws		        - Adds draws to a certain player.
*/

/** Type for defining the player_data */
typedef struct player_data *PlayerData;

/**
* playerDataCreate: Allocates a new player.
*
*
* @return
* 	NULL - if  allocations failed.
* 	A new player in case of success.
*/
PlayerData playerDataCreate();

/**
* playerDataDestroy: Deallocates an existing game.
*
* @param p_data - Target game to be deallocated. If game is NULL nothing will be done
*/
void playerDataDestroy(MapDataElement p_data);

/**
* gameCopy: Creates a copy of target player.
*
* @param player - Target player.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A new player containing the same elements as player otherwise.
*/
MapDataElement playerDataCopy(MapDataElement player);

/**
* playerGetPoints: Returns the player's points.
*
* @param player_data - The player we want his points.
* @return
* 	-1 - if a NULL was sent as input.
*   The player actual points otherwise.
*/
int playerGetPoints(PlayerData player_data);

/**
* playerSetPoints: Adds new points to the player.
*
* @param player_data - The player we want to change his points.
* @param add_points - The number of points we want to add.
*/
void playerSetPoints(PlayerData player_data, int add_poinst);

/**
* playerGetWins: Returns the player's number of wins.
*
* @param player_data - The player we want his number of wins.
* @return
* 	-1 - if a NULL was sent as input.
*   The player actual number wins otherwise.
*/
int playerGetWins(PlayerData player_data);

/**
* playerSetWins: Adds new wins to the player.
*
* @param player_data - The player we want to change his wins.
* @param add_wins - The number of wins we want to add.
*/
void playerSetWins(PlayerData player_data, int add_wins);

/**
* playerGetLosses: Returns the player's number of losses.
*
* @param player_data - The player we want his number of losses.
* @return
* 	-1 - if a NULL was sent as input.
*   The player actual number losses otherwise.
*/
int playerGetLosses(PlayerData player_data);

/**
* playerSetLosses: Adds new losses to the player.
*
* @param player_data - The player we want to change his losses.
* @param add_loss - The number of losses we want to add.
*/
void playerSetLosses(PlayerData player_data, int add_loss);

/**
* playerGeDraws: Returns the player's number of draws.
*
* @param player_data - The player we want his number of draws.
* @return
* 	-1 - if a NULL was sent as input.
*   The player actual number draws otherwise.
*/
int playerGetDraws(PlayerData player_data);

/**
* playerSetDraws: Adds new draws to the player.
*
* @param player_data - The player we want to change his draws.
* @param add_draws - The number of draws we want to add.
*/
void playerSetDraws(PlayerData player_data, int add_draws);

#endif

