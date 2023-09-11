//
//  mechanics.cpp
//  joming-server
//
//  Created by Jocelyn Chang on 9/9/23.
//

#include "gamephysics.hpp"

namespace gamephysics {
//    struct PositionVector {
//        float x;
//        float y;
//        float z;
//    };
//    struct VelocityVector {
//        float x;
//        float y;
//        float z;
//    };

    void to_json(json& j, const PositionVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, PositionVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }

    void to_json(json& j, const VelocityVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, VelocityVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }
}
