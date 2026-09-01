#pragma once

#include <functional>
#include <string>
#include <libwebsockets.h>

namespace Json { class Value; }

class GateWebsocket {
public:
    using Callback = std::function<void(const Json::Value&)>;

    GateWebsocket(std::string channel, std::string payload, Callback callback);
    ~GateWebsocket();

    bool connect();
    void service(int timeout_ms = 0);
    bool matches(const std::string& channel, const std::string& payload) const;
    static int handler(lws* socket, lws_callback_reasons reason, void* user, void* input, size_t length);

private:
    int handle(lws* socket, lws_callback_reasons reason, void* input, size_t length);
    std::string subscription() const;

    lws_context* _context = nullptr;
    lws* _socket = nullptr;
    std::string _channel;
    std::string _payload;
    Callback _callback;
};
