#ifndef CARD_CONTAINER_H
#define CARD_CONTAINER_H

#include <vector>
#include <string> 

#include "card.h"

class CardContainer
{
 public:
    /*
     * @brief Gets the string representation of the hand
     *
     * Example: "AS 2D 3H 4S 5C"
     */
    virtual std::string to_string() const {
        std::string str;
        for (const Card& card : cards) {
            str += card.to_string() + " ";
        }
        return str;
    }

    // Print the hand to the given output stream
    friend std::ostream& operator<<(std::ostream& os, const CardContainer& container) {
        os << container.to_string();
        return os;
    }

    Card& operator[](std::size_t idx) { return cards[idx]; }
    Card operator[](std::size_t idx) const { return cards[idx]; }

    std::size_t size() const { return cards.size(); }
    bool empty() const { return cards.empty(); }



    std::vector<Card>::iterator begin() { return cards.begin(); }
    std::vector<Card>::iterator end() { return cards.end(); }
    std::vector<Card>::const_iterator begin() const { return cards.begin(); }
    std::vector<Card>::const_iterator end() const { return cards.end(); }
    std::vector<Card>::const_iterator cbegin() const { return cards.cbegin(); }
    std::vector<Card>::const_iterator cend() const { return cards.cend(); }

    std::vector<Card>::reverse_iterator rbegin() { return cards.rbegin(); }
    std::vector<Card>::reverse_iterator rend() { return cards.rend(); }
    std::vector<Card>::const_reverse_iterator rbegin() const { return cards.rbegin(); }
    std::vector<Card>::const_reverse_iterator rend() const { return cards.rend(); }
    std::vector<Card>::const_reverse_iterator crbegin() const { return cards.crbegin(); }
    std::vector<Card>::const_reverse_iterator crend() const { return cards.crend(); }

 protected:
  std::vector<Card> cards;
};

#endif // CARD_CONTAINER_H
