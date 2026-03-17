//
// Created by yzd on 13/03/2026.
//

#ifndef YAZINO_BOT_USER_H
#define YAZINO_BOT_USER_H
#include <dpp/dpp.h>

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

class User {
public:
    User(const dpp::snowflake id) : id_(id), balance_("0"), bank_("0") {};
    User() : id_(0), balance_("0"), bank_("0") {} // NOTE: shouldn't be used.
    void add_balance(const cpp_int& amount); // passer toutes ces méthodes avec des const cpp_int
    void deduce_balance(const cpp_int& amount);
    void add_bank(const cpp_int &amount);
    void withdraw(const cpp_int& amount);
    void deposit(const cpp_int& amount);
    void transfer(const cpp_int &amount, User& target);
    void set_balance(const cpp_int& amount);
    void set_bank(const cpp_int& amount);
    void set_id(const dpp::snowflake& id);
    cpp_int get_balance() const;
    cpp_int get_bank() const;
    dpp::snowflake get_id() const;
    void reset();
private:
    dpp::snowflake id_;
    cpp_int balance_;
    cpp_int bank_;
    /* TODO: ajouter le type d'utilisateur avec un type enum class ? (0 = user classique, 1 = owner, etc). */
};


#endif //YAZINO_BOT_USER_H