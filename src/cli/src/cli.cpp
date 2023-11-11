#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>

#include "cli.h"
#include "cribbage_random.h"

#define TABLE_DIR std::string("../tables/")

void test_hand_deck() {
  Hand hand;
  Deck deck;
  deck.shuffle();
  hand.add_card(deck.deal_card());
  hand.add_card(deck.deal_card());
  hand.add_card(deck.deal_card());
  hand.add_card(deck.deal_card());
  hand.sort();
  Card cut = deck.deal_card();
  std::cout << "Hand: " << hand << std::endl;
  std::cout << "Cut: " << cut << std::endl;
  std::cout << "Score: " << score_hand(hand, cut, false) << std::endl;
}

void test_hand() {
  Hand hand;
  hand.add_card(Card(Suit::CLUBS, Rank::THREE));
  hand.add_card(Card(Suit::HEARTS, Rank::FOUR));
  hand.add_card(Card(Suit::HEARTS, Rank::FIVE));
  hand.add_card(Card(Suit::SPADES, Rank::KING));
  hand.sort();
  Card cut = Card(Suit::HEARTS, Rank::KING);
  std::cout << "Hand: " << hand << std::endl;
  std::cout << "Cut: " << cut << std::endl;
  std::cout << "Score: " << score_hand(hand, cut, false) << std::endl;
}
void test_stats() {
  Deck deck;
  deck.shuffle();
  Hand hand1 = deck.deal_hand(6);
  hand1.sort();
  Hand hand2 = deck.deal_hand(6);
  hand2.sort();

  Player *p1 = new StatPlayer("Staples");
  p1->set_hand(hand1);
  Player *p2 = new StatPlayer("Stanley");
  p2->set_hand(hand2);

  GenerateCribStatistics gen_stats(p1, p2);
  std::string dirname = TABLE_DIR + std::string("stat_vs_stat/");
  gen_stats.load_tables(dirname);

  std::cout << "Hand: " << hand1 << std::endl;
  GenerateDiscardStatistics gen_discard(p1, false, &gen_stats);
  gen_discard.generate_discard_stats();
  gen_discard.sort_discard_stats(ScoreType::COMBINED, Statistic::MEAN);
  const DiscardStatistics &discard_stats = gen_discard.get_best_discard_stats();
  // std::cout << discard_stats << std::endl;

  // std::cout << gen_discard << std::endl;
}

/*
 * @breif Parse a string into a Hand and a is_dealer bool
 *
 * @param hand_str A string of cards possibly separated by spaces
 *              and ending with a Y or N.
 *
 * @return A pair of a Hand and a bool.
 */
std::pair<Hand, bool> string_to_hand(std::string hand_str) {
    std::transform(hand_str.begin(), hand_str.end(), hand_str.begin(),
            ::toupper);
    // Remove possible spaces
    hand_str.erase(std::remove(hand_str.begin(), hand_str.end(), ' '),
            hand_str.end());
    if (hand_str.size() != 13) {
        throw std::invalid_argument("Hand string must be 13 characters");
    }
    std::stringstream ss(hand_str);
    std::string card_str;
    Hand hand;
    // every two characters is a card
    for (int i = 0; i < 6; i++) {
        card_str = hand_str.substr(i * 2, 2);
        hand.add_card(Card(card_str));
    }
    bool is_dealer = hand_str[12] == 'Y';
    return {hand, is_dealer};
}

std::pair<Card, Card> string_to_discards(std::string discard_str) {
    std::transform(discard_str.begin(), discard_str.end(), discard_str.begin(),
            ::toupper);
    // Remove possible spaces
    discard_str.erase(std::remove(discard_str.begin(), discard_str.end(), ' '),
            discard_str.end());
    if (discard_str.size() != 4) {
        throw std::invalid_argument("Discard string must be 4 characters");
    }
    std::stringstream ss(discard_str);
    std::string card_str;
    Card card1 = Card(discard_str.substr(0, 2));
    Card card2 = Card(discard_str.substr(2, 2));
    return {card1, card2};
}

std::pair<Statistic, ScoreType> string_to_sort_by(std::string sort_by_str) {
    std::transform(sort_by_str.begin(), sort_by_str.end(), sort_by_str.begin(),
            ::tolower);
    std::stringstream ss(sort_by_str);
    std::string stat_str;
    std::string score_type_str;
    ss >> stat_str;
    ss >> score_type_str;
    Statistic stat;
    ScoreType score_type;
    if (stat_str == "max") {
        stat = Statistic::MAX;
    } else if (stat_str == "min") {
        stat = Statistic::MIN;
    } else if (stat_str == "mean") {
        stat = Statistic::MEAN;
    } else if (stat_str == "median") {
        stat = Statistic::MEDIAN;
    } else if (stat_str == "variance") {
        stat = Statistic::VARIANCE;
    } else if (stat_str == "std_dev") {
        stat = Statistic::STD_DEV;
    } else {
        throw std::invalid_argument("Invalid statistic");
    }

    if (score_type_str == "hand") {
        score_type = ScoreType::HAND;
    } else if (score_type_str == "crib") {
        score_type = ScoreType::CRIB;
    } else if (score_type_str == "combined") {
        score_type = ScoreType::COMBINED;
    } else {
        throw std::invalid_argument("Invalid score type");
    }

    return {stat, score_type};
}

ParseCommandLineArgs::ParseCommandLineArgs(int argc, char **argv) : argc(argc), argv(argv) {
    int ret = parse();
    if (ret != 0) {
        throw std::invalid_argument("Invalid command line arguments");
    }
}

int ParseCommandLineArgs::parse() {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--hand") {
            if (i + 1 < argc) {
                hand_str = argv[++i];
                input_hand = true;
            } else {
                std::cerr << "--hand option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "-c" || arg == "--cut") {
            if (i + 1 < argc) {
                cut_str = argv[++i];
                input_cut = true;
            } else {
                std::cerr << "--cut option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "-d" || arg == "--discards") {
            if (i + 1 < argc) {
                discard_str = argv[++i];
                input_discards = true;
            } else {
                std::cerr << "--discards option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "-s" || arg == "--sort-by") {
            if (i + 1 < argc) {
                sort_by_str = argv[++i];
                input_sort_by = true;
            } else {
                std::cerr << "--sort-by option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "-t" || arg == "--table") {
            if (i + 1 < argc) {
                table_str = argv[++i];
                input_table = true;
            } else {
                std::cerr << "--table option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--top-discards") {
            if (i + 1 < argc) {
                top_discards = std::stoi(argv[++i]);
                input_top_discards = true;
            } else {
                std::cerr << "--top-discards option requires one argument."
                    << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }
    return 0;
}

std::optional<std::pair<Hand, bool>> ParseCommandLineArgs::get_hand() const {
    if (!input_hand) {
        return std::nullopt;
    }
    return string_to_hand(hand_str);
}

std::optional<Card> ParseCommandLineArgs::get_cut() const {
    if (!input_cut) {
        return std::nullopt;
    }
    return Card(cut_str);
}

std::optional<std::pair<Card, Card>> ParseCommandLineArgs::get_discards() const {
    if (!input_discards) {
        return std::nullopt;
    }
    return string_to_discards(discard_str);
}

std::pair<Statistic, ScoreType> ParseCommandLineArgs::get_sort_by() const {
    if (!input_sort_by) {
        return {Statistic::MEAN, ScoreType::COMBINED};
    }
    return string_to_sort_by(sort_by_str);
}

std::string ParseCommandLineArgs::get_table() const {
    if (!input_table) {
        return "stat_vs_stat/";
    }
    return table_str;
}

int ParseCommandLineArgs::get_top_discards() const {
    if (!input_top_discards) {
        return 15;
    }
    return top_discards;
}

/*
 * Command line arguments:
 *  -h, --hand=HAND
 *      HAND is "AH 2H 3H 4H 5H 6H <Y/N>" for example.
 *      6 cards, not necessarily separated by spaces, and a Y/N.
 *      Each card is a rank followed by a suit (S, C, D, H)
 *      Rank: A, 1, 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K
 *      Examples: 1H, AH, 2H, TH, TH, JH, QH, KH
 *      A and 1 is the same thing.
 *      The last character signifies whether you are the dealer or not.
 *      Leave it blank for a random hand and dealer.
 *
 *  -c, --cut=CUT
 *      If you know the cut card, you can specify it here.
 *      CUT is a rank followed by a suit (S, C, D, H)
 *
 *  -d, --discards=DISCARDS
 *      DISCARDS is two cards like "AH 2H"
 *      Can ONLY be used if you specify the hand
 *
 *  -s, --sort-by=SORT_BY
 *      SORT_BY is the following:
 *       - "<statistic> <score_type>"
 *       - <statistic> is one of "max", "min", "mean", "median", "variance",
 * "std_dev"
 *       - <score_type> is one of "hand", "crib", "combined"
 *
 *  -t, --table=TABLE
 *    TABLE is the name of the table to load.
 *    If you don't specify a table, it will look in the default directory.
 *    If it doesn't find a table, it will generate one.
 *
 *  --top-discards=TOP_DISCARDS
 *      TOP_DISCARDS is the number of top discards to print.
 *      Must be between 1 and 15.
 *
 *
 *  Examples:
 *      most used:
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H Y"
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -c --top-discards=3
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -d "AH 2H"
 *
 *      other:
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -c "7H" -d "AH 2H" -s "median
 * combined"
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -c "7H" -d "AH 2H" -s "mean hand" -t
 * "stat_vs_stat"
 *
 */

int cli_main(int argc, char **argv) {
  ParseCommandLineArgs args(argc, argv);
  auto hand_input = args.get_hand();
  Hand hand;
  bool is_dealer;
  if (!hand_input) {
    std::cout << "No hand specified. Generating random hand." << std::endl;
    Deck deck;
    deck.shuffle();
    hand = deck.deal_hand(6);
    hand.sort();
    is_dealer = CribbageRandom::get_instance()->get_bool();

  } else {
    hand = hand_input->first;
    is_dealer = hand_input->second;
  }
  auto cut = args.get_cut();
  auto discards = args.get_discards();
  auto sort_by = args.get_sort_by();
  std::string table = args.get_table();
  int top_discards = args.get_top_discards();

  Player *p1 = new StatPlayer("Staples");
  p1->set_hand(hand);

  GenerateCribStatistics gen_stats(p1, nullptr);
  std::string dirname = TABLE_DIR + std::string(table);
  gen_stats.load_tables(dirname);

  std::cout << "Hand: " << hand << std::endl;
  GenerateDiscardStatistics gen_discard(p1, is_dealer, &gen_stats);
  gen_discard.generate_discard_stats(cut);
  gen_discard.sort_discard_stats(sort_by.second, sort_by.first);
  const DiscardStatistics &discard_stats = gen_discard.get_best_discard_stats();
  // std::cout << discard_stats << std::endl;
  std::cout << gen_discard.get_discard_stats_string(top_discards) << std::endl;
  return 0;
}
