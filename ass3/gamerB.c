#include "gamer.h"

/*
 * Count how many cards do I have in hand
 *
 * playerID: the ID of player whom we are going to count
 *
 * returns: total number of cards
 */
int get_my_cards_count(int playerID) {
    return gamer.playersHub[playerID].hand[0].count
            + gamer.playersHub[playerID].hand[1].count
            + gamer.playersHub[playerID].hand[2].count
            + gamer.playersHub[playerID].hand[3].count
            + gamer.playersHub[playerID].hand[4].count;
}

/*
 * Get largest cards' number that others' have
 *
 * returns: largest cards' number
 */
int get_others_most_cards(void) {
    int otherMost = 0,otherMost2 = 0;
    for (int i = 0; i < gamer.playerID; ++i) {
        int myCards = get_my_cards_count(i);
        otherMost = (myCards > otherMost) ? myCards : otherMost;
    }
    for (int i = gamer.playerID + 1; i < gamer.playersCount; ++i) {
        int myCards = get_my_cards_count(i);
        otherMost2 = (myCards > otherMost2) ? myCards : otherMost2;
    }
    if (otherMost > otherMost2) {
        return otherMost;
    }
    return otherMost2;
}

/*
 * Move forward to the earliest site which has room
 * returns that site number
 */
int get_final_pick(void) {
    int next = gamer.playersHub[gamer.playerID].atSite + 1;
    while (!has_room(next, gamer.path)) {
        ++next;
    }
    return next;
//    while (!gamer.path->sites[next].isBarrier) {
//        if (has_room(next, gamer.path)) {
//            return next;
//        }
//        ++next;
//    }
//    return next;
}

/*
 * Player B pick a site to go
 *
 * returns: the number of site picked
 */
int pick_site(void) {
    int index = gamer.playersHub[gamer.playerID].atSite; //current site
    if (has_room(index + 1, gamer.path)) {
        bool later = true; //others are later than me
        for (int i = 0; i < index; ++i) {
            if (gamer.path->sites[i].population != 0) {
                later = false;
            }
        }
        if (gamer.path->sites[index].population != 1) {
            later = false;
        }
        if (later) {
            return index + 1;
        }
    }
    if (gamer.playersHub[gamer.playerID].money % 2 == 1) {
        while (!gamer.path->sites[index + 1].isBarrier) {
            if (gamer.path->sites[index + 1].name[0] == 'M'
                    && has_room(index + 1, gamer.path)) {
                return index + 1;
            }
            ++index;
        }
    }
    int myCards = get_my_cards_count(gamer.playerID);
    int otherMost = get_others_most_cards();
    if (myCards > otherMost || (myCards == 0 && otherMost == 0)) {
        int next = gamer.playersHub[gamer.playerID].atSite + 1; //next site
        while (!gamer.path->sites[next].isBarrier) {
            if (gamer.path->sites[next].name[0] == 'R'
                    && has_room(next, gamer.path)) {
                return next;
            }
            ++next;
        }
    }
    if (true) {
        int next = gamer.playersHub[gamer.playerID].atSite + 1; //next site
        while (!gamer.path->sites[next].isBarrier) {
            if (gamer.path->sites[next].name[0] == 'V'
                    && gamer.path->sites[next].name[1] == '2'
                    && has_room(next, gamer.path)) {
                return next;
            }
            ++next;
        }
    }
    return get_final_pick();
}
