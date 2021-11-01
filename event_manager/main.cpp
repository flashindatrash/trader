#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <tgbot/tgbot.h>

#include <argparser/ArgumentParser.hpp>
#include "Logger.hpp"
#include "Config.hpp"

using namespace TgBot;

void sendLogs(Bot& bot, std::int64_t chat_id, const std::string& username) {
    std::string filename = "/tmp/" + username + ".log";
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    std::vector<std::string> lines = StringTools::split(content, '\n');

    for (const std::string& line : lines)
        bot.getApi().sendMessage(chat_id, line);
}

int main(int argc, char** argv) {
    std::string cfg_file;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));

        auto parsed = args.parse(argc, argv);
        cfg_file = parsed.getValue("--config");
    } catch(...) {
        args.showHelp("event_manager -c ./config/default.cfg");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(cfg_file.c_str())) {
        Logger::info("Can't init config %s", cfg_file.c_str());
        return EXIT_FAILURE;
    }

    const std::string& token = cfg.asString("EVENT_TOKEN");
    if (token.empty()) {
        Logger::info("undefined EVENT_TOKEN");
        return EXIT_FAILURE;
    }

    Bot bot(token);
    std::vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray(new BotCommand);
    cmdArray->command = "log";
    cmdArray->description = "receive logs";
    commands.push_back(cmdArray);
    bot.getApi().setMyCommands(commands);

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        if (StringTools::startsWith(message->text, "/log")) {
            std::vector<std::string> command = StringTools::split(message->text, ' ');
            if (command.size() != 2) {
                bot.getApi().sendMessage(message->chat->id, "Invalid command: " + message->text);
                return;
            }

            sendLogs(bot, message->chat->id, command.at(1));
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }

    return EXIT_SUCCESS;
}
