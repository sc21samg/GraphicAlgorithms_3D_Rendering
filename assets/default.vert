
#version 430

// The positions
layout(location = 0) in vec3 iPosition;
// The colors
layout(location = 1) in vec3 iColor;
// The normals 
layout( location = 2 ) in vec3 iNormal;

layout(location = 0) uniform mat4 uProjCameraWorld ;
layout( location = 1 ) uniform mat3 uNormalMatrix;
layout(location = 3) in vec2 iTexCoord; // Add texture coordinates


out vec3 v2fPosition;
out vec3 v2fColor;
out vec3 v2fNormal;
out vec2 v2fTexCoord; // Pass texture coordinates to fragment shader

// task 1.6
out vec3 v2fWorldPosition;


void main()
{	
    v2fColor = iColor; 
    gl_Position = uProjCameraWorld * vec4(iPosition, 1.0);
    v2fNormal = normalize(uNormalMatrix * iNormal);
    v2fTexCoord = iTexCoord; // Pass texture coordinates to fragment shader
    // task 1.6
    // Transform vertex position to world space and store in v2fWorldPosition
    v2fWorldPosition = (uProjCameraWorld * vec4(iPosition, 1.0)).xyz;
}