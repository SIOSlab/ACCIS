#ifndef ACCIS_SAT_HPP
#define ACCIS_SAT_HPP

#include "accis_io.hpp"
#include "filter.hpp"

#include <memory>

class accis_sat {

    public:

        accis_sat(const sim_info& sim_i, const sat_info& sat_i); 

        void step();

        void transmit_to(accis_sat& other_sat);

        sat_results get_results();

    private:

        std::shared_ptr<filter::base> filt;

        static void show_error(const std::string& msg);

};

#endif
