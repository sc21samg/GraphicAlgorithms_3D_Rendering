#include "simple_mesh.hpp"

SimpleMeshData concatenate( SimpleMeshData aM, SimpleMeshData const& aN )
{
	aM.positions.insert( aM.positions.end(), aN.positions.begin(), aN.positions.end() );
	aM.colors.insert( aM.colors.end(), aN.colors.begin(), aN.colors.end() );
	aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());
	aM.texcoords.insert(aM.texcoords.end(), aN.texcoords.begin(), aN.texcoords.end());
	return aM;
}


GLuint create_vao( SimpleMeshData const& aMeshData )
{
	//TODO: implement me
	 
	//================== vbo =====================	
	// Position vertex Buffer object  
	GLuint positionVBO = 0;			// initialise VBO container 
	glGenBuffers(1, &positionVBO);	// generate a VBO and store it in 'positionVBO'
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO); // bining with array buffer
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f),
		aMeshData.positions.data(), GL_STATIC_DRAW); //send the vertex position data to the GPU
	
	//Colour Vertex Buffer Object
	GLuint colorVBO = 0;
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f),
		aMeshData.colors.data(), GL_STATIC_DRAW);
	//Normal Vertex Buffer Object
	GLuint normalVBO = 0;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f),
		aMeshData.normals.data(), GL_STATIC_DRAW);

	// Texture Coordinates vertex buffer objecct 
	GLuint texCoordVBO = 0;
	glGenBuffers(1, &texCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f),
		aMeshData.texcoords.data(), GL_STATIC_DRAW);
	// ================== vao ====================
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// positionVBO binding 
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(
		0, // location = 0 in vertex shader 
		3, GL_FLOAT, GL_FALSE, // 2 -> how many attributes composed, GL_FlOAT -> attribute type, GL_FALSE -> not normalised 
		0,	//  indicates that there is no padding between inputs 
		0	// data starts at oset 0 in the VBO.
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glVertexAttribPointer(
		1,
		3, GL_FLOAT, GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(
		2,
		3, GL_FLOAT, GL_FALSE,
		0,
		0
	);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glVertexAttribPointer(
		3, // location = 1 in vertex shader 
		2, GL_FLOAT, GL_FALSE, // 2D, floats, not normalized to 0..1 (GL FALSE) 
		0, // see above
		nullptr //see above
	);
	glEnableVertexAttribArray(3);

	// Reset state 
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// clean up buffers 
	glDeleteBuffers(1, &positionVBO);
	glDeleteBuffers(1, &colorVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &texCoordVBO);

	return vao;
}

  