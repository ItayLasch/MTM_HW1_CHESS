#ifndef TOURNAMENT_DATA_H
#define TOURNAMENT_DATA_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "game_data.h"
#include "chess_utilities.h"
#define POSITIVE 1
#define NEGATIVE -1
#define NO_WINNER -1

/*
* The following functions are available:
*   tournamentCreate		 - Creates a new tournament
*   tournamentDestroy		 - Deletes an existing tournament
*   tournamentCopy	     	 - Copies an existing tournament
*   tournamentAddGame    	 - Adding a new game to the tournament
*   tournamentRemovePlayer   - Remove one player from the tournament
*   tournamentEnd            - End the tournament and decide the winner
*   tournamentGetLocation    - Return the location of the tournament
*   isValidLocationName      - Check if the location name is valid
*   tournamentGetStatus      - Return the status(if the tournament ended or is it still going) of the tournament
*   tournamentIsPlayerExist  - Check if the player parcipited in one (at least) of the tournament games
*   tournamentGetWinner      - Return the winner of the tournament if the tournament ended
*   copyPlayersToMap         - Copy all the players data to an outside map of players
*   tournamentLongestGameTime- Find and return the time of the longest game
*   tournamentNumberOfGames  - Return the number of games in the tournament
*/
/** Type for defining the tournament */
typedef struct tournament_t *Tournament;

/** Type for defining the location */
typedef char *Location;

/** Type for defining the winner */
typedef int WinnerId;

/** Type for defining the tournament status - is the tournament ended or is it still going */
typedef bool TournamentStatus;

/**
* tournamentCreate: Allocates a new tournament.
*
* @param location - The location of the tournament.
* @param max_games_per_player - Maximum number of games a player can participate in that tournamen.

*
* @return
* 	NULL - if allocations failed.
* 	A new tournament in case of success.
*/
Tournament tournamentCreate(const char *location, int max_games_per_player);

/**
* tournamentDestroy: Deallocates an existing tournament.
*
* @param tournament - Target tournament to be deallocated. If tournament is NULL nothing will be done.
*/
void tournamentDestroy(MapDataElement tournament);

/**
* tournamentCopy: Creates a copy of target tournament.
*
* @param tournament - Target tournament.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	Otherwise a new tournament containing the same elements as tournament.
*/
MapDataElement tournamentCopy(MapDataElement source);

/**
 * tournamenAddGame: add a new match to the tournament.
 *
 * @param tournament - tournament to add the game to.
 * @param winner - indicates the winner in the match. if it is FIRST_PLAYER, then the first player won.
 *                 if it is SECOND_PLAYER, then the second player won, otherwise the match has ended with a draw.
 * @param first_player - first player id. Must be non-negative.
 * @param second_player - second player id. Must be non-negative.
 * @param play_time - The duration of the match in seconds. Must be non-negative.
 *
 * @return
 *     CHESS_GAME_ALREADY_EXIST - if there is already a game in the tournament with the same two players.
 *     CHESS_INVALID_PLAY_TIME - if the play time is negative.
 *     CHESS_OUT_OF_MEMORY  - if there was a problem allocate memory for the new game.
 *     CHESS_SUCCESS - if game was added successfully.
 */
ChessResult tournamentAddGame(Tournament tournament, Winner winner, Player_Id player1, Player_Id player2, Time time, int* key_game);

/**
 * tournamentRemovePlayer: Removes a specific player from the tournament.
 *                      In games where the player has participated and not yet ended,
 *                      the opponent is the winner automatically after removal.
 *
 * @param tournament - tournament that contains the player. Must be non-NULL.
 * @param player_id - the player's id. Must be non-negative.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if the turnament is NULL.
 *     CHESS_SUCCESS - if there are no games in the tournament or player was removed successfully.
 */
ChessResult tournamentRemovePlayer(Tournament tournament, Player_Id player);

/**
 * turnamentEnd: The function will end the tournament and calculate the id of the winner.
 *                     The winner of the tournament is the player with the highest score.
 *                     Once the tournament is over, no games can be added for that tournament.
 *
 * @param tournament - the tournament to remove the player from. 
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if tournament is NULL.
 *     CHESS_TOURNAMENT_ENDED - if the tournament is already ended.
 *     CHESS_NO_GAMES - if there are no games in the.
 *     CHESS_SUCCESS - if tournament was ended successfully.
 */
ChessResult tournamentEnd(Tournament tournament);

/**
* tournamentGetLocation: Returns the location of the tournament.
*
* @param tournament - The tournament we want it's location.
* @return
* 	CHESS_NULL_ARGUMENT - if a NULL was sent as input.
*   The tournament location otherwise.
*/
const char *tournamentGetLocation(Tournament touranament);

/**
* tournamentGetStatus: Returns the status of the tournament - false if the tournament ended, true if the tournament still going.
*
* @param tournament - The tournament we want it's status.
* @return
* 	CHESS_NULL_ARGUMENT - if a NULL was sent as input.
*   The tournament location otherwise.
*/
TournamentStatus tournamentGetStatus(Tournament tournament);

/**
* tournamentIsPlayerExist: Returns true if the player parcipitate in at least one of the tournament's games.
*
* @param tournament - A tournament.
* @param player_id - The player id. Must be non-negative.
* @return
* 	CHESS_NULL_ARGUMENT - if a NULL was sent as input.
*   true if the player parcipitate in at least one of the tournaments games otherwise.
*/
bool tournamentIsPlayerExist(Tournament tournament, Player_Id player_id);

/**
* tournamentPlayerTotalTime: calcolate the sum of all the games time and the number of games that the player participate in.
*
* @param tournament - A tournament.
* @param player_id - The player id. Must be non-negative.
* @param numberOfGamesPerPlayer - pointer to a counter that count the number of games for the player. initialized to 0.
* @return
* 	-1 - if tournament is NULL or if player id is negative.
*   the sum of the time otherwise.
*/
int tournamentPlayerTotalTime(Tournament tournament, Player_Id player_id, int *number_of_games_per_player);

/**
* tournamentsNumberOfPlayers: return the numbers of players in the tournament.
*
* @param tournament - A tournament.
* 
* @return
* 	-1 - if tournament is NULL.
*   the number of players otherwise.
*/
int tournamentsNumberOfPlayers(Tournament tournament);


/**
* tournamentGetWinner: return the winner of tournament in case the tournament ended.
*
* @param tournament - a tournament. tournament status must be false(ended).
*
* @return
* 	-1 - if tournament is NULL.
*   The winner otherwise.
*/
int tournamentGetWinner(Tournament tournament);


/**
 * tournamentCopyPlayersToMap: copy all the tournament players to a given map of players.
 *
 * @param tournament - a tournament.
 * @param total_player_map - A players map that we copy into her.
 * @return
 *     CHESS_NULL_ARGUMENT - if turnament is NULL.
 *     CHESS_OUT_OF_MEMORY - if there was a problem copy player data. 
 *     CHESS_SUCCESS - if the copying was successfull.
 */
ChessResult tournamentCopyPlayersToMap(Tournament tournament, Map total_player_map);


/**
* tournamentsNumberOfGames: return the numbers of games in the tournament.
*
* @param tournament - a tournament.
* 
* @return
* 	-1 - if tournament is NULL.
*   the number of games otherwise.
*/
int tournamentNumberOfGames(Tournament tournament);

/**
* tournamentLongestGameTime: return the time of the longest game in the turnament, 
*       and sum up the total time of all the games and number of games.
*
* @param tournament - a tournament.
* @param total_time - a pointer for a sum of the total time. initialized to 0.
* @return
* 	-1 - if tournament is NULL.
*   the longest game, 0 if there is no games in the tournament time otherwise.
*/
int tournamentLongestGameTime(Tournament tournament, double *total_time);

/**
* tournamentGetGamesMap: Returns the tournament's games map.
*
* @param tournament - The requiered tournament.
* @return
* 	NULL if the input is NULL or if it's games map is NULL.
*   The tournament games map otherwise.
*/
Map tournamentGetGamesMap(Tournament tournament);

#endif