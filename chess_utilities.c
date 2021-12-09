#include "chess_utilities.h"

int keyCompare(MapKeyElement x, MapKeyElement y)
{
    return (*(int *)x - *(int *)y);
}

void keyFree(MapKeyElement x)
{
    if(x == NULL){
        return;
    }
    free(x);
}

MapKeyElement keyCopy(MapKeyElement x)
{
    if(x == NULL){
        return NULL;
    }
    int* copy_ptr = malloc(sizeof(*copy_ptr));
    if(copy_ptr == NULL){
        return NULL;
    }
    *copy_ptr = *(int *)x;
    return copy_ptr;
}

bool isValidLocationName(const char *location)
{
    int current = 0;
    if (location[0] < CAPITAL_A || location[0] > CAPITAL_Z)
    {
        return false;
    }
    current++;
    while (current < strlen(location))
    {
        if ((location[current] < LOWER_A || location[current] > LOWER_Z) && location[current] != SPACE)
        {
            return false;
        }
        current++;
    }
    return true;
}
