#pragma once

struct matrix3x4
{
	float m[3][4];

	float* operator[](int row)
	{
		return m[row];
	}

	const float* operator[](int row) const
	{
		return m[row];
	}

	float get(int row, int col) const { return m[row][col]; }
	void set(int row, int col, float val) { m[row][col] = val; }
};

struct matrix3x4_array
{
	matrix3x4* data;
	int count;

	matrix3x4_array(matrix3x4* ptr = nullptr, int size = 128) : data(ptr), count(size) {}

	matrix3x4* get(int index)
	{
		if (data && index >= 0 && index < count)
			return &data[index];

		return nullptr;
	}

	int size() const { return count; }
};

using matrix3x4_t = matrix3x4;