#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <tgbot/tgbot.h>

using namespace TgBot;

void sendLogs(Bot& bot, std::int64_t chat_id, const std::string& username) {
    std::string filename = "/tmp/" + username + ".log";
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    std::vector<std::string> lines = StringTools::split(content, '\n');

    for (const std::string& line : lines)
        bot.getApi().sendMessage(chat_id, line);
}

int main() {
    std::string token = "2049249720:AAF5EgAvwRkTLUag8URHHGwxffCmfY_vq_Q";

    Bot bot(token);
    std::vector<BotCommand::Ptr> commands;
    BotCommand::Ptr cmdArray(new BotCommand);
    cmdArray->command = "log";
    cmdArray->description = "send logs";
    commands.push_back(cmdArray);
    bot.getApi().setMyCommands(commands);

    std::vector<BotCommand::Ptr> vectCmd;
    vectCmd = bot.getApi().getMyCommands();

    for(std::vector<BotCommand::Ptr>::iterator it = vectCmd.begin(); it != vectCmd.end(); ++it) {
        printf("cmd: %s -> %s\r",(*it)->command.c_str(),(*it)->description.c_str());
    }

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
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
