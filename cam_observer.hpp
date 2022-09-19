#ifndef CAM_OBSERVER_HPP
#define CAM_OBSERVER_HPP

#include "accis.hpp"
#include "mat.hpp"

class cam_observer {

    private:

        accis_sat sat1;
        accis_sat sat2;

        mat<4> pts1;
        mat<4> pts2;

    public:

        void set_param(const pydict::dict& d1, const pydict::dict& d2);

        void set_pts(const mat<>& pts1_in, const mat<>& pts2_in);

        vec<> h(cvec<> dx_cam);

};

#endif
