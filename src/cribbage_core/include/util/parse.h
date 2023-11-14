#pragma once

#include <string>
#include <utility>

#include "hand.h"
#include "statistics.h"

namespace cribbage {

Card parse_card(std::string card_str) noexcept(false);

Hand parse_hand(std::string hand_str) noexcept(false);

bool parse_is_dealer(std::string is_dealer_str) noexcept(false);

std::pair<Card, Card> parse_discards(std::string discard_str) noexcept(false);

std::pair<Statistic, ScoreType> parse_sort_by(std::string sort_by_str) noexcept(false);

}  // namespace cribbage
