
#include <stdio.h>
#include "tournament_data.h"

struct tournament_t
{
    Map games;
    Map player_list;
    WinnerId winner;
    Location location;
    int max_games_per_player;
    TournamentStatus status;
    int number_of_players;
};

void tournamentDestroyInternal(Tournament tournament);
Tournament tournamentCopyInternal(Tournament tournament);

Tournament tournamentCreate(const char *location, int max_games_per_player)
{
    Tournament tournament = malloc(sizeof(*tournament));
    if (tournament == NULL)
    {
        return NULL;
    }
    tournament->games = mapCreate(gameCopy, keyCopy, gameDestroy, keyFree, keyCompare);
    tournament->player_list = mapCreate(playerDataCopy, keyCopy, playerDataDestroy, keyFree, keyCompare);
    tournament->winner = NO_WINNER;
    tournament->location = malloc(strlen(location) + 1);
    if (tournament->location == NULL)
    {
        tournamentDestroyInternal(tournament);
    }
    strcpy(tournament->location, location); 
    tournament->status = true;
    tournament->max_games_per_player = max_games_per_player;
    tournament->number_of_players = 0;
    return tournament;
}



void tournamentDestroy(MapDataElement tournament)
{
    tournamentDestroyInternal(tournament);
}

void tournamentDestroyInternal(Tournament tournament)
{
    if (tournament == NULL)
    {
        return;
    }

    mapDestroy(tournament->games);
    mapDestroy(tournament->player_list);
    free(tournament->location);
    free(tournament);
}

MapDataElement tournamentCopy(MapDataElement source)
{
    return tournamentCopyInternal(source);
}

Tournament tournamentCopyInternal(Tournament tournament)
{
    if (tournament == NULL)
    {
        return NULL;
    }

    Tournament tournament_copy = tournamentCreate(tournament->location, tournament->max_games_per_player);
    if (tournament_copy == NULL)
    {
        return NULL;
    }
    mapDestroy(tournament_copy->games);
    tournament_copy->games = mapCopy(tournament->games);
    if (tournament_copy->games == NULL)
    {
        tournamentDestroyInternal(tournament_copy);
        return NULL;
    }
    mapDestroy(tournament_copy->player_list);
    tournament_copy->player_list = mapCopy(tournament->player_list);
    if (tournament_copy->player_list == NULL)
    {
        mapDestroy(tournament_copy->games);
        tournamentDestroyInternal(tournament_copy);
        return NULL;
    }
    strcpy(tournament_copy->location, tournament->location);
    tournament_copy->winner = tournament->winner;
    tournament_copy->status = tournament->status;
    tournament_copy->max_games_per_player = tournament->max_games_per_player;
    tournament_copy->number_of_players = tournament->number_of_players;
    return tournament_copy;
}

/**
 * playerGames: Returns the number of games a specific player plays in the tournament.
 *
 * @param tournament - The tournament.
 * @param player - The player's Id.
 * @return
 *     The number of games the player plays, 0 if he doesnt play at all.
 */
static int playerGames(Tournament tournament, Player_Id player)
{
    if (mapContains(tournament->player_list, &player) == false)
    {
        return 0;
    }
    int count = 0;
    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        if (gameContains(mapGet(tournament->games, (MapKeyElement)game_key), player))
        {
            count++;
        }
        keyFree(game_key);
    }
    return count;
}

/**
 * playsTogether: Checks if two players play a game in the tournament together.
 *
 * @param tournament - The tournament.
 * @param player1 - First player Id.
 * @param player2 - Second player Id.
 * @return
 *     false- if they dont have a game together.
 *     true - if they already play together in the tournament.
 */
static bool playsTogether(Tournament tournament, Player_Id player1, Player_Id player2)
{
    if (tournament == NULL)
    {
        return false;
    }
    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        if (gameContains(mapGet(tournament->games, game_key), player1) &&
            gameContains(mapGet(tournament->games, game_key), player2))
        {
            keyFree(game_key);
            return true;
        }
        keyFree(game_key);
    }
    return false;
}

/**
 * addPlayerToTournament: The function adds a player to the tournament's players map.
 *
 * @param tournament - The tournament.
 * @param player_id - The player's Id.
 * @return
 *     CHESS_NULL_ARGUMENT - if input is NULL.
 *     CHESS_OUT_OF_MEMORY - if the insertion failed.
 *     CHESS_SUCCESS otherwise.
 */
static ChessResult addPlayerToTournament(Tournament tournament, Player_Id player_id)
{
    if (tournament == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (mapContains(tournament->player_list, &player_id) == false)
    {
        PlayerData p_data = playerDataCreate();
        if (mapPut(tournament->player_list, (MapKeyElement)&player_id, (MapDataElement)p_data) == MAP_SUCCESS)
        {
            playerDataDestroy(p_data);
            tournament->number_of_players++;
            return CHESS_SUCCESS;
        }
        playerDataDestroy(p_data);
        return CHESS_OUT_OF_MEMORY;
    }

    if(tournamentIsPlayerExist(tournament, player_id))
    {
        return CHESS_SUCCESS;
    }

    tournament->number_of_players++;
    return CHESS_SUCCESS;
}

ChessResult tournamentAddGame(Tournament tournament, Winner winner, Player_Id player1, Player_Id player2, Time time, int* key_game)
{
    if (playsTogether(tournament, player1, player2)) 
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }
    if (playerGames(tournament, player1) >= tournament->max_games_per_player || 
        playerGames(tournament, player2) >= tournament->max_games_per_player)
    {
        return CHESS_EXCEEDED_GAMES; 
    }

    Game_Data new_game_data = gameCreate(winner, player1, player2, time);
    if (new_game_data == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    if (addPlayerToTournament(tournament, player1) == CHESS_OUT_OF_MEMORY)
    {
        gameDestroy(new_game_data);
        return CHESS_OUT_OF_MEMORY;
    }
    if (addPlayerToTournament(tournament, player2) == CHESS_OUT_OF_MEMORY)
    {
        gameDestroy(new_game_data);
        return CHESS_OUT_OF_MEMORY;
    }
    int new_game_key = mapGetSize(tournament->games) + 1;
    *key_game = new_game_key;
    if (mapPut(tournament->games, (MapKeyElement)&new_game_key, (MapDataElement)new_game_data) == MAP_OUT_OF_MEMORY)
    {
        gameDestroy(new_game_data);
        return CHESS_OUT_OF_MEMORY;
    }

    switch (winner)
    {
    case FIRST_PLAYER:
        playerSetPoints(mapGet(tournament->player_list, (MapKeyElement)&player1), TOW_POINTS);
        playerSetWins(mapGet(tournament->player_list, (MapKeyElement)&player1), ONE_POINT);
        playerSetLosses(mapGet(tournament->player_list, (MapKeyElement)&player2), ONE_POINT);
        break;

    case SECOND_PLAYER:
        playerSetPoints(mapGet(tournament->player_list, (MapKeyElement)&player2), TOW_POINTS);
        playerSetWins(mapGet(tournament->player_list, (MapKeyElement)&player2), ONE_POINT);
        playerSetLosses(mapGet(tournament->player_list, (MapKeyElement)&player1), ONE_POINT);

        break;
    case DRAW:
        playerSetPoints(mapGet(tournament->player_list, (MapKeyElement)&player1), ONE_POINT);
        playerSetDraws(mapGet(tournament->player_list, (MapKeyElement)&player1), ONE_POINT);
        playerSetPoints(mapGet(tournament->player_list, (MapKeyElement)&player2), ONE_POINT);
        playerSetDraws(mapGet(tournament->player_list, (MapKeyElement)&player2), ONE_POINT);
        break;
    default:
        gameDestroy(new_game_data);
        return CHESS_NULL_ARGUMENT;
    }
    gameDestroy(new_game_data); 
    return CHESS_SUCCESS;
}

ChessResult tournamentRemovePlayer(Tournament tournament, Player_Id player) //remove the player from all the games in the tournament
{
    if (tournament == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament->games == NULL)
    {
        return CHESS_SUCCESS;
    }

    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        gameRemovePlayer(mapGet(tournament->games, game_key), player, tournament->player_list);
        keyFree(game_key);
    }

    mapRemove(tournament->player_list, &player);
    return CHESS_SUCCESS;
}

int tournamentPlayerWins(Tournament tournament, Player_Id player)
{
    int wins = 0;
    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        if (gameGetWinner(mapGet(tournament->games, (MapKeyElement)game_key)) == player)
        {
            wins++;
        }
        keyFree(game_key);
    }
    return wins;
}

/**
 * comparePlayers: The function gets two player id's and compares between the two players
 * in order to decide who is the winner between them in the contest for the tournamnet winner.
 *
 * @param playre_map - The players_map of the tournament.
 * @param player1 - A pointer to the first player id.
 * @param player1 - A pointer to the second player id.
 * @return
 *     POSITIVE(1) - if player1 is the winner between them.
 *     NEGATIVE(-1) - if player2 is the winner between them.
 *     The difference between their id's otherwise.
 */
static int comparePlayers(Map players_map, Player_Id *player1, Player_Id *player2)
{
    if (*player2 == NO_WINNER)
    {
        return POSITIVE;
    }
    PlayerData player1_data = mapGet(players_map, (MapKeyElement)player1);
    PlayerData player2_data = mapGet(players_map, (MapKeyElement)player2);

    if (playerGetPoints(player1_data) > playerGetPoints(player2_data))
    {
        return POSITIVE;
    }
    else if (playerGetPoints(player1_data) < playerGetPoints(player2_data))
    {
        return NEGATIVE;
    }
    if (playerGetLosses(player1_data) < playerGetLosses(player2_data))
    {
        return POSITIVE;
    }
    else if (playerGetLosses(player1_data) > playerGetLosses(player2_data))
    {
        return NEGATIVE;
    }
    if (playerGetWins(player1_data) > playerGetWins(player2_data))
    {
        return POSITIVE;
    }
    else if (playerGetWins(player1_data) < playerGetWins(player2_data))
    {
        return NEGATIVE;
    }
    return *player2 - *player1;
}

int tournamentsNumberOfPlayers(Tournament tournament)
{
    if (tournament == NULL)
    {
        return 0;
    }
    return tournament->number_of_players;
}

ChessResult tournamentEnd(Tournament tournament)
{
    if (tournament == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament->status == false)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    if (tournamentNumberOfGames(tournament) == 0)
    {
        return CHESS_NO_GAMES;
    }
    int winner_id = NO_WINNER;
    MAP_FOREACH(MapKeyElement, player_p, tournament->player_list)
    {
        if (comparePlayers(tournament->player_list, player_p, &winner_id) > 0)
        {
            winner_id = *(int *)player_p;
        }
        keyFree(player_p);
    }
    tournament->winner = winner_id;
    tournament->status = false;

    return CHESS_SUCCESS;
}

const char *tournamentGetLocation(Tournament tournament)
{
    if (tournament == NULL || tournament->location == NULL)
    {
        return NULL;
    }

    return tournament->location;
}

int tournamentNumberOfGames(Tournament tournament)
{
    if (tournament == NULL)
    {
        return -1;
    }

    return mapGetSize(tournament->games);
}

TournamentStatus tournamentGetStatus(Tournament tournament)
{
    return tournament->status;
}

bool tournamentIsPlayerExist(Tournament tournament, Player_Id player_id)
{
    if (tournament == NULL || tournament->games == NULL)
    {
        return false;
    }

    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        if (gameContains(mapGet(tournament->games, game_key), player_id))
        {
            keyFree(game_key);
            return true;
        }
        keyFree(game_key);
    }

    return false;
}

int tournamentPlayerTotalTime(Tournament tournament, Player_Id player_id, int *number_of_games_per_player)
{
    if (tournament == NULL)
    {
        return NEGATIVE;
    }
    if (player_id <= 0)
    {
        return NEGATIVE;
    }
    int total_time = 0;
    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        Game_Data game_data = mapGet(tournament->games, game_key);
        if (gameContains(game_data, player_id))
        {
            (*number_of_games_per_player)++;
            total_time += gameGetTime(game_data);
        }
        keyFree(game_key);
    }
    return total_time;
}

int tournamentGetWinner(Tournament tournament)
{
    if (tournament == NULL)
    {
        return NO_WINNER;
    }
    return tournament->winner;
}

ChessResult tournamentCopyPlayersToMap(Tournament tournament, Map total_player_map)
{
    if (tournament == NULL || total_player_map == NULL || tournament->player_list == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    MAP_FOREACH(MapKeyElement, player_id, tournament->player_list)
    {
        PlayerData p_data = mapGet(tournament->player_list, player_id);
        if (p_data == NULL)
        {
            return CHESS_NULL_ARGUMENT;
        }

        if (mapContains(total_player_map, player_id))
        {
            p_data = mapGet(total_player_map, player_id);
            playerSetWins(p_data, playerGetWins(mapGet(tournament->player_list, player_id)));
            playerSetLosses(p_data, playerGetLosses(mapGet(tournament->player_list, player_id)));
            playerSetDraws(p_data, playerGetDraws(mapGet(tournament->player_list, player_id)));
            playerSetPoints(p_data, playerGetPoints(mapGet(tournament->player_list, player_id)));
        }
        else{
            if (mapPut(total_player_map, player_id, (MapDataElement)p_data) != MAP_SUCCESS)
            {
                keyFree(player_id);
                return CHESS_OUT_OF_MEMORY;
            }
        }
        keyFree(player_id);
    }

    return CHESS_SUCCESS;
}

int tournamentLongestGameTime(Tournament tournament, double *total_time)
{
    if (tournament == NULL)
    {
        return NEGATIVE;
    }
    int game_time, longestGameTime = 0;
    MAP_FOREACH(MapKeyElement, game_key, tournament->games)
    {
        Game_Data game_data = mapGet(tournament->games, (MapKeyElement)game_key);
        game_time = gameGetTime(game_data);
        *total_time += game_time;
        if (longestGameTime < game_time)
        {
            longestGameTime = game_time;
        }
        keyFree(game_key);
    }
    return longestGameTime;
}

Map tournamentGetGamesMap(Tournament tournament){
    if(tournament == NULL){
        return NULL;
    }
    if(tournament->games == NULL){
        return NULL;
    }

    return tournament->games;
}