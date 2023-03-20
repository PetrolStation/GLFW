#include <PCH.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utility>
#include "Static/Window/Window.h"
#include "GLFWWindow.h"
//#include <Core/Renderer/Renderer.h>
// TODO: Remove unnecessary type size conversions
#include <Static/Renderer/Renderer.h>

namespace PetrolEngine {

    UnorderedMap<Keys, int> keyToGlfwKey = {
            { Keys::KeySpace, GLFW_KEY_SPACE },
            { Keys::KeyQuote, GLFW_KEY_APOSTROPHE },
            { Keys::KeyComma, GLFW_KEY_COMMA },
            { Keys::KeyMinus, GLFW_KEY_MINUS },
            { Keys::KeyPeriod, GLFW_KEY_PERIOD },
            { Keys::KeySlash, GLFW_KEY_SLASH },
            { Keys::Key0, GLFW_KEY_0 },
            { Keys::Key1, GLFW_KEY_1 },
            { Keys::Key2, GLFW_KEY_2 },
            { Keys::Key3, GLFW_KEY_3 },
            { Keys::Key4, GLFW_KEY_4 },
            { Keys::Key5, GLFW_KEY_5 },
            { Keys::Key6, GLFW_KEY_6 },
            { Keys::Key7, GLFW_KEY_7 },
            { Keys::Key8, GLFW_KEY_8 },
            { Keys::Key9, GLFW_KEY_9 },
            { Keys::KeySemicolon, GLFW_KEY_SEMICOLON },
            { Keys::KeyEquals, GLFW_KEY_EQUAL },
            { Keys::KeyA, GLFW_KEY_A },
            { Keys::KeyB, GLFW_KEY_B },
            { Keys::KeyC, GLFW_KEY_C },
            { Keys::KeyD, GLFW_KEY_D },
            { Keys::KeyE, GLFW_KEY_E },
            { Keys::KeyF, GLFW_KEY_F },
            { Keys::KeyG, GLFW_KEY_G },
            { Keys::KeyH, GLFW_KEY_H },
            { Keys::KeyI, GLFW_KEY_I },
            { Keys::KeyJ, GLFW_KEY_J },
            { Keys::KeyK, GLFW_KEY_K },
            { Keys::KeyL, GLFW_KEY_L },
            { Keys::KeyM, GLFW_KEY_M },
            { Keys::KeyN, GLFW_KEY_N },
            { Keys::KeyO, GLFW_KEY_O },
            { Keys::KeyP, GLFW_KEY_P },
            { Keys::KeyQ, GLFW_KEY_Q },
            { Keys::KeyR, GLFW_KEY_R },
            { Keys::KeyS, GLFW_KEY_S },
            { Keys::KeyT, GLFW_KEY_T },
            { Keys::KeyU, GLFW_KEY_U },
            { Keys::KeyV, GLFW_KEY_V },
            { Keys::KeyW, GLFW_KEY_W },
            { Keys::KeyX, GLFW_KEY_X },
            { Keys::KeyEscape, GLFW_KEY_ESCAPE},
            { Keys::KeyLeftShift, GLFW_KEY_LEFT_SHIFT}
    };
    UnorderedMap<int, Keys> glfwKeyToKey = reverseMap(keyToGlfwKey);

    GLFWWindow::GLFWWindow(int width, int height, const String& title) { LOG_FUNCTION();
        this->windowData = { width, height, title };
        this->window     = nullptr;
    }

    void GLFWWindow::showCursor(bool enabled) { LOG_FUNCTION();
        glfwSetInputMode(window, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    void GLFWWindow::setVSync(bool enabled) { LOG_FUNCTION();
        glfwSwapInterval(enabled);
    }

    void GLFWWindow::setIcon(Image* image) { LOG_FUNCTION();
        GLFWimage icons[1];

        icons[0].pixels = image->getData  ();
        icons[0].height = image->getHeight();
        icons[0].width  = image->getWidth ();

        this->icon = image;

        glfwSetWindowIcon(window, 1, icons);
    }

    GLFWWindow::~GLFWWindow(){
        delete this->icon;
    }

    void GLFWWindow::createWindowSurface(void* i, const void* a, void* s){
//        glfwCreateWindowSurface(
//            *reinterpret_cast<VkInstance*>(i),
//            window,
//            reinterpret_cast<const VkAllocationCallbacks*>(a),
//            reinterpret_cast<      VkSurfaceKHR*         >(s)
//        );
    }
    void error_callback(int error, const char* msg) {
        LOG(" [" + std::to_string(error) + "] " + msg + '\n',3);
    }
    int GLFWWindow::init() { LOG_FUNCTION();
        int success = glfwInit();

        if (!success)
            return 0;

        glfwSetErrorCallback(error_callback);

        window = glfwCreateWindow(windowData.width, windowData.height, windowData.title.c_str(), nullptr, nullptr);

        if (!window) {
            LOG("Window failed to create" , 3);
            glfwTerminate();
            return 0;
        }

        glfwMakeContextCurrent(window);
        Renderer::createGraphicsContext()->init((void*)glfwGetProcAddress);

        glfwSetWindowUserPointer(window, &windowData);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* windowPtr, int newWidth, int newHeight) {
                auto* newWindowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(windowPtr));

                newWindowData->width  = newWidth ;
                newWindowData->height = newHeight;

                EventStack::addEvent( new WindowResizedEvent(*newWindowData) );
            }
        );

        glfwSetWindowCloseCallback(window, [](GLFWwindow* windowPtr) {
                WindowData& newWindowData = *(WindowData*)glfwGetWindowUserPointer(windowPtr);

                EventStack::addEvent(new WindowClosedEvent(newWindowData));
            }
        );

        glfwSetKeyCallback(window, [](GLFWwindow* windowPtr, int key, int scancode, int action, int mods) {
                switch (action) {
                    case GLFW_PRESS  : EventStack::addEvent(new KeyPressedEvent (glfwKeyToKey[key], false)); break;
                    case GLFW_REPEAT : EventStack::addEvent(new KeyPressedEvent (glfwKeyToKey[key], true )); break;
                    case GLFW_RELEASE: EventStack::addEvent(new KeyReleasedEvent(glfwKeyToKey[key]       )); break;
                    default          :                                                         break;
                }
            }
        );

        glfwSetCharCallback(window, [](GLFWwindow* windowPtr, unsigned int keycode) {
                EventStack::addEvent( new KeyTypedEvent( glfwKeyToKey[keycode] ) );
            }
        );

        // TODO: distinguish press and release events
        glfwSetMouseButtonCallback(window, [](GLFWwindow* windowPtr, int button, int action, int mods) {
            Keys key = glfwKeyToKey[button];

            switch (action) {
                case GLFW_PRESS  :
                case GLFW_RELEASE: EventStack::addEvent( new MouseClickEvent(key) ); break;
                default          :                                                   break;
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* windowPtr, double xOffset, double yOffset) {
                EventStack::addEvent( new MouseScrolledEvent(xOffset, yOffset) );
            }
        );

        glfwSetCursorPosCallback(window, [](GLFWwindow* windowPtr, double x, double y) {
                cursorXPos = x;
                cursorYPos = y;

                EventStack::addEvent( new MouseMovedEvent(x, y) );
            }
        );

        glfwMakeContextCurrent(window);

        glfwSetTime( 0.0 );

        return 0;
    }
}