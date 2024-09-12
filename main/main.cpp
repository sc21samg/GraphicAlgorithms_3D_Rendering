#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#include "defaults.hpp"
#include "loadobj.hpp"
#include "texture.hpp"
#include "cone.hpp"
#include "cube.hpp"
#include "cylinder.hpp"
#include "loadcustom.hpp"
#include <chrono>
#include <vector>

// Query objects for GPU timing
GLuint queryStart, queryEnd;
std::vector<GLuint> sectionQueries; // Queries for individual sections


namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";

	//by sandra
	constexpr float kPi_ = 3.1415926f;

	constexpr float kMovementPerSecond_ = 5.f; // units per second
	constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

	const float kPi = 3.14159265359f;
	const float curveDuration = 5.0f; // Duration of curve phase in seconds
	const float maxAscentHeight = 10.0f; // Maximum height before curving
	const float maxCurveAngle = kPi / 2; // Maximum angle for curve (90 degrees)

	struct State_
	{
		ShaderProgram* prog;

		struct CamCtrl_
		{
			bool cameraActive;
			bool actionZoomIn, actionZoomOut, actionSpeedUp, actionSlowDown, actionLeft, actionRight, actionLookUp, actionLookDown;

			float phi, theta;
			float radius;

			float lastX, lastY;

			Vec3f position;

		} camControl;

		bool isAnimating = false;
		Vec3f vehiclePosition = { 0.0f, -0.5f, 0.0f };
		Vec3f vehicleDirection = { 0.0f, 0.0f, 1.0f };
		float vehicleSpeed = 0.0f;
		float vehicleAngle = 0.0f;
		float curveRadius = 5.0f;
		float liftOffHeight = 5.0f;
		float liftOffSpeed = 0.1f;
		bool isLiftingOff = true;
		bool isCurving = false;
		double curveStartTime = 0.0;
		bool isHorizontalFlight = false;
		float horizontalFlightSpeed = 0.1f;
	};
	
	void glfw_callback_error_( int, char const* );

	void glfw_callback_key_( GLFWwindow*, int, int, int, int );
	void glfw_callback_motion_(GLFWwindow*, double, double);

	///

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};

	void InitializeQueryObjects() {
    glGenQueries(1, &queryStart);
    glGenQueries(1, &queryEnd);

    // Assuming there are 3 sections to measure
    sectionQueries.resize(6); // Start and end for each section
    glGenQueries(6, sectionQueries.data());
}

}

int main() try
{
	glQueryCounter(queryStart, GL_TIMESTAMP);
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	// TODO: Additional event handling setup
	State_ state{};

	glfwSetWindowUserPointer(window, &state);


	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);
	////

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n ", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

InitializeQueryObjects();

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_CULL_FACE);
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();
	

	// For Section 1.2 
glQueryCounter(sectionQueries[0], GL_TIMESTAMP);

	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	OGL_CHECKPOINT_ALWAYS();

	// Load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
		});

	state.prog = &prog;
	state.camControl.radius = 10.f;

	// Animation state
	auto last = Clock::now();

	float angle = 0.f;
	///////////////new/////////////

	// Create vertex buffers and VAO
	//TODO: create VBOs and VAO


	/////////////////////VAO/////////////////////////

	std::vector<Vec3f> positions;  // Your vertex positions
	std::vector<Vec3f> color;      // Your vertex colors

	SimpleMeshData aMeshData;
	aMeshData.positions = positions;
	aMeshData.colors = color;

	// Assuming you have a function create_vao that takes a SimpleMeshData and sets up a VAO
	GLuint vao = create_vao(aMeshData);

glQueryCounter(sectionQueries[1], GL_TIMESTAMP);

	// Load the parlahti object
	auto parlahti = load_wavefront_obj("assets/parlahti.obj");
	auto parlahtiVao = create_vao(parlahti);
	std::size_t parlahtiVertexCount = parlahti.positions.size();
	// Load texture
	auto mapTexture = load_texture_2d("assets/L4343A-4k.jpeg");
	//setting texture for launch pad
	auto whiteTexture = load_texture_2d("assets/white.png");

// Extract the texture coordinates from the loaded data
// std::vector<Vec2f> textureCoords = parlahti.v2fTexCoord;

// Start GPU time query for Section 1.4
glQueryCounter(sectionQueries[2], GL_TIMESTAMP);

	// task 1.4
// Load the landingpad object
	auto landingPad = load_wavefront_obj("assets/landingpad.obj");
	auto landingPadVao = create_vao(landingPad);
	std::size_t landingPadVertexCount = landingPad.positions.size();

	// Example values for landing pad instances
	float x1 = 0.0f, y1 = -0.90f, z1 = 0.0f, angle1 = 0.0f;
	float x2 = 1.0f, y2 = -0.90f, z2 = -25.0f, angle2 = 45.0f;  // Adjust as needed

	// Instance data for the first landing pad
	Mat44f instanceTransform1 = make_translation({ x1, y1, z1 }) * make_rotation_y(angle1);

	// Instance data for the second landing pad
	Mat44f instanceTransform2 = make_translation({ x2, y2, z2 }) * make_rotation_y(angle2);

// End GPU time query for Section 1.4
glQueryCounter(sectionQueries[3], GL_TIMESTAMP);


// Start GPU time query for Section 1.5
glQueryCounter(sectionQueries[4], GL_TIMESTAMP);

//// Generate different shapes for testing
	 auto testCylinder = make_cylinder(true, 128, { 0.4f, 0.4f, 0.4f }, 
	 	make_rotation_z(3.141592f / 2.f) 
	 	* make_scaling(8.f, 2.f, 2.f) 
	 );
	 GLuint vaoCylinder = create_vao(testCylinder);
	 std::size_t vertexCountCylinder = testCylinder.positions.size();

	 // cone on top of spaceship
	 auto topConeT = make_cone(true, 128, { 20.0f, 20.0f, 20.0f },
		 make_rotation_z(-3.141592f / 2.f)
		 * make_scaling(1.f, 0.5f, 0.5f)
		 * make_translation({ 0.f, 4.f, 0.f })
	 );
	 GLuint vaoCone = create_vao(topConeT);
	 std::size_t vertexCountCone = topConeT.positions.size();

	 //cube on the right
	 auto cubeT = make_cube(true, 20, { 1.0f, 1.0f, 0.0f },
		 make_rotation_z(-3.141592f / 2.f)
		 * make_scaling(0.5f, 0.6f, 0.6f)
		 * make_translation({ 0.f, 4.9f, 0.f })
	 );
	 GLuint vaoCube = create_vao(cubeT);
	 std::size_t vertexCountCube = cubeT.positions.size();
//// Generate different shapes for testing

	 // general observations for possitioning the spaceship
	//  * make_scaling(a.f, b.f, c.f)
	//  a.f vertical size of the object(how long it is)
	//	b.f horizontal size of the object( how wide it is), to the left
	//	c.f horizontal size of the object(how wide it is), to the right

	//  *make_translation({ a.f, b.f, c.f })
	//	  a.f moves the object up and down on the axis coord
	//	  b.f moves the objectd right and left on the axis coord
	//	  c.f moves the object forward and back on the axis coord

	// Task 1.5 
	// by sandra
	// main cylinder of the spaceship
	//a mock cube that sets the complete body of the spaceship on top of the platform
	auto cube0 = make_cube(true, 20, { 0.4f, 0.4f, 0.4f },
		make_rotation_z(-3.141592f / 2.f)
		* make_scaling(0.0f, 0.0f, 0.0f)
		* make_translation({ x1, y1, z1 })//platform coords
	);


	auto mainCylinder = make_cylinder(true, 128, { 3.4f, 0.4f, 0.4f },
		make_rotation_z(3.141592f / 2.f)
		* make_scaling(3.f, 0.5f, 0.5f)
		* make_translation({ 0.1f })//lifting the main body a bit
	);

	// Adjustments
	// Increase the X scale factor to make the cylinder longer
	//float scaleX = 1.0f * 3.5f; // Adjust this multiplier as needed for length
	//cylinder on the left
	auto smallCylinder1 = make_cylinder(true, 128, { 0.0f, 0.6f, 0.7f },
		make_rotation_z(3.141592f / 2.f)
		* make_scaling(0.9f, 0.3f, 0.4f) // Updated scaling for length
		* make_translation({ 1.3f, 2.5f, 0.f })
	);
	//cylinder on the right
	auto smallCylinder2 = make_cylinder(true, 128, { 0.0f, 0.6f, 0.7f },
		make_rotation_z(3.141592f / 2.f)
		* make_scaling(0.9f, 0.3f, 0.4f) // Updated scaling for length
		* make_translation({ 1.3f, -2.5f, 0.0f })
	);

	// cone on top of spaceship
	auto topCone = make_cone(true, 128, { 1.0f, 1.0f, 0.0f },
		make_rotation_z(-3.141592f / 2.f)
		* make_scaling(1.f, 0.5f, 0.5f)
		* make_translation({ -4.3f, 0.f, 0.f })
	);

	//putting together the small cylinders on the side of the main body of the spaceship
	auto smallCylinders = concatenate(std::move(smallCylinder1), smallCylinder2);
	auto mainShip = concatenate(std::move(mainCylinder), smallCylinders);

	//set the possition on top of the platform
	auto setPos = concatenate(std::move(mainShip), cube0);
	//merge the rest of the ship with the mock cube
	auto shipBody = concatenate(std::move(setPos), topCone);

	//cube in the middle
	auto cube1 = make_cube(true, 2, { 0.4f, 0.4f, 0.4f },
		make_rotation_z(-3.141592f / 2.f)
		* make_scaling(0.75f, 0.5f, 0.5f)
		* make_translation({ 0.1f, 0.f, 0.f })
	);
	//cube on the right
	auto cube2 = make_cube(true, 2, { 0.4f, 0.4f, 0.4f },
		make_rotation_z(-3.141592f / 2.f)
		* make_scaling(0.5f, 0.6f, 0.6f)
		* make_translation({ -0.6f, 0.9f, 0.f })
	);

	//merging 2 of the cubes
	auto cubesA = concatenate(std::move(cube1), cube2);

	//cube on the left
	auto cube3 = make_cube(true, 2, { 0.4f, 0.4f, 0.4f },
		make_rotation_z(-3.141592f / 2.f)
		* make_scaling(0.5f, 0.6f, 0.6f)
		* make_translation({ -0.6f, -0.9f, 0.f })
	);
	//merging all 3 cubes
	auto cubes = concatenate(std::move(cubesA), cube3);
	//GLuint vaoCube = create_vao(cubes);
	//std::size_t  vertexCountCube = cubes.positions.size();

	auto completeShip = concatenate(std::move(shipBody), cubes);

	//spaceship without cube base is done
	GLuint vaoShip = create_vao(completeShip);
	std::size_t vertexCountShipBody = completeShip.positions.size();

// End GPU time query for Section 1.5
glQueryCounter(sectionQueries[5], GL_TIMESTAMP);

	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		auto cpuStartTime = std::chrono::high_resolution_clock::now();

		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;

		//by sandra
	// Adjust speed based on actionSpeedUp and actionSlowDown
	//for the shift and ctrl keys
		float speedMultiplier = 1.0f;

		if (state.camControl.actionSpeedUp)
			speedMultiplier = 2.0f; // Adjust the speed factor as needed for faster movement

		if (state.camControl.actionSlowDown)
			speedMultiplier = 0.5f; // Adjust the speed factor as needed for slower movement

		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		//by sandra
		// Update camera state
		//with the shift and ctrl functionality
		// Compute forward direction based on camera's orientation
// Compute forward direction based on camera's orientation
		Vec3f forward;
		forward.x = sin(state.camControl.phi) * cos(state.camControl.theta);
		forward.y = sin(state.camControl.theta);
		forward.z = cos(state.camControl.phi) * cos(state.camControl.theta);

		// Normalize the forward vector
		float length = sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
		forward.x /= length;
		forward.y /= length;
		forward.z /= length;

		// Update camera state
		if (state.camControl.actionZoomIn) {
			state.camControl.radius -= kMovementPerSecond_ * speedMultiplier * dt;
		}
		else if (state.camControl.actionZoomOut) {
			state.camControl.radius += kMovementPerSecond_ * speedMultiplier * dt;
		}

		// Update camera position based on radius and direction
		state.camControl.position = -state.camControl.radius * forward;

		//so it doesn't go outside the map
		float maxRadius = 4242.6f; // Maximum radius, the calculated boundary of your map
		
		// Ensure the camera radius is not less than the minimum and not more than the maximum
		if (state.camControl.radius > maxRadius)
			state.camControl.radius = maxRadius;

		// Update lateral movement
		//its calling the phi functionality, so that it mimics the same movement, rotation
		//of the mouse controler
		if (state.camControl.actionLeft)
			state.camControl.theta -= kMovementPerSecond_ * speedMultiplier * dt;
		else if (state.camControl.actionRight)
			state.camControl.theta += kMovementPerSecond_ * speedMultiplier * dt;


		// Update vertical movement
		if (state.camControl.actionLookUp)
			state.camControl.theta += kMovementPerSecond_ * speedMultiplier * dt;
		else if (state.camControl.actionLookDown)
			state.camControl.theta -= kMovementPerSecond_ * speedMultiplier * dt;


		// Update: compute matrices
		//TODO: define and compute projCameraWorld matrix
		// step1) model to world
		//Mat44f model2world = kIdentity44f;
		// Task for arc ball 
		Mat44f T = make_translation(-state.camControl.position); // Translate to camera position
		Mat44f Rx = make_rotation_x(state.camControl.theta);     // Rotate around X-axis
		Mat44f Ry = make_rotation_y(state.camControl.phi);       // Rotate around Y-axis

		// Combine all together -> new world2camera
		Mat44f world2camera = T * Ry * Rx;

		// define projection, 
		Mat44f projection = make_perspective_projection(
			60.f * 3.1415926f / 180.f, // Yes, a proper pie would be useful. ( C++20: mathematical constants) 
			fbwidth / float(fbheight),
			0.1f, 100.0f
		);
		// concatenate the defined matrices  
		Mat44f projCameraWorld = projection * world2camera;
		// compute normal matrix from the model-to-world transform that we have defined previously
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(Ry)));

		// Draw scene
		OGL_CHECKPOINT_DEBUG();
		// Rendering in wireframe mode (as just a set of lines)
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//TODO: draw frame
		// make framebuffer empty  
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// step 1) select a program 
		glUseProgram(prog.programId());

		// Set uniform values for lighting
		Vec3f lightDir = normalize(Vec3f{ -1.f, 1.f, 0.5f });
		glUniform3fv(2, 1, &lightDir.x);  // Set uniform for light direction

		glUniform3f(3, 0.9f, 0.9f, 0.9f);  // Set uniform for diffuse illumination
		glUniform3f(4, 0.05f, 0.05f, 0.05f); // Set uniform for scene ambient illumination

		
	
		// step 2) set uniform values 
		glUniformMatrix3fv(
			1, // make sure this matches the location = N in the vertex shader!
			1, GL_TRUE, normalMatrix.v
		);
		glUniformMatrix4fv(
			0 /*location in shaders*/,
			1, GL_TRUE, projCameraWorld.v

		);

		// step 3) set input data 
		glBindVertexArray(parlahtiVao);	// source input as defined in our VAO 
		//Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mapTexture);
glUniform1i(glGetUniformLocation(prog.programId(), "uTexture"), 0); // Set the sampler uniform to the texture unit index

		// step 4) issue Drawing commands 
		glDrawArrays(GL_TRIANGLES, 0, parlahtiVertexCount);

		// task 1.4
		// Draw the first instance of the landingpad
		glUniformMatrix4fv(
			0, // location in shaders
			1, GL_TRUE, (projCameraWorld * instanceTransform1).v
		);
		glBindVertexArray(landingPadVao);
		//Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		glDrawArrays(GL_TRIANGLES, 0, landingPadVertexCount);

		// Draw the second instance of the landing pad
		glUniformMatrix4fv(
			0, // location in shaders
			1, GL_TRUE, (projCameraWorld * instanceTransform2).v
		);
		glBindVertexArray(landingPadVao);
		//Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		glDrawArrays(GL_TRIANGLES, 0, landingPadVertexCount);

		const float initialSpeed = 0.01f;       // Initial speed of the spaceship when horizontal
		const float accelerationRate = 0.001f; // Rate of acceleration

		double currentTime = glfwGetTime();
		if (state.isAnimating) {
			if (state.isLiftingOff) {
				// Continuous acceleration
				state.vehicleSpeed += accelerationRate;

				// Vertical ascent
				state.vehiclePosition.y += state.vehicleSpeed;
				if (state.vehiclePosition.y >= state.liftOffHeight) {
					state.isLiftingOff = false;
					state.isCurving = true;
					state.curveStartTime = currentTime;
					state.vehicleAngle = 0.0f;
				}
			}
			else if (state.isCurving) {
				float elapsedTime = currentTime - state.curveStartTime;
				if (elapsedTime < curveDuration) {
					state.vehicleAngle = maxCurveAngle * (elapsedTime / curveDuration);
					// Update position for curving
					state.vehiclePosition.y = state.liftOffHeight + state.curveRadius * (1 - cos(state.vehicleAngle));
					state.vehiclePosition.x = state.curveRadius * sin(state.vehicleAngle);
					// Update direction
					state.vehicleDirection.x = sin(state.vehicleAngle);
					state.vehicleDirection.y = cos(state.vehicleAngle);
				}
				else {
					state.isCurving = false;
					state.isHorizontalFlight = true;
					state.vehicleDirection = { 1.0f, 0.0f, 0.0f }; // Fully horizontal
				}
			}
			else if (state.isHorizontalFlight) {
				// Continuous acceleration
				state.vehicleSpeed += accelerationRate;

				// Move forward at the current speed
				state.vehiclePosition.x += state.vehicleSpeed;
			}
		}

		Mat44f vehicleTransform = make_translation(state.vehiclePosition);

		if (state.isCurving) {
			// Calculate the tilt angle for the curving phase
			float tiltAngle = state.vehicleAngle;

			// Apply a rotation around the Z-axis (or change to another axis if needed)
			vehicleTransform = vehicleTransform * make_rotation_z(-tiltAngle); // Note the negative sign for reverse direction
		}
		else if (state.isHorizontalFlight) {
			// Fully horizontal, rotate -90 degrees around the Z-axis
			vehicleTransform = vehicleTransform * make_rotation_z(-kPi / 2);
		}
		if (!state.isLiftingOff) {
			float rotationAngle = atan2(state.vehicleDirection.z, state.vehicleDirection.x);
			vehicleTransform = vehicleTransform * make_rotation_y(rotationAngle);
		}

		glUniformMatrix4fv(
			0, // location in shaders
			1, GL_TRUE, (projCameraWorld * vehicleTransform).v
		);

		//// Generate different shapes for testing
		//glBindVertexArray(vaoCube);	// source input as defined in our VAO 
		//glDrawArrays(GL_TRIANGLES, 0, vertexCountCube);
		//OGL_CHECKPOINT_DEBUG();
//		glBindVertexArray(vaoCone);	// source input as defined in our VAO 
//		glDrawArrays(GL_TRIANGLES, 0, vertexCountCone);
//		OGL_CHECKPOINT_DEBUG();
///		glBindVertexArray(vaoCylinder);	// source input as defined in our VAO 
//		glDrawArrays(GL_TRIANGLES, 0, vertexCountCylinder);
//		OGL_CHECKPOINT_DEBUG();
		//// Generate different shapes for testing


		// Task 1.5 display the body of the ship
		glBindVertexArray(vaoShip);	// source input as defined in our VAO 
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountShipBody);


		///task 1.6
		// Ensure these variables are declared and properly assigned
		float completeShipX = 1.0f;
		float completeShipY = -0.5f;
		float completeShipZ = -3.0f;

		Mat44f completeShipTransform = make_translation({ completeShipX, completeShipY, completeShipZ });
		// I made this to set the spaceship's position. But i dont know why, nothing changed :(

		Vec3f localLightPositions[3] = {
		Vec3f{4.0f, 18.0f, 0.0f}, // Position relative to the completeShip
		Vec3f{0.0f, 22.0f, 0.0f},
		Vec3f{0.0f, 1.0f, 4.0f}
		};
		// I think when i change Vec3f y-axis, the lights' position is changed.

		Vec3f pointLightPositions[3];
		for (int i = 0; i < 3; ++i) {
			// Transform each local position to world space based on the spaceship's transformation
			Vec4f transformedPosition = completeShipTransform * Vec4f{ localLightPositions[i].x, localLightPositions[i].y, localLightPositions[i].z, 1.0f };
			pointLightPositions[i] = Vec3f{ transformedPosition.x, transformedPosition.y, transformedPosition.z };
		}

		Vec3f pointLightColors[3];
		pointLightColors[0].x = 1.0f; pointLightColors[0].y = 0.0f; pointLightColors[0].z = 0.0f; // Red
		pointLightColors[1].x = 0.0f; pointLightColors[1].y = 0.0f; pointLightColors[1].z = 1.0f; // Blue
		pointLightColors[2].x = 0.0f; pointLightColors[2].y = 1.0f; pointLightColors[2].z = 0.0f; // Green  // This is for checking the light colour. 
		// After fixing the lights, we can change these to other colours.
		// (Just in case if you cannot see the exact colours/ positions) When you run, the lights are shown the order blue-green-no colour-red (blue is the widest, then, green, red).

		// Set the uniforms for the point lights
		glUniform3fv(glGetUniformLocation(prog.programId(), "pointLightPositions"), 3, &pointLightPositions[0].x);
		glUniform3fv(glGetUniformLocation(prog.programId(), "pointLightColors"), 3, &pointLightColors[0].x);

		OGL_CHECKPOINT_DEBUG();

glQueryCounter(queryEnd, GL_TIMESTAMP);

auto cpuEndTime = std::chrono::high_resolution_clock::now();
auto cpuElapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(cpuEndTime - cpuStartTime).count();

		// Display results
		glfwSwapBuffers(window);
	}

	// Cleanup.
	//TODO: additional cleanup
	state.prog = nullptr;

	GLint available = 0;
while (!available) {
    glGetQueryObjectiv(queryEnd, GL_QUERY_RESULT_AVAILABLE, &available);
}
GLuint64 startTime, endTime;
glGetQueryObjectui64v(queryStart, GL_QUERY_RESULT, &startTime);
glGetQueryObjectui64v(queryEnd, GL_QUERY_RESULT, &endTime);

// Calculate the elapsed time in nanoseconds
GLuint64 elapsedTime = endTime - startTime;

	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}

namespace
{
	void glfw_callback_error_(int aErrNum, char const* aErrDesc)
	{
		std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
	}

	void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;

				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			// Camera controls if camera is active
			if (state->camControl.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionZoomIn = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionZoomIn = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionZoomOut = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionZoomOut = false;
				}
				//by sandra
				// shift to speed up, and ctrl to slow down
				else if (GLFW_KEY_LEFT_SHIFT == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionSpeedUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionSpeedUp = false;
				}
				else if (GLFW_KEY_LEFT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionSlowDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionSlowDown = false;
				}
				//left movement for A key pressed & 
				//right movement for D key pressed
				else if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionLeft = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionLeft = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionRight = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionRight = false;
				}
				//look upwards movement for E key pressed & 
				//look downwards movement for Q key pressed
				else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionLookUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionLookUp = false;
				}
				else if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionLookDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionLookDown = false;
				}
				// Handling key press events
				if (GLFW_KEY_F == aKey && GLFW_PRESS == aAction)
				{
					state->isAnimating = true;
					state->vehicleSpeed = 0.0f; // Reset speed for horizontal flight
					state->isLiftingOff = true; // Begin with the lift-off phase
					state->vehiclePosition = { 0.0f, 0.0f, 0.0f }; // Reset position
					state->vehicleDirection = { 0.0f, 0.0f, 1.0f }; // Reset direction
					state->vehicleAngle = 0.0f; // Reset angle
					state->isCurving = false; // Reset curving flag
					state->isHorizontalFlight = false; // Reset horizontal flight flag
				}

				if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
				{
					state->isAnimating = false;
					state->vehiclePosition = { 0.0f, -0.5f, 0.0f }; // Reset position
					state->vehicleDirection = { 0.0f, 0.0f, 1.0f }; // Reset direction
					state->vehicleSpeed = 0.0f; // Reset speed
					state->vehicleAngle = 0.0f; // Reset angle
					state->isLiftingOff = false; // Reset lift-off flag
					state->isCurving = false; // Reset curving flag
					state->isHorizontalFlight = false; // Reset horizontal flight flag
				}

			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = float(aX - state->camControl.lastX);
				auto const dy = float(aY - state->camControl.lastY);

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}

			state->camControl.lastX = float(aX);
			state->camControl.lastY = float(aY);
		}
	}

}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}

