#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Program
{
public:

	void Run()
	{
		InitializeWindow();
		InitializeVulkan();
		MainLoop();
		Cleanup();
	}

private:
	GLFWwindow* window;
	const uint32_t width = 800;
	const uint32_t height = 600;

	void InitializeWindow()
	{
		//initialze glfw
		glfwInit();

		//tell it to intialize without an OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//disables window resizing
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


		//create a window and store in a pointer
		window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	}
	void InitializeVulkan()
	{

	}
	void MainLoop()
	{
		//checks whether the window should be closing
		while(!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}
	void Cleanup()
	{
		//clears memory associated with the window
		glfwDestroyWindow(window);
		//terminates glfw
		glfwTerminate();
	}
};

int main()
{
	std::cout << "Vulkan Time!" << std::endl;

	Program triangleProgram;

	try
	{
		triangleProgram.Run();
	}
	catch (const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}