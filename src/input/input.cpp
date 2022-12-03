#include "input.h"

namespace craft{

    input input::s_input;

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        input::get().SetScroll(yoffset);
    }

    input &input::get() {
        return s_input;
    }

    void input::setMainWindow(GLFWwindow *glfwWindow) {
        m_window = glfwWindow;
        glfwSetScrollCallback(m_window, scroll_callback);
    }

    bool input::getKeyDowm(const KeyCode &key) const{
        return glfwGetKey(m_window, key);;
    }

    bool input::getKeyDownOnce(const KeyCode key) {
        int state = glfwGetKey(m_window, key);

        if (state == GLFW_PRESS && !VectorContains<KeyCode>(key, m_pressedInFrame))
        {
            m_pressedInFrame.push_back(key);
            return true;
        }
        if (state != GLFW_PRESS)
            RemoveFromVector<uint16_t>(key, m_pressedInFrame);
        return false;
    }

    void input::poolInputs() {
        m_scroll = 0;
        glfwPollEvents();
    }

    bool input::getMouseButton(MouseCode button) const{
        return glfwGetMouseButton(m_window, button);
    }

    bool input::getMouseButtonOnce(MouseCode button) {
        int state = glfwGetMouseButton(m_window, button);

        if (state == GLFW_PRESS && !VectorContains<MouseCode>(button, m_pressedInFrame))
        {
            m_pressedInFrame.push_back(button);
            return true;
        }
        if (state != GLFW_PRESS)
            RemoveFromVector<MouseCode>(button, m_pressedInFrame);
        return false;
    }

    glm::dvec2 input::GetMousePosition() const{
        double mousePosition[2];
        glfwGetCursorPos(m_window, &mousePosition[0], &mousePosition[1]);

        return {float(mousePosition[0]), float(mousePosition[1])};
    }

    double input::GetMouseScroll() const {
        return m_scroll;
    }

    void input::SetScroll(double scroll) {
        m_scroll = scroll;
    }

    const char *input::getClipBoard() {
        return glfwGetClipboardString(nullptr);
    }


}
