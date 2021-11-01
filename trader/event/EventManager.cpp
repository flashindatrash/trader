//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "EventManager.hpp"
#include "Config.hpp"

bool EventManager::init(const core::Config& config) {
    _curl = curl_easy_init();

    std::string url = config.asString("EVENT_HOST");
    int port = config.asInt("EVENT_PORT");

    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_PORT, port);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, EventManager::curl_cb);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(_curl, CURLOPT_ENCODING, "gzip");
    return true;
}

bool EventManager::send(const char* fmt, ...) {
    if (_curl == nullptr)
        return false;

    va_list arg;
    va_start(arg, fmt);
    char buffer[256];
    size_t size = std::vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    if (size == 0)
        return false;

    std::string request = buffer;
    std::string result;

    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, &request);

    CURLcode code = curl_easy_perform(_curl);
    if (code != CURLE_OK)
        return false;

    return true;
}

size_t EventManager::curl_cb(void* content, size_t size, size_t nmemb, std::string* buffer) {
    buffer->append((char*)content, size*nmemb);
    return size*nmemb;
}