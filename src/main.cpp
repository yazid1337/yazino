#include <dpp/dpp.h>

#include "modules/headers/DatabaseConnection.h"
#include "modules/headers/UserException.h"
#include "services/economy/headers/Games.h"

const char *TOKEN = getenv("TOKEN");

#include "modules/headers/User.h"
#include <map>
#include <set>
#include <sqlite3.h>

int main() {
    std::map<dpp::snowflake, User> users;
    std::set<dpp::snowflake> owners = {};

    if (TOKEN == nullptr) {
        std::cout << "Token is not set!\n";
        return 1;
    }

    constexpr char prefix = '!';


    dpp::cluster bot(TOKEN, dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create([&bot, &users, &owners](const dpp::message_create_t& event) {
        if (event.msg.author == bot.me) {
            return;
        }
        if (event.msg.author.is_bot()) {
            return;
        }

        if (event.msg.content == bot.me.get_mention()) {
            event.reply("do you know 99% of gamblers quit before their big win");
        }
        if (event.msg.content.starts_with(prefix)) {
            if (event.msg.content.substr(1, event.msg.content.length()) == "register") {
                User user(event.msg.author.id);
                if (users.contains(event.msg.author.id)) {
                    event.reply("you are already registered!");
                    return;
            }
            users.insert({event.msg.author.id, user});
            event.reply(event.msg.author.username + " registered!");
        }

            if (event.msg.content.substr(1, event.msg.content.length()) == "balance" || event.msg.content.substr(1, event.msg.content.length()) == "bal") {
                if (!users.contains(event.msg.author.id)) {
                    event.reply("you are not registered! type `!register` to register!");
                    return;
                }
                dpp::embed embed;
                embed.title = "Your balance";
                embed.description = "> **Wallet** : $" + to_string(users[event.msg.author.id].get_balance()) + "\n> **Bank** : $" + to_string(users[event.msg.author.id].get_bank()); // TODO: ajouter le timestamp de création du compte dans la classe User et dans cet embed.
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                event.reply(embed);
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("addbalance") || event.msg.content.substr(1, event.msg.content.length()).starts_with("addbal")) {
                if (!users.contains(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are not registered! Type `!register` to register!";
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(bot.me.get_avatar_url());
                    event.reply(embed);
                    return;
                }

                if (!owners.contains(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Permission error";
                    embed.description = "You are not an owner!";
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(bot.me.get_avatar_url());
                    event.reply(embed);
                    return;
                }

                std::istringstream iss(event.msg.content);
                std::string token;
                iss >> token;
                std::string amount;
                iss >> amount;

                dpp::embed embed;
                embed.title = "Add balance";
                embed.description = "Added $" + amount + " to " + event.msg.author.username + "'s balance!";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(bot.me.get_avatar_url());

                try {
                    users[event.msg.author.id].add_balance(cpp_int(amount));
                } catch (const UserException::MissingAmountException& e) {
                    embed.description = "Amount is missing!";
                    event.reply(embed);
                    return;
                } catch (UserException::InvalidAmountException& e) {
                    embed.description = "Amount is invalid!";
                    event.reply(embed);
                    return;
                } catch (UserException::UserException& e) {
                    embed.description = e.what();
                    event.reply(e.what());
                    return;
                } catch (std::exception& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                }

                event.reply(embed);
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("withdraw") || event.msg.content.substr(1, event.msg.content.length()).starts_with("!with")) {
                if (!users.contains(event.msg.author.id)) {
                    event.reply("you are not registered! type `!register` to register!");
                    return;
                }

                std::istringstream iss(event.msg.content);
                std::string token;
                iss >> token;
                std::string amount;
                iss >> amount;

                try {
                    users[event.msg.author.id].withdraw(cpp_int(amount));
                    event.reply("withdrew $" + amount + " from " + event.msg.author.username + "'s bank!");
                } catch (UserException::MissingAmountException& e) {
                    event.reply(e.what());
                    return;
                } catch (UserException::InvalidAmountException& e) {
                    event.reply(e.what());
                    return;
                } catch (UserException::InsufficientFundsException& e) {
                    event.reply(e.what());
                    return;
                } catch (UserException::UserException& e) {
                    event.reply(e.what());
                    return;
                }
                } catch (std::exception& e) {
                    event.reply("Invalid number!");
                    return;
                }
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("deposit") || event.msg.content.substr(1, event.msg.content.length()).starts_with("dep")) {
                if (!users.contains(event.msg.author.id)) {
                    event.reply("you are not registered! type `!register` to register!");
                    return;
                }

                std::istringstream iss(event.msg.content);
                std::string token;
                iss >> token;
                std::string amount;
                iss >> amount;

                try {
                    users[event.msg.author.id].deposit(cpp_int(amount));
                    event.reply("deposited $" + amount + " to " + event.msg.author.username + "'s bank!");
                } catch (UserException& e) {
                    event.reply(e.what());
                    return;
                } catch (std::exception& e) {
                    event.reply("Invalid number!");
                    return;
                }
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("transfer")) {
                if (!users.contains(event.msg.author.id)) {
                    event.reply("you are not registered! type `!register` to register!");
                    return;
                }

                std::istringstream iss(event.msg.content);
                std::string token;
                iss >> token;
                std::string amount = "";
                iss >> amount;

                if (amount.empty()) {
                    event.reply("Specify an amount.");
                    return;
                }

                if (event.msg.mentions.empty()) {
                    event.reply("Specify a user.");
                    return;
                }

                if (!users.contains(event.msg.mentions[0].first.id)) {
                    event.reply("User not found.");
                    return;
                }

                try {
                    cpp_int to_transfer = cpp_int(amount);
                    users[event.msg.author.id].transfer(to_transfer, users[event.msg.mentions[0].first.id]);
                    event.reply("Transfered $" + amount + " to " + event.msg.mentions[0].first.username + "!");
                } catch (UserException& e) {
                    event.reply(e.what());
                    return;
                } catch (std::exception& e) {
                    event.reply("Invalid number!");
                    return;
                }
            }

            if (event.msg.content.substr(1, event.msg.content.length()) == "reset") {}

            if (event.msg.content.substr(1, event.msg.content.length()) == "help") {}


            /*
            if (event.msg.content.starts_with("!test")) {
                dpp::message message{users[event.msg.author.id].get_user().get_mention()};
                message.allowed_mentions.users.push_back(event.msg.author.id);
                event.reply(message);
            }
            */

            if (event.msg.content.starts_with("!gamble")) {
                if (!users.contains(event.msg.author.id)) {
                    event.reply("you are not registered! type `!register` to register!");
                    return;
                }

                std::istringstream iss(event.msg.content);
                std::string token;
                iss >> token;
                std::string amount;
                iss >> amount;



                try {
                    if (users[event.msg.author.id].get_balance() < cpp_int(amount)) {
                        event.reply("You don't have enough money!");
                        return;
                    }

                    cpp_int prize = Games::gamble(cpp_int(amount));
                    if (prize != 0) {
                        users[event.msg.author.id].add_balance(prize);
                        event.reply("You won $**" + to_string(prize) + "**!");
                    } else {
                        users[event.msg.author.id].deduce_balance(cpp_int(amount));
                        event.reply("You lost **$" + amount + "**!");
                    }
                } catch (UserException& e) {
                    event.reply(e.what());
                } catch (std::exception& e) {
                    event.reply("Invalid number!");
                }
            }
        }

        if (event.msg.content.starts_with("!help")) {
            dpp::message message{"**Commands**\n\n**!register** - Register your account\n**!balance** - Check your balance\n**!gamble** - Gamble money\n**!help** - Show this message"};
            event.reply(message);
        }

        if (event.msg.content.starts_with("!ping")) {
            event.reply("Pong!");
        }

        if (event.msg.content.starts_with("!test")) {
            try {
                DatabaseConnection &db = DatabaseConnection::instance();
                db.create_tables();
                event.reply("Tables created!");
            } catch (std::exception& e) {
                event.reply("Error: " + std::string(e.what()));
            }
        }

        if (event.msg.content.substr(1, event.msg.content.length()).starts_with("retrieve_user")) {
            if (!users.contains(event.msg.author.id)) {
                event.reply("you are not registered! type `!register` to register!");
                return;
            }

            std::istringstream iss(event.msg.content);
            std::string token;
            iss >> token;

            if (event.msg.mentions.empty()) {
                event.reply("Specify a user.");
                return;
            }

            if (!users.contains(event.msg.mentions[0].first.id)) {
                event.reply("User not found.");
                return;
            }

            try {
                DatabaseConnection &db = DatabaseConnection::instance();
                User user = db.get_user(event.msg.mentions[0].first.id);
                event.reply("User retrieved : $" + user.get_balance().str() + ", $" + user.get_bank().str());
            } catch (UserException& e) {
                event.reply(e.what());
            } catch (std::exception& e) {
                event.reply("Invalid user!");
            }
        }

        if (event.msg.content.substr(1, event.msg.content.length()).starts_with("kill")) {
            if (!owners.contains(event.msg.author.id)) {
                event.reply("You are not an owner!");
                return;
            }

            DatabaseConnection &db = DatabaseConnection::instance();
            db.save_users(users);
            event.send("Stopping...");
            bot.terminating = true;
        }
    });

    bot.on_ready([&bot, &users](const dpp::ready_t& event) {
        bot.set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_streaming, ""));

        DatabaseConnection &db = DatabaseConnection::instance();
        users = db.load_users();

        std::cout << bot.me.username << " is ready!\n";
    });

    bot.start(dpp::st_wait);
}