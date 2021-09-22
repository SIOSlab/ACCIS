#ifndef ACCIS_SAT_HPP
#define ACCIS_SAT_HPP

#include "cross_cal.hpp"

#include <queue>
#include <string>
#include <unordered_map>

class accis_sat {

    public:

        // Constructor
        accis_sat(const std::unordered_map<std::string,int>& par_i_in,  
                  const std::unordered_map<std::string,double>& par_d_in);

        // Get all parameters
        std::unordered_map<std::string,int>    get_params_i();
        std::unordered_map<std::string,double> get_params_d();

        // Move simulation by one time step
        void step();

        // Send transmission to another satellite (if available or possible)
        void transmit(accis_sat& other_sat);

    private:

        // Get parameter or set value to default (used by constructor)
        int    get_pard_i(const std::string var, int    val);
        double get_pard_d(const std::string var, double val);

        // Parameters
        std::unordered_map<std::string,int>    params_i;
        std::unordered_map<std::string,double> params_d;

        // Transmissions received but not yet processed
        std::queue<cross_cal::transmission> trq; 

};

#endif
