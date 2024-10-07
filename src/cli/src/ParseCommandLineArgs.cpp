#include "ParseCommandLineArgs.h"
#include <string>
#include "util/parse.h"

using namespace cribbage;

ParseCommandLineArgs::ParseCommandLineArgs(int argc, char** argv) {
    result = parse_command_type(argc, argv);
}

ParseCommandLineArgs::ParseResult ParseCommandLineArgs::parse_command_type(
  int argc, char** argv) {
    ParseResult result;
    if (argc == 1 || std::string(argv[1]) == "help" ||
        std::string(argv[1]) == "--help") {
        result = ParseResult::HELP;
    } else if (std::string(argv[1]) == "discards") {
        discard_stats = ParseDiscardStatsArgs(argc - 1, &argv[1]);
        if (discard_stats.is_good_parse()) {
            result = ParseResult::DISCARDS;
        }
        else {
            result = ParseResult::ERROR;
        }
    } else if (std::string(argv[1]) == "game") {
        game_args = ParseGameArgs(argc - 1, &argv[1]);
        if (game_args.is_good_parse()) {
            result = ParseResult::GAME;
        }
        else {
            result = ParseResult::ERROR;
        }
    } else if (std::string(argv[1]) == "play") {
        // TODO: Return a card to play during the pegging phase
        result = ParseResult::PLAY;
    } else if (std::string(argv[1]) == "pgn") {
        pgn_args = ParsePgnArgs(argc - 1, &argv[1]);
        if (pgn_args.is_good_parse()) {
            result = ParseResult::PGN;
        }
        else {
            result = ParseResult::ERROR;
        }
    } else {
        result = ParseResult::ERROR;
    }
    return result;
}

ParseDiscardStatsArgs::ParseDiscardStatsArgs(int argc, char** argv) {
    int ret = parse(argc, argv);
    good_parse = ret == 0;
}

int ParseDiscardStatsArgs::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--hand") {
            if (i + 1 < argc) {
                hand_str = argv[++i];
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
            } else {
                std::cerr << "--dealer option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "-c" || arg == "--cut") {
            if (i + 1 < argc) {
                cut_str = argv[++i];
            } else {
                std::cerr << "--cut option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "-d" || arg == "--discards") {
            if (i + 1 < argc) {
                discard_str = argv[++i];
            } else {
                std::cerr << "--discards option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "-s" || arg == "--sort-by") {
            if (i + 1 < argc) {
                sort_by_str = argv[++i];
            } else {
                std::cerr << "--sort-by option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "-t" || arg == "--table") {
            if (i + 1 < argc) {
                table_str = argv[++i];
            } else {
                std::cerr << "--table option requires one argument."
                          << std::endl;
                return 1;
            }
        } else if (arg == "--top-discards") {
            if (i + 1 < argc) {
                top_discards = std::stoi(argv[++i]);
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

std::optional<Hand> ParseDiscardStatsArgs::get_hand() const {
    if (hand_str)
        return parse_hand(*hand_str);
    return std::nullopt;
}

std::optional<bool> ParseDiscardStatsArgs::get_is_dealer() const {
    return is_dealer;
}

std::optional<Card> ParseDiscardStatsArgs::get_cut() const {
    if (cut_str)
        return Card(*cut_str);
    return std::nullopt;
}

std::optional<std::pair<Card, Card>> ParseDiscardStatsArgs::get_discards()
  const {
    if (discard_str) {
        return parse_discards(*discard_str);
    }
    return std::nullopt;
}

std::optional<std::pair<ScoreType, Statistic>>
ParseDiscardStatsArgs::get_sort_by() const {
    if (sort_by_str) {
        return parse_sort_by(*sort_by_str);
    }
    return std::nullopt;
}

std::optional<std::string> ParseDiscardStatsArgs::get_table() const {
    return table_str;
}

std::optional<int> ParseDiscardStatsArgs::get_top_discards() const {
    return top_discards;
}


// ParsePgnArgs

ParsePgnArgs::ParsePgnArgs(int argc, char** argv) {
    int ret = parse(argc, argv);
    good_parse = ret == 0;
}

int ParsePgnArgs::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc) {
                pgn_file_name = argv[++i];
            } else {
                std::cerr << arg << " requires a file name" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }
    return 0;
}



// ParseGameArgs

ParseGameArgs::ParseGameArgs(int argc, char** argv) {
    int ret = parse(argc, argv);
    good_parse = ret == 0;
}

int ParseGameArgs::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-n" || arg == "--num-games") {
            if (i + 1 < argc) {
                num_games = std::stoi(argv[++i]);
            } else {
                std::cerr << arg << " requires the number of games"
                          << std::endl;
                return 1;
            }
        } 
        else if (arg == "--output") {
            if (i + 1 < argc) {
                std::string output_type_str = argv[++i];
                if (output_type_str == "pgn") {
                    output_type = OutputType::PGN;
                } else if (output_type_str == "summary") {
                    output_type = OutputType::SUMMARY;
                } else {
                    std::cerr << arg << " invalid output type" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << arg << " requires an output type" << std::endl;
                return 1;
            }

        } 
        else if (arg == "-t" || arg == "--table") {
            if (i + 1 < argc) {
                table = argv[++i];
            } else {
                std::cerr << "--table option requires one argument."
                          << std::endl;
                return 1;
            }
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }
    return 0;
}
