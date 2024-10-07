#pragma once

#include "hand.h"
#include "statistics/stat_enums.h"

#include <optional>

class ParseDiscardStatsArgs {
  public:
    ParseDiscardStatsArgs() = default;
    ParseDiscardStatsArgs(int argc, char** argv);

    bool is_good_parse() { return good_parse; }

    std::optional<cribbage::Hand> get_hand() const;

    std::optional<bool> get_is_dealer() const;

    std::optional<cribbage::Card> get_cut() const;

    std::optional<std::pair<cribbage::Card, cribbage::Card>> get_discards()
      const;

    std::optional<std::pair<cribbage::ScoreType, cribbage::Statistic>>
    get_sort_by() const;

    std::optional<std::string> get_table() const;

    std::optional<int> get_top_discards() const;

  private:
    int parse(int argc, char** argv);

  private:
    bool good_parse;

    std::optional<std::string> hand_str;
    std::optional<bool> is_dealer;
    std::optional<std::string> cut_str;
    std::optional<std::string> discard_str;
    std::optional<std::string> sort_by_str;
    std::optional<std::string> table_str;
    std::optional<int> top_discards;
};

class ParsePgnArgs {
    public:
        ParsePgnArgs() = default;
        ParsePgnArgs(int argc, char** argv);
    
        bool is_good_parse() { return good_parse; }
    
        std::optional<std::string> get_pgn_file_name() const { return pgn_file_name; }
    
    private:
        int parse(int argc, char** argv);
    
    private:
        bool good_parse;
    
        std::optional<std::string> pgn_file_name;
};

class ParseGameArgs {
  public:
    ParseGameArgs() = default;
    ParseGameArgs(int arg, char** argv);

    bool is_good_parse() { return good_parse; }

    enum class OutputType {
        PGN,
        SUMMARY
    };

    std::optional<int> get_num_games() const { return num_games; }
    std::optional<OutputType> get_output_type() const { return output_type; }
    std::optional<std::string> get_table() const { return table; }

  private:
    int parse(int argc, char** argv);

  private:
    bool good_parse;

    std::optional<std::string> dealer_name;
    std::optional<std::string> pone_name;
    std::optional<int> num_games;
    std::optional<OutputType> output_type;
    std::optional<std::string> table;
};

class ParseCommandLineArgs {
  public:
    ParseCommandLineArgs(int argc, char** argv);

    enum class ParseResult { DISCARDS, GAME, PLAY, HELP, PGN, ERROR };

    ParseResult get_command_type() { return result; }

    ParseDiscardStatsArgs get_discard_stats_args() { return discard_stats; }
    ParseGameArgs get_game_args() { return game_args; }
    ParsePgnArgs get_pgn_args() { return pgn_args; }

  private:
    ParseResult parse_command_type(int argc, char** argv);

  private:
    ParseResult result;

    ParseDiscardStatsArgs discard_stats;
    ParseGameArgs game_args;
    ParsePgnArgs pgn_args;
};
