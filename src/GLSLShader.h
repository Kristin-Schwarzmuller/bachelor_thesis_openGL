#pragma once


// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
//#include "gl_core_4_4.hpp"
#include <glad.h>
#include <memory>
#include <vector>
#include <string>
//	===========================================================================================================================================================================================		#####




namespace cgbv
{
	namespace shader
	{
		// GLSLShader																																										   GLSLShader
		//	===================================================================================================================================================================================		#####
		class GLSLShader
		{
			GLuint shader;

			void buildShader(const std::vector<char>& sourceCode, const std::string& filepath, const GLenum type);
            unsigned int getFileSize(const std::string& file);
            void readFile(const std::string& srcFile, std::vector<char>& target);


		public:
			GLSLShader(const std::string& sourcePath, GLenum type);
			~GLSLShader();

			friend class GLSLShaderprogram;
		};
		//	===================================================================================================================================================================================		#####



		// GLSLShaderprogram																																							GLSLShaderprogram
		//	===================================================================================================================================================================================		#####
		class GLSLShaderprogram
		{
			GLuint program;

		public:
			GLSLShaderprogram();
			GLSLShaderprogram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
			~GLSLShaderprogram(void);

			void attachShader(const std::unique_ptr<GLSLShader>& shader);

			void bind();
			void use();

			GLuint getUniformLocation(const std::string& name);
			GLuint getAttribLocation(const std::string& name);
			GLuint getSubroutineIndex(unsigned int shaderType, const std::string& name);
		};
		//	===================================================================================================================================================================================		#####
	}
}

