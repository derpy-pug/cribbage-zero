#ifndef HAND_H
#define HAND_H

#include <vector>
#include <string>

#include "card.h"

class Hand
{
public:
    Hand();
    Hand(std::vector<Card> cards);

    void add_card(Card card);
    void remove_card(Card card);

    void sort();
    Hand sort() const;

    /*
     * @brief get the begin iterator for the hand
     */
    std::vector<Card>::const_iterator begin() const;
    std::vector<Card>::iterator begin();

    /*
     * @brief get the end iterator for the hand
     */
    std::vector<Card>::const_iterator end() const;
    std::vector<Card>::iterator end();


    /*
     * @brief Returns the number of cards in the hand
     *
     * @return The number of cards in the hand
     */
    int size() const;

    /*
     * @brief Gets the string representation of the hand
     *
     * Example: "AS 2D 3H 4S 5C"
     */
    std::string to_string() const;

    // Print the hand to the given output stream
    friend std::ostream& operator<<(std::ostream& os, const Hand& hand);
private:
    std::vector<Card> cards;

};

#endif // HAND_H
