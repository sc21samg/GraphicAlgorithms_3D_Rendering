#include "cube.hpp"


#include "../vmlib/mat33.hpp"

SimpleMeshData make_cube(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
    // Initialize 'pos' to contain the vertex positions of the cube.
    std::vector<Vec3f> pos;

    // initialize 'nor' for storing normal vec postitions for each generated vertex.
    std::vector<Vec3f> nor;

    // Define step for subdivision
    float step = 1.0f / aSubdivs;

    // Define faces of the cube
    for (std::size_t i = 0; i < aSubdivs; ++i) {
        for (std::size_t j = 0; j < aSubdivs; ++j) {
            float x = -0.5f + i * step;
            float y = -0.5f + j * step;
            float nextX = x + step;
            float nextY = y + step;

            // Front face (Z = 0.5)
            pos.emplace_back(Vec3f{ x, y, 0.5f });
            pos.emplace_back(Vec3f{ nextX, y, 0.5f });
            pos.emplace_back(Vec3f{ x, nextY, 0.5f });
            pos.emplace_back(Vec3f{ nextX, y, 0.5f });
            pos.emplace_back(Vec3f{ nextX, nextY, 0.5f });
            pos.emplace_back(Vec3f{ x, nextY, 0.5f });

            // Front face (Z = 0.5)
            nor.emplace_back(Vec3f{ x, y, 0.5f });
            nor.emplace_back(Vec3f{ nextX, y, 0.5f });
            nor.emplace_back(Vec3f{ x, nextY, 0.5f });
            nor.emplace_back(Vec3f{ nextX, y, 0.5f });
            nor.emplace_back(Vec3f{ nextX, nextY, 0.5f });
            nor.emplace_back(Vec3f{ x, nextY, 0.5f });

            // Front face (Z = -0.5)
            pos.emplace_back(Vec3f{ x, y, -0.5f });
            pos.emplace_back(Vec3f{ nextX, y, -0.5f });
            pos.emplace_back(Vec3f{ x, nextY, -0.5f });
            pos.emplace_back(Vec3f{ nextX, y, -0.5f });
            pos.emplace_back(Vec3f{ nextX, nextY, -0.5f });
            pos.emplace_back(Vec3f{ x, nextY, -0.5f });

            // Front face (Z = -0.5)
            nor.emplace_back(Vec3f{ x, y, -0.5f });
            nor.emplace_back(Vec3f{ nextX, y, -0.5f });
            nor.emplace_back(Vec3f{ x, nextY, -0.5f });
            nor.emplace_back(Vec3f{ nextX, y, -0.5f });
            nor.emplace_back(Vec3f{ nextX, nextY, -0.5f });
            nor.emplace_back(Vec3f{ x, nextY, -0.5f });

            // Left face (X = -0.5)
            pos.emplace_back(Vec3f{ -0.5f, y, x });
            pos.emplace_back(Vec3f{ -0.5f, y, nextX });
            pos.emplace_back(Vec3f{ -0.5f, nextY, x });
            pos.emplace_back(Vec3f{ -0.5f, y, nextX });
            pos.emplace_back(Vec3f{ -0.5f, nextY, nextX });
            pos.emplace_back(Vec3f{ -0.5f, nextY, x });


            // Left face (X = -0.5)
            nor.emplace_back(Vec3f{ -0.5f, y, x });
            nor.emplace_back(Vec3f{ -0.5f, y, nextX });
            nor.emplace_back(Vec3f{ -0.5f, nextY, x });
            nor.emplace_back(Vec3f{ -0.5f, y, nextX });
            nor.emplace_back(Vec3f{ -0.5f, nextY, nextX });
            nor.emplace_back(Vec3f{ -0.5f, nextY, x });

            // Right face (X = 0.5)
            pos.emplace_back(Vec3f{ 0.5f, y, x });
            pos.emplace_back(Vec3f{ 0.5f, y, nextX });
            pos.emplace_back(Vec3f{ 0.5f, nextY, x });
            pos.emplace_back(Vec3f{ 0.5f, y, nextX });
            pos.emplace_back(Vec3f{ 0.5f, nextY, nextX });
            pos.emplace_back(Vec3f{ 0.5f, nextY, x });


            // Right face (X = 0.5)
            nor.emplace_back(Vec3f{ 0.5f, y, x });
            nor.emplace_back(Vec3f{ 0.5f, y, nextX });
            nor.emplace_back(Vec3f{ 0.5f, nextY, x });
            nor.emplace_back(Vec3f{ 0.5f, y, nextX });
            nor.emplace_back(Vec3f{ 0.5f, nextY, nextX });
            nor.emplace_back(Vec3f{ 0.5f, nextY, x });

            // Top face (Y = 0.5)
            pos.emplace_back(Vec3f{ x, 0.5f, y });
            pos.emplace_back(Vec3f{ nextX, 0.5f, y });
            pos.emplace_back(Vec3f{ x, 0.5f, nextY });
            pos.emplace_back(Vec3f{ nextX, 0.5f, y });
            pos.emplace_back(Vec3f{ nextX, 0.5f, nextY });
            pos.emplace_back(Vec3f{ x, 0.5f, nextY });

            // Top face (Y = 0.5)
            nor.emplace_back(Vec3f{ x, 0.5f, y });
            nor.emplace_back(Vec3f{ nextX, 0.5f, y });
            nor.emplace_back(Vec3f{ x, 0.5f, nextY });
            nor.emplace_back(Vec3f{ nextX, 0.5f, y });
            nor.emplace_back(Vec3f{ nextX, 0.5f, nextY });
            nor.emplace_back(Vec3f{ x, 0.5f, nextY });

            // Bottom face (Y = -0.5)
            pos.emplace_back(Vec3f{ x, -0.5f, y });
            pos.emplace_back(Vec3f{ nextX, -0.5f, y });
            pos.emplace_back(Vec3f{ x, -0.5f, nextY });
            pos.emplace_back(Vec3f{ nextX, -0.5f, y });
            pos.emplace_back(Vec3f{ nextX, -0.5f, nextY });
            pos.emplace_back(Vec3f{ x, -0.5f, nextY });

            // Bottom face (Y = -0.5)
            nor.emplace_back(Vec3f{ x, -0.5f, y });
            nor.emplace_back(Vec3f{ nextX, -0.5f, y });
            nor.emplace_back(Vec3f{ x, -0.5f, nextY });
            nor.emplace_back(Vec3f{ nextX, -0.5f, y });
            nor.emplace_back(Vec3f{ nextX, -0.5f, nextY });
            nor.emplace_back(Vec3f{ x, -0.5f, nextY });


            //more complete sides of the cube
            // Front face (Zdiff = 0.5)
            //nope
            pos.emplace_back(Vec3f{ 0.5f, x, y });
            pos.emplace_back(Vec3f{ 0.5f, nextX, y });
            pos.emplace_back(Vec3f{ 0.5f, x, nextY });
            pos.emplace_back(Vec3f{ 0.5f, nextX, y });
            pos.emplace_back(Vec3f{ 0.5f, nextX, nextY });
            pos.emplace_back(Vec3f{ 0.5f, x, nextY });

            // Right face (Xx = 0.5)
            //top side i think
            pos.emplace_back(Vec3f{  y, 0.5f, x });
            pos.emplace_back(Vec3f{  y, 0.5f, nextX });
            pos.emplace_back(Vec3f{ nextY, 0.5f, x });
            pos.emplace_back(Vec3f{ y, 0.5f, nextX });
            pos.emplace_back(Vec3f{ nextY, 0.5f, nextX });
            pos.emplace_back(Vec3f{ nextY, 0.5f, x });

            // Right face (Xx = 0.5)
            //top side i think
            nor.emplace_back(Vec3f{ y, 0.5f, x });
            nor.emplace_back(Vec3f{ y, 0.5f, nextX });
            nor.emplace_back(Vec3f{ nextY, 0.5f, x });
            nor.emplace_back(Vec3f{ y, 0.5f, nextX });
            nor.emplace_back(Vec3f{ nextY, 0.5f, nextX });
            nor.emplace_back(Vec3f{ nextY, 0.5f, x });

            // Right face (Xx = -0.5)
            //top side i think
            pos.emplace_back(Vec3f{ y, -0.5f, x });
            pos.emplace_back(Vec3f{ y, -0.5f, nextX });
            pos.emplace_back(Vec3f{ nextY, -0.5f, x });
            pos.emplace_back(Vec3f{ y, -0.5f, nextX });
            pos.emplace_back(Vec3f{ nextY, -0.5f, nextX });
            pos.emplace_back(Vec3f{ nextY, -0.5f, x });

            // Right face (Xx = -0.5)
            //top side i think
            nor.emplace_back(Vec3f{ y, -0.5f, x });
            nor.emplace_back(Vec3f{ y, -0.5f, nextX });
            nor.emplace_back(Vec3f{ nextY, -0.5f, x });
            nor.emplace_back(Vec3f{ y, -0.5f, nextX });
            nor.emplace_back(Vec3f{ nextY, -0.5f, nextX });
            nor.emplace_back(Vec3f{ nextY, -0.5f, x });

        }
    }

    if (aCapped) {
        // Iterate over the edge of each face and add caps
        for (std::size_t i = 0; i < aSubdivs; ++i) {
            float x = -0.5f + i * step;
            float nextX = x + step;

            // Cap at Y = 0.5 (top face)
            pos.emplace_back(Vec3f{ x, 0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 1, 0 });

            pos.emplace_back(Vec3f{ nextX, 0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 1, 0 });

            pos.emplace_back(Vec3f{ x, 0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 1, 0 });

            pos.emplace_back(Vec3f{ nextX, 0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 1, 0 });

            pos.emplace_back(Vec3f{ nextX, 0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 1, 0 });

            pos.emplace_back(Vec3f{ x, 0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 1, 0 });

            // Similar caps can be added for the other 5 faces
            // Adjust the coordinates and normals accordingly

            // Bottom Face (Y = -0.5)
            pos.emplace_back(Vec3f{ x, -0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, -1, 0 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, -1, 0 });

            pos.emplace_back(Vec3f{ x, -0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, -1, 0 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, -1, 0 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, -1, 0 });

            pos.emplace_back(Vec3f{ x, -0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, -1, 0 });


            // Front Face (Z = 0.5)
            pos.emplace_back(Vec3f{ x, -0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 0, 1 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 0, 1 });

            pos.emplace_back(Vec3f{ x, 0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 0, 1 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 0, 1 });

            pos.emplace_back(Vec3f{ nextX, 0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 0, 1 });

            pos.emplace_back(Vec3f{ x, 0.5f, 0.5f });
            nor.emplace_back(Vec3f{ 0, 0, 1 });

            // Back Face (Z = -0.5)
            pos.emplace_back(Vec3f{ x, -0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 0, -1 });

            pos.emplace_back(Vec3f{ x, 0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 0, -1 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 0, -1 });

            pos.emplace_back(Vec3f{ x, 0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 0, -1 });

            pos.emplace_back(Vec3f{ nextX, 0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 0, -1 });

            pos.emplace_back(Vec3f{ nextX, -0.5f, -0.5f });
            nor.emplace_back(Vec3f{ 0, 0, -1 });

        }
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
