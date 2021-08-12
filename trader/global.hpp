#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <vector>

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)

namespace Json {
    class Value;
}
