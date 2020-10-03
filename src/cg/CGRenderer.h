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
#include <FBXModel.h>
#include <bitset>
#include <fstream>
#include <fstream>
#include <Autopilot.h>




namespace cgbv
{
	enum class ObserverSelection
	{
		Viewer,
		Light
	};
	enum class PostProcessing
	{
		Direct_Output,
		Post_Processing
	};

	struct ShaderLocations
	{
		unsigned int vertex, normal;

		unsigned int modelViewProjection, normalmatrix, modelview, model, biasedModelViewProjection;

		unsigned int placementVS, lightingVS, lambertFS, depthmapFS, canvasPlacementVS, canvasDisplayFS;

		unsigned int lightPos;

		unsigned int shadowmap;

		unsigned int rgb_tex;

		unsigned int lightPhong;

		unsigned int ambientLight;

		unsigned int diffusLight;

		unsigned int spekularLight;

		unsigned int ambientMaterial;

		unsigned int diffusMaterial;

		unsigned int spekularMaterial;

		unsigned int shininessMaterial;

		unsigned int brightnessFactor;

		unsigned int near_cp, far_cp;

		unsigned int redFS, blackFS;
	};

	struct BufferCombo
	{
		unsigned int vao, vbo, vertsToDraw;

		std::vector<glm::vec3> boundingVertices, boundingBoxViewSpace;
	};

	struct UIParameter
	{
		glm::quat globalRotation;
		float distanceLight =  200.f;
		float distanceCamera =  150.f;
		float observerprojection_near = .1f;
		float observerprojection_far = 200.f;
		float lightprojection_x_min = -15.f;
		float lightprojection_x_max = 15.f;
		float lightprojection_y_min = -15.f;
		float lightprojection_y_max = 15.f;
		float lightprojection_z_min = .1f;
		float lightprojection_z_max = 200.f;
		float modelScalation = 7.5f;

		glm::vec4 lightPos = glm::vec4(0.f, 0.f, distanceLight, 1.f);
		glm::vec4 camPos = glm::vec4(0.f, 0.f, distanceCamera, 1.f);

		// Light
		glm::vec4 ambientLight = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
		glm::vec4 diffusLight = glm::vec4(1.f, 1.f, 1.f, 1.f);
		glm::vec4 specularLight = glm::vec4(1.f, 1.f, 1.f, 1.f);
		glm::vec4 ambientMaterial = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
		glm::vec4 diffusMaterial = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
		glm::vec4 spekularMaterial = glm::vec4(1.f, 1.f, 1.f, 1.f);
		float shininessMaterial = 20.f;

		float brightnessFactor = 1.0f;

		// Shadow 
		//factor: Specifies a scale factor that is used to create a variable depth offset for each polygon.The initial value is 0.
		GLfloat offsetFactor = 1.2f;
		// units: Is multiplied by an implementation - specific value to create a constant depth offset.The initial value is 0.
		GLfloat offsetUnits = 3.2f;

		float f;
		// Model parameters 
		float modelRotation;
		std::vector<std::string> screenShotNames;

		bool enabledLightAdjustment = true;
		bool showLightDot = true;
	};

	struct Framebuffers
	{
		unsigned int shadowmap_buffer;
		unsigned int default_buffer = 0;
		unsigned int image_buffer;
	};


	struct ModelFBX
	{
		const   std::string buddha = "../modelsScaled/budda.fbx";
		const   std::string bunny = "../modelsScaled/bunny.fbx";
		const   std::string box = "../modelsScaled/box.fbx";
		const   std::string cone = "../modelsScaled/cone.fbx";
		const   std::string cylinder = "../modelsScaled/cylinder.fbx";
		const   std::string ball = "../modelsScaled/ball.fbx";
		const   std::string donut = "../modelsScaled/donut.fbx";

		const std::vector<std::string> modelPaths{ buddha,  bunny,  box,    cone,   cylinder,   ball,   donut };
		const std::vector<float> modelMaxTurn{ 360.f,       360.f,  90.f,    5.f,      5.f,    5.f,      180.f };
		const std::vector<std::string> modelNames{ "buddha", "bunny", "box", "cone", "cylinder", "ball", "donut" };

		const fbxmodel::FBXModel models[7] =
		{
			fbxmodel::FBXModel(modelPaths[0]),
			fbxmodel::FBXModel(modelPaths[1]),
			fbxmodel::FBXModel(modelPaths[2]),
			fbxmodel::FBXModel(modelPaths[3]),
			fbxmodel::FBXModel(modelPaths[4]),
			fbxmodel::FBXModel(modelPaths[5]),
			fbxmodel::FBXModel(modelPaths[6])
		};

		// global path variable that defines the fbx Model that is going to be drawn (inizializted with buddha) 
		unsigned int modelSelection;
		unsigned int lastDrawnFBX = modelSelection;
	};

	struct TweakbarPackage
	{
		void* object1;
		void* object2;
	};


	class CGRenderer : public Renderer
	{
		std::unique_ptr<cgbv::shader::GLSLShaderprogram> shader;

		ShaderLocations locs;

		BufferCombo basesurface, object, canvas, boundingBox, lightDot, background;

		glm::mat4 observer_projection, lightsource_projection, model, bias;

		glm::mat3 normal;

		UIParameter parameter;

		Framebuffers framebuffers;

		Autopilot autopilot;
		cgbv::Autopilot::ReturnValues returnValues;

		TweakbarPackage modelSelectionPackage;
		//TweakbarPackage lightPosPackage;
		TweakbarPackage camPosPackage;

		std::unique_ptr<cgbv::textures::Texture> shadowmap;
		unsigned int shadowmap_sampler;
		int shadowmap_width = 4096, shadowmap_height = 4096;

		std::unique_ptr<cgbv::textures::Texture> rgb_output;
		std::unique_ptr<cgbv::textures::Texture> normal_output;
		std::unique_ptr<cgbv::textures::Texture> depth_output;
		std::unique_ptr<cgbv::textures::Texture> depth_lin_output;
		std::unique_ptr<cgbv::textures::Texture> depth_lin_ints_output;
		std::unique_ptr<cgbv::textures::Texture> depth_ints_output;
		std::unique_ptr<cgbv::textures::Texture> sc_output;
		std::unique_ptr<cgbv::textures::Texture> rgbd_output;
		unsigned int rgb_sampler;
		unsigned int rgb_depth_rbo;

		cgbv::Camera observer_camera, lightsource_camera;

		std::bitset<1> screenshot;

		ObserverSelection viewpoint = ObserverSelection::Viewer;
		PostProcessing post_processing_pass = PostProcessing::Post_Processing;

		void shadowmap_pass();
		void final_pass();
		void canvas_pass();

		void create_image_framebuffer();
		std::vector<glm::vec3> findBoundingVertices(std::vector<float> vertices);
		void drawBoundingBox();
		void drawLightDot(glm::vec4 lightPos);
		void adjustLight(glm::mat4 shadow_view);

	public:

		CGRenderer(GLFWwindow* window);
		~CGRenderer(void);

		ModelFBX modelfbx;

		virtual void destroy();
		virtual void resize(int width, int height);
		virtual void input(int key, int scancode, int action, int modifiers);
		virtual bool setup();
		virtual void render();
		virtual void update();
		void capture();
		void loadFBX(int currentMod); 
	};
}