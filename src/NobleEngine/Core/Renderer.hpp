#ifndef RENDERER_H_
#define RENDERER_H_

#include <GL/glew.h>

#include <iostream>

#include "Screen.h"

namespace NobleCore
{
	/**
	*Determines which API options there are.
	*/
	enum GraphicsAPI
	{
		OpenGL,
		Vulkan
	};

	class Renderer
	{

	private:
		/**
		* If using OpenGL, this stores the GL context of the program.
		*/
		SDL_GLContext glContext;
		/**
		* Initializes OpenGL.
		*/
		void InitializeOpenGL()
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
		}
		/**
		* Initializes Vulkan. Not yet implemented.
		*/
		void InitializeVulkan()
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

	public:
		/**
		* Stores the current API that the engine is using.
		*/
		GraphicsAPI currentAPI;
		/**
		* Initializes the graphics API.
		*/
		Renderer(GraphicsAPI _graphicsAPI)
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
		}
	};
}

#endif