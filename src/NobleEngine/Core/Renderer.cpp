#include "Renderer.h"

namespace NobleCore
{
	SDL_GLContext Renderer::glContext;

	GraphicsAPI Renderer::currentAPI;
	GraphicsOptions Renderer::graphicsOptions;

	void Renderer::InitializeOpenGL()
	{
		Screen::window = SDL_CreateWindow(Screen::GetWindowName().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Screen::GetScreenWidth(), Screen::GetScreenHeight(),
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		if (!Screen::window)
		{
			std::cout << "Failed to create window " << SDL_GetError() << std::endl;
			throw std::exception();
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		glContext = SDL_GL_CreateContext(Screen::GetWindow());
		if (!glContext)
		{
			std::cout << "Failed to create OpenGL context!" << std::endl;
			throw std::exception();
		}

		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			std::cout << "Application failed to initialize glew! " << glewGetString(err) << std::endl;
			throw std::exception();
		}

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.0f, 0.45f, 0.45f, 1.0f);
	}

	void Renderer::InitializeVulkan()
	{
		Screen::window = SDL_CreateWindow(Screen::GetWindowName().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Screen::GetScreenWidth(), Screen::GetScreenHeight(),
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
		if (!Screen::window)
		{
			std::cout << "Failed to create window " << SDL_GetError() << std::endl;
			throw std::exception();
		}

		std::cout << "Vulkan is not yet implemented!!" << std::endl;
		throw std::exception();
	}

	void Renderer::ClearBuffer()
	{
		switch (currentAPI)
		{
		case OpenGL:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;
		case Vulkan:
			break;
		}
	}
	void Renderer::SwapGraphicsBuffer()
	{
		switch (currentAPI)
		{
		case OpenGL:
			SDL_GL_SwapWindow(Screen::GetWindow());
			break;
		case Vulkan:
			break;
		}
	}

	GraphicsOptions Renderer::LoadGraphicsOptions(std::string fileDirectory)
	{
		GraphicsOptions loadedOptions;
		return loadedOptions;
	}

	Renderer::Renderer(GraphicsAPI _graphicsAPI)
	{
		currentAPI = _graphicsAPI;
		switch (currentAPI)
		{
		case OpenGL:
			InitializeOpenGL();
			break;
		case Vulkan:
			InitializeVulkan();
			break;
		default:
			std::cout << "Invalid Graphics API!" << std::endl;
			throw std::exception();
		}

		graphicsOptions = LoadGraphicsOptions(""); // temp
		UpdateGraphicsOptions();
	}

	void Renderer::UpdateGraphicsOptions()
	{
		SDL_GL_SetSwapInterval(graphicsOptions.vsync);
	}
}