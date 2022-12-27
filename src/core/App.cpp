#include "App.h"

namespace craft
{

	bool IsGood(VkPhysicalDeviceProperties &pt, VkPhysicalDeviceFeatures &ft)
	{
		if (pt.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return false;

		if(ft.samplerAnisotropy != VK_TRUE)
			return false;

		if (!ft.geometryShader)
			return false;
		
		return true;
	}

	void mouse_callback(GLFWwindow *window, double xpos, double ypos)
	{
	}

	void App::setMainLoopFramerate(float newFrameRate)
	{
		m_refreshRate = newFrameRate;
		m_mainLoopFrameTime.frameObjective = newFrameRate;
	}

	void App::defaultInputs()
	{

		// Window
		if (input::get().getKeyDownOnce(Key::F11))
		{
			static bool state = false;
			static GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			static const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			if (state)
				glfwSetWindowMonitor(m_window.mainWindow, monitor, 0, 0, mode->width, mode->height, m_refreshRate);
			else
				glfwSetWindowMonitor(m_window.mainWindow, nullptr, 0, 0, m_window.getWindowSizeDefault().x, m_window.getWindowSizeDefault().y, m_refreshRate);
			state = !state;
		}

		if (input::get().getMouseButtonOnce(Mouse::ButtonMiddle))
		{
			static bool state = false;
			if (state)
				glfwSetInputMode(m_window.mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(m_window.mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			state = !state;
		}

		if (input::get().getKeyDownOnce(Key::F10))
		{
			setMainLoopFramerate(m_refreshRate + 10);
			LOG("Refresh rate updated: " + std::to_string(m_refreshRate), 1, 1)
		}

		if (input::get().getKeyDownOnce(Key::F9))
			if (m_refreshRate >= 11)
			{
				setMainLoopFramerate(m_refreshRate - 10);
				LOG("Refresh rate updated: " + std::to_string(m_refreshRate), 1, 1)
			}
			else
			{
				LOG("The refresh rate can't be equal or lower than 0", 1, 0)
			}
	}

	App::App(const char *appName, uint32_t appVersion, uint32_t apiVersion,
			 const std::vector<std::string> &layers, const std::vector<std::string> &extensions)
	{

		vk_instance::get().initialize(appName, appVersion, apiVersion, layers, extensions);

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = vk_window({1200, 675}, 60);
		input::get().setMainWindow(m_window.mainWindow);

		glfwSetCursorPosCallback(m_window.mainWindow, mouse_callback);

		m_window.createSurface();

		vk_graphic_device::get().initialize({IsGood});

		m_window.createSwapChainProperties(vk_graphic_device::get().getPhysicalDevice());

		vk_renderer::get().temporal = vk_graphic_device::get().getPhysicalDevice();

		float priority = 1.0f;
		vk_graphic_device::get().createDeviceAbstraction(m_window.findQueueFamily(vk_graphic_device::get().getPhysicalDevice()), "QUEUE_KHR", priority, {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

		std::string vert = "../src/shaders/compiled/vert.spv";

		std::string frag = "../src/shaders/compiled/frag.spv";
		compile("../src/shaders/verts/basic_vert.vert", EMPTY, vert, true);
		compile("../src/shaders/frags/basic_frag.frag", EMPTY, frag, true);

		vk_renderer::get().initialize(&vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR"));
		vk_renderer::get().loadShaders(vert.c_str(), frag.c_str());
		vk_renderer::get().setMainWindow(&m_window);
		vk_renderer::get().setDynamicStates({VK_DYNAMIC_STATE_VIEWPORT,
											 VK_DYNAMIC_STATE_SCISSOR});

		Camera::initMainBuffer(vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR"));
		vk_renderer::get().setCameraBuffer(Camera::s_cameraBuffer);
		vk_renderer::get().createShaderPipeline();
		m_mainLoopFrameTime = frameRate();

		Texture::createDefaultTexture("QUEUE_KHR");

	}

	int App::mainLoop()
	{
		LOG("Main loop has started", 0, 1)

		/*

		//Extensions
		LOG("Available Extensions",0,1)
		auto extensions = m_instance.getAvailableExtensions();
		for(auto o : extensions)
			std::cout<<'\t'<<o.extensionName<<'\n';

		//Layers
		LOG("Available layers\n",0,1)
		auto layers = m_instance.getAvailableLayers();
		for(auto o : layers)
			std::cout<<'\t'<<o.layerName<<'\n';

		//Device Extensions

		LOG("Device Extensions\n",0,1)
		auto deviceExtensions = m_gpu.getDeviceExtensions();
		for(VkExtensionProperties pt : deviceExtensions)
			std::cout<<"\t"<<pt.extensionName<<" Version: " <<pt.specVersion<<'\n';
		m_gpu.getDeviceExtensions();
		*/
		/*
	
		std::vector<vertex> vertices{
		 	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f,1}, {1.0f, 0.0f}},
    		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f,1}, {0.0f, 0.0f}},
		    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f,1}, {0.0f, 1.0f}},
    		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f,1}, {1.0f, 1.0f}},
		};

		std::vector<uint32_t> indices{
			0, 1, 2, 2, 3, 0,
		};
		*/

		std::vector<vertex> vertices{
			{{-1, -1, 1}, {1.0f, 1.0f, 1.0f, 1},{1,0}},  // 0
			{{1, -1, 1}, {1.0f, 1.0f, 1.0f, 1},{0,0}},	  // 1
			{{-1, 1, 1}, {1.0f, 1.0f, 1.0f, 1},{0,1}},	  // 2
			{{1, 1, 1}, {1.0f, 1.0f, 1.0f, 1},{1,1}},	  // 3
			{{-1, -1, -1}, {1.0f, 1.0f, 1.0f, 1},{1,0}}, // 4
			{{1, -1, -1}, {1.0f, 1.0f, 1.0f, 1},{0,0}},  // 5
			{{-1, 1, -1}, {1.0f, 1.0f, 1.0f, 1},{0,1}},  // 6
			{{1, 1, -1}, {1.0f, 1.0f, 1.0f, 1},{1,1}}	  // 7

		};

		std::vector<uint32_t> indices{
			// Top
			2, 6, 7,
			2, 3, 7,

			// Bottom
			0, 4, 5,
			0, 1, 5,

			// Left
			0, 2, 6,
			0, 4, 6,

			// Right
			1, 3, 7,
			1, 5, 7,

			// Front
			0, 2, 3,
			0, 1, 3,

			// Back
			4, 6, 7,
			4, 5, 7};

		Geometry geo(vertices, indices);

		Texture txt("../src/textures/statue-gc949741ae_1920.jpg", "QUEUE_KHR");
		Texture txt_2("../src/textures/dragon-g3dcd44d14_1920.jpg", "QUEUE_KHR");

		Mesh m("QUEUE_KHR", geo);
		
		Entity cube;
		attachExtencion<Transform>(cube);
		attachExtencion<Drawable3D>(cube,&m,nullptr);

		Entity cube2;
		attachExtencion<Transform>(cube2)->position.x = 10;
		attachExtencion<Drawable3D>(cube2,&m,nullptr);

		float aspectRatio = (float)m_window.getWindowSize().x / (float)m_window.getWindowSize().y;

		Entity camera;
		Transform *cameraTransform = attachExtencion<Transform>(camera);
		Update *cameraUpdateComponent = attachExtencion<Update>(camera);
		Camera *cameraComponent = attachExtencion<Camera>(camera, aspectRatio);
		CameraController *cameraController = attachExtencion<CameraController>(camera);

		cameraController->m_window = m_window.mainWindow;
		cameraController->m_window = m_window.mainWindow;
		cameraController->m_aspectRation = aspectRatio;
		cameraController->m_camera_sens = 0.35f;
		cameraController->m_camera_speed = 2.0f;

		cameraUpdateComponent->function = cameraUpdate;

		while (!glfwWindowShouldClose(m_window.mainWindow))
		{
			m_mainLoopFrameTime.wait();

			if (input::get().getKeyDownOnce(Key::F))
				std::cout << m_mainLoopFrameTime.getElapsedTime() << '\n';

			// 0.016f

			for (uint32_t i = 0; i < Update::s_instancies.size(); i++)
				Update::s_instancies[i]->function(Update::s_instancies[i]->attachment);

			for (uint32_t i = 0; i < Drawable3D::s_instancies.size(); i++)
				Drawable3D::s_instancies[i]->draw();

			m_window.update(vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR").device);
			vk_renderer::get().updateFrame();
			input::get().poolInputs();
			defaultInputs();
		}
		vk_renderer::get().waitToFinish();

		
		txt.free();
		txt_2.free();

		LOG("Main loop has ended", 0, 1)
		return 0;
	}

	void App::clean()
	{
		Texture::freeDefault();
		Camera::freeMainBuffer();
		m_window.free(vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR").device);
		vk_renderer::get().free();
		vk_graphic_device::get().free();
		glfwTerminate();
	}
}