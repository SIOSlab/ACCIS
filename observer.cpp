#include "observer.hpp"

void observer::set_param(const pydict::dict& d1, const pydict::dict& d2) {
    sat1.set_param(d1);
    sat2.set_param(d2);
}

vec<> observer::f(cvec<> x) {

}

vec<> observer::h(cvec<> x) {

}
