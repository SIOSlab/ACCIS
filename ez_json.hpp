#ifndef EZ_JSON_HPP
#define EZ_JSON_HPP

#include "nlohmann/json.hpp"

#include <fstream>
#include <string>

template <typename T> void ez_json_read(const std::string filename, T& obj) {

    nlohmann::json j;

    std::ifstream i(filename);

    i >> j;

    i.close();

    obj = j.get<T>();

}

#endif
