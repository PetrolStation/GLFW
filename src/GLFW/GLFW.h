#pragma once

#include "Core/Window/WindowResourceCreator.h"
#include "GLFWWindow.h"

namespace PetrolEngine {
    class GLFW_: public WindowResourceCreator {
    public:
        WindowApi* newWindow(int width, int height, const String& title) override { return new GLFWWindow(width, height, title); }
    };

    extern GLFW_ GLFW;
}
