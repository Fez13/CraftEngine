#pragma once

#include <GLFW/glfw3.h>

#include "../utils/utils.hpp"
#include "../vendor/glm/glm.hpp"
#include "../input/keyCodes.h"

namespace craft{

    class input{
    public:

        input(const input&) = delete;

        static input &get();

        bool getKeyDowm(const KeyCode &key) const;

        void setMainWindow(GLFWwindow* glfwWindow);

        void poolInputs();

        bool getKeyDownOnce(KeyCode key);

        bool getMouseButton(MouseCode button) const;

        bool getMouseButtonOnce(MouseCode button);

        glm::dvec2 GetMousePosition() const;

        const char* getClipBoard();

        double GetMouseScroll() const;

        void SetScroll(double scroll);

    private:

        GLFWwindow *m_window;

        input() = default;

        static input s_input;

        double m_scroll;

        std::vector<KeyCode> m_pressedInFrame;
    };
}
