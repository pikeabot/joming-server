//
//  mechanics.hpp
//  joming-server
//
//  Created by Jocelyn Chang on 9/9/23.
//

#ifndef gamephysics_hpp
#define gamephysics_hpp

#include <stdio.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace gamephysics {
    struct PositionVector {
        float x;
        float y;
        float z;
    };
    struct VelocityVector {
        float x;
        float y;
        float z;
    };
    void to_json(json& j, const PositionVector& p);
    void from_json(const json& j, PositionVector& p);
    void to_json(json& j, const VelocityVector& p);
    void from_json(const json& j, VelocityVector& p);
}


#endif /* gamephysics_hpp */
