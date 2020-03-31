//Kevin Lounsbury 100654226
//Dimitrios Stefanakos 100621470

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> 

#include<sstream>
#include <string>

///// Networking //////
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")
///////////////////////

GLFWwindow* window;

unsigned char* image;
int width, height;
int clientID;

void loadImage() {
	int channels;
	stbi_set_flip_vertically_on_load(true);
	image = stbi_load("box.jpg",
		&width,
		&height,
		&channels,
		0);
	if (image) {
		std::cout << "Image LOADED" << width << " " << height << std::endl;
	}
	else {
		std::cout << "Failed to load image!" << std::endl;
	}

}

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "Window", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return false;
	}
}


GLuint shader_program;

bool loadShaders() {
	// Read Shaders from file
	std::string vert_shader_str;
	std::ifstream vs_stream("vertex_shader.glsl", std::ios::in);
	if (vs_stream.is_open()) {
		std::string Line = "";
		while (getline(vs_stream, Line))
			vert_shader_str += "\n" + Line;
		vs_stream.close();
	}
	else {
		printf("Could not open vertex shader!!\n");
		return false;
	}
	const char* vs_str = vert_shader_str.c_str();

	std::string frag_shader_str;
	std::ifstream fs_stream("frag_shader.glsl", std::ios::in);
	if (fs_stream.is_open()) {
		std::string Line = "";
		while (getline(fs_stream, Line))
			frag_shader_str += "\n" + Line;
		fs_stream.close();
	}
	else {
		printf("Could not open fragment shader!!\n");
		return false;
	}
	const char* fs_str = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_str, NULL);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	return true;
}

//INPUT handling
float tx = 0.0f;
float ty = 0.0f;
float updadteInterval = 0.100f;
GLuint filter_mode = GL_LINEAR;


class Ball
{
public:
	
	float x = 0.f;
	float y = 0.f;

	glm::mat4 Model = glm::mat4(1.0f);

	glm::vec2 force = glm::vec2(0.0f, 0.0f);
	glm::vec2 acceleration = glm::vec2(0.0f, 0.0f);
	glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	float mass = 1;

	void col()
	{
		if (position.x > 2.5 && (position.y > 1.0f || position.y < -1.0))
		{
			velocity.x = -abs(velocity.x);
			
		}
		else if (position.x > 2.5)
		{
			velocity = glm::vec2(0.0f, 0.0f);
			acceleration = glm::vec2(0.0f, 0.0f);
			force = glm::vec2(0.0f, 0.0f);
			position = glm::vec2(-0.7f, 0.0f);
		}
		if (position.x < -2.5 && (position.y > 1.0f || position.y < -1.0))
		{
			velocity.x = abs(velocity.x);
			
		}
		else if (position.x < -2.5)
		{
			velocity = glm::vec2(0.0f, 0.0f);
			acceleration = glm::vec2(0.0f, 0.0f);
			force = glm::vec2(0.0f, 0.0f);

			position = glm::vec2(0.7f, 0.0f);
		}
	}

	void update(float dt)
	{

		acceleration = force / mass;
		velocity = velocity + acceleration * dt;
		if (glm::length(velocity) > 0)
			velocity = glm::normalize(velocity) * 1.5f;
		position = position + velocity * dt + 0.5f * acceleration * dt * dt;

		x = position.x;
		y = position.y;

		force = glm::vec2(0.0f, 0.0f);
	}

private:

};

void DeadRec(float& x, float& y, float &prevx, float &prevy, float dt)
{
	if (x != prevx || y != prevy)
	{
		if (true) {
			glm::vec2 newPos = glm::vec2(x, y) - glm::vec2(prevx, prevy);
			newPos /= updadteInterval;

			glm::vec2 newPosition = glm::vec2(x, y) + newPos * dt /** 20.f*/;

			prevx = x;
			prevy = y;

			x = newPosition.x;
			y = newPosition.y;
			
		}
	}
}




float UPDATE_INTERVAL = 0.100;
void keyboard() {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		ty += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		ty -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && clientID == 1 && tx < 0) {
		tx += 0.01;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && clientID == 2)
	{
		tx += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && clientID == 2 && tx > 0) {
		tx -= 0.01;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && clientID == 1) {
		tx -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		UPDATE_INTERVAL += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && UPDATE_INTERVAL > 0.05) {
		UPDATE_INTERVAL -= 0.01;
	}
	

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (filter_mode == GL_LINEAR) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			filter_mode = GL_NEAREST;
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			filter_mode = GL_LINEAR;
		}
	}



}


// Networking
SOCKET client_socket;
struct addrinfo* ptr = NULL;
#define SERVER "127.0.0.1"
#define PORT "8888"
#define BUFLEN 512
//#define UPDATE_INTERVAL 0.030 //seconds

bool initNetwork(std::string ip) {
	//Initialize winsock
	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (error != 0) {
		printf("Failed to initialize %d\n", error);
		return 0;
	}

	//Create a client socket

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if (getaddrinfo(ip.c_str(), PORT, &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	
	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	/// Change to non-blocking mode
	u_long mode = 1;// 0 for blocking mode
	ioctlsocket(client_socket, FIONBIO, &mode);

	if (client_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	return 1;
}


int main() {

	
	std::string IP;
	std::cout << "Enter the client ID(1 or 2): ";
	std::cin >> clientID;
	std::cout << "Enter the IP Adress: ";
	std::cin >> IP;
	Ball ball;

	float score1tx = 0.f;
	float score1ty = 0.f;
	float score2tx = 0.f;
	float score2ty = 0.f;
	float score3tx = 0.f;
	float score3ty = 0.f;
	float score4tx = 0.f;
	float score4ty = 0.f;
	float score5tx = 0.f;
	float score5ty = 0.f;
	float score6tx = 0.f;
	float score6ty = 0.f;


	//Initialize GLFW
	if (!initGLFW())
		return 1;



	//Initialize GLAD
	if (!initGLAD())
		return 1;

	//Initialize Network
	if (!initNetwork(IP))
		return 1;

	// Cube data
	static const GLfloat points[] = {//front face, 2 triangles
		-0.5f, -0.5f, 0.5f,//0  front face
		0.5f, -0.5f, 0.5f, //3
		-0.5f, 0.5f, 0.5f, //1
		0.5f, -0.5f, 0.5f, //3
		0.5f, 0.5f, 0.5f, //2
		-0.5f, 0.5f, 0.5f, //1
		0.5f, -0.5f, 0.5f, //3 Right face
		0.5f, -0.5f, -0.5f, //7
		0.5f, 0.5f, 0.5f, //2
		0.5f, -0.5f, -0.5f, //7
		0.5f, 0.5f, -0.5f, //6
		0.5f, 0.5f, 0.5f,  //2
		-0.5f, -0.5f, -0.5f, //4 Left face
		-0.5f, -0.5f, 0.5f, //0
		-0.5f, 0.5f, -0.5f, //5
		-0.5f, -0.5f, 0.5f, //0
		-0.5f, 0.5f, 0.5f,  //1
		-0.5f, 0.5f, -0.5f, //5
		-0.5f, 0.5f, 0.5f,  //1 Top face
		0.5f, 0.5f, 0.5f,  //2
		-0.5f, 0.5f, -0.5f,//5
		0.5f, 0.5f, 0.5f,   //2
		0.5f, 0.5f, -0.5f, //6
		-0.5f, 0.5f, -0.5f, //5
		-0.5f, -0.5f, -0.5f, //4 Bottom face
		0.5f, -0.5f, -0.5f, //7
		-0.5f, -0.5f, 0.5f, //0
		0.5f, -0.5f, -0.5f, //7
		0.5f, -0.5f, 0.5f, //3
		-0.5f, -0.5f, 0.5f, //0
		-0.5f, 0.5f, -0.5f, //5 Back face
		0.5f, 0.5f, -0.5f, //6
		-0.5f, -0.5f, -0.5f, //4
		0.5f, 0.5f, -0.5f, //6
		0.5f, -0.5f, -0.5f, //7
		-0.5f, -0.5f, -0.5f, //4
	};
	
	// Color data
	static const GLfloat colors[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};
	
	///////// TEXTURES ///////
	static const GLfloat uv[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};


	//VBO
	GLuint pos_vbo = 0;
	glGenBuffers(1, &pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint color_vbo = 1;
	glGenBuffers(1, &color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);


	
	// VAO
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLuint uv_vbo = 2;
	glGenBuffers(1, &uv_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	loadImage();
	
	GLuint textureHandle;
	
	glGenTextures(1, &textureHandle);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Release the space used for your image once you're done
	stbi_image_free(image);

	// Load your shaders
	if (!loadShaders())
		return 1;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 Projection = 
		glm::perspective(glm::radians(45.0f), 
		(float)width / (float)height, 0.0001f, 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 3), // Camera is at (0,0,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 otherModel = glm::mat4(1.0f);
	//glm::mat4 ballModel = glm::mat4(1.0f);
	ball.Model = glm::mat4(1.0f);

	glm::mat4 Model1 = glm::mat4(1.0f);
	glm::mat4 Model2 = glm::mat4(1.0f);
	glm::mat4 Model3 = glm::mat4(1.0f);
	glm::mat4 Model4 = glm::mat4(1.0f);
	glm::mat4 Model5 = glm::mat4(1.0f);
	glm::mat4 Model6 = glm::mat4(1.0f);
	// create individual matrices glm::mat4 T R and S, then multiply them
	if (clientID == 1)
	{
		Model = glm::translate(Model, glm::vec3(-1.5f, 0.0f, 0.0f));
		tx = -1.5f;
	}
	else
	{
		Model = glm::translate(Model, glm::vec3(1.5f, 0.0f, 0.0f));
		tx = 1.5f;
	}
	
	otherModel = glm::translate(otherModel, glm::vec3(0.0f, 0.0f, 0.0f));
	ball.Model = glm::translate(ball.Model, glm::vec3(0.0f, 0.0f, 0.0f));


	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Get a handle for our "MVP" uniform
	// Only during initialisation
	GLuint MatrixID = 
		glGetUniformLocation(shader_program, "MVP");


	// Face culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);


	/////// TEXTURE
	glUniform1i(glGetUniformLocation(shader_program, "myTextureSampler"), 0);

	
	// Timer variables for sending network updates
	float time = 0.0;
	float previous = glfwGetTime();
	float otherx = 0;
	float otherprevx = 0;
	float othery = 0;
	float otherprevy = 0;
	float ballx = 0;
	float ballprevx = 0;
	float bally = 0;
	float ballprevy = 0;
	
	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		/////// Timer for network updates
		float now = glfwGetTime();
		float delta = now - previous;
		previous = now;

		// When timer goes off, send an update
		time -= delta;
		if (time <= 0.f)
		{
			// Code to send position updates go HERE...
			char message[BUFLEN];

			std::string msg;
			msg = std::to_string(clientID);
			msg += "\n";
			msg += std::to_string(tx);
			msg += "\n";
			msg += std::to_string(ty);
			msg += "\n";
			msg += std::to_string(UPDATE_INTERVAL);
			strcpy(message, (char*)msg.c_str());

			if (sendto(client_socket, message, BUFLEN, 0, 
				ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR) 
			{
				std::cout << "Cant sendto()...\n";
			}
			std::cout << "Sent: " << message << std::endl;
			memset(message, '\0', BUFLEN);

			time = UPDATE_INTERVAL; // reset the timer
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		Model = glm::mat4(1.0f);
		otherModel = glm::mat4(1.0f);
		ball.Model = glm::mat4(1.0f);
		Model1 = glm::mat4(1.0f);
		Model2 = glm::mat4(1.0f);
		Model3 = glm::mat4(1.0f);
		Model4 = glm::mat4(1.0f);
		Model5 = glm::mat4(1.0f);
		Model6 = glm::mat4(1.0f);

		keyboard();

		

		struct sockaddr_in fromAddr;
		int fromlen;
		fromlen = sizeof(fromAddr);

		char buffer[BUFLEN];
		memset(buffer, 0, BUFLEN);
		if (recvfrom(client_socket, buffer, BUFLEN, 0,
			(struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR)
		{
			//std::cout << "Cant sendto()...\n";
		}
		std::cout << "Recvfrom: " << buffer << std::endl;
		std::string sbuffer = (std::string)buffer;

		std::istringstream iss(sbuffer);
		if (sbuffer != "")
		{
			std::string temp;
			std::getline(iss, temp);
			otherx = std::stof(temp);
			std::getline(iss, temp);
			othery = std::stof(temp);
			std::getline(iss, temp);
			ballx = std::stof(temp);
			std::getline(iss, temp);
			bally = std::stof(temp);

			std::getline(iss, temp);
			score1tx = std::stof(temp);
			std::getline(iss, temp);
			score1ty = std::stof(temp);

			std::getline(iss, temp);
			score2tx = std::stof(temp);
			std::getline(iss, temp);
			score2ty = std::stof(temp);

			std::getline(iss, temp);
			score3tx = std::stof(temp);
			std::getline(iss, temp);
			score3ty = std::stof(temp);

			std::getline(iss, temp);
			score4tx = std::stof(temp);
			std::getline(iss, temp);
			score4ty = std::stof(temp);

			std::getline(iss, temp);
			score5tx = std::stof(temp);
			std::getline(iss, temp);
			score5ty = std::stof(temp);

			std::getline(iss, temp);
			score6tx = std::stof(temp);
			std::getline(iss, temp);
			score6ty = std::stof(temp);

			std::getline(iss, temp);
			updadteInterval = std::stof(temp);

		}


		DeadRec(otherx, othery, otherprevx, otherprevy, delta);
		DeadRec(ballx, bally, ballprevx, ballprevy, delta);

		Model = glm::translate(Model, glm::vec3(tx, ty, -4.0f));
		mvp = Projection * View * Model;

		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1, 
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		otherModel = glm::translate(otherModel, glm::vec3(otherx, othery, -4.0f));
		mvp = Projection * View * otherModel;

		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		ball.col();
		ball.update(delta);

		ball.Model = glm::translate(ball.Model, glm::vec3(ballx, bally, -4.0f));
		ball.Model = glm::scale(ball.Model, glm::vec3(0.5f, 0.5f, 0.5f));
		mvp = Projection * View * ball.Model;

		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model1 = glm::translate(Model1, glm::vec3(score1tx, score1ty, -4.0f));
		Model1 = glm::scale(Model1, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = Projection * View * Model1;
		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model2 = glm::translate(Model2, glm::vec3(score2tx, score2ty, -4.0f));
		Model2 = glm::scale(Model2, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = Projection * View * Model2;
		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model3 = glm::translate(Model3, glm::vec3(score3tx, score3ty, -4.0f));
		Model3 = glm::scale(Model3, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = Projection * View * Model3;
		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model4 = glm::translate(Model4, glm::vec3(score4tx, score4ty, -4.0f));
		Model4 = glm::scale(Model4, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = Projection * View * Model4;
		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model5 = glm::translate(Model5, glm::vec3(score5tx, score5ty, -4.0f));
		Model5 = glm::scale(Model5, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = Projection * View * Model5;
		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Model6 = glm::translate(Model6, glm::vec3(score6tx, score6ty, -4.0f));
		Model6 = glm::scale(Model6, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = Projection * View * Model6;
		glBindVertexArray(vao);

		glUniformMatrix4fv(MatrixID, 1,
			GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
	}
	return 0;

}