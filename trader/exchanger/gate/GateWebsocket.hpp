#pragma once

#include <functional>
#include <memory>
#include <string>

namespace Json { class Value; }

class GateWebsocket {
public:
    using Callback = std::function<void(const Json::Value&)>;

    GateWebsocket(std::string channel, std::string payload, Callback callback);
    ~GateWebsocket();

    bool connect();
    void service(int timeout_ms = 0);
    bool matches(const std::string& channel, const std::string& payload) const;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};
