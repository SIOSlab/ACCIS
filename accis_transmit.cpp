#include "accis.hpp"

void accis_sat::transmit(accis_sat& target) {

    if (tr_last.step == step_no && target.sat_id != sat_id)
        target.cc.train.push_back(tr_last);

}
