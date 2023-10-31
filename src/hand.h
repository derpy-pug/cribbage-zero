#ifndef HAND_H
#define HAND_H

#include <vector>
#include <string>

#include "card.h"
#include "card_container.h"



class Hand : public CardContainer
{
public:
    Hand();
    Hand(std::vector<Card> cards);

    void add_card(Card card);
    void remove_card(Card card);

    void sort();
    Hand sort() const;

    /*
     * @brief Gets the string representation of the hand
     *
     * Example: "AS 2D 3H 4S 5C"
     */
    std::string to_string() const;

    // Print the hand to the given output stream
    friend std::ostream& operator<<(std::ostream& os, const Hand& hand);
};

#endif // HAND_H
