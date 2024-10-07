#include "cribbage_random.h"

#include <chrono>

CribbageRandom* CribbageRandom::instance = nullptr;

CribbageRandom::CribbageRandom() {
    std::random_device rd;
    generator = std::mt19937_64(rd());
    // seed(std::chrono::system_clock::now().time_since_epoch().count());
}

CribbageRandom* CribbageRandom::get_instance() {
    if (instance == nullptr) {
        instance = new CribbageRandom();
    }
    return instance; 
}

void CribbageRandom::seed(unsigned int seed) {
    generator.seed(seed);
}

CribbageRandom::~CribbageRandom() {
}

int CribbageRandom::get_int(int min_inclusive, int max_exclusive) {
    std::uniform_int_distribution<int> distribution(min_inclusive, max_exclusive - 1);
    return distribution(generator);
}

int CribbageRandom::get_int(int max_exclusive) {
    return get_int(0, max_exclusive);
}

bool CribbageRandom::get_bool() {
    return get_int(2) == 0;
}
