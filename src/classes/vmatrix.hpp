//
// Created by tevin on 16/07/2026.
//

#ifndef GUARACI_VMATRIX_HPP
#define GUARACI_VMATRIX_HPP

class VMatrix
{
public:
	float m[4][4];

	const float *operator[](const int index) const
	{
		return m[index];
	}
};

#endif //GUARACI_VMATRIX_HPP
