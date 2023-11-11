#ifndef BOARD_H
#define BOARD_H

#include <optional>

enum class Player { ONE = 1, TWO };

/*
 * @brief Represents a cribbage board
 *
 * A cribbage board is 121 holes long, with 2 pegs for each player.
 */
class Board {
  public:
    Board();

    /*
     * @brief Moves the given peg forward the given number of holes
     *
     * @param player The player whose peg to move
     * @param holes The number of holes to move the peg
     */
    void move(Player player, int holes);

    /*
     * @brief Returns the number of holes the given player's peg is from the end
     *
     * @param player The player whose peg to check
     *
     * @return The number of holes the given player's peg is from the end
     */
    int get_holes_from_end(Player player) const;

    /*
     * @brief Returns the number of holes the given player's peg is from the start
     *
     * @param player The player whose peg to check
     *
     * @return The number of holes the given player's peg is from the start
     */
    int get_holes_from_start(Player player) const;

    /*
     * @brief Gets the winner of the game, if there is one
     *
     * @return The winner of the game, if there is one
     *        std::nullopt if there is no winner
     */
    std::optional<Player> get_winner() const;

  private:
    struct peg_pair {
        int front;
        int back;
    };

  private:
    peg_pair& get_pegs(Player player);

    const peg_pair& get_pegs(Player player) const;

  private:
    peg_pair player1;
    peg_pair player2;
};

#endif  // BOARD_H
