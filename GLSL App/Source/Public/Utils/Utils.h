#pragma once
#include <iostream>
#include <fstream>
#include "glew.h"
#include <vector>
#include "SOIL/SOIL.h"
#include "GL/freeglut.h"
#include "glm/detail/type_vec.hpp"
#include "glm/gtc/type_ptr.hpp"

// Small number for small float comparison accuracy.
#define SMALL_NUMBER 0.00001
#define LARGE_NUMBER 100000.0

// Directional unit vectors.
const glm::vec3 FORWARD_VECTOR = glm::vec3(0, 0, -1);
const glm::vec3 UP_VECTOR = glm::vec3(0, 1, 0);
const glm::vec3 RIGHT_VECTOR = glm::vec3(1, 0, 0);

class Utils {
public:
	/**
	* Returns the length of a file.
	* Parameter: ifstream& file  Input file stream.
	* Returns:   unsigned long  File length.
	*/
	inline static unsigned long getFileLength(std::ifstream& file) {
		if (!file.good()) return 0;
		file.seekg(0, std::ios::end);
		unsigned long len = file.tellg();
		file.seekg(std::ios::beg);

		return len;
	};

	/** Loads a texture from a file using SOIL. */
	inline static int loadTexture(const char* path) {
		std::cout << "Loading texture: " << path << std::endl;
		return SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, 0, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
	}
	inline static unsigned char* loadTextureRaw(const char* path, int width, int height) {
		std::cout << "Loading texture: " << path << std::endl;
		int channels = 3;
		unsigned char* data = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGB);
		if (data == 0) {
			std::cout << SOIL_last_result() << std::endl;
		}
		return data;
	};

	/** Loads a cubemap from a file using SOIL. Each file is a single face on the cube. */
	inline static int loadCubemap(const char* rightfile, const char* leftFile, const char* topFile, const char* bottomFile, const char* backFile, const char* frontFile) {
		std::cout << "Loading cubemap" << std::endl;
		GLuint cubemap = SOIL_load_OGL_cubemap(rightfile, leftFile, topFile, bottomFile, backFile, frontFile, SOIL_LOAD_AUTO, 0, 0);
		// Ensure the cubemap stretches from edge to edge to prevent seams.
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

		return cubemap;
	}

	/**
	* Draws a string using glut
	* Parameter: std::string str  String to draw
	*/
	static inline void renderString(float x, float y, std::string str) {
		glColor3f(1, 1, 1);
		glRasterPos2f(x, y);
		for (unsigned i = 0; i < str.length(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}
	};

	static inline glm::vec3 rgbToGL(float r, float g, float b) {
		return glm::vec3(r / 255.f, g / 255.f, b / 255.f);
	};
};

/** 
* Call loadShader() to load a shader from a file and compile it. 
*/
class ShaderLoader {

public:
	/** Variables used in the shader program. */
	struct Vars {
		/** Model matrix. */
		static constexpr const char* MODEL = "model";
		/**View matrix. */
		static constexpr const char* VIEW = "view";
		/** Projection matrix. */
		static constexpr const char* PROJECTION = "projection";
		/** Camera position. */
		static constexpr const char* VIEW_POSITION = "viewPosition";

		/** Material diffuse. Should be followed by a number e.g. diffuse1 */
		static constexpr const char* MAT_DIFFUSE = "material.diffuse";
		/** Material specular. Should be followed by a number e.g. specular1 */
		static constexpr const char* MAT_SPECULAR = "material.specular";
		/** Material normal. Should be followed by a number e.g. normal1 */
		static constexpr const char* MAT_NORMAL = "material.normal";
		/** Material diffuse colour. */
		static constexpr const char* MAT_DIFFUSE_COLOUR = "material.diffuseColour";
		/** Material specular colour */
		static constexpr const char* MAT_SPECULAR_COLOUR = "material.specularColour";
		/** Material shininess value. */
		static constexpr const char* MAT_SHININESS = "material.shininess";
		/** Whether the material has a valid normal map set. */
		static constexpr const char* MAT_HAS_NORMALMAP = "material.hasNormalMap";

		/** Selection colour code. */
		static constexpr const char* COLOUR_CODE = "colourCode";
	};	

public:
	/** Sets a uniform glm::vec3 in the specified shader with the specified name. */
	inline static void setShaderValue(GLuint shader, const char* key, glm::vec3 value) {
		GLuint loc = glGetUniformLocation(shader, key);
		glUniform3f(loc, value.x, value.y, value.z);
	}
	/** Sets a uniform glm::mat4 in the specified shader with the specified name. */
	inline static void setShaderValue(GLuint shader, const char* key, glm::mat4 value) {
		GLuint loc = glGetUniformLocation(shader, key);
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
	}
	/** Sets a uniform value in the specified shader with the specified name. */
	inline static void setShaderValue(GLuint shader, const char* key, float value) {
		GLuint loc = glGetUniformLocation(shader, key);
		glUniform1f(loc, value);
	}
	/** Sets a uniform value in the specified shader with the specified name. */
	inline static void setShaderValue(GLuint shader, const char* key, GLuint value) {
		GLuint loc = glGetUniformLocation(shader, key);
		glUniform1i(loc, value);
	}

	/**
	* Loads a GL shader.
	* Parameter: const GLuint& shaderHandle  Shader handle to load into.
	* Parameter: const char* filename  Shader file to load.
	* Parameter: bool compile Whether to compile the shader as well. Default = true.
	*/
	inline static void loadShader(const GLuint& shaderHandle, const char* filename, bool compile = true) {
		const GLchar* shaderSource = readShaderFile(filename);;
		glShaderSource(shaderHandle, 1, &shaderSource, nullptr);
		if (compile) compileShader(shaderHandle);

		// Free source memory.
		unloadShaderSource(shaderSource);
	};

	/**
	* Reads a shader file into a GLchar array.
	* Parameter: const char* filename  path of the file to load.
	* Returns:   GLchar*  File data.
	*/
	inline static GLchar* readShaderFile(const char* filename) {
		std::ifstream file;
		// Open the file.
		file.open(filename, std::ios::in);
		if (!file) {
			std::cout << "Error opening shader file: '" << filename << "'" << std::endl;
			return nullptr;
		}

		// Get length of the file.
		auto len = Utils::getFileLength(file);
		if (len == 0) {
			std::cout << "Empty shader file: '" << filename << "'" << std::endl;
			return nullptr;
		}

		GLchar* ShaderSource = new char[len + 1];
		if (!ShaderSource) {
			std::cout << "Failed to allocate memory whilst loading '" << filename << "'" << std::endl;
			return nullptr;
		}
		ShaderSource[len] = 0;
		unsigned int i = 0;
		// Read the contents of the file into the char array.
		while (file.good()) {
			ShaderSource[i] = file.get();
			if (!file.eof()) i++;
		}
		ShaderSource[i] = 0;
		file.close();

		return ShaderSource;
	};

	inline static void unloadShaderSource(const GLchar* ShaderSource) {
		if (ShaderSource) {
			delete[] ShaderSource;
		}
		ShaderSource = nullptr;
	};

	/**
	* Compiles a loaded shader. Errors will be logged if they occur.
	* Parameter: const GLuint & shaderHandle
	* Returns:   void
	*/
	inline static void compileShader(const GLuint& shaderHandle) {
		glCompileShader(shaderHandle);
		// Check if the compile was successful.
		GLint result;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
		if (!result) {
			std::cout << "Shader compilation failed: " << std::endl;
			// Compile failed, try and get the log.
			GLint logLen;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0) {
				// Print log info.
				char *log = (char*)malloc(logLen);
				GLsizei written;
				glGetShaderInfoLog(shaderHandle, logLen, &written, log);
				std::cout << log << std::endl;
				free(log);
			}
		}
	}

	inline static GLuint createShaderProgram(GLuint& vertexShader, GLuint& fragmentShader) {
		GLuint programObj = glCreateProgram();
		glAttachShader(programObj, vertexShader);
		glAttachShader(programObj, fragmentShader);

		glLinkProgram(programObj);

		// Check if the link was successful.
		GLint result;
		glGetProgramiv(programObj, GL_LINK_STATUS, &result);
		if (!result) {
			std::cout << "Program creation failed: " << std::endl;
			// Link failed, try and get the log.
			GLint logLen;
			glGetProgramiv(programObj, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0) {
				// Print log info.
				char *log = (char*)malloc(logLen);
				GLsizei written;
				glGetProgramInfoLog(programObj, logLen, &written, log);
				std::cout << log << std::endl;
				free(log);
			}
		}

		return programObj;
	}

	/**
	* Compiles a vertex and fragment shader and creates a GL Shader program with them.
	* Parameter: const char* vertexShaderFile  Path to the vertex shader file.
	* Parameter: const char* fragmentShaderFile  Path to the fragment shader file.
	* Returns: GLuint  Created shader program handle.
	*/
	inline static GLuint createShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile) {
		GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);

		// Read and compile the shaders.
		ShaderLoader::loadShader(vertexShaderObj, vertexShaderFile);
		ShaderLoader::loadShader(fragmentShaderObj, fragmentShaderFile);
		// Create program and attach the created shaders.
		return ShaderLoader::createShaderProgram(vertexShaderObj, fragmentShaderObj);
	} 
};