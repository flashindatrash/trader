#include "GateWebsocket.hpp"
#include "core/Logger.hpp"
#include <json/json.h>
#include <libwebsockets.h>
#include <cstring>
#include <ctime>
#include <sstream>
#include <vector>

struct GateWebsocket::Impl {
    Impl(std::string channel, std::string payload, Callback callback)
        : channel(std::move(channel)), payload(std::move(payload)), callback(std::move(callback)) {}

    ~Impl() {
        if (context != nullptr)
            lws_context_destroy(context);
    }

    static int handler(lws* socket, lws_callback_reasons reason, void* user, void* input, size_t length) {
        auto* self = static_cast<Impl*>(user);
        return self != nullptr ? self->handle(socket, reason, input, length) : 0;
    }

    static lws_protocols* protocols() {
        static lws_protocols value[] = {
            {"gate-v4", &Impl::handler, 0, 65536},
            {nullptr, nullptr, 0, 0}
        };
        return value;
    }

    std::string subscription() const {
        return "{\"time\":" + std::to_string(std::time(nullptr)) + ",\"channel\":\"" + channel +
               "\",\"event\":\"subscribe\",\"payload\":" + payload + "}";
    }

    int handle(lws* socket, lws_callback_reasons reason, void* input, size_t length) {
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
                    callback(json);
                break;
            }
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            case LWS_CALLBACK_CLIENT_CLOSED:
            case LWS_CALLBACK_CLOSED:
                websocket = nullptr;
                break;
            default:
                break;
        }
        return 0;
    }

    lws_context* context = nullptr;
    lws* websocket = nullptr;
    std::string channel;
    std::string payload;
    Callback callback;
};

GateWebsocket::GateWebsocket(std::string channel, std::string payload, Callback callback)
    : _impl(std::make_unique<Impl>(std::move(channel), std::move(payload), std::move(callback))) {}

GateWebsocket::~GateWebsocket() = default;

bool GateWebsocket::connect() {
    if (_impl->websocket != nullptr)
        return true;
    if (_impl->context == nullptr) {
        lws_context_creation_info info{};
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = Impl::protocols();
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.gid = -1;
        info.uid = -1;
        _impl->context = lws_create_context(&info);
    }
    if (_impl->context == nullptr)
        return false;

    lws_client_connect_info info{};
    info.context = _impl->context;
    info.address = "api.gateio.ws";
    info.port = 443;
    info.path = "/ws/v4/";
    info.host = info.address;
    info.origin = info.address;
    info.protocol = Impl::protocols()[0].name;
    info.ssl_connection = LCCSCF_USE_SSL;
    info.userdata = _impl.get();
    _impl->websocket = lws_client_connect_via_info(&info);
    return _impl->websocket != nullptr;
}

void GateWebsocket::service(int timeout_ms) {
    if (_impl->context != nullptr)
        lws_service(_impl->context, timeout_ms);
}

bool GateWebsocket::matches(const std::string& channel, const std::string& payload) const {
    return _impl->channel == channel && _impl->payload == payload;
}
