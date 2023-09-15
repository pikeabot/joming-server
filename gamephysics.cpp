//
//  mechanics.cpp
//  joming-server
//
//  Created by Jocelyn Chang on 9/9/23.
//

#include "gamephysics.hpp"

namespace gamephysics {
    void to_json(json& j, const PositionVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, PositionVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }

    void to_json(json& j, const VelocityVector& v) {
        j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
    }

    void from_json(const json& j, VelocityVector& v) {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.x);
    }

    void to_json(json& j, const MotionVector& m) {
        j = json{ {"t", m.t}, {"xp", m.xp},{"yp", m.yp}, {"zp", m.zp}, {"xv", m.xv}, {"yv", m.yv}, {"zv", m.zv} };
    }

    void from_json(const json& j, MotionVector& m) {
        j.at("t").get_to(m.t);
        j.at("xp").get_to(m.xp);
        j.at("yp").get_to(m.yp);
        j.at("zp").get_to(m.xp);
        j.at("xv").get_to(m.xv);
        j.at("yv").get_to(m.yv);
        j.at("zv").get_to(m.xv);
    }
} // namespace ns
