#include "game.h"
#include "scoring.h"

namespace cribbage {

static const bool TWO_PLAYER = true; // Two player cribbage or Three player cribbage

Game::Game(Player* player1, Player* player2,
           const CribDiscardProbabilities& crib_discard_probs)
    : player1(player1),
      player2(player2),
      crib_discard_probs(crib_discard_probs),
      round(),
      pgn(),
      deck(),
      board(),
      round_number(0) {
    deck.shuffle();
}

bool Game::play_game() {
    bool winner = false;

    PGN::GameInfo game_info;
    game_info.first_dealer_name = player1->get_info().get_name();
    game_info.first_pone_name = player2->get_info().get_name();
    game_info.first_dealer_type = player1->get_info().get_type();
    game_info.first_pone_type = player2->get_info().get_type();
    pgn.set_game_info(game_info);

    while (true) {
        play_round();
        if (board.get_winner().has_value()) {
            winner = board.get_winner().value() == WhichPlayer::FIRST_DEALER;
            break;
        }
    }

    PGN::GameResult result = winner ? PGN::GameResult::FIRST_DEALER
                                        : PGN::GameResult::FIRST_PONE;
    game_info.result = result;
    pgn.set_game_info(game_info);

    return winner;
}

void Game::play_round() {
    round_number++;
    round = PGN::Round();
    round.round_number = round_number;

    deal();
    discard();

    Card cut = deck.deal_card();
    round.cut = cut;
    if (cut.get_rank() == Rank::JACK) {
        board.move(is_player1_dealer() ? WhichPlayer::FIRST_DEALER
                                       : WhichPlayer::FIRST_PONE,
                   2);
        round.cut_score = 2;
    }

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

    hand1.sort();
    hand2.sort();

    round.hand1 = hand1;
    round.hand2 = hand2;

    player1->set_hand(hand1);
    player2->set_hand(hand2);
}

void Game::discard() {
    std::pair<Card, Card> discards1 =
      player1->make_discards(is_player1_dealer(), crib_discard_probs);
    std::pair<Card, Card> discards2 =
      player2->make_discards(!is_player1_dealer(), crib_discard_probs);

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
                bool winner =
                  score_pegging(is_player1_turn, pegging_cards, true);
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
            const Hand& dealt_hand =
              is_player1_turn ? round.hand1.value() : round.hand2.value();
            Player* player = is_player1_turn ? player1 : player2;
            Card card = player->play_card(pegging_cards, dealt_hand, cut);

            pegging_cards.add_card(card);
            bool winner = score_pegging(is_player1_turn, pegging_cards);
            if (winner) {
                return true;
            }
        }

        is_player1_turn = !is_player1_turn;
        are_cards_left =
          player1->get_hand().size() > 0 || player2->get_hand().size() > 0;
    }
    round.pegging_cards = pegging_cards;

    // Last card
    bool winner = score_pegging(!is_player1_turn, pegging_cards, true);
    if (winner) {
        return true;
    }

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

// The pegging_cards has a get_scores() that could be used instead
bool Game::score_pegging(bool is_player1_turn, const CardPile& pegging_cards,
                         bool is_go) {
    if (!round.pegging_player.has_value()) {
        round.pegging_player = std::vector<bool>(8, false);
    }
    if (!round.pegging_scores.has_value()) {
        round.pegging_scores = std::vector<int>();
        round.pegging_scores->reserve(8);
    }

    if (is_go) {
        if (TWO_PLAYER && round.pegging_scores->size() != 8) {
            round.pegging_scores->back() += 1;
        }
        WhichPlayer player =
          is_player1_turn ? WhichPlayer::FIRST_DEALER : WhichPlayer::FIRST_PONE;
        board.move(player, 1);
        return board.get_winner().has_value();
    }

    int score = pegging_cards.score_pile();
    round.pegging_scores->push_back(score);
    if (is_player1_turn) {
        board.move(WhichPlayer::FIRST_DEALER, score);
        round.pegging_player->push_back(true);
    } else {
        board.move(WhichPlayer::FIRST_PONE, score);
        round.pegging_player->push_back(false);
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
    round.hand1_score = score;
    if (board.get_winner().has_value()) {
        return;
    }

    Hand hand2 = round.hand2.value();
    hand2.remove_card(round.discards2.value().first);
    hand2.remove_card(round.discards2.value().second);
    score = score_hand(hand2, cut, false);
    board.move(WhichPlayer::FIRST_PONE, score);
    round.hand2_score = score;
    if (board.get_winner().has_value()) {
        return;
    }

    Hand crib;
    crib.add_card(round.discards1.value().first);
    crib.add_card(round.discards1.value().second);
    crib.add_card(round.discards2.value().first);
    crib.add_card(round.discards2.value().second);
    score = score_hand(crib, cut, true);
    board.move(WhichPlayer::FIRST_DEALER, score);
    round.crib_score = score;
}

}  // namespace cribbage
