#include "GateWebsocket.hpp"
#include "core/Logger.hpp"
#include <json/json.h>
#include <libwebsockets.h>
#include <cstring>
#include <ctime>
#include <sstream>
#include <vector>

namespace {
lws_protocols protocols[] = {
    {"gate-v4", &GateWebsocket::handler, 0, 65536},
    {nullptr, nullptr, 0, 0}
};
}

GateWebsocket::GateWebsocket(std::string channel, std::string payload, Callback callback)
    : _channel(std::move(channel)), _payload(std::move(payload)), _callback(std::move(callback)) {}

GateWebsocket::~GateWebsocket() {
    if (_context != nullptr)
        lws_context_destroy(_context);
}

bool GateWebsocket::connect() {
    if (_socket != nullptr)
        return true;
    if (_context == nullptr) {
        lws_context_creation_info info{};
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.gid = -1;
        info.uid = -1;
        _context = lws_create_context(&info);
    }
    if (_context == nullptr)
        return false;

    lws_client_connect_info info{};
    info.context = _context;
    info.address = "api.gateio.ws";
    info.port = 443;
    info.path = "/ws/v4/";
    info.host = info.address;
    info.origin = info.address;
    info.protocol = protocols[0].name;
    info.ssl_connection = LCCSCF_USE_SSL;
    info.userdata = this;
    _socket = lws_client_connect_via_info(&info);
    return _socket != nullptr;
}

void GateWebsocket::service(int timeout_ms) {
    if (_context != nullptr)
        lws_service(_context, timeout_ms);
}

bool GateWebsocket::matches(const std::string& channel, const std::string& payload) const {
    return _channel == channel && _payload == payload;
}

std::string GateWebsocket::subscription() const {
    return "{\"time\":" + std::to_string(std::time(nullptr)) + ",\"channel\":\"" + _channel +
           "\",\"event\":\"subscribe\",\"payload\":" + _payload + "}";
}

int GateWebsocket::handler(lws* socket, lws_callback_reasons reason, void* user, void* input, size_t length) {
    auto* self = static_cast<GateWebsocket*>(user);
    return self != nullptr ? self->handle(socket, reason, input, length) : 0;
}

int GateWebsocket::handle(lws* socket, lws_callback_reasons reason, void* input, size_t length) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lws_callback_on_writable(socket);
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            const std::string message = subscription();
            std::vector<unsigned char> buffer(LWS_PRE + message.size());
            std::memcpy(buffer.data() + LWS_PRE, message.data(), message.size());
            lws_write(socket, buffer.data() + LWS_PRE, message.size(), LWS_WRITE_TEXT);
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE: {
            Json::Value json;
            Json::CharReaderBuilder builder;
            std::string errors;
            std::istringstream stream(std::string(static_cast<char*>(input), length));
            if (Json::parseFromStream(builder, stream, &json, &errors) && json["event"].asString() == "update")
                _callback(json);
            break;
        }
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        case LWS_CALLBACK_CLIENT_CLOSED:
        case LWS_CALLBACK_CLOSED:
            _socket = nullptr;
            break;
        default:
            break;
    }
    return 0;
}
