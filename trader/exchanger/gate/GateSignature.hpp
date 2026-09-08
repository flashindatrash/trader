#pragma once

#include <string>

namespace gate {
// Keep OpenSSL types out of headers used by GnuTLS-backed libwebsockets.
std::string hmac512(const std::string& key, const std::string& value);
}
