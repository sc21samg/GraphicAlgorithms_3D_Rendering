// You will need to define your own tests. Refer to CW1 or Exercise G.3 for
// examples.
#define CATCH_CONFIG_MAIN


#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"


TEST_CASE("Matrix multiplication", "[mat44]")
{
	// Test matrix multiplication

	// Define two matrices for multiplication
	Mat44f matA = { {
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	} };

	Mat44f matB = { {
		17.0f, 18.0f, 19.0f, 20.0f,
		21.0f, 22.0f, 23.0f, 24.0f,
		25.0f, 26.0f, 27.0f, 28.0f,
		29.0f, 30.0f, 31.0f, 32.0f
	} };

	// Expected result of matrix multiplication (pre-calculated or using a trusted library)
	Mat44f expectedResult = { {
		250.0f, 260.0f, 270.0f, 280.0f,
		618.0f, 644.0f, 670.0f, 696.0f,
		986.0f, 1028.0f, 1070.0f, 1112.0f,
		1354.0f, 1412.0f, 1470.0f, 1528.0f
	} };

	// Perform matrix multiplication
	Mat44f result = matA * matB;

	// Verify each element of the resulting matrix
	for (size_t i = 0; i < 16; ++i) {
		REQUIRE(result.v[i] == Catch::Approx(expectedResult.v[i]));
	}
}

TEST_CASE("Matrix-vector multiplication", "[mat44]")
{
	// Test matrix-vector multiplication

	// Define a 4x4 matrix
	Mat44f matrix = { {
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	} };

	// Define a 4-dimensional vector
	Vec4f vector = { 1.0f, 2.0f, 3.0f, 4.0f };

	// Expected result of matrix-vector multiplication (pre-calculated or using a trusted library)
	Vec4f expectedResult = { 30.0f, 70.0f, 110.0f, 150.0f };

	// Perform matrix-vector multiplication
	Vec4f result = matrix * vector;

	// Verify each element of the resulting vector
	for (size_t i = 0; i < 4; ++i) {
		REQUIRE(result[i] == Catch::Approx(expectedResult[i]));
	}
}


TEST_CASE("4x4 rotation around Z axis", "[rotation][mat44]")

{
	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	// Simple check: rotating zero degrees should yield an idenity matrix
	SECTION("Identity")
	{
		auto const identity = make_rotation_z(0.f);

		REQUIRE_THAT(identity(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 3), WithinAbs(1.f, kEps_));
	}

	// Rotating 90 degrees = pi/2 radians.
	SECTION("90 degrees")
	{
		auto const right = make_rotation_z(3.1415926f / 2.f);

		REQUIRE_THAT(right(0, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 1), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(right(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(1, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(1, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 3), WithinAbs(1.f, kEps_));
	}

	// Rotating -90 degrees = -pi/2 radians.
	SECTION("-90 degrees")
	{
		auto const right = make_rotation_z(-3.1415926f / 2.f);

		REQUIRE_THAT(right(0, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(1, 0), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(right(1, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(right(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(right(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(right(3, 3), WithinAbs(1.f, kEps_));
	}
}


TEST_CASE("Perspective projection", "[mat44]")
{
	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	// "Standard" projection matrix presented in the exercises. Assumes
	// standard window size (e.g., 1280x720).
	//
	// Field of view (FOV) = 60 degrees
	// Window size is 1280x720 and we defined the aspect ratio as w/h
	// Near plane at 0.1 and far at 100
	SECTION("Standard")
	{
		auto const proj = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			1280 / float(720),
			0.1f, 100.f
		);

		REQUIRE_THAT(proj(0, 0), WithinAbs(0.974279, kEps_));
		REQUIRE_THAT(proj(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(proj(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(1, 1), WithinAbs(1.732051f, kEps_));
		REQUIRE_THAT(proj(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(proj(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(2, 2), WithinAbs(-1.002002f, kEps_));
		REQUIRE_THAT(proj(2, 3), WithinAbs(-0.200200f, kEps_));

		REQUIRE_THAT(proj(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(proj(3, 2), WithinAbs(-1.f, kEps_));
		REQUIRE_THAT(proj(3, 3), WithinAbs(0.f, kEps_));
	}
}

TEST_CASE("Make translation matrix", "[mat44]")
{
	// Test make_translation function

	// Define translation vector
	Vec3f translationVector = { 1.0f, 2.0f, 3.0f };

	// Expected translation matrix
	Mat44f expectedTranslationMatrix = { {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 2.0f,
		0.0f, 0.0f, 1.0f, 3.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	} };

	// Create the translation matrix
	Mat44f translationMatrix = make_translation(translationVector);

	// Verify each element of the translation matrix
	for (size_t i = 0; i < 16; ++i) {
		REQUIRE(translationMatrix.v[i] == Catch::Approx(expectedTranslationMatrix.v[i]));
	}
}