//
// Created by yzd on 14/03/2026.
//

#ifndef YAZINO_BOT_DATABASE_H
#define YAZINO_BOT_DATABASE_H
#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <stdexcept>

#include "User.h"
#include "UserException.h"

struct DatabaseConnection
{
    static
    DatabaseConnection &
    instance() {
        static DatabaseConnection s;
        return s;
    }

    DatabaseConnection(const DatabaseConnection &) = delete;
    DatabaseConnection & operator = (const DatabaseConnection &) = delete;

    void create_tables() {
        char* err_msg;
        const char* sql = "CREATE TABLE USERS("
                    "ID INTEGER PRIMARY KEY NOT NULL,"
                    "BALANCE TEXT NOT NULL,"
                    "BANK TEXT NOT NULL)";

        if(const int rc = sqlite3_exec(db_, sql, nullptr, 0, &err_msg); rc != SQLITE_OK){
            std::cout << "Error in executing SQL: %s \n" << err_msg << "\n";
            sqlite3_free(err_msg);
        } else {
            std::cout << "table users made successfully";
        }
    }

    User get_user(dpp::snowflake id) const {
        std::stringstream req;
        req << "SELECT * FROM USERS WHERE id = " << std::to_string(id);

        sqlite3_stmt *stmt = nullptr;
        User user{};

        if (sqlite3_prepare_v2(db_, req.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        int step_result = sqlite3_step(stmt);
        if (step_result == SQLITE_ROW) {
            user.set_id(dpp::snowflake(sqlite3_column_int(stmt, 0)));
            user.set_balance(cpp_int(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));
            user.set_bank(cpp_int(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))));
        } else if (step_result == SQLITE_DONE) {
            throw UserException("User not found");
        } else {
            throw std::runtime_error("Could not step statement");
        }

        sqlite3_finalize(stmt);

        return user;
    }

    void save_users(const std::map<dpp::snowflake, User>& users) {
        std::stringstream req;
        req << "INSERT OR REPLACE INTO USERS VALUES (?, ?, ?)";
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(db_, req.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }
        for (const auto &user: users | std::views::values) {
            sqlite3_bind_int64(stmt, 1, user.get_id());
            sqlite3_bind_text(stmt, 2, user.get_balance().str().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, user.get_bank().str().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
        sqlite3_finalize(stmt);
    }

    std::map<dpp::snowflake, User> load_users() {
        std::stringstream req;
        req << "SELECT * FROM USERS";
        sqlite3_stmt *stmt = nullptr;
        std::map<dpp::snowflake, User> users;
        if (sqlite3_prepare_v2(db_, req.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            users[dpp::snowflake(sqlite3_column_int64(stmt, 0))] = User(dpp::snowflake(sqlite3_column_int64(stmt, 0)));
            users[dpp::snowflake(sqlite3_column_int64(stmt, 0))].set_balance(cpp_int(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));
            users[dpp::snowflake(sqlite3_column_int64(stmt, 0))].set_bank(cpp_int(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))));
        }
        sqlite3_finalize(stmt);
        return users;
    }

private:
    sqlite3 *db_;

    DatabaseConnection() {
        if (sqlite3_open("../src/db/db.sqlite", &db_) != SQLITE_OK) {
            throw std::runtime_error("Could not open database");
        }
    }

    ~DatabaseConnection() {
        sqlite3_close(db_);
    }

};

#endif //YAZINO_BOT_DATABASE_H