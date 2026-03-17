//
// Created by yzd on 13/03/2026.
//

#include "headers/User.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "headers/UserException.h"

void User::add_balance(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount <= 0) {
        throw UserException::InvalidAmountException();
    }

    balance_ += amount;
}

void User::deduce_balance(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount <= 0) {
        throw UserException::InvalidAmountException();
    }

    if (balance_ < amount) {
        throw UserException::InsufficientFundsException();
    }

    balance_ -= amount;
}

void User::add_bank(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount <= 0) {
        throw UserException::InvalidAmountException();
    }

    bank_ += amount;
}

void User::withdraw(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount <= 0) {
        throw UserException::InvalidAmountException();
    }

    if (bank_ < amount) {
        throw UserException::InsufficientFundsException();
    }

    bank_ -= amount;
    balance_ += amount;
}

void User::deposit(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount <= 0) {
        throw UserException::InvalidAmountException();
    }

    if (balance_ < amount) {
        throw UserException::InsufficientFundsException();
    }
    balance_ -= amount;
    bank_ += amount;


}

void User::transfer(const cpp_int& amount, User& target) {
    if (amount <= 0) {
        throw UserException::InvalidAmountException();
    }

    if (target.get_id() == id_) {
        throw UserException::InvalidUserException();
    }

    if (balance_ < amount) {
        throw UserException::InsufficientFundsException();
    }

    try {
        target.add_balance(amount);
        balance_ -= amount;
    } catch (const UserException::MissingAmountException& e) {
        throw UserException::MissingAmountException();
    } catch (const UserException::InvalidAmountException& e) {
        throw UserException::InvalidAmountException();
    } catch (const std::exception& e) {
        throw UserException::UserException("Couldn't perform the action!");
    }
}

void User::set_balance(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount < 0) {
        throw UserException::InvalidAmountException();
    }

    balance_ = amount;
}

void User::set_bank(const cpp_int& amount) {
    if (amount.str().empty()) {
        throw UserException::MissingAmountException();
    }

    if (amount < 0) {
        throw UserException::InvalidAmountException();
    }

    bank_ = amount;
}

void User::set_id(const dpp::snowflake& id) {
    id_ = id;
}

void User::reset() {
    balance_ = 0;
    bank_ = 0;
}

cpp_int User::get_balance() const {
    return balance_;
}

cpp_int User::get_bank() const {
    return bank_;
}

dpp::snowflake User::get_id() const {
    return id_;
}