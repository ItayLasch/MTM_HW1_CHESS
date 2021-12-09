#include <stdlib.h>
#include "player_data.h"


#define P_NULL -1

struct player_data{
    int points;
    int wins;
    int losses;
    int draws;
};

static PlayerData playerDataCopyInternal(PlayerData source);
static void playerDataDestroyInternal(PlayerData p_data);

PlayerData playerDataCreate()
{
    PlayerData p_data = malloc(sizeof(*p_data));
    if (p_data == NULL)
    {
        return NULL;
    }
    p_data->points = 0;
    p_data->wins = 0;
    p_data->losses = 0;
    p_data->draws = 0;
    return p_data;
}

void playerDataDestroy(MapDataElement p_data)
{
    playerDataDestroyInternal(p_data);
}

static void playerDataDestroyInternal(PlayerData p_data)
{
    if (p_data == NULL)
    {
        return;
    }
    free(p_data);
}

MapDataElement playerDataCopy(MapDataElement player)
{
    PlayerData player_copy = playerDataCopyInternal(player);
    return player_copy;
}

static PlayerData playerDataCopyInternal(PlayerData source)
{
    if (source == NULL)
    {
        return NULL;
    }

    PlayerData copy = playerDataCreate();
    if (copy == NULL)
    {
        return NULL;
    }

    copy->points = source->points;
    copy->wins = source->wins;
    copy->losses = source->losses;
    copy->draws = source->draws;

    return copy;
}

int playerGetPoints(PlayerData player_data)
{
    if (player_data == NULL)
    {
        return P_NULL;
    }

    return player_data->points;
}

void playerSetPoints(PlayerData player_data, int add_points)
{
    if (player_data == NULL)
    {
        return;
    }

    player_data->points += add_points;
}

int playerGetWins(PlayerData player_data)
{
    if (player_data == NULL)
    {
        return P_NULL;
    }

    return player_data->wins;
}

void playerSetWins(PlayerData player_data, int add_wins)
{
    if (player_data == NULL)
    {
        return;
    }

    player_data->wins += add_wins;
}

int playerGetLosses(PlayerData player_data)
{
    if (player_data == NULL)
    {
        return P_NULL;
    }

    return player_data->losses;
}

void playerSetLosses(PlayerData player_data, int add_loss)
{
    if (player_data == NULL)
    {
        return;
    }

    player_data->losses += add_loss;
}

int playerGetDraws(PlayerData player_data)
{
    if (player_data == NULL)
    {
        return P_NULL;
    }

    return player_data->draws;
}

void playerSetDraws(PlayerData player_data, int add_draws)
{
    if (player_data == NULL)
    {
        return;
    }

    player_data->draws += add_draws;
}