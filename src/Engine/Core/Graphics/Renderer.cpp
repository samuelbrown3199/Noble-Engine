#include "Renderer.h"

#include "../Logger.h"
#include "../../Useful.h"

SDL_Window* Renderer::m_gameWindow;
SDL_GLContext Renderer::m_glContext;

int Renderer::m_iScreenWidth = 500;
int Renderer::m_iScreenHeight = 500;
float Renderer::m_fNearPlane = 0.1f;
float Renderer::m_fFarPlane = 1000.0f;
float Renderer::m_fScale = 20;

glm::vec3 Renderer::m_clearColour;

float Renderer::m_fFov = 90.0f;
const float Renderer::m_fMaxScale = 1000;
const float Renderer::m_fMinScale = 3;

Camera* Renderer::m_camera = nullptr;
GLuint Renderer::m_renderMode = GL_TRIANGLES;

//---------- public functions ---------

Renderer::Renderer(const std::string _windowName)
{
	Logger::LogInformation("Creating engine renderer.");

	m_gameWindow = SDL_CreateWindow(_windowName.c_str(), 15, 75, m_iScreenWidth, m_iScreenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!m_gameWindow)
	{
		Logger::LogInformation("Failed to create game window");
		std::cout << "Failed to create game window " << SDL_GetError() << std::endl;
		throw std::exception();
	}
	Logger::LogInformation("Created game window.");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	m_glContext = SDL_GL_CreateContext(m_gameWindow);
	if (!m_glContext)
	{
		Logger::LogInformation("Failed to create OpenGL context!");
		std::cout << "Failed to create OpenGL context!" << std::endl;
		throw std::exception();
	}
	Logger::LogInformation("Created OpenGL context.");

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Logger::LogInformation("Application failed to initialize GLEW!");
		std::cout << "Application failed to initialize GLEW! " << glewGetString(err) << std::endl;
		throw std::exception();
	}
	Logger::LogInformation("Initialized GLEW.");
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	Logger::LogInformation("Setup default settings for OpenGL.");
}

Renderer::~Renderer()
{
	SDL_DestroyWindow(m_gameWindow);
}

void Renderer::AdjustScale(const float& _amount)
{ 
	m_fScale += _amount;
	if (m_fScale > m_fMaxScale)
		m_fScale = m_fMaxScale;
	else if (m_fScale < m_fMinScale)
		m_fScale = m_fMinScale;
}

void Renderer::ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::SwapGraphicsBuffer()
{
	SDL_GL_SwapWindow(m_gameWindow);
}
void Renderer::UpdateScreenSize()
{
	int newWidth, newHeight;
	SDL_GetWindowSize(m_gameWindow, &newWidth, &newHeight);
	if (newWidth != m_iScreenWidth || newHeight != m_iScreenHeight)
	{
		m_iScreenWidth = newWidth;
		m_iScreenHeight = newHeight;
		glViewport(0, 0, m_iScreenWidth, m_iScreenHeight);
		Logger::LogInformation(FormatString("Resized game window to %dx%d.", m_iScreenWidth, m_iScreenHeight));
	}
}

void Renderer::UpdateScreenSize(const int& _height, const int& _width)
{
	int newWidth = _width, newHeight = _height;
	if (newWidth != m_iScreenWidth || newHeight != m_iScreenHeight)
	{
		m_iScreenWidth = newWidth;
		m_iScreenHeight = newHeight;
		glViewport(0, 0, m_iScreenWidth, m_iScreenHeight);
		SDL_SetWindowSize(m_gameWindow, m_iScreenWidth, m_iScreenHeight);
		Logger::LogInformation(FormatString("Resized game window to %dx%d.", m_iScreenWidth, m_iScreenHeight));
	}
}

void Renderer::SetWindowFullScreen(const int& _mode)
{
	Logger::LogInformation(FormatString("Changing window fullscreen mode to %d", _mode));
	switch (_mode)
	{
	case 0:
		SDL_SetWindowBordered(m_gameWindow, SDL_TRUE);
		SDL_SetWindowFullscreen(m_gameWindow, 0);
		break;
	case 2:
		SDL_SetWindowBordered(m_gameWindow, SDL_FALSE);
	case 1:
		SDL_SetWindowFullscreen(m_gameWindow, SDL_WINDOW_FULLSCREEN);
		break;
	}

	Renderer::UpdateScreenSize();
}

std::string Renderer::GetWindowTitle()
{
	std::string name(SDL_GetWindowTitle(m_gameWindow));
	return name;
}

// 0 No VSync, 1 VSync, 2 Adaptive VSync
void Renderer::SetVSyncMode(const int& _mode)
{
	Logger::LogInformation(FormatString("Changing VSync mode to %d", _mode));
	SDL_GL_SetSwapInterval(_mode);
}

glm::mat4 Renderer::GenerateProjMatrix()
{
	if (m_camera == nullptr)
		return glm::mat4(1.0f);

	if (m_camera->m_viewMode == projection)
		return GenerateProjectionMatrix();

	return GenerateOrthographicMatrix();
}

glm::mat4 Renderer::GenerateProjectionMatrix()
{
	glm::mat4 projMatrix = glm::perspective(glm::radians(m_fFov), (float)m_iScreenWidth / (float)m_iScreenHeight, m_fNearPlane, m_fFarPlane);
	return projMatrix;
}

glm::mat4 Renderer::GenerateOrthographicMatrix()
{
	glm::mat4 orthoMatrix = glm::ortho(0.0f, (float)m_iScreenWidth / m_fScale, (float)m_iScreenHeight / m_fScale, 0.0f, 0.0f, m_fFarPlane);
	return orthoMatrix;
}

glm::mat4 Renderer::GenerateUIOrthographicMatrix()
{
	glm::mat4 orthoMatrix = glm::ortho(0.0f, (float)m_iScreenWidth, (float)m_iScreenHeight, 0.0f, 0.0f, m_fFarPlane);
	return orthoMatrix;
}

glm::mat4 Renderer::GenerateViewMatrix()
{
	if (m_camera == nullptr)
		return glm::mat4(1.0f);

	glm::mat4 viewMatrix = glm::lookAt(m_camera->m_camTransform->m_position, m_camera->m_camTransform->m_position + m_camera->m_camTransform->m_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	return viewMatrix;
}

void Renderer::SetRenderMode(GLenum renderMode)
{
	if (renderMode != GL_TRIANGLES && renderMode != GL_LINES)
		return;

	m_renderMode = renderMode;
}

void Renderer::SetClearColour(glm::vec3 colour)
{
	m_clearColour = colour;
	glClearColor(colour.x, colour.y, colour.z, 1.0f);
}

void Renderer::SetCullFace(bool value)
{
	if (value)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}