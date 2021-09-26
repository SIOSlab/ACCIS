#ifndef ACCIS_SAT_HPP
#define ACCIS_SAT_HPP

#include "pydict.hpp"

#include <vector>

class accis_sat {

    public:

        accis_sat();

        void set_param(const pydict::dict& d);

        pydict::dict get_param();

        pydict::dict get_results();

        void step();

        void transmit(accis_sat& target);

    private:

        // Called by constructor and set_param
        void setup();

        // Input parameters
        pydict::dict param;

};

void accis_run(std::vector<accis_sat>& sats, int steps);

#endif
