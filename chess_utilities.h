#ifndef CHESS_UTILITIES_H
#define CHESS_UTILITIES_H
#include <stdlib.h>
#include <string.h>
#include "./mtm_map/map.h"

#define CAPITAL_A 'A'
#define CAPITAL_Z 'Z'
#define LOWER_A 'a'
#define LOWER_Z 'z'
#define SPACE ' '

/**
 * keyCompare: Compares between two given keys.
 *
 * @param x - first key.
 * @param y - second key.
 * @return
 *     The difference between the first key and the second key.
 */
int keyCompare(MapKeyElement x, MapKeyElement y);

/**
 * keyFree: Deallocates a specific key.
 *
 * @param x - the key we want to deallocate.
 */
void keyFree(MapKeyElement x);

/**
 * keyCopy: Creates a copy of target key.
 *
 * @param x - Target key.
 * @return
 *     NULL if input is NULL or allocation failed.
 *     A copy of the target key otherwise.
 */
MapKeyElement keyCopy(MapKeyElement x);

/**
 * isValidLocationName: returns if a tournament location name is valid according to the rules.
 *
 * @param location - The touranment's location.
 * @return
 *     false - if the name is not written according to the rules.
 *     true - if the location name is valid.
 */
bool isValidLocationName(const char *location);


#endif