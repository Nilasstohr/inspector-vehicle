//
// Created by robot1 on 5/5/26.
//

#ifndef POSITIONREQUESTCOMMAND_H
#define POSITIONREQUESTCOMMAND_H

#include "StringUtils.h"

class DistanceReadRequestCommand {
public:
    template<size_t N = 32>
    [[nodiscard]] static etl::string<N> handle(const float left, const float right){
        etl::string<32> request;
        auto precision = etl::format_spec().precision(3);
        request.append(utils::floatToString(left,precision));
        request.append(" ");
        request.append(utils::floatToString(right,precision));
        return request;
    }
};



#endif //POSITIONREQUESTCOMMAND_H
