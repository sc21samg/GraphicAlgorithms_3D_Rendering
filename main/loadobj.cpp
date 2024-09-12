#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include <cstdio>
#include <cstddef>
#include <cstring>
#include <vector>

#include "../support/error.hpp"
namespace
{
	// See readme.md for a description of the file structure!

	// The "file magic" occurs at the very start of the file and serves as an
	// identifier for this type of files. "Magic" codes such as these are very
	// common in many file formats. You can find a list of some of these at
	//   https://en.wikipedia.org/wiki/List_of_file_signatures
	// 
	// There are a few considerations when creating a file magic. If the file
	// is a binary file (not text), it is good practice to include a NUL byte
	// ('\0') or a similarly unprintable character early, preferable in the
	// first 4 or 8 bytes. This prevents the file from being misidentified as
	// text. It is probably a good idea to use a file magic that is a multiple
	// of 4 or 8 bytes in length, to help with alignment further down the file
	// (this is more important for memory mapped IO than for stream-based IO).
	// The first 4 or 8 bytes should be as unique as possible, as some
	// utilities may only consider those. It's probably also a good idea to
	// avoid any common existing magics (e.g. starting the file with "\177ELF"
	// is less-than-optimal).
	// 
	// 
	//char kFileMagic[16] = "\0COMP3811mesh00";

	void fread_(void* aPtr, std::size_t, std::FILE*);

	struct FileDeleter
	{
		~FileDeleter();
		std::FILE* file;
	};
}


SimpleMeshData load_wavefront_obj(char const* aPath)
{
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
		throw Error("Unable to load OBJ file ?%s?: %s", aPath, result.error.code.message().c_str());
	// OBJ files can define faces that are not triangles. However, OpenGL will only render triangles (and lines
	// and points), so we must triangulate any faces that are not already triangles. Fortunately, rapidobj can do
	// this for us.
	rapidobj::Triangulate(result);
	// Convert the OBJ data into a SimpleMeshData structure. For now, we simply turn the object into a triangle14g
	// soup, ignoring the indexing information that the OBJ file contains.
	SimpleMeshData ret;
	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			// Add vertex normals
			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			// Extract texture coordinates if available
			if (!result.attributes.texcoords.empty()) 
			{             
				ret.texcoords.emplace_back(Vec2f
				{                 
					result.attributes.texcoords[idx.texcoord_index * 2 + 0],                 
					result.attributes.texcoords[idx.texcoord_index * 2 + 1]             
				});         
			}

			// Always triangles, so we can find the face index by dividing the vertex index by three
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];

			// Just replicate the material ambient color for each vertex...
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});

			Vec3f diffuseColor{
			   mat.diffuse[0],
			   mat.diffuse[1],
			   mat.diffuse[2]
			};

			// Use the light direction (0, 1, -1) and normalize it
			//Vec3f lightDirection{ 0.0f, 1.0f, -1.0f };
			//Vec3f finalAmbientColor, finalDiffuseColor;

			//calculateLighting(finalAmbientColor, finalDiffuseColor, lightDirection, ret.normals.back());

			// Just replicate the material ambient and diffuse colors for each vertex...
			//ret.colors.emplace_back(finalAmbientColor + finalDiffuseColor);

		}
	}

	return ret;
}


namespace
{
	void fread_(void* aPtr, std::size_t aBytes, std::FILE* aFile)
	{
		auto* ptr = static_cast<std::byte*>(aPtr);
		while (aBytes)
		{
			auto const ret = std::fread(ptr, 1, aBytes, aFile);
			if (0 == ret)
			{
				if (auto const err = std::ferror(aFile))
					throw Error("fread_(): error while reading %zu bytes : %d", aBytes, err);
				if (std::feof(aFile))
					throw Error("fread_(): unexpected EOF (%zu still to read)", aBytes);
			}

			aBytes -= ret;
			ptr += ret;
		}
	}

	FileDeleter::~FileDeleter()
	{
		if (file) std::fclose(file);
	};
}
