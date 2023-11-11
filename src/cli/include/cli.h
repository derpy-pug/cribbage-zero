#ifndef CLI_H
#define CLI_H

#include <optional>
#include <string>

#include "cribbage.h"

int cli_main(int argc, char** argv);

class ParseCommandLineArgs {
  public:
    ParseCommandLineArgs(int argc, char** argv);

    int parse();

    std::optional<std::pair<Hand, bool>> get_hand() const;

    std::optional<Card> get_cut() const;

    std::optional<std::pair<Card, Card>> get_discards() const;

    std::pair<Statistic, ScoreType> get_sort_by() const;

    std::string get_table() const;

    int get_top_discards() const;

  private:
    int argc;
    char** argv;
    std::string hand_str;
    bool input_hand = false;
    std::string cut_str;
    bool input_cut = false;
    std::string discard_str;
    bool input_discards = false;
    std::string sort_by_str;
    bool input_sort_by = false;
    std::string table_str;
    bool input_table = false;
    int top_discards = 15;
    bool input_top_discards = false;
};

#endif  // CLI_H
