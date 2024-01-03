#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cli.h"
#include "cribbage_random.h"

#include "help.h"

#define TABLE_DIR std::string("tables/")

using namespace cribbage;

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

    auto p1 =
      PlayerFactory::create_player(PlayerInfo{"Staples", PlayerType::STAT});
    p1->set_hand(hand1);
    auto p2 =
      PlayerFactory::create_player(PlayerInfo{"Stanley", PlayerType::STAT});
    p2->set_hand(hand2);

    GenerateCribStatistics gen_stats(p1.get(), p2.get());
    /* std::string dirname = TABLE_DIR + std::string("stat_vs_stat/"); */
    gen_stats.get_stat_table().load_tables(std::nullopt);

    CribDiscardProbabilities probs = gen_stats.get_crib_discard_probs();

    std::cout << "Hand: " << hand1 << std::endl;
    GenerateDiscardStatistics gen_discard(p1.get(), false, probs);
    gen_discard.generate_all_discard_stats();

    AllDiscardStatistics all_discard_stats =
      gen_discard.get_all_discard_stats();
    all_discard_stats.sort_discard_stats(ScoreType::COMBINED, Statistic::MEAN);
    const DiscardStatistics& discard_stats =
      all_discard_stats.get_best_discard_stats();
    // std::cout << discard_stats << std::endl;

    // std::cout << gen_discard << std::endl;
}

ParseCommandLineArgs::ParseCommandLineArgs(int argc, char** argv)
    : argc(argc), argv(argv) {
    int ret = parse();
    if (ret == 2) {
        std::cout << help() << std::endl;
        exit(0);
    } else if (ret != 0) {
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
                std::cerr << "--hand option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "--dealer") {
            if (i + 1 < argc) {
                std::string dealer_str = argv[++i];
                // Lower case
                is_dealer = parse_is_dealer(dealer_str);
                input_is_dealer = true;
            } else {
                std::cerr << "--dealer option requires one argument."
                          << std::endl;
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
                std::cerr << "--discards option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "-s" || arg == "--sort-by") {
            if (i + 1 < argc) {
                sort_by_str = argv[++i];
                input_sort_by = true;
            } else {
                std::cerr << "--sort-by option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "-t" || arg == "--table") {
            if (i + 1 < argc) {
                table_str = argv[++i];
                input_table = true;
            } else {
                std::cerr << "--table option requires one argument."
                          << std::endl;
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
        } else if (arg == "--help") {
            std::cout << help() << std::endl;
            return 2;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }
    return 0;
}

std::optional<Hand> ParseCommandLineArgs::get_hand() const {
    if (!input_hand) {
        return std::nullopt;
    }
    return parse_hand(hand_str);
}

std::optional<bool> ParseCommandLineArgs::get_is_dealer() const {
    if (!input_is_dealer) {
        return std::nullopt;
    }
    return std::optional<bool>(is_dealer);
}

std::optional<Card> ParseCommandLineArgs::get_cut() const {
    if (!input_cut) {
        return std::nullopt;
    }
    return Card(cut_str);
}

std::optional<std::pair<Card, Card>> ParseCommandLineArgs::get_discards()
  const {
    if (!input_discards) {
        return std::nullopt;
    }
    return parse_discards(discard_str);
}

std::pair<Statistic, ScoreType> ParseCommandLineArgs::get_sort_by() const {
    if (!input_sort_by) {
        return {Statistic::MEAN, ScoreType::COMBINED};
    }
    return parse_sort_by(sort_by_str);
}

std::optional<std::string> ParseCommandLineArgs::get_table() const {
    if (!input_table) {
        return std::nullopt;
    }
    return table_str;
}

int ParseCommandLineArgs::get_top_discards() const {
    if (!input_top_discards) {
        return 15;
    }
    return top_discards;
}

int cli_game(const ParseCommandLineArgs& args) {
    PlayerInfo p1_info{"Staples", PlayerType::STAT};
    PlayerInfo p2_info{"Stanley", PlayerType::STAT};
    std::unique_ptr<Player> p1 = PlayerFactory::create_player(p1_info);
    std::unique_ptr<Player> p2 = PlayerFactory::create_player(p2_info);

    GenerateCribStatistics gen_stats(p1.get(), p2.get());
    gen_stats.get_stat_table().load_tables(args.get_table());

    CribDiscardProbabilities probs = gen_stats.get_crib_discard_probs();

    Game game(p1.get(), p2.get(), probs);
    game.play_game();

    /* std::cout << game.get_pgn() << std::endl; */
    /* game.get_pgn().save("game.pgn"); */

    PGN pgn;
    std::fstream pgn_file("pgn/game.pgn", std::ios::in);
    std::stringstream buffer;
    buffer << pgn_file.rdbuf();
    pgn.load(buffer);
    std::cout << pgn << std::endl;

    return 0;
}

int cli_discard_stats(const ParseCommandLineArgs& args) {
    auto hand_input = args.get_hand();
    auto is_dealer_input = args.get_is_dealer();
    Hand hand;
    if (!hand_input) {
        std::cout << "No hand specified. Generating random hand." << std::endl;
        Deck deck;
        deck.shuffle();
        hand = deck.deal_hand(6);
        hand.sort();
    } else {
        hand = hand_input.value();
    }
    bool is_dealer;
    if (!is_dealer_input) {
        std::cout << "No dealer specified. Generating random dealer."
                  << std::endl;
        is_dealer = CribbageRandom::get_instance()->get_bool();
    } else {
        is_dealer = is_dealer_input.value();
    }
    auto cut = args.get_cut();
    auto discards = args.get_discards();
    auto sort_by = args.get_sort_by();
    auto table = args.get_table();
    int top_discards = args.get_top_discards();

    PlayerInfo p1_info{"Staples", PlayerType::STAT};
    PlayerInfo p2_info{"Stanley", PlayerType::STAT};
    std::unique_ptr<Player> p1 = PlayerFactory::create_player(p1_info);
    std::unique_ptr<Player> p2 = PlayerFactory::create_player(p2_info);

    GenerateCribStatistics gen_stats(p1.get(), p2.get());
    int num_tables_loaded = gen_stats.get_stat_table().load_tables(table);
    /* if (num_tables_loaded == 0) { */
    /*   std::cout << "No tables found. Generating tables." << std::endl; */
    /*   gen_stats.generate_probabilities(); */
    /*   gen_stats.save_tables(dirname); */
    /* } */
    if (num_tables_loaded == 0) {
        std::cout << "No tables found. Exiting." << std::endl;
        return 1;
    }
    CribDiscardProbabilities probs = gen_stats.get_crib_discard_probs();

    p1->set_hand(hand);
    std::cout << "Hand: " << hand << std::endl;
    std::cout << "Your Crib: " << (is_dealer ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    GenerateDiscardStatistics gen_discard(p1.get(), is_dealer, probs);
    gen_discard.generate_all_discard_stats(cut);

    AllDiscardStatistics all_discard_stats =
      gen_discard.get_all_discard_stats();

    all_discard_stats.sort_discard_stats(sort_by.second, sort_by.first);

    /* const DiscardStatistics& discard_stats_best = */
    /*     gen_discard.get_best_discard_stats(); */

    if (discards) {
        const DiscardStatistics& discard_stats =
          all_discard_stats.get_discard_stats(discards->first,
                                              discards->second);
        std::cout << discard_stats << std::endl;
    } else {
        std::cout << all_discard_stats.get_discard_stats_string(top_discards)
                  << std::endl;
    }

    // Simulate discards
    /* GenerateDiscardStatistics gen_discard_sim(p1, is_dealer, probs); */
    /* gen_discard_sim.generate_discard_stats(cut, true, p2); */
    /* gen_discard_sim.sort_discard_stats(sort_by.second, sort_by.first); */

    /* if (discards) { */
    /*     const DiscardStatistics& discard_stats = */
    /*         gen_discard_sim.get_discard_stats(discards->first, */
    /*                                           discards->second); */
    /*     std::cout << discard_stats << std::endl; */
    /* } else { */
    /*     std::cout << gen_discard_sim.get_discard_stats_string(top_discards) */
    /*               << std::endl; */
    /* } */

    return 0;
}

/*
 * Command line arguments:
 *  -h, --hand HAND
 *      HAND is "AH 2H 3H 4H 5H 6H" for example.
 *      6 cards, not necessarily separated by spaces.
 *      Each card is a rank followed by a suit (S, C, D, H)
 *      Rank: A, 1, 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K
 *      Examples: 1H, AH, 2H, TH, TH, JH, QH, KH
 *      A and 1 is the same thing.
 *      Leave it blank for a random hand.
 *
 *  --dealer DEALER
 *      DEALER is a truthy value (1, true, True, T, t, yes, Yes, Y, y)
 *      or a falsey value (0, false, False, F, f, no, No, N, n)
 *      If you don't specify a dealer, it will be random.
 *
 *  -c, --cut CUT
 *      If you know the cut card, you can specify it here.
 *      CUT is a rank followed by a suit (S, C, D, H)
 *
 *  -d, --discards DISCARDS
 *      DISCARDS is two cards like "AH 2H"
 *      Can ONLY be used if you specify the hand
 *
 *  -s, --sort-by SORT_BY
 *      SORT_BY is the following:
 *       - "<statistic> <score_type>"
 *       - <statistic> is one of "max", "min", "mean", "median", "variance",
 * "std_dev"
 *       - <score_type> is one of "hand", "crib", "combined"
 *
 *  -t, --table TABLE
 *    TABLE is the name of the table to load.
 *    If you don't specify a table, it will look in the default directory.
 *    If it doesn't find a table, it will generate one.
 *
 *  --top-discards TOP_DISCARDS
 *      TOP_DISCARDS is the number of top discards to print.
 *      Must be between 1 and 15.
 *
 *  --help
 *      Print this help message.
 *
 *
 *  Examples:
 *      most used:
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H" --dealer Y
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H" --dealer 0 -c --top-discards=3
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H" --dealer N -d "AH 2H"
 *
 *      other:
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H" --dealer 1 -c "7H" -d "AH 2H" -s "median
 * combined"
 *      ./cribbage -h "AH 2H 3H 4H 5H 6H" -c "7H" -d "AH 2H" -s "mean hand" -t
 * "stat_vs_stat"
 *
 */

int cli_main(int argc, char** argv) {

    ParseCommandLineArgs args(argc, argv);

    int ret = cli_discard_stats(args);
    /* int ret = cli_game(args); */

    return ret;
}
