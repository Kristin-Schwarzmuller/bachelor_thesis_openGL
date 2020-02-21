// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <GLSLShader.h>

#include <iostream>

#include <sstream>
#include <fstream>
//	===========================================================================================================================================================================================		#####




// Usings																																														   Usings
//	===========================================================================================================================================================================================		#####
//	===========================================================================================================================================================================================		#####




// GLSLShader																																												   GLSLShader
//	===========================================================================================================================================================================================		#####
cgbv::shader::GLSLShader::GLSLShader(const std::string& sourcePath, GLenum type)
{
	std::vector<char> source;

	unsigned int filesize = getFileSize(sourcePath);

	source.reserve(filesize + 1);

	readFile(sourcePath, source);

	buildShader(source, sourcePath, type);
}

void cgbv::shader::GLSLShader::buildShader(const std::vector<char>& sourceCode, const std::string& filepath, const GLenum type)
{
	shader = glCreateShader(type);

	const char* src = sourceCode.data();
	glShaderSource(shader, 1, &src, nullptr);

	glCompileShader(shader);

	GLint err;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &err);

	if(err == GL_FALSE)
	{
		int infolog_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolog_len);

		std::unique_ptr<char[]> infoLog = std::make_unique<char[]>(infolog_len);
		glGetShaderInfoLog(shader, infolog_len, nullptr, infoLog.get());
		
		std::cout << "Fehler beim Kompilieren des Shaders: " << filepath << std::endl << "Folgender Fehler trat auf: " << std::endl << infoLog.get() << std::endl;

		glDeleteShader(shader);
	}
}

void cgbv::shader::GLSLShader::readFile(const std::string& srcFile, std::vector<char>& target)
{
    std::ifstream input(srcFile, std::ifstream::in);

    std::stringstream stream;
    std::string s;

    while(std::getline(input, s))
        stream << s << std::endl;

    stream << "// make peace with the compiler";

    std::string code = stream.str();

    std::copy(std::begin(code), std::end(code), std::back_inserter(target));
}

unsigned int cgbv::shader::GLSLShader::getFileSize(const std::string& file)
{
    std::ifstream input(file);
    input.seekg(0, std::ios::end);

    unsigned int filesize = static_cast<unsigned int>(input.tellg());

    input.close();

    return filesize;
}

cgbv::shader::GLSLShader::~GLSLShader()
{
	glDeleteShader(shader);
}
//	===========================================================================================================================================================================================		#####




// GLSLShaderprogram																																									GLSLShaderprogram
//	===========================================================================================================================================================================================		#####
cgbv::shader::GLSLShaderprogram::GLSLShaderprogram()
{
	program = glCreateProgram();
}


cgbv::shader::GLSLShaderprogram::GLSLShaderprogram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::unique_ptr<GLSLShader> vertex(new GLSLShader(vertexShaderPath, GL_VERTEX_SHADER));
	std::unique_ptr<GLSLShader> fragment(new GLSLShader(fragmentShaderPath, GL_FRAGMENT_SHADER));

	program = glCreateProgram();

	attachShader(vertex);
	attachShader(fragment);

	bind();
}


cgbv::shader::GLSLShaderprogram::~GLSLShaderprogram(void)
{
	glDeleteProgram(program);
}


void cgbv::shader::GLSLShaderprogram::attachShader(const std::unique_ptr<GLSLShader>& shader)
{
	glAttachShader(program, shader->shader);
}


void cgbv::shader::GLSLShaderprogram::bind()
{
	glLinkProgram(program);

	GLint err;
	glGetProgramiv(program, GL_LINK_STATUS, &err);

	if(err == GL_FALSE)
	{
		int infolog_len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len);

		std::unique_ptr<char[]> infoLog = std::make_unique<char[]>(infolog_len);
		glGetProgramInfoLog(program, infolog_len, nullptr, infoLog.get());

		std::cout << "Fehler beim Linken des Programms: " << program << std::endl << "Folgender Fehler trat auf:" << std::endl << infoLog.get() << std::endl;

		glDeleteProgram(program);
	}
}


void cgbv::shader::GLSLShaderprogram::use(void)
{
	glUseProgram(program);
}


GLuint cgbv::shader::GLSLShaderprogram::getUniformLocation(const std::string& name)
{
	return glGetUniformLocation(program, name.c_str());
}


GLuint cgbv::shader::GLSLShaderprogram::getAttribLocation(const std::string& name)
{
	return glGetAttribLocation(program, name.c_str());
}


GLuint cgbv::shader::GLSLShaderprogram::getSubroutineIndex(unsigned int shaderType, const std::string& name)
{
	return glGetSubroutineIndex(program, shaderType, name.c_str());
}
//	===========================================================================================================================================================================================		#####
