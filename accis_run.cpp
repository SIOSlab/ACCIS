#include "accis.hpp"

void accis_run(std::vector<accis_sat>& sats, int steps) {

    int num_sat = sats.size();

    for (int k = 0; k < steps; k++) {

        for (int i = 0; i < num_sat; i++)
            sats[i].step();

        for (int i = 0; i < num_sat; i++)
            for (int j = 0; j < num_sat; j++)
                if (i != j)
                    sats[i].transmit(sats[j]);

    }

}
