#include <cg/CGRenderer.h>

#include <iostream>
#include <AntTweakBar.h>



namespace cgbv
{
    CGRenderer::CGRenderer(GLFWwindow *window) : Renderer(window)
    {

    }


    CGRenderer::~CGRenderer()
    {

    }


    void CGRenderer::destroy()
    {
        glDeleteVertexArrays(1, &cone.vao);
        glDeleteBuffers(1, &cone.vbo);
    }


    void CGRenderer::resize(int width, int height)
    {
        window_width = width;
        window_height = height;

        glViewport(0, 0, width, height);

        projection = glm::perspective(float(M_PI) / 5.f, float(window_width) / float(window_height), .1f, 200.f);

        TwWindowSize(width, height > 0 ? height : 1);
    }


    void CGRenderer::input(int key, int scancode, int action, int modifiers)
    {
        TwEventCharGLFW(key, action);
        TwEventKeyGLFW(key, action);

        if(action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_W:
                    camera.moveForward(.1f);
                    break;
                case GLFW_KEY_S:
                    camera.moveForward(-.1f);
                    break;
                case GLFW_KEY_A:
                    camera.moveRight(.1f);
                    break;
                case GLFW_KEY_D:
                    camera.moveRight(-.1f);
                    break;
                default:
                    break;
            }
        }
    }


    bool CGRenderer::setup()
    {
        glfwGetFramebufferSize(window, &window_width, &window_height);


        if(!gladLoadGL())
            return false;


        // GL States
        glClearColor(0.4f, 0.4f, 0.4f, 1.f);

        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);



        projection = glm::perspective(float(M_PI) / 5.f, float(window_width) / float(window_height), .1f, 200.f);
		camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
		camera.moveTo(0.f, -.5f, 5.f);


        // Shader
        shader = std::make_unique<cgbv::shader::GLSLShaderprogram>("../shader/cg/VertexShader.glsl", "../shader/cg/FragmentShader.glsl");
        locs.vertex = shader->getAttribLocation("vertex");
        locs.normal = shader->getAttribLocation("normal");
        locs.modelViewProjection = shader->getUniformLocation("matrices.mvp");
        locs.normalmatrix = shader->getUniformLocation("matrices.normal");
        locs.modelview = shader->getUniformLocation("matrices.mv");
        locs.lightPos = shader->getUniformLocation("light.lightPos");
        locs.subFragment = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "toon");
        locs.subVertex = shader->getSubroutineIndex(GL_VERTEX_SHADER, "verts_and_normals");




        // Geometrie
        std::vector<glm::vec3> basevertices;
        std::vector<glm::vec3> basenormals;

        float step = 2.f * float(M_PI) / 32.f;
        for(float f = 0; f <= 2.f * float(M_PI) + step; f += step)
        {
            float x = std::sin(f);
            float z = std::cos(f);
            basevertices.push_back(glm::vec3(x, 0.f, z));
            basenormals.push_back(glm::vec3(x, std::sin(float(M_PI) / 12.6), z));
        }

        std::vector<float> data;
        glm::vec3 top(0.f, 2.f, 0.f);
        for(unsigned int i = 0; i < basevertices.size(); ++i)
        {
            int next = (i + 1 == basevertices.size()) ? 0 : i + 1;

            glm::vec3 topnormal = glm::normalize(basenormals[i] + basenormals[next]);

            data.insert(std::end(data), glm::value_ptr(basevertices[i]), glm::value_ptr(basevertices[i]) + sizeof(glm::vec3) / sizeof(float));
            data.insert(std::end(data), glm::value_ptr(basenormals[i]), glm::value_ptr(basenormals[i]) + sizeof(glm::vec3) / sizeof(float));
            cone.vertsToDraw++;

            data.insert(std::end(data), glm::value_ptr(top), glm::value_ptr(top) + sizeof(glm::vec3) / sizeof(float));
            data.insert(std::end(data), glm::value_ptr(topnormal), glm::value_ptr(topnormal) + sizeof(glm::vec3) / sizeof(float));
            cone.vertsToDraw++;

            data.insert(std::end(data), glm::value_ptr(basevertices[next]), glm::value_ptr(basevertices[next]) + sizeof(glm::vec3) / sizeof(float));
            data.insert(std::end(data), glm::value_ptr(basenormals[next]), glm::value_ptr(basenormals[next]) + sizeof(glm::vec3) / sizeof(float));
            cone.vertsToDraw++;
        }

        glGenVertexArrays(1, &cone.vao);
        glBindVertexArray(cone.vao);

        glGenBuffers(1, &cone.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, cone.vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(locs.vertex);
        glVertexAttribPointer(locs.vertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(locs.normal);
        glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*) size_t(3 * sizeof(float)));



        // GUI
        TwInit(TW_OPENGL_CORE, nullptr);
        TwWindowSize(1280, 720);
        TwBar *tweakbar = TwNewBar("TweakBar");
        TwDefine(" TweakBar size='300 400'");

        TwAddVarRW(tweakbar, "Global Rotation", TW_TYPE_QUAT4F, &parameter.globalRotation, "showval=false opened=true");
        TwAddVarRW(tweakbar, "Lichtrichtung", TW_TYPE_DIR3F, &parameter.lightPos, "group=Light axisx=-x axisy=-y axisz=-z opened=true");

        return true;
    }


    void CGRenderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
        //camera.moveTo(0.f, -.5f, 5.f);
        glm::mat4 view = camera.getViewMatrix();


        model = glm::mat4_cast(parameter.globalRotation);


        shader->use();
        normal = glm::transpose(glm::inverse(view * model));
        glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(projection * view * model));
        glUniformMatrix4fv(locs.modelview, 1, GL_FALSE, glm::value_ptr(view * model));
        glUniformMatrix3fv(locs.normalmatrix, 1, GL_FALSE, glm::value_ptr(normal));


        glUniform3fv(locs.lightPos, 1, glm::value_ptr(parameter.lightPos));


        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &locs.subVertex);
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &locs.subFragment);

        glBindVertexArray(cone.vao);
        glDrawArrays(GL_TRIANGLES, 0, cone.vertsToDraw);


        TwDraw();
    }


    void CGRenderer::update()
    {

    }
}