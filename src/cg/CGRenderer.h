#pragma once

#include <Renderer.h>
#include <memory>
#include <vector>
#include <GLSLShader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Texture.h>
#include <FrameCounter.h>
#include <Camera.h>


namespace cgbv
{
    struct ShaderLocations
    {
        unsigned int vertex, normal;

        unsigned int modelViewProjection, normalmatrix, modelview;

        unsigned int subVertex, subFragment;

		unsigned int lightPos;
    };

    struct BufferCombo
    {
        unsigned int vao, vbo, vertsToDraw;
    };

	struct UIParameter
	{
		glm::quat globalRotation;
        glm::vec4 lightPos = glm::vec4(0.f, 0.f, 5.f, 1.f);

        float f;
	};


	class CGRenderer : public Renderer
	{
        std::unique_ptr<cgbv::shader::GLSLShaderprogram> shader;

        ShaderLocations locs;

        BufferCombo cone;

        glm::mat4 projection, model;

        glm::mat3 normal;

		UIParameter parameter;

        cgbv::Camera camera;

	public:
        CGRenderer(GLFWwindow *window);
		~CGRenderer(void);

		virtual void destroy();
		virtual void resize(int width, int height);
		virtual void input(int key, int scancode, int action, int modifiers);
		virtual bool setup();
		virtual void render();
		virtual void update();
	};
}
