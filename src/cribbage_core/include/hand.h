#ifndef HAND_H
#define HAND_H

#include <string>
#include <vector>

#include "card.h"
#include "card_container.h"

namespace cribbage {

class Hand : public CardContainer {
  public:
    Hand();
    ~Hand() = default;
    Hand(std::vector<Card> cards);

    void add_card(Card card);
    void remove_card(Card card);

    void sort();
    Hand sort() const;

    std::vector<Card>& get_cards() { return cards; }
    const std::vector<Card>& get_cards() const { return cards; }

    bool contains(Card card) const;

    /*
     * @brief Gets the string representation of the hand
     *
     * Example: "AS 2D 3H 4S 5C"
     */
    std::string to_string() const;

    // Print the hand to the given output stream
    friend std::ostream& operator<<(std::ostream& os, const Hand& hand);
};

}  // namespace cribbage

#endif  // HAND_H
