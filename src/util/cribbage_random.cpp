#include "cribbage_random.h"

CribbageRandom* CribbageRandom::instance = nullptr;

CribbageRandom::CribbageRandom() {
    std::random_device rd;
    generator = std::mt19937_64(rd());
}

CribbageRandom* CribbageRandom::get_instance() {
    if (instance == nullptr) {
        instance = new CribbageRandom();
    }
    return instance; 
}

CribbageRandom::~CribbageRandom() {
    delete instance;
}

int CribbageRandom::get_random_int(int min_inclusive, int max_exclusive) {
    std::uniform_int_distribution<int> distribution(min_inclusive, max_exclusive - 1);
    return distribution(generator);
}

int CribbageRandom::get_random_int(int max_exclusive) {
    return get_random_int(0, max_exclusive);
}

bool CribbageRandom::get_random_bool() {
    return get_random_int(2) == 0;
}
