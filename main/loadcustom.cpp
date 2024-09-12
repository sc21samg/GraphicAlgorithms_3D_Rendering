#include "loadcustom.hpp"

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
	char kFileMagic[16] = "\0COMP3811mesh00";

	void fread_( void* aPtr, std::size_t, std::FILE* );

	struct FileDeleter
	{
		~FileDeleter();
		std::FILE* file;
	};
}

SimpleMeshData load_simple_binary_mesh( char const* aPath )
{
	if( std::FILE* fin = std::fopen( aPath, "rb" ) )
	{
		FileDeleter fd{ fin };

		// Verify the "file magic" at the start of the file. If the file magic
		// matches the expected value, this is likely a valid COMP3811 simple
		// mesh file.
		char magic[sizeof(kFileMagic)];
		fread_( magic, sizeof(kFileMagic), fin );

		if( 0 != std::memcmp( magic, kFileMagic, sizeof(kFileMagic) ) )
			throw Error( "'%s': not a COMP3811 mesh\n", aPath );

		// Read number of verts and indices (in that order)
		std::uint32_t meta[2];
		fread_( meta, sizeof(meta), fin );

		// Indices
		std::vector<std::uint32_t> indices( meta[1] );
		fread_( indices.data(), sizeof(std::uint32_t)*meta[1], fin );

		// Allocate space for the results
		// Note: that the SimpleMeshData is not indexed, so we have to unwrap
		// the mesh.
		SimpleMeshData ret;
		ret.positions.reserve( meta[1] );
		ret.colors.reserve( meta[1] );
		ret.normals.reserve( meta[1] );

		// Temporary buffer for reading
		std::vector<Vec3f> temp( meta[0] );

		// Read positions
		fread_( temp.data(), sizeof(Vec3f)*meta[0], fin );
		for( auto const idx : indices )
			ret.positions.emplace_back( temp[idx] );

		// Read colors
		fread_( temp.data(), sizeof(Vec3f)*meta[0], fin );
		for( auto const idx : indices )
			ret.colors.emplace_back( temp[idx] );

		// Read normals
		fread_( temp.data(), sizeof(Vec3f)*meta[0], fin );
		for( auto const idx : indices )
			ret.normals.emplace_back( temp[idx] );

		return ret;
	}
	
	throw Error( "load_simple_binary_mesh(): Unable to open '%s' for reading", aPath );
}


namespace
{
	void fread_( void* aPtr, std::size_t aBytes, std::FILE* aFile )
	{
		auto* ptr = static_cast<std::byte*>(aPtr);
		while( aBytes )
		{
			auto const ret = std::fread( ptr, 1, aBytes, aFile );
			if( 0 == ret )
			{
				if( auto const err = std::ferror( aFile ) )
					throw Error( "fread_(): error while reading %zu bytes : %d", aBytes, err );
				if( std::feof( aFile ) )
					throw Error( "fread_(): unexpected EOF (%zu still to read)", aBytes );
			}

			aBytes -= ret;
			ptr += ret;
		}
	}

	FileDeleter::~FileDeleter()
	{
		if( file ) std::fclose( file );
	};
}

