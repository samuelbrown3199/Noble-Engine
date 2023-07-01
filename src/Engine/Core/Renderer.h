#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <exception>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct Camera
{
	glm::vec3 m_position = glm::vec3(0,0,5);
	glm::vec3 m_rotation = glm::vec3(0, 0, -1);
};

class Renderer
{
private:
	static SDL_Window* m_gameWindow;
	static SDL_GLContext m_glContext;

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";

	static int m_iScreenWidth, m_iScreenHeight;
	static float m_fNearPlane, m_fFarPlane;
	static float m_fScale;

	static Camera* m_camera;

	static float m_fFov;
	static const float m_fMaxScale, m_fMinScale;

	static bool m_bProjectionRendering;

	static glm::mat4 GenerateProjectionMatrix();
	static glm::mat4 GenerateOrthographicMatrix();

	static GLenum m_renderMode;

public:

	Renderer(const std::string _windowName);
	~Renderer();

	void ClearBuffer();
	void SwapGraphicsBuffer();
	static void UpdateScreenSize();
	static void UpdateScreenSize(const int& _height, const int& _width);
	static SDL_Window* GetWindow() { return m_gameWindow; }
	static SDL_GLContext* GetGLContext() { return &m_glContext; }
	const char* GetGLSLVersion() { return glsl_version; }

	// 0 Windowed, 1 Fullscreen, 2 Borderless Windowed
	static void SetWindowFullScreen(const int& _mode);
	static void SetVSyncMode(const int& _mode);

	//Adds the amount onto scale. The larger the scale the larger the world.
	static void AdjustScale(const float& _amount);

	static Camera* GetCamera() { return m_camera; };
	static float GetScale() { return m_fScale; };
	static glm::vec2 GetScreenSize() { return glm::vec2(m_iScreenWidth, m_iScreenHeight); };

	static glm::mat4 GenerateProjMatrix();
	static glm::mat4 GenerateUIOrthographicMatrix();
	static glm::mat4 GenerateViewMatrix();

	static std::string GetWindowTitle();
	static void UpdateWindowTitle(const std::string& _windowTitle) { SDL_SetWindowTitle(m_gameWindow, _windowTitle.c_str()); };

	static void SetRenderMode(GLenum renderMode);
	static GLenum GetRenderMode() { return m_renderMode; }

	static void SetFov(float value) { m_fFov = value; }
	static float GetFov() { return m_fFov; }

	static void SetClearColour(glm::vec3 colour);
	static void SetCullFace(bool value);
};

#endif