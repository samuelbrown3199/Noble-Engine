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
	/**
	* Stores information about graphics options.
	*/
	struct GraphicsOptions
	{
		/**
		* Stores how vsync should be handled.
		* 
		* OpenGL values: -1 Adaptive Vsync, 0 No Vsync, 1 Vsync On.
		*/
		int vsync = 1;
	};

	class Renderer
	{
		friend class Application;
	private:
		/**
		* If using OpenGL, this stores the GL context of the program.
		*/
		static SDL_GLContext glContext;
		/**
		* Initializes OpenGL.
		*/
		void InitializeOpenGL();
		/**
		* Initializes Vulkan. Not yet implemented.
		*/
		void InitializeVulkan();

		static void ClearBuffer();
		static void SwapGraphicsBuffer();
		static GraphicsOptions LoadGraphicsOptions(std::string fileDirectory);
	public:
		/**
		* Stores the current API that the engine is using.
		*/
		static GraphicsAPI currentAPI;
		/**
		* Stores the applications graphics options.
		*/
		static GraphicsOptions graphicsOptions;
		/**
		* Initializes the graphics API.
		*/
		Renderer(GraphicsAPI _graphicsAPI);
		/**
		* Updates the engines graphics settings to the ones currently stored in the graphicsOptions.
		*/
		static void UpdateGraphicsOptions();
	};
}

#endif