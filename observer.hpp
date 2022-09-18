#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include "accis.hpp"

class observer {

    private:

        accis_sat sat1;
        accis_sat sat2;

    public:

        void set_param(const pydict::dict& d1, const pydict::dict& d2);

        vec<> f(cvec<> x);

        vec<> h(cvec<> x);

}

#endif
