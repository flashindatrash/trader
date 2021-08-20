#include "Algorithm.hpp"
#include "Settings.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    Algorithm* algorithm = new Algorithm();
    return algorithm;
}
