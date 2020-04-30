#include <iostream>
#include <string>
#include <memory>
#include <cg/CGRenderer.h>
#include <AntTweakBar.h>



void resize(GLFWwindow *window, int width, int height);
void destroy(GLFWwindow *window);
void input(GLFWwindow *window, int key, int scancode, int action, int modifiers);
void errorHandler(int error, const char* description);
void cursorPos(GLFWwindow *window, double x, double y);
void mouseButton(GLFWwindow *window, int button, int action, int mods);
//void inputChar(GLFWwindow* window, unsigned int keycode);

void update();
void render();


std::unique_ptr<cgbv::Renderer> renderer;

int main()
{
    if(!glfwInit())
    {
        std::cout << "GLFW failed!" << std::endl;
        return -1;
    }

    glfwSetErrorCallback(errorHandler);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Computergrafik", nullptr, nullptr);
    if(!window)
    {
        std::cout << "Failed to open window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    renderer = std::make_unique<cgbv::CGRenderer>(window);
    if(!renderer->setup())
    {
        return -1;
    }


    glfwSetWindowCloseCallback(window, destroy);
    glfwSetKeyCallback(window, input);
    glfwSetFramebufferSizeCallback(window, resize);

	glfwSetCursorPosCallback(window, cursorPos);
	glfwSetMouseButtonCallback(window, mouseButton);
    //glfwSetCharCallback(window, inputChar);
    //glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);

            // OLD SHIT THAT IS TOTALLY OUT DATED 
            //--- Callbacks
            //--- https://github.com/davidcox/AntTweakBar/blob/master/examples/TwSimpleGLFW.c

            //// Set GLFW event callbacks
            //// - Redirect window size changes to the callback function WindowSizeCB
            ////glfwSetWindowSizeCallback(resize(w,b));
            //// - Directly redirect GLFW mouse button events to AntTweakBar
            //glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
            //// - Directly redirect GLFW mouse position events to AntTweakBar
            //glfwSetCursorPos((GLFWmouseposfun)TwEventMousePosGLFW);
            //// - Directly redirect GLFW mouse wheel events to AntTweakBar
            //glfwSetScrollCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
            //// - Directly redirect GLFW key events to AntTweakBar
            //glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
            //// - Directly redirect GLFW char events to AntTweakBar
            //glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);

            // THE NEW STUFF	
            // https://www.glfw.org/docs/3.3/quick.html



    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSwapInterval(0);

    do
    {
        render();
        update();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while(!glfwWindowShouldClose(window));

    glfwDestroyWindow(window);
    window = nullptr;


    TwTerminate();
    glfwTerminate();

    return 0;
}


void resize(GLFWwindow *window, int width, int height)
{
    renderer->resize(width, height);
}


void destroy(GLFWwindow *window)
{
    renderer->destroy();
}


void input(GLFWwindow *window, int key, int scancode, int action, int modifiers)
{
    renderer->input(key, scancode, action, modifiers);
}


void errorHandler(int error, const char* description)
{
    std::string msg(description);
    std::cout << msg << std::endl;
}


void render()
{
    renderer->render();
}


void update()
{
    renderer->update();
}


void cursorPos(GLFWwindow *window, double x, double y)
{
	TwEventMousePosGLFW(static_cast<int>(x), static_cast<int>(y));
}


void mouseButton(GLFWwindow *window, int button, int action, int mods)
{
	TwEventMouseButtonGLFW(button, action);
}

//void inputChar(GLFWwindow* window, unsigned int keycode)
//{
//    TwEventCharGLFW()
//}