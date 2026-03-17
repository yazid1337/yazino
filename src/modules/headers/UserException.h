//
// Created by yzd on 14/03/2026.
//

#ifndef YAZINO_BOT_USEREXCEPTION_H
#define YAZINO_BOT_USEREXCEPTION_H

#include <exception>
#include <string>

namespace UserException {
    class UserException : public std::exception {
    public:
        explicit UserException(const std::string& message) : message_(message) {}
        const char* what() const noexcept override {
            return message_.c_str();
        }
    private:
        std::string message_;
    };

    class InsufficientFundsException : public UserException {
    public:
        explicit InsufficientFundsException() : UserException("Insufficient funds !") {}
    };

    class InvalidAmountException : public UserException {
    public:
        explicit InvalidAmountException() : UserException("Invalid amount !") {}
    };

    class MissingAmountException : public UserException {
    public:
        explicit MissingAmountException() : UserException("Missing amount !") {}
    };

    class InvalidUserException : public UserException {
    public:
        explicit InvalidUserException() : UserException("Invalid user !") {}
    };

    class UserAlreadyExistsException : public UserException {
    public:
        explicit UserAlreadyExistsException() : UserException("User already exists !") {}
    };

    class UserNotFoundException : public UserException {
    public:
        explicit UserNotFoundException() : UserException("User not found !") {}
    };

}



#endif //YAZINO_BOT_USEREXCEPTION_H