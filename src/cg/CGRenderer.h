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
#include <bitset>


namespace cgbv
{
	struct ShaderLocations
    {
        unsigned int vertex, normal;

        unsigned int modelViewProjection, normalmatrix, modelview, biasedModelViewProjection;

        unsigned int placementVS, lightingVS, lambertFS, depthmapFS;

		unsigned int lightPos;

        unsigned int shadowmap;

        // new 
        unsigned int lightPhong;

        unsigned int ambientLight;

        unsigned int diffusLight;

        unsigned int spekularLight;

        unsigned int ambientMaterial;

        unsigned int diffusMaterial;

        unsigned int spekularMaterial;

        //unsigned int emissivMaterial;

        unsigned int shininessMaterial;
    };

    struct BufferCombo
    {
        unsigned int vao, vbo, vertsToDraw;
    };

	struct UIParameter
	{
		glm::quat globalRotation;
        glm::vec4 lightPos = glm::vec4(0.f, 0.f, 50.f, 1.f);

        // new 
        glm::vec4 ambientLight = glm::vec4(0.1f, 0.1f, 0.1f, 1.f);
        glm::vec4 diffusLight = glm::vec4(1.f, 1.f, 1.f, 1.f);
        glm::vec4 specularLight = glm::vec4(1.f, 1.f, 1.f, 1.f);
        glm::vec4 ambientMaterial = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
        glm::vec4 diffusMaterial = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
        glm::vec4 spekularMaterial = glm::vec4(1.f, 1.f, 1.f, 1.f);
        glm::vec4 emissivMaterial = glm::vec4(0.f, 0.f, 0.f, 0.f);
        float shininessMaterial = 10.f;

        float f;
	};

    struct Framebuffers
    {
        unsigned int shadowmap_buffer;
        unsigned int default_buffer = 0;
    };


	class CGRenderer : public Renderer
	{
        std::unique_ptr<cgbv::shader::GLSLShaderprogram> shader;

        ShaderLocations locs;

        BufferCombo basesurface, object;

        glm::mat4 observer_projection, lightsource_projection, model, bias;

        glm::mat3 normal;

		UIParameter parameter;

        Framebuffers framebuffers;

        std::unique_ptr<cgbv::textures::Texture> shadowmap;
        unsigned int shadowmap_sampler;
        int shadowmap_width = 4096, shadowmap_height = 4096;

        cgbv::Camera observer_camera, lightsource_camera;

        std::bitset<1> screenshot;

        void shadowmap_pass();
        void final_pass();

	public:
        CGRenderer(GLFWwindow *window);
		~CGRenderer(void);

		virtual void destroy();
		virtual void resize(int width, int height);
		virtual void input(int key, int scancode, int action, int modifiers);
		virtual bool setup();
		virtual void render();
		virtual void update();
        void capture();
	};
}
