#include <iostream>

#include "cribbage.h"
#include "ParseCommandLineArgs.h"
#include "cli.h"
#include "cribbage_random.h"

#include "help.h"

// Location of the tables directory
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
    all_discard_stats.sort(ScoreType::COMBINED, Statistic::MEAN);
    const DiscardStatistics& discard_stats =
      all_discard_stats.get_best_discard_stats();
    // std::cout << discard_stats << std::endl;

    // std::cout << gen_discard << std::endl;
}

int cli_game(const ParseGameArgs& args) {
    PlayerInfo p1_info{"Staples", PlayerType::STAT};
    PlayerInfo p2_info{"Stanley", PlayerType::HUMAN};
    std::unique_ptr<Player> p1 = PlayerFactory::create_player(p1_info);
    std::unique_ptr<Player> p2 = PlayerFactory::create_player(p2_info);

    GenerateCribStatistics gen_stats(p1.get(), p2.get());
    gen_stats.get_stat_table().load_tables(args.get_table());

    const CribDiscardProbabilities& probs = gen_stats.get_crib_discard_probs();

    Game game(p1.get(), p2.get(), probs);
    game.play_game();

    std::cout << game.get_pgn() << std::endl;
    game.get_pgn().save("game2.pgn");

    /* Test loading PGN */
    /* PGN pgn; */
    /* std::fstream pgn_file("pgn/game.pgn", std::ios::in); */
    /* std::stringstream buffer; */
    /* buffer << pgn_file.rdbuf(); */
    /* pgn.load(buffer); */
    /* std::cout << pgn << std::endl; */

    return 0;
}

int cli_pgn(const ParsePgnArgs& args) {
    if (!args.get_pgn_file_name()) {
        std::cout << "No PGN file specified. Exiting." << std::endl;
        return 1;
    }
    std::string pgn_file_name = args.get_pgn_file_name().value();
    std::cout << "Loading PGN file: " << pgn_file_name << std::endl;
    std::fstream pgn_file(pgn_file_name, std::ios::in);
    std::stringstream buffer;
    buffer << pgn_file.rdbuf();
    PGN pgn;
    bool good_load = pgn.load(buffer);
    if (!good_load) {
        std::cout << "Error loading PGN file. Exiting." << std::endl;
        return 1;
    }
    /* std::cout << pgn << std::endl; */
    PGN::ValidationType validation = pgn.validate();
    if (validation == PGN::ValidationType::FINISHED) {
        std::cout << "PGN is valid." << std::endl;
    } 
    else if (validation == PGN::ValidationType::INVALID) {
        std::cout << "PGN is invalid." << std::endl;
    }
    else {
        std::cout << "PGN is missing something. ";
        switch (validation) {
            case PGN::ValidationType::MISSING_ROUND:
                std::cout << "(Missing round)" << std::endl;
                break;
            case PGN::ValidationType::MISSING_SCORES:
                std::cout << "(Missing Scores or Bad Score)" << std::endl;
                break;
            case PGN::ValidationType::PARTIAL:
                std::cout << "(Partial)" << std::endl;
                break;
            case PGN::ValidationType::UNFINISHED:
                std::cout << "(Unfinished)" << std::endl;
                break;
            default:
                std::cout << "Unknown error." << std::endl;
                break;
        }
    }
    return 0;
}

int cli_discard_stats(const ParseDiscardStatsArgs& args) {
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
    auto top_discards = args.get_top_discards();

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
    gen_discard.generate_heuristics();

    AllDiscardStatistics all_discard_stats =
      gen_discard.get_all_discard_stats();

    if (sort_by) {
        all_discard_stats.sort(sort_by->first, sort_by->second);
    } else {
        all_discard_stats.sort();  // default sort
    }

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

    int ret;
    ParseCommandLineArgs::ParseResult command_type = args.get_command_type();

    //CribbageRandom::get_instance()->seed(3);
    if (command_type == ParseCommandLineArgs::ParseResult::HELP) {
        std::cout << help() << std::endl;
        ret = 0;
    } else if (command_type == ParseCommandLineArgs::ParseResult::GAME) {
        ret = cli_game(args.get_game_args());
    } else if (command_type == ParseCommandLineArgs::ParseResult::DISCARDS) {
        ret = cli_discard_stats(args.get_discard_stats_args());
    } else if (command_type == ParseCommandLineArgs::ParseResult::PGN) {
        ret = cli_pgn(args.get_pgn_args());
    } else if (command_type == ParseCommandLineArgs::ParseResult::ERROR) {
        std::cout << "Error parsing command. Exiting." << std::endl;
        ret = 1;
    } else {
        std::cout << "Command not implemented yet." << std::endl;
        ret = 1;
    }
    return ret;
}
