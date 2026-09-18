//
// Created by tevin on 11/09/2026.
//

#ifndef JACI_MULTIPOINT_HPP
#define JACI_MULTIPOINT_HPP

#include <cstdint>
#include <vector>

namespace multipoint
{
	using points = std::vector<float>;

	[[nodiscard]] points generate_points(uint32_t amount, float min, float max);
}

#endif //JACI_MULTIPOINT_HPP
