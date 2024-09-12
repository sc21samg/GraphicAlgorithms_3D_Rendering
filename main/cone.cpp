#include "cone.hpp"


#include "../vmlib/mat33.hpp"


SimpleMeshData make_cone(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
    // initialize 'pos' for containing the vertex positions of the cone's shell.
    std::vector<Vec3f> pos;
    std::vector<Vec3f> nor;
    // initialize previous YZ Coordinates to hold the cosine and sine value
    float prevY = std::cos(0.f);
    float prevZ = std::sin(0.f);
    // loop over subdivisions
    for (std::size_t i = 0; i < aSubdivs; ++i)
    {
        // calculate current angle
        float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
        // calculate current YZ Coordinates
        float y = std::cos(angle);
        float z = std::sin(angle);
        // create triangle for one shell segment
        // one triangle (= 3 positions) create one segment of the cone's shell.
        // emplace_back (=push back) -> different point is inserted directly into vec without copying
        pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f }); // Apex of the cone
        nor.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
        pos.emplace_back(Vec3f{ 1.f, y, z });
        nor.emplace_back(Vec3f{ 1.f, y, z });
        pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
        nor.emplace_back(Vec3f{ 1.f, prevY, prevZ });


        // update previous YZ coordinates
        prevY = y;
        prevZ = z;
    }

    if (aCapped)
    {
        // Add bottom cap
        pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
        nor.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
        pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
        nor.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
        pos.emplace_back(Vec3f{ 0.5f, 0.f, 0.5f });
        nor.emplace_back(Vec3f{ 0.5f, 0.f, 0.5f });
    }

    // pre-compute N once by extracting the 3x3 submatrix of inverse-transpose of M
    Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

    // Transform the positions with the aPreTransform matrix
    for (auto& p : pos)
    {
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
