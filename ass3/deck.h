#ifndef A3_DECK_H
#define A3_DECK_H

typedef struct {
    char suit;
} Card;

typedef struct {
    int count;
    int top; // index of top card in deck
    Card *cards;
} Deck;

bool load_deck(const char *deckName, Deck *deck);
bool is_valid_suit(char suit);
void clean_deck(Deck *deck);
char draw_card(Deck *deck);

#endif //A3_DECK_H
