#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deck.h"

#define MIN_DECK_SIZE 4

/*
 * Load and initialize a deck from a given file
 *
 * deckName: a file name where load deck from
 * deck: a pointer where store cards to
 *
 * returns: true if load deck successfully, false if not
 */
bool load_deck(const char *deckName, Deck *deck) {
    FILE *file = fopen(deckName, "r");
    if (!file) {
        return false;
    }
    int cardsCount, value;
    char suit;
    value = fscanf(file, "%d", &cardsCount);
    if ((value != 1) || cardsCount < MIN_DECK_SIZE) {
        return false;
    }
    deck->cards = (Card *)malloc(sizeof(Card) * cardsCount);
    memset(deck->cards, 0, sizeof(Card) * cardsCount);
    for (int i = 0; i < cardsCount; ++i) {
        if ((fscanf(file, "%c", &suit) != 1) || !is_valid_suit(suit)) {
            clean_deck(deck);
            return false;
        }
        deck->cards[i].suit = suit;
    }
    if (fgetc(file) != '\n' || fgetc(file) != EOF) {
        clean_deck(deck);
        return false;
    }
    deck->count = cardsCount;
    deck->top = 0;
    return true;
}

/*
 * Verify a given card's suit is a valid suit
 *
 * suit: a given card's suit
 *
 * returns: true if it is a valid suit, false if not
 */
bool is_valid_suit(char suit) {
    if ((suit < 'A') || (suit > 'E')) {
        return false;
    }
    return true;
}

/*
 * Release memory held by deck->cards, initialize deck parameters
 *
 * deck: a deck which is going to be initialized
 */
void clean_deck(Deck *deck) {
    if (deck->cards) {
        free(deck->cards);
        deck->cards = 0;
    }
    deck->count = 0;
    deck->top = 0;
}

/*
 * Draw a card from deck list, if runs out of cards, go back to beginning
 *
 * deck: the deck where player draws a card from
 *
 * returns: the card drawn by player
 */
char draw_card(Deck *deck) {
    int top = deck->top;
    char card = deck->cards[top].suit;
    deck->top = (top + 1) % deck->count; // draw card recurrently
    return card;
}