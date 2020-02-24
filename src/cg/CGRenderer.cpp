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
        glDeleteVertexArrays(1, &basesurface.vao);
        glDeleteBuffers(1, &basesurface.vbo);

        glDeleteVertexArrays(1, &object.vao);
        glDeleteBuffers(1, &object.vbo);
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
        TwEventCharGLFW(key, action);
        TwEventKeyGLFW(key, action);

        if(action == GLFW_PRESS)
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



        observer_projection = glm::perspective(float(M_PI) / 5.f, float(window_width) / float(window_height), .1f, 200.f);
		observer_camera.setTarget(glm::vec3(0.f, 0.f, 0.f));
		observer_camera.moveTo(0.f, 2.5f, 5.f);


        // Shader
        shader = std::make_unique<cgbv::shader::GLSLShaderprogram>("../shader/VertexShader.glsl", "../shader/FragmentShader.glsl");
        locs.vertex = shader->getAttribLocation("vertex");
        locs.normal = shader->getAttribLocation("normal");
        locs.modelViewProjection = shader->getUniformLocation("matrices.mvp");
        locs.normalmatrix = shader->getUniformLocation("matrices.normal");
        locs.modelview = shader->getUniformLocation("matrices.mv");
        locs.lightPos = shader->getUniformLocation("light.lightPos");
        locs.subFragment = shader->getSubroutineIndex(GL_FRAGMENT_SHADER, "lambert");
        locs.subVertex = shader->getSubroutineIndex(GL_VERTEX_SHADER, "verts_and_normals");




        // Geometrie
        std::vector<glm::vec3> vertices;

        glm::vec3 a(-5.f, 0.f, -5.f);
        glm::vec3 b(5.f, 0.f, -5.f);
        glm::vec3 c(5.f, 0.f, 5.f);
        glm::vec3 d(-5.f, 0.f, 5.f);

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
        glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*) size_t(3 * sizeof(float)));


        data.clear();
        vertices.clear();

        a = glm::vec3(-1.f, 0.f, 0.f);
        b = glm::vec3(1.f, 0.f, 0.f);
        c = glm::vec3(0.f, 2.f, 0.f);

        n = glm::vec3(0.f, 0.f, 1.f);

        vertices.push_back(a); vertices.push_back(b); vertices.push_back(c);

        for (auto v : vertices)
        {
            data.insert(std::end(data), glm::value_ptr(v), glm::value_ptr(v) + sizeof(glm::vec3) / sizeof(float));
            data.insert(std::end(data), glm::value_ptr(n), glm::value_ptr(n) + sizeof(glm::vec3) / sizeof(float));
            object.vertsToDraw++;
        }

        glGenVertexArrays(1, &object.vao);
        glBindVertexArray(object.vao);

        glGenBuffers(1, &object.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(locs.vertex);
        glVertexAttribPointer(locs.vertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(locs.normal);
        glVertexAttribPointer(locs.normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)size_t(3 * sizeof(float)));


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
        glm::mat4 view = observer_camera.getViewMatrix();

        model = glm::mat4_cast(parameter.globalRotation);

        shader->use();
        normal = glm::transpose(glm::inverse(view * model));
        glUniformMatrix4fv(locs.modelViewProjection, 1, GL_FALSE, glm::value_ptr(observer_projection * view * model));
        glUniformMatrix4fv(locs.modelview, 1, GL_FALSE, glm::value_ptr(view * model));
        glUniformMatrix3fv(locs.normalmatrix, 1, GL_FALSE, glm::value_ptr(normal));


        glUniform3fv(locs.lightPos, 1, glm::value_ptr(parameter.lightPos));


        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &locs.subVertex);
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &locs.subFragment);

        glBindVertexArray(basesurface.vao);
        glDrawArrays(GL_TRIANGLES, 0, basesurface.vertsToDraw);

        glBindVertexArray(object.vao);
        glDrawArrays(GL_TRIANGLES, 0, object.vertsToDraw);

        TwDraw();
    }


    void CGRenderer::update()
    {

    }
}