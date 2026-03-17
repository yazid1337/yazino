#include <dpp/dpp.h>

#include "modules/headers/DatabaseConnection.h"
#include "modules/headers/UserException.h"
#include "services/economy/headers/Games.h"



#include "modules/headers/User.h"
#include <map>
#include <set>

int main() {
    std::map<dpp::snowflake, User> users = {{1478118482917982239, User(1478118482917982239)}};
    std::set<dpp::snowflake> owners = {1478118482917982239};

    const char *TOKEN = nullptr;
    try {
        TOKEN = DatabaseConnection::instance().get_token();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    char prefix = ' ';
    try {
        prefix = DatabaseConnection::instance().get_prefix();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }


    dpp::cluster bot(TOKEN, dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create([&bot, &users, &prefix](const dpp::message_create_t& event) {
        if (event.msg.author == bot.me) {
            return;
        }
        if (event.msg.author.is_bot()) {
            return;
        }

        if (event.msg.content == bot.me.get_mention()) {
            dpp::embed embed;
            embed.title = "Hello!";
            embed.description = "Do you know 99% of gamblers quit before their big win ?";
            embed.color = 0xFF0000;
            embed.thumbnail.emplace(bot.me.get_avatar_url());
            event.reply(embed);
        }

        if (event.msg.content.starts_with(prefix)) {
            if (event.msg.content.substr(1, event.msg.content.length()) == "register") {
                if (DatabaseConnection::instance().exists(event.msg.author.id)) {
                    // TODO: éventuellement remplacer ça par une exception de type DatabaseException (crée par moi ?)
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are already registered!";
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed);
                    return;
                }

                User user(event.msg.author.id);

                try {
                    DatabaseConnection::instance().save_user(user);
                } catch (const std::exception& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed);
                    return;
                }

                dpp::embed embed;
                embed.title = "Registration success";
                embed.description = "You are now registered! Type `!balance` to check your balance!";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                event.reply(embed);
            }

            if (event.msg.content.substr(1, event.msg.content.length()) == "balance" || event.msg.content.substr(1, event.msg.content.length()) == "bal") {
                if (!DatabaseConnection::instance().exists(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are not registered! Type `!register` to register!";
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed);
                    return;
                }

                User user;

                try {
                    user = DatabaseConnection::instance().get_user(event.msg.author.id);
                } catch (const UserException::UserNotFoundException& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed);
                    return;
                } catch (const UserException::UserException& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    return;
                } catch (const std::exception& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    return;
                }

                dpp::embed embed;
                embed.title = "Your balance";
                embed.description = "> **Wallet** : $" + to_string(user.get_balance()) + "\n> **Bank** : $" + to_string(user.get_bank()); // TODO: ajouter le timestamp de création du compte dans la classe User et dans cet embed.
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                event.reply(embed);
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("addbalance") || event.msg.content.substr(1, event.msg.content.length()).starts_with("addbal")) {
                if (!DatabaseConnection::instance().exists(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are not registered! Type `!register` to register!";
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(bot.me.get_avatar_url());
                    event.reply(embed);
                    return;
                }

                if (!DatabaseConnection::instance().is_owner(event.msg.author.id)) {
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

                User user;
                try {
                    user = DatabaseConnection::instance().get_user(event.msg.author.id);
                } catch (const UserException::UserNotFoundException& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    embed.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed);
                    return;
                } catch (const UserException::UserException& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    return;
                } catch (const std::exception& e) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = e.what();
                    embed.color = 0xFF0000;
                    return;
                }

                dpp::embed embed;
                embed.title = "Add balance";
                embed.description = "Added $" + amount + " to " + event.msg.author.username + "'s balance!";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(bot.me.get_avatar_url());

                try {
                    user.add_balance(cpp_int(amount));
                } catch (const UserException::MissingAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InvalidAmountException& e) {
                    embed.description = e.what();
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

                try {
                    DatabaseConnection::instance().save_user(user);
                } catch (const std::exception& e) {
                    dpp::embed embed_error;
                    embed_error.title = "Registration error";
                    embed_error.description = e.what();
                    embed_error.color = 0xFF0000;
                    embed_error.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed_error);
                    return;
                }

                event.reply(embed);
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("withdraw") || event.msg.content.substr(1, event.msg.content.length()).starts_with("!with")) {
                if (!DatabaseConnection::instance().exists(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are not registered! Type `!register` to register!";
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
                embed.title = "Withdraw";
                embed.description = "Withdrew $" + amount + " from " + event.msg.author.username + "'s bank!";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(bot.me.get_avatar_url());

                User user;
                try {
                    user = DatabaseConnection::instance().get_user(event.msg.author.id);
                } catch (const UserException::UserNotFoundException& e) {
                    dpp::embed embed_error;
                    embed_error.title = "Registration error";
                    embed_error.description = e.what();
                    embed_error.color = 0xFF0000;
                    embed_error.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed_error);
                    return;
                } catch (const UserException::UserException& e) {
                    dpp::embed embed_error;
                    embed_error.title = "Registration error";
                    embed_error.description = e.what();
                    embed_error.color = 0xFF0000;
                    return;
                } catch (const std::exception& e) {
                    dpp::embed embed_error;
                    embed_error.title = "Registration error";
                    embed_error.description = e.what();
                    embed_error.color = 0xFF0000;
                    return;
                }

                try {
                    user.withdraw(cpp_int(amount));
                } catch (UserException::MissingAmountException& e) {
                    embed.description = e.what();
                } catch (UserException::InvalidAmountException& e) {
                    embed.description = e.what();
                } catch (UserException::InsufficientFundsException& e) {
                    embed.description = e.what();
                } catch (UserException::UserException& e) {
                    embed.description = e.what();
                    return;
                } catch (std::exception& e) {
                    embed.description = e.what();
                    return;
                }

                try {
                    DatabaseConnection::instance().save_user(user);
                } catch (const std::exception& e) {
                    dpp::embed embed_error;
                    embed_error.title = "Registration error";
                    embed_error.description = e.what();
                    embed_error.color = 0xFF0000;
                    embed_error.thumbnail.emplace(event.msg.author.get_avatar_url());
                    event.reply(embed_error);
                    return;
                }

                event.reply(embed);
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("deposit") || event.msg.content.substr(1, event.msg.content.length()).starts_with("dep")) {
                if (!DatabaseConnection::instance().exists(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are not registered! Type `!register` to register!";
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
                embed.title = "Deposit";
                embed.description = "Deposited $" + amount + " to " + event.msg.author.username + "'s bank!";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(bot.me.get_avatar_url());


                try {
                    users[event.msg.author.id].deposit(cpp_int(amount));
                    event.reply(embed);
                    return;
                } catch (UserException::MissingAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InvalidAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InsufficientFundsException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::UserException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (std::exception& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                }
            }

            if (event.msg.content.substr(1, event.msg.content.length()).starts_with("transfer")) {
                if (!users.contains(event.msg.author.id)) {
                    event.reply("you are not registered! type `!register` to register!");
                    return;
                }

                dpp::embed embed;
                embed.title = "Transfer";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(bot.me.get_avatar_url());

                std::istringstream iss(event.msg.content);
                std::string token;
                iss >> token;

                if (iss.str().empty()) {
                    embed.description = "Specify an amount.";
                    event.reply(embed);
                    return;
                }

                std::string amount = "";
                iss >> amount;



                embed.description = "Transferred $" + amount + " to " + event.msg.mentions[0].first.username + "'s bank!";


                if (amount.empty()) {
                    embed.description = "Specify an amount.";
                    event.reply(embed);
                    return;
                }

                if (event.msg.mentions.empty()) {
                    embed.description = "Specify a user.";
                    event.reply(embed);
                    return;
                }

                if (!users.contains(event.msg.mentions[0].first.id)) {
                    embed.description = "User not found.";
                    event.reply(embed);
                    return;
                }

                try {
                    cpp_int to_transfer = cpp_int(amount);
                    users[event.msg.author.id].transfer(to_transfer, users[event.msg.mentions[0].first.id]);
                    event.reply("Transfered $" + amount + " to " + event.msg.mentions[0].first.username + "!");
                } catch (UserException::MissingAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InvalidAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InsufficientFundsException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InvalidUserException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::UserException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (std::exception& e) {
                    embed.description = e.what();
                    event.reply(embed);
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
                if (!DatabaseConnection::instance().exists(event.msg.author.id)) {
                    dpp::embed embed;
                    embed.title = "Registration error";
                    embed.description = "You are not registered! Type `!register` to register!";
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
                embed.title = "Gamble";
                embed.description = "You gambled $" + amount + "!";
                embed.color = 0xFF0000;
                embed.thumbnail.emplace(bot.me.get_avatar_url());

                try {
                    if (users[event.msg.author.id].get_balance() < cpp_int(amount)) {
                        embed.description = "You don't have enough money!";
                        event.reply(embed);
                        return;
                    }

                    cpp_int prize = Games::gamble(cpp_int(amount));
                    if (prize != 0) {
                        users[event.msg.author.id].add_balance(prize);
                        embed.add_field("Result", "You won **$" + amount + "** !", true);
                        event.reply(embed);
                    } else {
                        users[event.msg.author.id].deduce_balance(cpp_int(amount));
                        embed.add_field("Result", "You lost **$" + amount + "** !", true);
                        event.reply(embed);
                    }
                } catch (UserException::MissingAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InvalidAmountException& e) {
                    embed.description = e.what();
                    event.reply(embed);
                    return;
                } catch (UserException::InsufficientFundsException& e) {
                    embed.description = e.what();
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
            }
        }

        if (event.msg.content.starts_with("help")) {
            dpp::message message{"**Commands**\n\n**!register** - Register your account\n**!balance** - Check your balance\n**!gamble** - Gamble money\n**!help** - Show this message"};
            event.reply(message);
        }

        if (event.msg.content.starts_with("ping")) {
            event.reply("Pong!");
        }


        if (event.msg.content.substr(1, event.msg.content.length()).starts_with("kill")) {
            if (!DatabaseConnection::instance().is_owner(event.msg.author.id)) {
                event.reply("You are not an owner!");
                return;
            }

            event.send("Stopping...");
            bot.terminating = true;
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        bot.set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_streaming, ""));
        std::cout << bot.me.username << " is ready!\n";
    });

    bot.start(dpp::st_wait);
}