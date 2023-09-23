#pragma once
#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <exception>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/hash.hpp>

#include "..\..\Components\Camera.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
	}
};

struct Renderable;

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

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

	static glm::vec3 m_clearColour;
	static glm::vec3 m_ambientColour;
	static float m_ambientStrength;

	static Camera* m_camera;
	static const float m_fMaxScale, m_fMinScale;

	static glm::mat4 GenerateProjectionMatrix();
	static glm::mat4 GenerateOrthographicMatrix();

	static GLenum m_renderMode;

	static std::vector<Renderable*> m_onScreenObjects;

public:

	Renderer(const std::string _windowName);
	~Renderer();

	void StartFrameRender();
	void SetShaderInformation();

	void EndFrameRender();
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

	static void SetCamera(Camera* cam) { m_camera = cam; }
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

	static void SetClearColour(glm::vec3 colour);
	static glm::vec3 GetClearColour() { return m_clearColour; }

	static void SetAmbientColour(glm::vec3 colour, float strength);
	static glm::vec3 GetAmbientColour() { return m_ambientColour; }
	static float GetAmbientStrength() { return m_ambientStrength; }

	static void SetCullFace(bool value);

	static void AddOnScreenObject(Renderable* comp);
};

#endif