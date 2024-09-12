#include "cylinder.hpp"

#include "../vmlib/mat33.hpp"
/*
	Generating the vertex positions of the cylinder's outer shell by walking around a circle
	in the YZ-plane and creating triangles for each segment
*/
SimpleMeshData make_cylinder(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
	//TODO: implement me

	// initialize 'pos' for containg the vertex positions of the cylinder's shell. 
	std::vector<Vec3f> pos;
	// initialize 'nor' for storing normal vec postitions for each generated vertex.
	std::vector<Vec3f> nor;
	// initialize previous YZ Coordinates to hold the cosine and sine value 
	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	// loop over subdivisions 
	for (std::size_t i = 0; i < aSubdivs; ++i) {
		// calculate current angle 
		float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
		// calculate current YZ Coordinates 
		float y = std::cos(angle);
		float z = std::sin(angle);
		// create 2 triangles(forming a quad) for one shell segment	
		// Two triangles (= 3*2 positions) create one segment of the cylinder's shell. 
		// emplace_back (=push back) -> �ٸ� ���� ��Ұ� ������� �ʰ� �ٷ� vec�� ���� 
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		// compute normals for each generated vertex as well 
		nor.emplace_back(Vec3f{ 0.f,prevY, prevZ });
		pos.emplace_back(Vec3f{ 0.f, y, z });
		nor.emplace_back(Vec3f{ 0.f,y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		nor.emplace_back(Vec3f{ 0.f,prevY, prevZ });

		pos.emplace_back(Vec3f{ 0.f, y, z });
		nor.emplace_back(Vec3f{ 0.f,y, z });
		pos.emplace_back(Vec3f{ 1.f, y, z });
		nor.emplace_back(Vec3f{ 0.f,y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		nor.emplace_back(Vec3f{ 0.f,prevY, prevZ });
		// cap on 
		if (aCapped) {
			// Add top cap
			// (-1; 0; 0) if point on cap at x = 0
			pos.emplace_back(Vec3f{ 0.f, y, z });
nor.emplace_back(Vec3f{ -1, 0, 0 }); // Assuming the normal for the top cap is along -x axis

pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
nor.emplace_back(Vec3f{ -1, 0, 0 });

pos.emplace_back(Vec3f{ 0.f, 0, 0 });
nor.emplace_back(Vec3f{ -1, 0, 0 });

			// Add bottom cap
			//(1; 0; 0); if point on cap at x = 1.
pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
nor.emplace_back(Vec3f{ 1, 0, 0 }); // Assuming the normal for the bottom cap is along +x axis

pos.emplace_back(Vec3f{ 1.f, y, z });
nor.emplace_back(Vec3f{ 1, 0, 0 });

pos.emplace_back(Vec3f{ 1.f, 0, 0 });
nor.emplace_back(Vec3f{ 1, 0, 0 });
		}

		// update previous YZ coordinates 
		prevY = y;
		prevZ = z;
	}
	// pre-compute N once by extracting the 3x3 submatrix of inverse-transpose of M
	Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

	// Transform the positions with the aPreTransform matrix
	for (auto& p : pos) {
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;

		p = Vec3f{ t.x, t.y, t.z };
	}
	for (auto& n : nor)
	{
		Vec3f p4{ n.x, n.y, n.z };
		Vec3f t = N * p4;

		n = Vec3f{ t.x, t.y, t.z };
	}

	std::vector col(pos.size(), aColor);
	return SimpleMeshData{ std::move(pos), std::move(col), std::move(nor) };

}
