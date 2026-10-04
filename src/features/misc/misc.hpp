//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_MISC_HPP
#define JACI_MISC_HPP

#include "../feature_register.hpp"

// gotta register the f_sv_pure_bypass::enabled
class f_sv_pure_bypass : public i_feature
{
public:
        F_NAME_CATEGORY(sv_pure bypass, category::misc)
};

#endif //JACI_MISC_HPP
