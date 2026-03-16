//
// Created by yzd on 13/03/2026.
//

#ifndef YAZINO_BOT_GAMES_H
#define YAZINO_BOT_GAMES_H
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

class Games {
public:
    static cpp_int gamble(cpp_int amount);
    static cpp_int coinflip(cpp_int amount);
    static cpp_int roulette(cpp_int amount);
    static cpp_int slots(cpp_int amount);
    static cpp_int blackjack(cpp_int amount);
    static cpp_int guess_the_number(cpp_int amount);
    static cpp_int guess_the_word(cpp_int amount);
    static cpp_int guess_the_color(cpp_int amount);
    static cpp_int guess_the_emoji(cpp_int amount);
private:
    Games() = delete;
};


#endif //YAZINO_BOT_GAMES_H