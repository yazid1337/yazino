//
// Created by yzd on 14/03/2026.
//

#ifndef YAZINO_BOT_DATABASE_H
#define YAZINO_BOT_DATABASE_H
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

    /*
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
    */

    const char *get_token() const {
        std::stringstream req;
        req << "SELECT * FROM CONFIGURATION WHERE TYPE='token'";

        sqlite3_stmt *stmt = nullptr;
        const char *text_ptr = nullptr;

        if (sqlite3_prepare_v2(db_, req.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        int step_result = sqlite3_step(stmt);
        if (step_result == SQLITE_ROW) {
            const void *raw_buffer = nullptr;
            raw_buffer = sqlite3_column_text(stmt, 1);
            text_ptr = static_cast<const char*>(raw_buffer);
        } else if (step_result == SQLITE_DONE) {
            throw std::runtime_error("No token specified in the DB!");
        }

        return text_ptr;
    }

    char get_prefix() const {
        std::stringstream req;
        req << "SELECT * FROM CONFIGURATION WHERE TYPE='prefix'";

        sqlite3_stmt *stmt = nullptr;
        const char *text_ptr = nullptr;

        if (sqlite3_prepare_v2(db_, req.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        int step_result = sqlite3_step(stmt);
        if (step_result == SQLITE_ROW) {
            const void *raw_buffer = nullptr;
            raw_buffer = sqlite3_column_text(stmt, 1);
            text_ptr = static_cast<const char*>(raw_buffer);
        } else if (step_result == SQLITE_DONE) {
            throw std::runtime_error("No prefix specified in the DB!");
        }

        return text_ptr[0];
    }

    User get_user(const dpp::snowflake& id) const {
        const char *req = "SELECT * FROM USERS WHERE ID = ?";

        sqlite3_stmt *stmt = nullptr;
        User user{};

        if (sqlite3_prepare_v2(db_, req, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        sqlite3_bind_int64(stmt, 1, id);

        int step_result = sqlite3_step(stmt);

        if (step_result == SQLITE_ROW) {
            user.set_id(dpp::snowflake(sqlite3_column_int64(stmt, 0)));
            const void *raw_buffer = sqlite3_column_text(stmt, 1);
            const char *text_ptr = static_cast<const char*>(raw_buffer);
            user.set_balance(cpp_int(text_ptr));
            raw_buffer = sqlite3_column_text(stmt, 2);
            text_ptr = static_cast<const char*>(raw_buffer);
            user.set_bank(cpp_int(text_ptr));
        } else if (step_result == SQLITE_DONE) {
            throw UserException::UserNotFoundException();
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

    void save_user(const User& user) {
        std::string req = "INSERT OR REPLACE INTO USERS VALUES (?, ?, ?, ?)";
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(db_, req.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        sqlite3_bind_int64(stmt, 1, user.get_id());
        sqlite3_bind_text(stmt, 2, user.get_balance().str().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, user.get_bank().str().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, 0);

        sqlite3_step(stmt);

        sqlite3_finalize(stmt);
    }

    bool is_owner(const dpp::snowflake& id) const {
        /* TODO: essayer de changer ça en const char* pour économiser l'overhead d'un std::string */
        const std::string req = "SELECT TYPE FROM USERS WHERE ID = ?";

        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(db_, req.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        sqlite3_bind_int64(stmt, 1, id);

        bool result = false;

        int step_result = sqlite3_step(stmt);
        if (step_result == SQLITE_ROW) {
            if (sqlite3_column_int(stmt, 0) == 1) {
                result = true;
            }
        } else if (step_result == SQLITE_DONE) {
            throw UserException::UserNotFoundException();
        } else {
            throw std::runtime_error("Could not step statement");
        }

        sqlite3_finalize(stmt);

        return result;
    }

    bool exists(const dpp::snowflake& id) const {
        const std::string req = "SELECT COUNT(*) FROM USERS WHERE ID = ?";
        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(db_, req.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Could not prepare statement");
        }

        int bind_result = sqlite3_bind_int64(stmt, 1, id);
        if (bind_result != SQLITE_OK) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Could not bind parameter");
        }

        int step_result = sqlite3_step(stmt);
        bool exists = false;

        if (step_result == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            exists = (count > 0);
        }

        sqlite3_finalize(stmt);
        return exists;
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