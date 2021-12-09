#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game_data.h"


struct game_data_t
{
    Player_Id player1;
    Player_Id player2;
    Winner winner;
    Time time;
};

Game_Data gameCreate(Winner winner, Player_Id player1, Player_Id player2, Time time)
{
    Game_Data game_data = malloc(sizeof(*game_data));
    if (game_data == NULL)
    {
        return NULL;
    }
    game_data->player1 = player1;
    game_data->player2 = player2;
    game_data->winner = winner;
    game_data->time = time;

    return game_data;
}

void gameDestroy(MapDataElement game_data)
{
    if (game_data == NULL)
    {
        return;
    }
    free(game_data);
}

static Game_Data gameCopyInternal(Game_Data game_data)
{
    if (game_data == NULL)
    {
        return NULL;
    }

    Game_Data game_data_copy = gameCreate(game_data->winner, game_data->player1, game_data->player2, game_data->time);
    if (game_data_copy == NULL)
    {
        return NULL;
    }
    return game_data_copy;
}

MapDataElement gameCopy(MapDataElement game_data)
{
    if (game_data == NULL)
    {
        return NULL;
    }
    Game_Data game_data_copy = gameCopyInternal((Game_Data)game_data);
    return game_data_copy;
}

bool gameContains(Game_Data game_data, Player_Id player)
{
    if (game_data == NULL)
    {
        return false;
    }
    return (game_data->player1 == player || game_data->player2 == player);
}

Player_Id gameGetWinner(Game_Data game_data)
{
    return game_data->winner;
}

/**
* playerListUpdateAfterRemovePlayer: Updates a player stats after his game partner was removed.
*
* @param result - The result of the game, one of the players is the winner or draw.
* @param playerList - The map that contains the player's stats.
* @param player - The player's ID
* @param time - How much time the game was played.
*
*/
static void playerListUpdateAfterRemovePlayer(Winner result, Map playerList, MapKeyElement player)
{
    if (result == DRAW)
    {
        playerSetPoints(mapGet(playerList, player), ONE_POINT);
        playerSetWins(mapGet(playerList, player), ONE_POINT);
        playerSetDraws(mapGet(playerList, player), -ONE_POINT);
    }
    else
    {
        playerSetPoints(mapGet(playerList, player), TOW_POINTS);
        playerSetWins(mapGet(playerList, player), ONE_POINT);
        playerSetLosses(mapGet(playerList, player), -ONE_POINT);
    }
}

void gameRemovePlayer(Game_Data game_data, Player_Id player, Map playerList)
{
    if (game_data == NULL || playerList == NULL)
    {
        return;
    }
    if (gameContains(game_data, player) == false)
    {
        return;
    }
    if (game_data->player1 == player)
    {
        if (game_data->player2 != DELETE_PLAYER && gameGetWinner(game_data) != SECOND_PLAYER)
        {
            playerListUpdateAfterRemovePlayer(game_data->winner, playerList, &game_data->player2);
            game_data->winner = SECOND_PLAYER;
        }
        game_data->player1 = DELETE_PLAYER;
    }
    else if (game_data->player2 == player)
    {
        if (game_data->player1 != DELETE_PLAYER && gameGetWinner(game_data) != FIRST_PLAYER)
        {
            playerListUpdateAfterRemovePlayer(game_data->winner, playerList, &game_data->player1);
            game_data->winner = FIRST_PLAYER;
        }
        game_data->player2 = DELETE_PLAYER;
    }
}

Time gameGetTime(Game_Data game_data)
{
    if (game_data == NULL)
    {
        return 0;
    }
    return game_data->time;
}
