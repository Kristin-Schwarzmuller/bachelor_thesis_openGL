#include <cg/CGRenderer.h>

#include <iostream>
#include <AntTweakBar.h>

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

		glDeleteFramebuffers(1, &framebuffers.shadowmap_buffer);
		glDeleteSamplers(1, &shadowmap_sampler);
	}


	void CGRenderer::resize(int width, int height)
	{
		window_width = width;
		window_height = height;

		glViewport(0, 0, width, height);

		observer_projection = glm::perspective(float(M_PI) / 5.f, float(window_width) / float(window_height), .1f, 200.f);

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

			case GLFW_KEY_B:
				model = glm::rotate(glm::mat4(1.f), glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f)) * model;
				break;
			case GLFW_KEY_N:
				model *= glm::rotate(glm::mat4(1.f), -(float)M_PI / 6.f, glm::vec3(1.f, 0.f, 0.f));
				break;
			case GLFW_KEY_M:
				model *= glm::rotate(glm::mat4(1.f), (float)M_PI / 6.f, glm::vec3(0.f, 1.f, 0.f));
				break;
			case GLFW_KEY_RIGHT:
				model *= glm::rotate(glm::mat4(1.f), -(float)M_PI / 6.f, glm::vec3(0.f, 1.f, 0.f));
				break;
			default:
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
		// Disable vertical sync (faster image generation) 
		glfwSwapInterval(0);

		// GL States
		glClearColor(0.4f, 0.4f, 0.4f, 1.f);

		glEnable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);


		// Matrices 
		{
			//observer_projection = glm::perspective(float(M_PI) / 5.f, float(window_width) / float(window_height), .1f, 200.f);
			observer_projection = glm::perspective(glm::pi<float>() / 5.f, float(window_width) / float(window_height), .1f, 20.f);
			observer_camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
			observer_camera.moveTo(0.f, 2.5f, parameter.distanceCamera); // here changes happend 5.f --> 10.f

			lightsource_projection = glm::ortho(-8.f, 8.f, -8.f, 8.f, 1.f, 80.f);
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
			locs.lambertFS = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "lambert");
			locs.depthmapFS = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "depthmap");
			locs.lightingVS = shader->getSubroutineIndex(GL_VERTEX_SHADER, "verts_and_normals");
			locs.placementVS = shader->getSubroutineIndex(GL_VERTEX_SHADER, "simple_placement");
			// new 
			locs.lightPhong = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "phongWithLambert");
			locs.ambientLight = shader->getUniformLocation("light.ambient");
			locs.ambientMaterial = shader->getUniformLocation("material.ambient");
			locs.diffusLight = shader->getUniformLocation("light.diffus");
			locs.diffusMaterial = shader->getUniformLocation("material.diffus");
			//locs.emissivMaterial = shader->getUniformLocation("material.emissiv");
			locs.spekularLight = shader->getUniformLocation("light.specular");
			locs.spekularMaterial = shader->getUniformLocation("material.spekular");
			locs.shininessMaterial = shader->getUniformLocation("material.shininess");
			locs.brightnessFactor = shader->getUniformLocation("light.brightnessFactor");
		}




		// Geometrie
		{
			// Base
			std::vector<glm::vec3> vertices;

			//glm::vec3 a(-750.f, 0.f, -750.f);
			//glm::vec3 b(750.f, 0.f, -750.f);
			//glm::vec3 c(750.f, 0.f, 750.f);
			//glm::vec3 d(-750.f, 0.f, 750.f);
			glm::vec3 a(-50.f, 0.f, -50.f);
			glm::vec3 b(50.f, 0.f, -50.f);
			glm::vec3 c(50.f, 0.f, 50.f);
			glm::vec3 d(-50.f, 0.f, 50.f);
			/*glm::vec3 a(-5000.f, 0.f, -5000.f);
			glm::vec3 b(5000.f, 0.f, -5000.f);
			glm::vec3 c(5000.f, 0.f, 5000.f);
			glm::vec3 d(-5000.f, 0.f, 5000.f);*/

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
			// ============= wall for testing 
			//glm::vec3 e(-10.f, 0.f, 10.f);
			//glm::vec3 f(-10.f, 0.f, -10.f);
			//glm::vec3 g(-10.f, 10.f, 10.f);
			//glm::vec3 h(-10.f, 10.f, -10.f);
			///*glm::vec3 a(-5000.f, 0.f, -5000.f);
			//glm::vec3 b(5000.f, 0.f, -5000.f);
			//glm::vec3 c(5000.f, 0.f, 5000.f);
			//glm::vec3 d(-5000.f, 0.f, 5000.f);*/

			//vertices.push_back(h); vertices.push_back(g); vertices.push_back(e);
			//vertices.push_back(e); vertices.push_back(g); vertices.push_back(f);

			//for (auto v : vertices)
			//{
			//	data.insert(std::end(data), glm::value_ptr(v), glm::value_ptr(v) + sizeof(glm::vec3) / sizeof(float));
			//	data.insert(std::end(data), glm::value_ptr(n), glm::value_ptr(n) + sizeof(glm::vec3) / sizeof(float));
			//	basesurface.vertsToDraw++;
			//}

			//  ========== wall for testing end 

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


		}
		loadFBX(modelfbx.modelSelection);


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
			//TwAddVarRW(tweakbar, "Emissives Material", TW_TYPE_COLOR4F, &parameter.emissivMaterial, " group=Material alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "Shininess", TW_TYPE_FLOAT, &parameter.shininessMaterial, " group=Material");
			TwAddVarRW(tweakbar, "Ambient Material", TW_TYPE_COLOR4F, &parameter.ambientMaterial, " group=Material alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "diffuse Material", TW_TYPE_COLOR4F, &parameter.diffusMaterial, " group=Material alpha help='Color and transparency of the cube.' ");
			TwAddVarRW(tweakbar, "Specular  Material", TW_TYPE_COLOR4F, &parameter.spekularMaterial, " group=Material alpha help='Color and transparency of the cube.' ");

			// ====== Shadow ======
			TwAddVarRW(tweakbar, "Shadow Offset Factor", TW_TYPE_FLOAT, &parameter.offsetFactor, " group = 'Shadow' min = 0.0f max = 128.0f step = 0.1f");
			TwAddVarRW(tweakbar, "Shadowmap Offset Units", TW_TYPE_FLOAT, &parameter.offsetUnits, " group = 'Shadow' min = 0.0f max = 128.0f step = 0.1f");
			// ====== Modelauswahl ======
			// Create an internal enum to name the meshes
			//typedef enum { BUDDHA, BUNNY, BOX, CONE, CYLINDER, BALL, DONUT } MESH_TYPE;
			std::string meshtypes = "BUDDHA, BUNNY, BOX, CONE, CYLINDER, BALL, DONUT";
			TwType meshType = TwDefineEnumFromString("vertexType", meshtypes.c_str());

			// A variable for the current selection - will be updated by ATB
			//MESH_TYPE m_currentMesh = BUDDHA;

			// Array of drop down items
			//TwEnumVal Meshes[] = { {BUDDHA, "Buddha"}, {BUNNY, "Bunny"}, {BOX, "Box"},  };

			// ATB identifier for the array
			//TwType MeshTwType = TwDefineEnum("MeshType", Meshes, 3);

			// Link it to the tweak bar
			//TwAddVarRW(tweakbar, "Mesh", meshType, &cgbv::ModelFBX::modelSelection, NULL);
			modelSelectionPackage.object1 = &modelfbx;
			modelSelectionPackage.object2 = this;
			TwAddVarCB(tweakbar, "Model", meshType, modelSetCallback, modelGetCallback, &modelSelectionPackage, "");// min = 0 max = 6");
		}





		// Framebuffer
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
				std::cout << "Shadowmap Framebuffer failed" << std::endl;
		}



		return true;
	}


	void CGRenderer::render()
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(parameter.offsetFactor, parameter.offsetUnits);
		shadowmap_pass();
		final_pass();
	}

	// ERstellt die Tiefenkarte / Tiefentextur 
	void CGRenderer::shadowmap_pass()
	{
		glViewport(0, 0, shadowmap_width, shadowmap_height);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.shadowmap_buffer);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// model = glm::mat4_cast(parameter.globalRotation);
		model = glm::scale(glm::mat4_cast(parameter.globalRotation), glm::vec3(.35f));
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

		//std::cout << "Test" << std::endl;
	}

	void CGRenderer::final_pass()
	{
		glViewport(0, 0, window_width, window_height);


		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.default_buffer);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 view = observer_camera.getViewMatrix();
		//glm::mat4 view = lightsource_camera.getViewMatrix();

		//model = glm::mat4_cast(parameter.globalRotation);
		//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

		// Scaling the object
		model = glm::scale(glm::mat4_cast(parameter.globalRotation), glm::vec3(.35f));
		model = glm::rotate(glm::mat4(1.f), glm::radians(parameter.modelRotation), glm::vec3(0.f, 1.f, 0.f)) * model;

		shader->use();
		//new 
		glUniform4fv(locs.ambientLight, 1, glm::value_ptr(parameter.ambientLight));
		glUniform4fv(locs.diffusLight, 1, glm::value_ptr(parameter.diffusLight));
		glUniform4fv(locs.spekularLight, 1, glm::value_ptr(parameter.specularLight));
		glUniform4fv(locs.ambientMaterial, 1, glm::value_ptr(parameter.ambientMaterial));
		glUniform4fv(locs.diffusMaterial, 1, glm::value_ptr(parameter.diffusMaterial));
		glUniform4fv(locs.spekularMaterial, 1, glm::value_ptr(parameter.spekularMaterial));
		//glUniform4fv(locs.emissivMaterial, 1, glm::value_ptr(parameter.emissivMaterial));
		glUniform1f(locs.shininessMaterial, parameter.shininessMaterial);
		glUniform1f(locs.brightnessFactor, parameter.brightnessFactor);

		normal = glm::transpose(glm::inverse(view * model));
		glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(observer_projection * view * model));
		//glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(lightsource_projection * view * model));
		glUniformMatrix4fv(locs.modelview, 1, GL_FALSE, glm::value_ptr(view * model));
		glUniformMatrix3fv(locs.normalmatrix, 1, GL_FALSE, glm::value_ptr(normal));

		glm::mat4 shadow_view = lightsource_camera.getViewMatrix();
		glUniformMatrix4fv(locs.biasedModelViewProjection, 1, GL_FALSE, glm::value_ptr(bias * lightsource_projection * shadow_view * model));

		glUniform3fv(locs.lightPos, 1, glm::value_ptr(parameter.lightPos));

		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &locs.lightingVS);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &locs.lightPhong);//&locs.lambertFS);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmap->getTextureID());
		glBindSampler(0, shadowmap_sampler);
		glUniform1i(locs.shadowmap, 0);

		glBindVertexArray(basesurface.vao);
		glDrawArrays(GL_TRIANGLES, 0, basesurface.vertsToDraw);

		glBindVertexArray(object.vao);
		glDrawArrays(GL_TRIANGLES, 0, object.vertsToDraw);

		//if (screenshot[0])
		//{
		//	std::cout << "Storing Shadowmap to Disk...";
		//	std::unique_ptr<GLubyte[]> pixel = std::make_unique<GLubyte[]>(shadowmap_width * shadowmap_height);
		//	//glGetTextureImage(shadowmap->getTextureID(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, shadowmap_width * shadowmap_height, pixel.get());
		//	glGetTexImage(shadowmap->getTextureID(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pixel.get());

		//	cgbv::textures::Texture2DStorage::StoreGreyscale(parameter.imageName, pixel.get(), shadowmap_width, shadowmap_height, 0);
		//	std::cout << "done" << std::endl;
		//	screenshot.reset();
		//}

		TwDraw();
	}


	void CGRenderer::update()
	{
		returnValues = autopilot.getValues();
		// Light
		lightsource_camera.moveTo(returnValues.getLightPos());
		parameter.lightPos = glm::vec4(returnValues.getLightPos(),0); 
		// Camera view on the model
		observer_camera.moveTo(returnValues.getCameraPos());
		parameter.modelRotation = returnValues.getModelRotation();
		// Model
		if (returnValues.getModelID() != modelfbx.modelSelection)
		{
			modelfbx.modelSelection = returnValues.getModelID();
			loadFBX(modelfbx.modelSelection);
		}
		parameter.screenShotName = returnValues.getImageName();
		//screenshot.set();
		autopilot.step();
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
			glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * mesh.VertexCount() * sizeof(GLfloat), mesh.VertexData().data());
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
}
