#include "accis_sim.hpp"

#include "accis_sat.hpp"

std::vector<sat_results> run_accis_sim(const sim_info& sim_i,
        const std::vector<sat_info>& sat_i) {

    int num_sats = sat_i.size();

    std::vector<accis_sat> sats;
    for (int i = 0; i < num_sats; i++)
        sats.emplace_back(sim_i, sat_i[i]);

    for (int k = 0; k <= sim_i.num_steps; k++) {
        for (int i = 0; i < num_sats; i++)
            sats[i].step();
        for (int i = 0; i < num_sats; i++)
            for (int j = 0; j < num_sats; j++)
                if (i != j)
                    sats[i].transmit_to(sats[j]);
    }

    std::vector<sat_results> res;
    for (int i = 0; i < num_sats; i++)
        res.push_back(sats[i].get_results());

    return res;

}
