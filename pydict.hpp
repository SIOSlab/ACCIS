#ifndef PYDICT_HPP
#define PYDICT_HPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

#include <string>
#include <unordered_map>

namespace pydict {

    using dict = std::unordered_map<std::string, pybind11::object>;

    template <typename T>
    T get(const dict& d, const std::string& key) {
        return d.at(key).cast<T>();
    }

    template <typename T> 
    void set(dict& d, const std::string& key, const T& val) {
        d[key] = pybind11::cast(val);
    }

    inline bool contains(const dict& d, const std::string& key) {
        return d.count(key) > 0;
    }

    template <typename T>
    T getset(dict& d, const std::string& key, const T& default_val) {
        if (contains(d, key)) {
            return get<T>(d, key);
        } else {
            set<T>(d, key, default_val);
            return default_val;
        }
    }

}

#endif
