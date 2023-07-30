#include "cards.h"
#include "player.h"
#include "board.h"


void discard_turn(Hand* hand, Hand* crib, PlayerInfo* player, char is_my_crib, Board board);

void discard_into_crib(Hand* hand, Hand* crib, const int discards[2]);

char play_turn(Hand hand, CardPile pile, PlayerInfo* player, Card cut, Board board);

typedef enum {
    NO_GO,
    GO,
    GO_TWICE
}GoState;
GoState get_go_state(const Hand hand, const CardPile pile);

typedef enum {
    NO_WINNER,
    DEALER,
    PONE,
} Winner;
Winner play_phase(const Hand hand1, const Hand hand2, PlayerInfo* dealer, PlayerInfo* pone, Card cut, Board* board);

Winner cribbage_round(const Card* deck, PlayerInfo* dealer, PlayerInfo* pone, Board* board);

Winner cribbage_game(Card* deck, PlayerInfo* player1, PlayerInfo* player2);
