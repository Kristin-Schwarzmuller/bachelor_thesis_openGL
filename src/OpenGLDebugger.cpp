#include <OpenGLDebugger.h>
#include <glad.h>
#include <iostream>

namespace cgbv
{
	void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		auto msg_type = "";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			msg_type = "*** Error ***";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			msg_type = "Deprecated Behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			msg_type = "Undefined Behavior";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			msg_type = "Performance";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			msg_type = "Portability";
			break;
		case GL_DEBUG_TYPE_OTHER:
			msg_type = "Other";
			break;
		default:
			msg_type = "Unknown";
		}

		auto msg_source = "";

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			msg_source = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			msg_source = "Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			msg_source = "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			msg_source = "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			msg_source = "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			msg_source = "Other";
			break;
		default:
			msg_source = "Unknown";
		}

		auto msg_severity = "";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			msg_severity = "High";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			msg_severity = "Medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			msg_severity = "Low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			msg_severity = "Notification";
			break;
		default:
			msg_severity = "Unknown";
		}

		//printf("\nGL DEBUG: %s, source = %, severity = 0x%x, message = %s\n", msg_type, msg_source, msg_severity, message);
		printf("\nGL DEBUG: %s, source = %s, severity = %s, message = %s\n", msg_type, msg_source, msg_severity, message);
	}
}

cgbv::OpenGLDebugger::OpenGLDebugger()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(cgbv::MessageCallback, 0);
}

cgbv::OpenGLDebugger::~OpenGLDebugger()
{

}


