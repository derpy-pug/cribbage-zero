#ifndef CRIBBAGE_RANDOM_H
#define CRIBBAGE_RANDOM_H

/* 
 * @brief A random number generator
 *
 * It is a singleton, so there is only one instance of it.
 * Use CribbageRandom::get_instance() to get the instance.
 *
 * Uses the Mersenne Twister algorithm
 */
#include <random>

class CribbageRandom
{
 public:
    CribbageRandom(const CribbageRandom&) = delete;
    CribbageRandom& operator=(const CribbageRandom&) = delete;

    ~CribbageRandom();

    /*
     * @brief Constructs a random number generator
     *
     * @param min_inclusive The minimum value to generate (inclusive)
     * @param max_exclusive The maximum value to generate (exclusive)
     */
    int get_random_int(int min_inclusive, int max_exclusive);

    /*
     * @brief Constructs a random number generator
     *
     * Between 0 and max_exclusive
     *
     * @param max_exclusive The maximum value to generate (exclusive)
     */
    int get_random_int(int max_exclusive);

    /*
     * @brief Constructs a random boolean generator
     *
     * @return A random boolean
     */
    bool get_random_bool();

    static CribbageRandom* get_instance();

    std::mt19937_64& get_generator() { return generator; }

 private:
    CribbageRandom();
 private:
    std::mt19937_64 generator;
    static CribbageRandom* instance;
};

#endif // CRIBBAGE_RANDOM_H
