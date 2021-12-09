#include <stdbool.h>
#include <stdlib.h>
#include "chessSystem.h"
#include "tournament_data.h"

#define INTIAL_SIZE 50
#define EXPAND 2
#define NO_ID -1
#define WINS_MULTIPLY 6
#define LOSSES_MULTIPLY 10
#define DRAWS_MULTIPLY 2

struct chess_system_t
{
    Map tournament_list;
    Map total_player_list;
};

ChessSystem chessCreate()
{
    ChessSystem chess_sys = malloc(sizeof(*chess_sys));
    if (chess_sys == NULL)
    {
        return NULL;
    }
    chess_sys->tournament_list = mapCreate(tournamentCopy, keyCopy, tournamentDestroy, keyFree, keyCompare);
    if (chess_sys->tournament_list == NULL)
    {
        free(chess_sys);
        return NULL;
    }

    chess_sys->total_player_list = mapCreate(playerDataCopy, keyCopy, playerDataDestroy, keyFree, keyCompare);
    if (chess_sys->total_player_list == NULL)
    {
        free(chess_sys);
        mapDestroy(chess_sys->tournament_list);
        return NULL;
    }
    return chess_sys;
}

void chessDestroy(ChessSystem chess)
{
    if (chess == NULL)
    {
        return;
    }

    mapDestroy(chess->tournament_list);
    mapDestroy(chess->total_player_list);
    free(chess);
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id, int max_games_per_player, const char *tournament_location)
{
    if (chess == NULL || chess->tournament_list == NULL || tournament_location == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    if (mapContains(chess->tournament_list, (MapKeyElement)&tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }

    if (isValidLocationName(tournament_location) == false)
    {
        return CHESS_INVALID_LOCATION;
    }

    if (max_games_per_player <= 0)
    {
        return CHESS_INVALID_MAX_GAMES;
    }

    Tournament tournament_data = tournamentCreate(tournament_location, max_games_per_player);
    if (tournament_data == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }

    if (mapPut(chess->tournament_list, &tournament_id, (MapKeyElement)tournament_data) != MAP_SUCCESS)
    {
        tournamentDestroy(tournament_data);
        return CHESS_OUT_OF_MEMORY;
    }
    tournamentDestroy(tournament_data);
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner, int play_time)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player)
    {
        return CHESS_INVALID_ID;
    }

    if (mapContains(chess->tournament_list, (MapKeyElement)&tournament_id) == false)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    Tournament tournament = mapGet(chess->tournament_list, (MapKeyElement)&tournament_id);
    if (tournament == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (tournamentGetStatus(tournament) == false)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    int *key_game = malloc(sizeof(int *));
    ChessResult result = tournamentAddGame(tournament, winner, first_player, second_player, play_time, key_game);
    if (result == CHESS_OUT_OF_MEMORY)
    {
        free(key_game);
        return CHESS_OUT_OF_MEMORY;
    }

    if (result == CHESS_SUCCESS)
    {
        PlayerData player1_data = playerDataCreate();
        if (player1_data == NULL)
        {
            gameDestroy(mapGet(tournamentGetGamesMap(tournament), key_game));
            free(key_game);
            return CHESS_OUT_OF_MEMORY;
        }

        PlayerData player2_data = playerDataCreate();
        if (player2_data == NULL)
        {
            gameDestroy(mapGet(tournamentGetGamesMap(tournament), key_game));
            free(key_game);
            playerDataDestroy(player1_data);
            return CHESS_OUT_OF_MEMORY;
        }

        if (mapPut(chess->total_player_list, &first_player, player1_data) != MAP_SUCCESS ||
            mapPut(chess->total_player_list, &second_player, player2_data) != MAP_SUCCESS)
        {
            gameDestroy(mapGet(tournamentGetGamesMap(tournament), key_game));
            result = CHESS_OUT_OF_MEMORY;
        }

        playerDataDestroy(player1_data);
        playerDataDestroy(player2_data);
    }
    free(key_game);
    return result;
}

ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    if (mapContains(chess->tournament_list, (MapKeyElement)&tournament_id) == false)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    mapRemove(chess->tournament_list, &tournament_id);
    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    if (mapContains(chess->total_player_list, &player_id) == false)
    {
        return CHESS_PLAYER_NOT_EXIST;
    }

    MAP_FOREACH(MapKeyElement, tournament_key, chess->tournament_list)
    {
        Tournament temporary_tournament = mapGet(chess->tournament_list, tournament_key);
        if (tournamentGetStatus(temporary_tournament) && tournamentIsPlayerExist(temporary_tournament, player_id))
        {
            tournamentRemovePlayer(temporary_tournament, player_id);
        }
        keyFree(tournament_key);
    }

    mapRemove(chess->total_player_list, &player_id);
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    if (mapContains(chess->tournament_list, (MapKeyElement)&tournament_id) == false)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    Tournament tournament = mapGet(chess->tournament_list, (MapKeyElement)&tournament_id);
    return tournamentEnd(tournament);
}

double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result)
{
    if (chess == NULL)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return 0;
    }
    if (player_id <= 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return 0;
    }
    if (mapContains(chess->total_player_list, &player_id) == false)
    {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return 0;
    }
    double total_time = 0;
    int number_of_games = 0;
    MAP_FOREACH(MapKeyElement, tournament_key, chess->tournament_list)
    {
        total_time += tournamentPlayerTotalTime(mapGet(chess->tournament_list, (MapKeyElement)tournament_key),
                                                player_id, &number_of_games);
        keyFree(tournament_key);
    }
    *chess_result = CHESS_SUCCESS;
    if (total_time == 0)
    {
        return total_time;
    }

    return total_time / number_of_games;
}

/**
 * calulateLevel: the function returns the level of a particular player.
 *
 * @param wins - The player's number of wins.
 * @param losses - The player's number of losses.
 * @param draws - The player's number of draws.
 * @return
 *     The player's level according to the formula.
 */
static double calulateLevel(int wins, int losses, int draws)
{
    double totalGames = (double)wins + (double)losses + (double)draws;

    return (double)(((wins * WINS_MULTIPLY) - (losses * LOSSES_MULTIPLY) + (draws * DRAWS_MULTIPLY)) / totalGames);
}

/**
 * CreatePlayerMap: The function gets an empty map and fills it with players stats 
 * that play in a specific chess system.
 *
 * @param chess - The chess System we check.
 * @param total_player_map - The map we want to fill.
 * @return
 *     CHESS_NULL_ARGUMENT if one of the inputs is NULL.
 *     CHESS_OUT_OF_MEMORY if there was a problem in the process of filling the map.
 *     CHESS_SUCCESS otherwise.
 */
static ChessResult CreatePlayerMap(ChessSystem chess, Map total_player_map)
{
    if (chess == NULL || total_player_map == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    MAP_FOREACH(MapKeyElement, tournament_key, chess->tournament_list)
    {
        if (tournamentCopyPlayersToMap(mapGet(chess->tournament_list, (MapKeyElement)tournament_key), total_player_map) != CHESS_SUCCESS)
        {
            keyFree(tournament_key);
            return CHESS_OUT_OF_MEMORY;
        }
        keyFree(tournament_key);
    }

    return CHESS_SUCCESS;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file)
{
    if (file == NULL || chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    Map total_player_map = mapCreate(playerDataCopy, keyCopy, playerDataDestroy, keyFree, keyCompare);
    if (total_player_map == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }

    if (CreatePlayerMap(chess, total_player_map) != CHESS_SUCCESS)
    {
        mapDestroy(total_player_map);
        return CHESS_OUT_OF_MEMORY;
    }
    int map_size = mapGetSize(total_player_map);
    Player_Id max_id = NO_ID;
    double max_level = 0, level;
    bool first;
    PlayerData p_data;
    for (int i = 0; i < map_size; i++)
    {
        first = true;
        MAP_FOREACH(MapKeyElement, player_id, total_player_map)
        {
            if (mapContains(chess->total_player_list, player_id) == false)
            {
                keyFree(player_id);
                continue;
            }

            p_data = mapGet(total_player_map, player_id);
            if (playerGetWins(p_data) == 0 && playerGetLosses(p_data) == 0 && playerGetDraws(p_data) == 0)
            {
                keyFree(player_id);
                continue;
            }
            level = calulateLevel(playerGetWins(p_data), playerGetLosses(p_data), playerGetDraws(p_data));
            if (first == true || level > max_level)
            {
                first = false;
                max_level = level;
                max_id = *(int *)player_id;
            }
            else if (level == max_level)
            {
                if (keyCompare(player_id, &max_id) < 0)
                {
                    max_id = *(int *)player_id;
                }
            }
            keyFree(player_id);
        }
        if (max_id != NO_ID)
        {
            fprintf(file, "%d %.2f\n", max_id, max_level);
            mapRemove(total_player_map, &max_id);
            max_id = NO_ID;
        }
    }
    mapDestroy(total_player_map);
    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file)
{
    if (path_file == NULL || chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    bool ended_tournament_exsist = false;
    MAP_FOREACH(MapKeyElement, key_element, chess->tournament_list)
    {
        if (tournamentGetStatus(mapGet(chess->tournament_list, (MapKeyElement)key_element)) == false)
        {
            ended_tournament_exsist = true;
        }
        keyFree(key_element);
    }

    if (ended_tournament_exsist == false)
    {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }

    FILE *file_name = fopen(path_file, "w");
    if (file_name == NULL)
    {
        return CHESS_SAVE_FAILURE;
    }
    int longest_game_time, number_of_players, number_of_games;
    double total_game_time, average_game_time;
    MAP_FOREACH(MapKeyElement, tour_key, chess->tournament_list)
    {
        Tournament tour_data = mapGet(chess->tournament_list, (MapKeyElement)tour_key);
        if (tour_data == NULL)
        {
            return CHESS_NULL_ARGUMENT;
        }
        if (tournamentGetStatus(tour_data) == false)
        {
            number_of_games = tournamentNumberOfGames(tour_data);
            total_game_time = 0;
            longest_game_time = tournamentLongestGameTime(tour_data, &total_game_time);
            if (number_of_games == 0)
            {
                average_game_time = 0;
            }
            else
            {
                average_game_time = total_game_time / (double)number_of_games;
            }
            number_of_players = tournamentsNumberOfPlayers(tour_data);
            fprintf(file_name, "%d\n", tournamentGetWinner(tour_data));
            fprintf(file_name, "%d\n", longest_game_time);
            fprintf(file_name, "%.2f\n", average_game_time);
            fprintf(file_name, "%s\n", tournamentGetLocation(tour_data));
            fprintf(file_name, "%d\n", number_of_games);
            fprintf(file_name, "%d\n", number_of_players);
        }
        keyFree(tour_key);
    }
    fclose(file_name);
    return CHESS_SUCCESS;
}
