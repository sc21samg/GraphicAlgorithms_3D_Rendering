#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	Mat44f result = kIdentity44f;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			int y1 = y * 4;
			result.v[x + (y * 4)] = (aLeft.v[y1] * aRight.v[x])
				+ (aLeft.v[y1 + 1] * aRight.v[x + 4])
				+ (aLeft.v[y1 + 2] * aRight.v[x + 8])
				+ (aLeft.v[y1 + 3] * aRight.v[x + 12]);
		}
	}
	return result;
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	float x = (aLeft.v[0] * aRight.x) + (aLeft.v[1] * aRight.y) + (aLeft.v[2] * aRight.z) + (aLeft.v[3] * aRight.w);
	float y = (aLeft.v[4] * aRight.x) + (aLeft.v[5] * aRight.y) + (aLeft.v[6] * aRight.z) + (aLeft.v[7] * aRight.w);
	float z = (aLeft.v[8] * aRight.x) + (aLeft.v[9] * aRight.y) + (aLeft.v[10] * aRight.z) + (aLeft.v[11] * aRight.w);
	float w = (aLeft.v[12] * aRight.x) + (aLeft.v[13] * aRight.y) + (aLeft.v[14] * aRight.z) + (aLeft.v[15] * aRight.w);
	return { x, y, z, w };
}

// Functions:

Mat44f invert( Mat44f const& aM ) noexcept;

inline
Mat44f transpose( Mat44f const& aM ) noexcept
{
	Mat44f ret;
	for( std::size_t i = 0; i < 4; ++i )
	{
		for( std::size_t j = 0; j < 4; ++j )
			ret(j,i) = aM(i,j);
	}
	return ret;
}

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f result = {
		1, 0, 0, 0,
		0, static_cast<float>(cos(aAngle)), -1 * static_cast<float>(sin(aAngle)), 0,
		0, static_cast<float>(sin(aAngle)), static_cast<float>(cos(aAngle)), 0,
		0, 0, 0, 1
	};
	return result;
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f result = {
		static_cast<float>(cos(aAngle)), 0, static_cast<float>(sin(aAngle)), 0,
		0, 1, 0, 0,
		-static_cast<float>(sin(aAngle)), 0, static_cast<float>(cos(aAngle)), 0,
		0, 0, 0, 1
	};

	return result;
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f result = {
		static_cast<float>(cos(aAngle)), -static_cast<float>(sin(aAngle)), 0, 0,
		static_cast<float>(sin(aAngle)), static_cast<float>(cos(aAngle)), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	return result;
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f result = {
	1, 0, 0, aTranslation.x,
	0, 1, 0, aTranslation.y,
	0, 0, 1, aTranslation.z,
	0, 0, 0, 1
	};
	return result;
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	Mat44f result = {
	aSX, 0, 0, 0,
	0, aSY, 0, 0,
	0, 0, aSZ, 0,
	0, 0, 0, 1
	};
	return result;
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	float s = 1 / (tan(aFovInRadians / 2));
	Mat44f result = {
		s / aAspect, 0, 0, 0,
		0, s, 0, 0,
		0, 0, -1 * ((aFar + aNear) / (aFar - aNear)), -2 * ((aFar * aNear) / (aFar - aNear)),
		0, 0, -1, 0
	};
	return result;
}




#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
