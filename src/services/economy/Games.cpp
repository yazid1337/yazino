//
// Created by yzd on 13/03/2026.
//

#include "headers/Games.h"
#include <random>


cpp_int Games::gamble(cpp_int amount) {
    if (amount.str().empty()) {
        throw std::runtime_error("Specify an amount.");
    }

    if (amount.str().find_first_not_of("0123456789") != std::string::npos || amount.str().starts_with("-") || amount <= 0) {
        throw std::runtime_error("Invalid amount.");
    }


    std::uniform_int_distribution<int> dist(0, 1);
    std::mt19937 random_engine{std::random_device{}()};
    std::mt19937::result_type const seed = std::random_device()();
    random_engine.seed(seed);


    if (dist(random_engine) == 0) {
        return amount;
    }
    return 0;
}
