#ifndef FORMAT_HPP
#define FORMAT_HPP

#include <string>

inline std::string int2str0(int n, unsigned int w) {
    std::string s = std::to_string(n);
    s.insert(0, w - s.length(), '0');
    return s;
}

#endif
