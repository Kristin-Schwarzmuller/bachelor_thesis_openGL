#include <cg/CGRenderer.h>

#include <iostream>
#include <AntTweakBar.h>
#include <OpenGLDebugger.h>
#include <algorithm>

namespace cgbv
{
	// ======= Callbach funktions ===============================
	void TW_CALL handleScreenshot(void* data)
	{
		CGRenderer* renderer = reinterpret_cast<CGRenderer*>(data);
		renderer->capture();
	}

	void TW_CALL modelSetCallback(const void* value, void* clientData)
	{
		auto package = static_cast<TweakbarPackage*>(clientData);
		auto model = static_cast<ModelFBX*>(package->object1);
		auto cgrenderer = static_cast<CGRenderer*>(package->object2);
		model->modelSelection = (*(const unsigned int*)value);
		cgrenderer->loadFBX(model->modelSelection);
	}

	void TW_CALL modelGetCallback(void* value, void* clientData)
	{
		auto package = static_cast<TweakbarPackage*>(clientData);
		auto model = static_cast<ModelFBX*>(package->object1);
		*(unsigned int*)value = model->modelSelection;
	}

	void TW_CALL pp_SetCallback(const void* value, void* clientData)
	{
		auto post_processing_pass = static_cast<PostProcessing*>(clientData);
		*post_processing_pass = (*(PostProcessing*)value);
		std::cout << "Set Value" << std::endl;
	}

	void TW_CALL pp_GetCallback(void* value, void* clientData)
	{
		auto post_processing_pass = static_cast<PostProcessing*>(clientData);
		*(PostProcessing*)value = *post_processing_pass;
		std::cout << "Got Value" << std::endl;
	}
	// ==========================================================

	CGRenderer::CGRenderer(GLFWwindow* window) : Renderer(window)
	{

	}


	CGRenderer::~CGRenderer()
	{

	}


	void CGRenderer::capture()
	{
		screenshot.set();
	}


	void CGRenderer::destroy()
	{
		glDeleteVertexArrays(1, &basesurface.vao);
		glDeleteBuffers(1, &basesurface.vbo);

		glDeleteVertexArrays(1, &object.vao);
		glDeleteBuffers(1, &object.vbo);

		glDeleteVertexArrays(1, &canvas.vao);
		glDeleteBuffers(1, &canvas.vbo);
		
		glDeleteVertexArrays(1, &boundingBox.vao);
		glDeleteBuffers(1, &boundingBox.vbo);

		glDeleteFramebuffers(1, &framebuffers.shadowmap_buffer);
		glDeleteSamplers(1, &shadowmap_sampler);

		glDeleteFramebuffers(1, &framebuffers.image_buffer);
		glDeleteSamplers(1, &rgb_sampler);

		glDeleteRenderbuffers(1, &rgb_depth_rbo);
	}


	void CGRenderer::resize(int width, int height)
	{
		window_width = width;
		window_height = height;

		glViewport(0, 0, width, height);

		observer_projection = glm::perspective(float(M_PI) / 4.5f, float(window_width) / float(window_height), .1f, 200.f);

		create_image_framebuffer();

		TwWindowSize(width, height > 0 ? height : 1);
	}


	void CGRenderer::input(int key, int scancode, int action, int modifiers)
	{
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		{
			TwEventCharGLFW(GLFW_KEY_KP_ENTER, action);
			TwEventKeyGLFW(GLFW_KEY_KP_ENTER, action);
			return;
		}

		TwEventCharGLFW(key, action);
		TwEventKeyGLFW(key, action);
		std::cout << key << std::endl;

		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_W:
				observer_camera.moveForward(.1f);
				break;
			case GLFW_KEY_S:
				observer_camera.moveForward(-.1f);
				break;
			case GLFW_KEY_A:
				observer_camera.moveRight(.1f);
				break;
			case GLFW_KEY_D:
				observer_camera.moveRight(-.1f);
				break;
			case GLFW_KEY_E:
				modelfbx.modelSelection = 0;
				break;
			case GLFW_KEY_R:
				modelfbx.modelSelection = 1;
				break;
			case GLFW_KEY_T:
				modelfbx.modelSelection = 2;
				break;
			case GLFW_KEY_Y: // --> press z on the german keyboard --> translated to y 
				modelfbx.modelSelection = 3;
				break;
			case GLFW_KEY_U:
				modelfbx.modelSelection = 4;
				break;
			case GLFW_KEY_I:
				modelfbx.modelSelection = 5;
				break;
			case GLFW_KEY_O:
				modelfbx.modelSelection = 6;
				break;
			}
		}
	}


	bool CGRenderer::setup()
	{
		autopilot.init(modelfbx.modelMaxTurn, modelfbx.modelNames, parameter.distanceLight, parameter.distanceCamera);

		glfwGetFramebufferSize(window, &window_width, &window_height);


		if (!gladLoadGL())
			return false;

		// GL States
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glEnable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		
		OpenGLDebugger debugger;

		// Matrices 
		{

			observer_projection = glm::perspective(float(M_PI) / 5.f, float(window_width) / float(window_height), .1f, parameter.observerprojection_far);
			observer_camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
			observer_camera.moveTo(0.f, 2.5f, parameter.distanceCamera); // here changes happend 5.f --> 10.f

			lightsource_projection = glm::ortho(-parameter.lightprojection_xy, parameter.lightprojection_xy,
												-parameter.lightprojection_xy, parameter.lightprojection_xy, 
												parameter.lightprojection_zfrom, parameter.lightprojection_zto);
			//lightsource_projection = glm::ortho(-15.f, 15.f, -15.f, 15.f, .1f, 100.f);

			lightsource_camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
			lightsource_camera.moveTo(parameter.lightPos);

			bias = glm::mat4
			(
				0.5, 0.0, 0.0, 0.0,
				0.0, 0.5, 0.0, 0.0,
				0.0, 0.0, 0.5, 0.0,
				0.5, 0.5, 0.5, 1.0
			);
		}




		// Shader
		{
			shader = std::make_unique<cgbv::shader::GLSLShaderprogram>("../shader/VertexShader.glsl", "../shader/FragmentShader.glsl");
			locs.vertex = shader->getAttribLocation("vertex");
			locs.normal = shader->getAttribLocation("normal");
			//locs.uv = shader->getAttribLocation("uvs");
			locs.modelViewProjection = shader->getUniformLocation("matrices.mvp");
			locs.normalmatrix = shader->getUniformLocation("matrices.normal");
			locs.modelview = shader->getUniformLocation("matrices.mv");
			locs.biasedModelViewProjection = shader->getUniformLocation("matrices.bmvp");
			locs.lightPos = shader->getUniformLocation("light.lightPos");
			locs.shadowmap = shader->getUniformLocation("tex.shadowmap");
			locs.rgb_tex = shader->getUniformLocation("tex.canvas");
			locs.lambertFS = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "lambert");
			locs.depthmapFS = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "depthmap");
			locs.lightingVS = shader->getSubroutineIndex(GL_VERTEX_SHADER, "verts_and_normals");
			locs.placementVS = shader->getSubroutineIndex(GL_VERTEX_SHADER, "simple_placement");
			locs.canvasPlacementVS = shader->getSubroutineIndex(GL_VERTEX_SHADER, "canvas_placement");
			locs.canvasDisplayFS = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "canvas_display");
			locs.lightPhong = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "phongWithLambert");
			locs.ambientLight = shader->getUniformLocation("light.ambient");
			locs.ambientMaterial = shader->getUniformLocation("material.ambient");
			locs.diffusLight = shader->getUniformLocation("light.diffus");
			locs.diffusMaterial = shader->getUniformLocation("material.diffus");
			locs.spekularLight = shader->getUniformLocation("light.specular");
			locs.spekularMaterial = shader->getUniformLocation("material.spekular");
			locs.shininessMaterial = shader->getUniformLocation("material.shininess");
			locs.brightnessFactor = shader->getUniformLocation("light.brightnessFactor");

		}




		// Geometrie
		{
			// Base
			std::vector<glm::vec3> vertices;

			glm::vec3 a(-50.f, 0.f, -50.f);
			glm::vec3 b(50.f, 0.f, -50.f);
			glm::vec3 c(50.f, 0.f, 50.f);
			glm::vec3 d(-50.f, 0.f, 50.f);

			glm::vec3 n(0.f, 1.f, 0.f);

			vertices.push_back(d); vertices.push_back(c); vertices.push_back(a);
			vertices.push_back(a); vertices.push_back(c); vertices.push_back(b);

			std::vector<float> data;
			for (auto v : vertices)
			{
				data.insert(std::end(data), glm::value_ptr(v), glm::value_ptr(v) + sizeof(glm::vec3) / sizeof(float));
				data.insert(std::end(data), glm::value_ptr(n), glm::value_ptr(n) + sizeof(glm::vec3) / sizeof(float));
				basesurface.vertsToDraw++;
			}

			glGenVertexArrays(1, &basesurface.vao);
			glBindVertexArray(basesurface.vao);

			glGenBuffers(1, &basesurface.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, basesurface.vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(locs.vertex);
			glVertexAttribPointer(locs.vertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
			glEnableVertexAttribArray(locs.normal);
			glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)size_t(3 * sizeof(float)));


			data.clear();
			vertices.clear();

			// Canvas 
			a = glm::vec3(-1.f, -1.f, 0.f);
			b = glm::vec3(1.f, -1.f, 0.f);
			c = glm::vec3(-1.f, 1.f, 0.f);
			d = glm::vec3(1.f, 1.f, 0.f);

			n = glm::vec3(0.f, 0.f, 1.f);

			vertices.push_back(a); vertices.push_back(b); vertices.push_back(c); vertices.push_back(d);
			

			for (auto v : vertices)
			{
				data.insert(std::end(data), glm::value_ptr(v), glm::value_ptr(v) + sizeof(glm::vec3) / sizeof(float));
				data.insert(std::end(data), glm::value_ptr(n), glm::value_ptr(n) + sizeof(glm::vec3) / sizeof(float));
				canvas.vertsToDraw++;
			}

			glGenVertexArrays(1, &canvas.vao);
			glBindVertexArray(canvas.vao);

			glGenBuffers(1, &canvas.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, canvas.vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(locs.vertex);
			glVertexAttribPointer(locs.vertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
			glEnableVertexAttribArray(locs.normal);
			glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)size_t(3 * sizeof(float)));


			data.clear();
			vertices.clear();

			loadFBX(modelfbx.modelSelection);

			// Bounding Box
			a = glm::vec3(modelfbx.boundingBoxVals.x_min, modelfbx.boundingBoxVals.y_min, modelfbx.boundingBoxVals.z_min);
			b = glm::vec3(modelfbx.boundingBoxVals.x_max, modelfbx.boundingBoxVals.y_min, modelfbx.boundingBoxVals.z_min);
			c = glm::vec3(modelfbx.boundingBoxVals.x_min, modelfbx.boundingBoxVals.y_min, modelfbx.boundingBoxVals.z_max);
			d = glm::vec3(modelfbx.boundingBoxVals.x_max, modelfbx.boundingBoxVals.y_min, modelfbx.boundingBoxVals.z_max);
			glm::vec3 e(modelfbx.boundingBoxVals.x_min, modelfbx.boundingBoxVals.y_max, modelfbx.boundingBoxVals.z_min);
			glm::vec3 f(modelfbx.boundingBoxVals.x_max, modelfbx.boundingBoxVals.y_max, modelfbx.boundingBoxVals.z_min);
			glm::vec3 g(modelfbx.boundingBoxVals.x_min, modelfbx.boundingBoxVals.y_max, modelfbx.boundingBoxVals.z_max);
			glm::vec3 h(modelfbx.boundingBoxVals.x_max, modelfbx.boundingBoxVals.y_max, modelfbx.boundingBoxVals.z_max);

			glm::vec3 color(0.f, 1.f, 0.f);

			// floor 
			vertices.push_back(a); vertices.push_back(b); 
			vertices.push_back(c); vertices.push_back(d);
			vertices.push_back(a); vertices.push_back(c);
			vertices.push_back(b); vertices.push_back(d);
			// roof
			vertices.push_back(e); vertices.push_back(f);
			vertices.push_back(g); vertices.push_back(h);
			vertices.push_back(e); vertices.push_back(g);
			vertices.push_back(f); vertices.push_back(h);
			// edges
			vertices.push_back(a); vertices.push_back(e);
			vertices.push_back(b); vertices.push_back(f);
			vertices.push_back(c); vertices.push_back(g);
			vertices.push_back(d); vertices.push_back(h);

			for (auto v : vertices)
			{
				data.insert(std::end(data), glm::value_ptr(v), glm::value_ptr(v) + sizeof(glm::vec3) / sizeof(float));
				data.insert(std::end(data), glm::value_ptr(n), glm::value_ptr(n) + sizeof(glm::vec3) / sizeof(float));
				boundingBox.vertsToDraw++;
			}

			glGenVertexArrays(1, &boundingBox.vao);
			glBindVertexArray(boundingBox.vao);

			glGenBuffers(1, &boundingBox.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, boundingBox.vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(locs.vertex);
			glVertexAttribPointer(locs.vertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
			glEnableVertexAttribArray(locs.normal);
			glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)size_t(3 * sizeof(float)));


			data.clear();
			vertices.clear();
		}

		
		


		// GUI
		{
			TwInit(TW_OPENGL_CORE, nullptr);
			TwWindowSize(1280, 720);
			TwBar* tweakbar = TwNewBar("TweakBar");
			TwDefine(" TweakBar size='300 500'");

			TwAddVarRW(tweakbar, "Global Rotation", TW_TYPE_QUAT4F, &parameter.globalRotation, "showval=false opened=true");
			TwAddButton(tweakbar, "Take Screenshot", handleScreenshot, this, nullptr);
			TwAddVarRW(tweakbar, "Light direction", TW_TYPE_DIR3F, &parameter.lightPos, "group=Light axisx=-x axisy=-y axisz=-z opened=true");

			// ====== Light ======
			TwAddVarRW(tweakbar, "Ambient Light", TW_TYPE_COLOR4F, &parameter.ambientLight, "group=Light");
			TwAddVarRW(tweakbar, "diffuse Light", TW_TYPE_COLOR4F, &parameter.diffusLight, " group=Light alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "Specular  Light", TW_TYPE_COLOR4F, &parameter.specularLight, " group=Light alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "Brightness", TW_TYPE_FLOAT, &parameter.brightnessFactor, " group= 'Light' min = 0.0f max = 5.0f step = 0.1f");
			TwAddVarRW(tweakbar, "Shininess", TW_TYPE_FLOAT, &parameter.shininessMaterial, " group=Material");
			TwAddVarRW(tweakbar, "Ambient Material", TW_TYPE_COLOR4F, &parameter.ambientMaterial, " group=Material alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "diffuse Material", TW_TYPE_COLOR4F, &parameter.diffusMaterial, " group=Material alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "Specular  Material", TW_TYPE_COLOR4F, &parameter.spekularMaterial, " group=Material alpha help='Color and transparency of the cube.' ");

			// ====== Shadow ======
			TwAddVarRW(tweakbar, "Shadow Offset Factor", TW_TYPE_FLOAT, &parameter.offsetFactor, " group = 'Shadow' min = 0.0f max = 128.0f step = 0.1f");
			TwAddVarRW(tweakbar, "Shadowmap Offset Units", TW_TYPE_FLOAT, &parameter.offsetUnits, " group = 'Shadow' min = 0.0f max = 128.0f step = 0.1f");

			// ====== Modelauswahl ======
			std::string meshtypes = "BUDDHA, BUNNY, BOX, CONE, CYLINDER, BALL, DONUT";
			TwType meshType = TwDefineEnumFromString("vertexType", meshtypes.c_str());
			modelSelectionPackage.object1 = &modelfbx;
			modelSelectionPackage.object2 = this;
			TwAddVarCB(tweakbar, "Model", meshType, modelSetCallback, modelGetCallback, &modelSelectionPackage, "");// min = 0 max = 6");

			// ====== Direct output ======
			std::string post_processing_types = "Direct_Output, Post_Processing";
			TwType pp_type = TwDefineEnumFromString("pp_type", post_processing_types.c_str());
			TwAddVarCB(tweakbar, "Post Processing", pp_type, pp_SetCallback, pp_GetCallback, &post_processing_pass, "");
		}




		// Framebuffer shadowmap 
		{
			shadowmap = std::make_unique<cgbv::textures::Texture>();

			glGenFramebuffers(1, &framebuffers.shadowmap_buffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.shadowmap_buffer);

			glBindTexture(GL_TEXTURE_2D, shadowmap->getTextureID());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowmap_width, shadowmap_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

			glGenSamplers(1, &shadowmap_sampler);
			glSamplerParameteri(shadowmap_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glSamplerParameteri(shadowmap_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glSamplerParameteri(shadowmap_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(shadowmap_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(shadowmap_sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmap->getTextureID(), 0);
			glDrawBuffer(GL_NONE);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "Shadowmap Framebuffer failed" << std::endl;
				std::cin.get();
			}
		}

		create_image_framebuffer();

		return true;
	}


	void CGRenderer::create_image_framebuffer()
	{
		if (rgb_output)
		{
			rgb_output.reset();
			normal_output.reset();
			glDeleteFramebuffers(1, &framebuffers.image_buffer);
			glDeleteSamplers(1, &rgb_sampler);
			glDeleteRenderbuffers(1, &rgb_depth_rbo);
		}

		// Canvas Framebuffer
		{
			glGenFramebuffers(1, &framebuffers.image_buffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.image_buffer);

			rgb_output = std::make_unique<textures::Texture>();
			normal_output = std::make_unique<textures::Texture>();
			sc_output = std::make_unique<textures::Texture>();

			glBindTexture(GL_TEXTURE_2D, rgb_output->getTextureID());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

			glBindTexture(GL_TEXTURE_2D, normal_output->getTextureID());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);

			glBindTexture(GL_TEXTURE_2D, sc_output->getTextureID());
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);

			glGenSamplers(1, &rgb_sampler);
			glSamplerParameteri(rgb_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(rgb_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(rgb_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(rgb_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glGenRenderbuffers(1, &rgb_depth_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rgb_depth_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rgb_depth_rbo);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rgb_output->getTextureID(), 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normal_output->getTextureID(), 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, sc_output->getTextureID(), 0);

			GLenum draw_buffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, draw_buffers);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "Image Framebuffer failed" << std::endl;
				std::cin.get();
			}
		}
	}

	void CGRenderer::render()
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(parameter.offsetFactor, parameter.offsetUnits);
		shadowmap_pass();
		final_pass();
		switch (post_processing_pass)
		{
		case PostProcessing::Post_Processing:
			canvas_pass();
			break;
		case PostProcessing::Direct_Output:
			break;
		}
	}


	void CGRenderer::shadowmap_pass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.shadowmap_buffer);

		glViewport(0, 0, shadowmap_width, shadowmap_height);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		model = glm::scale(glm::mat4_cast(parameter.globalRotation), glm::vec3(parameter.model_scalation));
		model = glm::rotate(glm::mat4(1.f), glm::radians(parameter.modelRotation), glm::vec3(0.f, 1.f, 0.f)) * model;

		glm::mat4 shadow_view = lightsource_camera.getViewMatrix();

		shader->use();
		glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(lightsource_projection * shadow_view * model));

		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &locs.placementVS);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &locs.depthmapFS);

		glBindVertexArray(basesurface.vao);
		glDrawArrays(GL_TRIANGLES, 0, basesurface.vertsToDraw);

		glBindVertexArray(object.vao);
		glDrawArrays(GL_TRIANGLES, 0, object.vertsToDraw);
	}


	void CGRenderer::final_pass()
	{
		switch (post_processing_pass)
		{
		case cgbv::PostProcessing::Direct_Output:
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.default_buffer);
			break;
		case cgbv::PostProcessing::Post_Processing:
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.image_buffer);
			break;
		}	

		glViewport(0, 0, window_width, window_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		
		switch (viewpoint)
		{
		case ObserverSelection::Viewer:
			view = observer_camera.getViewMatrix();
			break;
		case ObserverSelection::Light:
			view = lightsource_camera.getViewMatrix();
			break;
		}

		// Scaling the object
		model = glm::scale(glm::mat4_cast(parameter.globalRotation), glm::vec3(parameter.model_scalation));
		model = glm::rotate(glm::mat4(1.f), glm::radians(parameter.modelRotation), glm::vec3(0.f, 1.f, 0.f)) * model;

		shader->use();
		//new 
		glUniform4fv(locs.ambientLight, 1, glm::value_ptr(parameter.ambientLight));
		glUniform4fv(locs.diffusLight, 1, glm::value_ptr(parameter.diffusLight));
		glUniform4fv(locs.spekularLight, 1, glm::value_ptr(parameter.specularLight));
		glUniform4fv(locs.ambientMaterial, 1, glm::value_ptr(parameter.ambientMaterial));
		glUniform4fv(locs.diffusMaterial, 1, glm::value_ptr(parameter.diffusMaterial));
		glUniform4fv(locs.spekularMaterial, 1, glm::value_ptr(parameter.spekularMaterial));
		glUniform1f(locs.shininessMaterial, parameter.shininessMaterial);
		glUniform1f(locs.brightnessFactor, parameter.brightnessFactor);

		normal = glm::transpose(glm::inverse(view * model));

		switch (viewpoint)
		{
		case ObserverSelection::Viewer:
			glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(observer_projection * view * model));
			break;
		case ObserverSelection::Light:
			glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(lightsource_projection * view * model));
			break;
		}
		
		glUniformMatrix4fv(locs.modelview, 1, GL_FALSE, glm::value_ptr(view * model));
		glUniformMatrix3fv(locs.normalmatrix, 1, GL_FALSE, glm::value_ptr(normal));

		glm::mat4 shadow_view = lightsource_camera.getViewMatrix();
		glUniformMatrix4fv(locs.biasedModelViewProjection, 1, GL_FALSE, glm::value_ptr(bias * lightsource_projection * shadow_view * model));

		glUniform3fv(locs.lightPos, 1, glm::value_ptr(parameter.lightPos));

		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &locs.lightingVS);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &locs.lightPhong);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmap->getTextureID());
		glBindSampler(0, shadowmap_sampler);
		glUniform1i(locs.shadowmap, 0);

		glBindVertexArray(basesurface.vao);
		glDrawArrays(GL_TRIANGLES, 0, basesurface.vertsToDraw);
		
		glBindVertexArray(object.vao);
		glDrawArrays(GL_TRIANGLES, 0, object.vertsToDraw);

		glColor3f(1.f, 1.f, 0.f);
		glBindVertexArray(boundingBox.vao);
		glDrawArrays(GL_LINES, 0, boundingBox.vertsToDraw);

		//if (screenshot[0])
		//{
		//	std::cout << "Storing Shadowmap to Disk...";
		//	std::unique_ptr<GLubyte[]> shadowmap_pixel = std::make_unique<GLubyte[]>(shadowmap_width * shadowmap_height);
		//	glGetTextureImage(shadowmap->getTextureID(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, shadowmap_width * shadowmap_height, shadowmap_pixel.get());

		//	//cgbv::textures::Texture2DStorage::StoreGreyscale("../shadowmap.png", shadowmap_pixel.get(), shadowmap_width, shadowmap_height, 0);
		//	cgbv::textures::Texture2DStorage::StoreGreyscale(parameter.screenShotNames[0], shadowmap_pixel.get(), shadowmap_width, shadowmap_height, 0);
		//	std::cout << "done" << std::endl;


		//	std::cout << "Storing RGB Image to Disk...";
		//	std::unique_ptr<GLubyte[]> rgb_pixel = std::make_unique<GLubyte[]>(window_width * window_height * 3);
		//	glGetTextureImage(rgb_output->getTextureID(), 0, GL_RGB, GL_UNSIGNED_BYTE, window_width * window_height * 3, rgb_pixel.get());

		//	//cgbv::textures::Texture2DStorage::Store("../rgb.png", rgb_pixel.get(), window_width, window_height, 0);
		//	cgbv::textures::Texture2DStorage::Store(parameter.screenShotNames[1], rgb_pixel.get(), window_width, window_height, 0);
		//	std::cout << "done" << std::endl;


		//	std::cout << "Storing Normal Image to Disk...";
		//	std::unique_ptr<GLubyte[]> normal_pixel = std::make_unique<GLubyte[]>(window_width * window_height * 3);
		//	glGetTextureImage(normal_output->getTextureID(), 0, GL_RGB, GL_UNSIGNED_BYTE, window_width * window_height * 3, normal_pixel.get());

		//	//cgbv::textures::Texture2DStorage::Store("../normal.png", normal_pixel.get(), window_width, window_height, 0);
		//	cgbv::textures::Texture2DStorage::Store(parameter.screenShotNames[2], normal_pixel.get(), window_width, window_height, 0);
		//	std::cout << "done" << std::endl;


		//	std::cout << "Storing Shadow Candidate Image to Disk...";
		//	std::unique_ptr<GLubyte[]> sc_pixel = std::make_unique<GLubyte[]>(window_width * window_height * 3);
		//	glGetTextureImage(sc_output->getTextureID(), 0, GL_RGB, GL_UNSIGNED_BYTE, window_width * window_height * 3, sc_pixel.get());

		//	//cgbv::textures::Texture2DStorage::Store("../shadow_candidate.png", sc_pixel.get(), window_width, window_height, 0);
		//	cgbv::textures::Texture2DStorage::Store(parameter.screenShotNames[3], sc_pixel.get(), window_width, window_height, 0);
		//	std::cout << "done" << std::endl;

		//	screenshot.reset();
		//}

		TwDraw();
	}

	void CGRenderer::canvas_pass()
	{
		glDisable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.default_buffer);

		glViewport(0, 0, window_width, window_height);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		shader->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rgb_output->getTextureID());
		glBindSampler(0, rgb_sampler);
		glUniform1i(locs.rgb_tex, 0); 

		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &locs.canvasPlacementVS);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &locs.canvasDisplayFS);

		glBindVertexArray(canvas.vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, canvas.vertsToDraw);
		glEnable(GL_DEPTH_TEST);
	}


	void CGRenderer::update()
	{
		//returnValues = autopilot.getValues();
		//// Light
		//lightsource_camera.moveTo(returnValues.getLightPos());
		//parameter.lightPos = glm::vec4(returnValues.getLightPos(), 0);

		//// Camera view on the model
		//observer_camera.moveTo(returnValues.getCameraPos());
		//parameter.modelRotation = returnValues.getModelRotation();
		//// Model
		//if (returnValues.getModelID() != modelfbx.modelSelection)
		//{
		//	modelfbx.modelSelection = returnValues.getModelID();
		//	loadFBX(modelfbx.modelSelection);
		//}
		//parameter.screenShotNames = returnValues.getImageNames();
		//screenshot.set();
		//autopilot.step();

		lightsource_camera.moveTo(parameter.lightPos);
	}

	void CGRenderer::loadFBX(int currentMod)
	{
		cgbv::fbxmodel::FBXModel fbx(modelfbx.modelPaths[currentMod]);

		glGenVertexArrays(1, &object.vao);
		glBindVertexArray(object.vao);

		for (cgbv::fbxmodel::Mesh mesh : fbx.Meshes())
		{
			glGenBuffers(1, &object.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
			glBufferData(GL_ARRAY_BUFFER, (3 * mesh.VertexCount() + 3 * mesh.NormalCount()) * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);

			// Buffer Vetex Data
			std::vector<float> vertexData = mesh.VertexData();
			glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * mesh.VertexCount() * sizeof(GLfloat), vertexData.data());
			modelfbx.boundingBoxVals = CGRenderer::findMinMaxXYZ(vertexData);
			// Buffer Normal Data
			glBufferSubData(GL_ARRAY_BUFFER, mesh.VertexCount() * 3 * sizeof(GLfloat), 3 * mesh.NormalCount() * sizeof(GLfloat), mesh.NormalData().data());

			/* Enable attribute index 0 as being used */
			glEnableVertexAttribArray(locs.vertex);
			/* Specify that our vertaex data is going into attribute index 0, and contains three floats per vertex */
			glVertexAttribPointer(locs.vertex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			/* Enable attribute index 1 as being used */
			glEnableVertexAttribArray(locs.normal);
			/* Specify that our coordinate data is going into attribute index 1, and contains three floats per vertex */
			glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 0, (const void*)(3 * mesh.VertexCount() * sizeof(GLfloat)));

			object.vertsToDraw = mesh.VertexCount();
		}
	}

	boundingBoxValues CGRenderer::findMinMaxXYZ(std::vector<float> vertices)
	{
		boundingBoxValues results;
		std::vector<float> x, y, z;

		for (int i = 0; i < vertices.size(); i++)
		{
			switch (i % 3)
			{
			case 0:
				x.push_back(vertices.at(i));
				break;
			case 1:
				y.push_back(vertices.at(i));
				break;
			case 2:
				z.push_back(vertices.at(i));
				break;
			}
		}
		results.z_max = (*std::max_element(z.begin(), z.end()));
		results.z_min =(*std::min_element(z.begin(), z.end()));
		results.y_max =(*std::max_element(y.begin(), y.end()));
		results.y_min =(*std::min_element(y.begin(), y.end()));
		results.x_max =(*std::max_element(x.begin(), x.end()));
		results.x_min =(*std::min_element(x.begin(), x.end()));

		return results ;
	}
}
