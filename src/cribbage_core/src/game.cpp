#include "game.h"
#include "scoring.h"

namespace cribbage {

Game::Game(Player* player1, Player* player2, GenerateCribStatistics* gen_crib_stats)
    : player1(player1),
      player2(player2),
      gen_crib_stats(gen_crib_stats),
      round(),
      pgn(),
      deck(),
      board(),
      round_number(0) {
    deck.shuffle();
}

bool Game::play_game() {
    bool winner = false;

    GamePgn::GameInfo game_info;
    game_info.first_dealer_name = player1->get_name();
    game_info.first_pone_name = player2->get_name();
    game_info.first_dealer_type = player1->get_type();
    game_info.first_pone_type = player2->get_type();
    pgn.set_game_info(game_info);

    while (true) {
        play_round();
        if (board.get_winner().has_value()) {
            winner = board.get_winner().value() == WhichPlayer::FIRST_DEALER;
            break;
        }
    }

    GamePgn::GameResult result = winner ? GamePgn::GameResult::FIRST_DEALER
                                        : GamePgn::GameResult::FIRST_PONE;
    game_info.result = result;
    pgn.set_game_info(game_info);

    return winner;
}

void Game::play_round() {
    round_number++;

    deal();
    discard();

    Card cut = deck.deal_card();
    round.cut = cut;

    bool winner = pegging();
    if (winner) {
        return;
    }
    score();

    pgn.add_round(round);
}

void Game::deal() {
    deck.shuffleTopCardsIntoDeck();

    Hand hand1 = deck.deal_hand(6);
    Hand hand2 = deck.deal_hand(6);

    round.hand1 = hand1;
    round.hand2 = hand2;

    player1->set_hand(hand1);
    player2->set_hand(hand2);
}

void Game::discard() {
    std::pair<Card, Card> discards1 =
      player1->make_discards(is_player1_dealer(), gen_crib_stats);
    std::pair<Card, Card> discards2 =
      player2->make_discards(!is_player1_dealer(), gen_crib_stats);

    round.discards1 = discards1;
    round.discards2 = discards2;
}

bool Game::pegging() {
    Card cut = round.cut.value();
    CardPile pegging_cards;

    bool is_player1_turn = !is_player1_dealer();
    bool are_cards_left =
      player1->get_hand().size() > 0 && player2->get_hand().size() > 0;
    bool go = false;
    while (are_cards_left) {
        const Hand& hand =
          is_player1_turn ? player1->get_hand() : player2->get_hand();
        if (!can_play_any_card(hand, pegging_cards)) {
            if (go) {
                // Double go (both players cannot play a card)
                bool winner = score_pegging(is_player1_turn, pegging_cards, true);
                if (winner) {
                    return true;
                }
                pegging_cards.next_round();
                go = false;
            } else {
                // Go
                go = true;
            }
        } else {
            go = false;

            // Play a card
            Card card = play_card(is_player1_turn ? player1 : player2,
                                  is_player1_turn ? round.hand1.value()
                                                  : round.hand2.value(),
                                  cut,
                                  pegging_cards);
            pegging_cards.add_card(card);
            bool winner = score_pegging(is_player1_turn, pegging_cards);
            if (winner) {
                return true;
            }
        }

        is_player1_turn = !is_player1_turn;
        are_cards_left =
          player1->get_hand().size() > 0 && player2->get_hand().size() > 0;
    }
    // Last card
    bool winner = score_pegging(!is_player1_turn, pegging_cards, true);
    if (winner) {
        return true;
    }

    round.pegging_cards = pegging_cards;
    return false;
}

bool Game::can_play_any_card(const Hand& hand,
                             const CardPile& pegging_cards) const {
    for (const Card& card : hand) {
        if (pegging_cards.can_play_card(card)) {
            return true;
        }
    }
    return false;
}

Card Game::play_card(Player* player, const Hand& dealt_hand, Card cut,
                     const CardPile& pegging_cards) {
    Card card = player->play_card(pegging_cards, dealt_hand, cut);
    player->get_hand().remove_card(card);
    return card;
}

bool Game::score_pegging(bool is_player1_turn, const CardPile& pegging_cards, bool is_go) {
    if (is_go) {
        if (is_player1_turn) {
            board.move(WhichPlayer::FIRST_DEALER, 1);
        } else {
            board.move(WhichPlayer::FIRST_PONE, 1);
        }
        return board.get_winner().has_value();
    }

    int score = pegging_cards.score_pile();
    if (is_player1_turn) {
        board.move(WhichPlayer::FIRST_DEALER, score);
        /* round.pegging_scores.value().push_back(score); */
        /* round.pegging_player.push_back(true); */
    } else {
        board.move(WhichPlayer::FIRST_PONE, score);
        /* round.pegging_scores.value().push_back(score); */
        /* round.pegging_player.push_back(false); */
    }
    return board.get_winner().has_value();
}

void Game::score() {
    Card cut = round.cut.value();
    Hand hand1 = round.hand1.value();
    hand1.remove_card(round.discards1.value().first);
    hand1.remove_card(round.discards1.value().second);
    int score = score_hand(hand1, cut, false);
    board.move(WhichPlayer::FIRST_DEALER, score);
    if (board.get_winner().has_value()) {
        return;
    }
    /* round.hand1_score = score; */


    Hand hand2 = round.hand2.value();
    hand2.remove_card(round.discards2.value().first);
    hand2.remove_card(round.discards2.value().second);
    score = score_hand(hand2, cut, false);
    board.move(WhichPlayer::FIRST_PONE, score);
    if (board.get_winner().has_value()) {
        return;
    }
    /* round.hand2_score = score; */

    Hand crib;
    crib.add_card(round.discards1.value().first);
    crib.add_card(round.discards1.value().second);
    crib.add_card(round.discards2.value().first);
    crib.add_card(round.discards2.value().second);
    score = score_hand(crib, cut, true);
    board.move(WhichPlayer::FIRST_DEALER, score);
    /* round.crib_score = score; */
}

}  // namespace cribbage
