#include "gamer.h"

/*
 * Player A pick a site to go
 *
 * returns: the number of site picked
 */
int pick_site(void) {
    if (gamer.playersHub[gamer.playerID].money > 0) { //has money
        int next = gamer.playersHub[gamer.playerID].atSite + 1; //next site
        while (!gamer.path->sites[next].isBarrier) { //stop at barrier
            if (gamer.path->sites[next].name[0] == 'D'
                    && has_room(next, gamer.path)) {
                return next;
            }
            ++next;
        }
    }
    int index = gamer.playersHub[gamer.playerID].atSite + 1; //next site
    if (gamer.path->sites[index].name[0] == 'M'
            && has_room(index, gamer.path)) {
        return index;
    }
    while (!gamer.path->sites[index].isBarrier) {
        if (gamer.path->sites[index].name[0] == 'V'
                && has_room(index, gamer.path)) {
            return index;
        }
        ++index;
    }
    return index;
}