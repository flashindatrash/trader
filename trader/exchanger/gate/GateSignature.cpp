#include "GateSignature.hpp"
#include <openssl/hmac.h>
#include <iomanip>
#include <sstream>

namespace gate {
std::string hmac512(const std::string& key, const std::string& value) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int length = 0;
    HMAC(EVP_sha512(), key.data(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(value.data()), value.size(), digest, &length);
    std::ostringstream signature;
    signature << std::hex << std::setfill('0');
    for (unsigned i = 0; i < length; ++i)
        signature << std::setw(2) << unsigned(digest[i]);
    return signature.str();
}
}
