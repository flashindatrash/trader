//
// Created by Вадим Проскурин on 01.11.2021.
//

#pragma once

#include "Proxy.hpp"
#include <curl/curl.h>

namespace core {
class Config;
}

class EventManager : public core::Proxy<EventManager> {
private: // static
    static size_t curl_cb(void *content, size_t size, size_t nmemb, std::string *buffer);

public: // methods
    EventManager() = default;

    bool init(const core::Config& config);
    bool send(const char* fmt, ...);

private: // vars
    CURL* _curl = nullptr;
};

#define Events() EventManager::getInstance()