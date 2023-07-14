#ifndef SPARSIFICATION_SOLUTION_H
#define SPARSIFICATION_SOLUTION_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Solver {

    class Solution {

    public:

        virtual void to_json(std::string fn) = 0;

        virtual void plot(std::string fn) = 0;

    };

}


#endif //SPARSIFICATION_SOLUTION_H
