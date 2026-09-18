#pragma once

#include <algorithm>
#include <cmath>

constexpr double RADIANS_TO_DEGREES = (static_cast<float>(180.0f / M_PI));
constexpr double DEGREES_TO_RADIANS = static_cast<double>(M_PI/180.0f);

constexpr int VEC3_X = 0;
constexpr int VEC3_Y = 1;
constexpr int VEC3_Z = 2;

struct Vec3
{
	float x, y, z;

	explicit Vec3(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) : x(x), y(y), z(z) {}

	float& operator[](const int index)
	{
		return (&x)[index];
	}

	const float& operator[](const int index) const
	{
		return (&x)[index];
	}

	Vec3 operator+(const Vec3& other) const
	{
		return Vec3{x + other.x, y + other.y, z + other.z};
	}

	Vec3 operator-(const Vec3& other) const
	{
		return Vec3{x - other.x, y - other.y, z - other.z};
	}

	Vec3 operator/(const float scalar) const
	{
		return Vec3{x / scalar, y / scalar, z / scalar};
	}

	Vec3 operator*(const float scalar) const
	{
		return Vec3{x * scalar, y * scalar, z * scalar};
	}

	Vec3 operator*(const Vec3& other) const
	{
		return Vec3{x * other.x, y * other.y, z * other.z};
	}

	Vec3& operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vec3& operator*=(const Vec3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	void Set(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	[[nodiscard]] float Length() const
	{
		return std::sqrt(x*x + y*y + z*z);
	}

	[[nodiscard]] float Length2D() const
	{
		return std::sqrt(x*x + y*y);
	}

	[[nodiscard]] float DistanceTo(const Vec3& to) const
	{
		const Vec3 diff = to - *this;
		const float len = diff.Length();

		if (len < 0.0001f)
			return 0.0f;

		return len;
	}

	[[nodiscard]] float Dot(const Vec3& other) const
	{
		return x*other.x + y*other.y + z*other.z;
	}

	float Normalize()
	{
		const float length = Length();

		if (length < 0.0001f)
			return 0.0f;

		x /= length;
		y /= length;
		z /= length;

		return length;
	}

	void AngleVectors(Vec3* forward = nullptr, Vec3* right = nullptr, Vec3* up = nullptr) const
	{
		const auto pitch_rad = static_cast<float>(x * DEGREES_TO_RADIANS);
		const auto yaw_rad = static_cast<float>(y * DEGREES_TO_RADIANS);
		const auto roll_rad = static_cast<float>(z * DEGREES_TO_RADIANS);

		const float sp = std::sin(pitch_rad);
		const float cp = std::cos(pitch_rad);
		const float sy = std::sin(yaw_rad);
		const float cy = std::cos(yaw_rad);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right || up)
		{
			const float sr = std::sin(roll_rad);
			const float cr = std::cos(roll_rad);

			if (right)
			{
				right->x = -1 * sr * sp * cy + -1 * cr * -sy;
				right->y = -1 * sr * sp * sy + -1 * cr * cy;
				right->z = -1 * sr * cp;
			}

			if (up)
			{
				up->x = cr * sp * cy + -sr * -sy;
				up->y = cr * sp * sy + -sr * cy;
				up->z = cr * cp;
			}
		}
	}

	Vec3& operator-=(const Vec3& vec3)
	{
		x -= vec3.x;
		y -= vec3.y;
		z -= vec3.z;
		return *this;
	}

	Vec3& operator+=(const Vec3& vec3)
	{
		x += vec3.x;
		y += vec3.y;
		z += vec3.z;
		return *this;
	}

	bool operator==(const Vec3& vec3) const
	{
		return x == vec3.x && y == vec3.y && z == vec3.z;
	}

	Vec3& operator=(const float v)
	{
		x = v;
		y = v;
		z = v;
		return *this;
	}

	static float NormalizeAngle(float angle)
	{
		if (std::isfinite(angle)) [[likely]]
			angle = std::remainder(angle, 360.0f);
		else
			angle = 0.0f;

		return angle;
	}

	static void ClampAngles(Vec3& vec)
	{
		vec.x = std::clamp(NormalizeAngle(vec.x), -89.0f, 89.0f);
		vec.y = NormalizeAngle(vec.y);
		vec.z = 0.0f;
	}

	[[nodiscard]] Vec3 AngleTo(const Vec3& to) const
	{
		const Vec3 diff = to - *this;

		Vec3 angle {};

		if (diff.x != 0.0f || diff.y != 0.0f)
		{
			angle.x = static_cast<float>(std::atan2(-diff.z, diff.Length2D()) * RADIANS_TO_DEGREES);
			angle.y = static_cast<float>(std::atan2(diff.y, diff.x) * RADIANS_TO_DEGREES);
		}
		else
		{
			angle.x = (diff.z > 0.0f) ? 270.0f : 90.0f;
			angle.y = 0.0f;
		}
		angle.z = 0.0f;

		ClampAngles(angle);
		return angle;
	}

	[[nodiscard]] double GetFovTo(const Vec3& angle) const
	{
		Vec3 from_forward;
		AngleVectors(&from_forward);

		Vec3 to_forward;
		angle.AngleVectors(&to_forward);

		float dot = from_forward.Dot(to_forward);
		dot = std::clamp(dot, -1.0f, 1.0f);

		return (std::acos(dot)) * RADIANS_TO_DEGREES;
	}

	[[nodiscard]] Vec3 GetEulerAngles() const
	{
		Vec3 angles{};

		if (const float forward = Length2D(); forward < 0.00001f) [[unlikely]]
		{
			angles.x = (z > 0.0f) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			auto pitch = static_cast<float>(std::atan2(-z, forward) * RADIANS_TO_DEGREES);

			if (pitch < 0.0f)
				pitch += 360.0f;

			auto yaw = static_cast<float>(std::atan2(y, x) * RADIANS_TO_DEGREES);

			if (yaw < 0.0f)
				yaw += 360.0f;

			angles.x = pitch;
			angles.y = yaw;
		}

		angles.z = 0.0f;

		return angles;
	}

	[[nodiscard]] Vec3 ToAngle() const
	{
		return Vec3
		{	static_cast<float>(RADIANS_TO_DEGREES * (std::atan2(-z, std::hypot(x, y)))),
			static_cast<float>(RADIANS_TO_DEGREES * (std::atan2(y, x))),
			0.f
		};
	}

	[[nodiscard]] float LengthSqr() const
	{
		return x*x + y*y + z*z;
	}
};

using QAngle = Vec3;

struct Vec3Aligned : public Vec3
{
	float w {};

	Vec3Aligned &operator=(const Vec3 &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
};