#include "GateWebsocket.hpp"
#include "core/Logger.hpp"
#include <json/json.h>
#include <libwebsockets.h>
#include <openssl/hmac.h>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <sstream>
#include <vector>

struct GateWebsocket::Impl {
    Impl(std::string channel, std::string payload, Callback callback,
         std::string api_key, std::string secret_key, std::string url)
        : channel(std::move(channel)), payload(std::move(payload)), callback(std::move(callback)),
          api_key(std::move(api_key)), secret_key(std::move(secret_key)), url(std::move(url)) {}

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
        const auto timestamp = std::time(nullptr);
        Json::Value message;
        message["time"] = Json::Int64(timestamp);
        message["channel"] = channel;
        message["event"] = "subscribe";
        Json::CharReaderBuilder reader;
        std::string errors;
        std::istringstream stream(payload);
        Json::parseFromStream(reader, stream, &message["payload"], &errors);
        if (!api_key.empty()) {
            const std::string input = "channel=" + channel + "&event=subscribe&time=" + std::to_string(timestamp);
            unsigned char digest[EVP_MAX_MD_SIZE];
            unsigned int length = 0;
            HMAC(EVP_sha512(), secret_key.data(), static_cast<int>(secret_key.size()),
                 reinterpret_cast<const unsigned char*>(input.data()), input.size(), digest, &length);
            std::ostringstream signature;
            signature << std::hex << std::setfill('0');
            for (unsigned i = 0; i < length; ++i) signature << std::setw(2) << unsigned(digest[i]);
            message["auth"]["method"] = "api_key";
            message["auth"]["KEY"] = api_key;
            message["auth"]["SIGN"] = signature.str();
        }
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "";
        return Json::writeString(writer, message);
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
                if (Json::parseFromStream(builder, stream, &json, &errors)) {
                    if (!json["error"].isNull())
                        Logger::info("GateWebsocket: subscription error for " + channel + ": " + json["error"].toStyledString());
                    else if (json["event"].asString() == "update" ||
                             (channel == "spot.balances" && json["event"].asString() == "subscribe"))
                        callback(json);
                }
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
    std::string api_key, secret_key, url;
};

GateWebsocket::GateWebsocket(std::string channel, std::string payload, Callback callback,
                           std::string api_key, std::string secret_key, std::string url)
    : _impl(std::make_unique<Impl>(std::move(channel), std::move(payload), std::move(callback),
                                 std::move(api_key), std::move(secret_key), std::move(url))) {}

GateWebsocket::~GateWebsocket() = default;

bool GateWebsocket::connect() {
    if (_impl->websocket != nullptr)
        return true;
    if (_impl->context == nullptr) {
        lws_set_log_level(LLL_ERR | LLL_WARN, nullptr);

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
    std::vector<char> url(_impl->url.begin(), _impl->url.end());
    url.push_back('\0');
    const char *protocol, *address, *path;
    int port;
    if (lws_parse_uri(url.data(), &protocol, &address, &port, &path) || std::string(protocol) != "wss")
        return false;
    const std::string request_path = "/" + std::string(path);
    info.context = _impl->context;
    info.address = address;
    info.port = port;
    info.path = request_path.c_str();
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
