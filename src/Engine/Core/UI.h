#pragma once
#ifndef UI_H_
#define UI_H_

#include <iostream>
#include <string>

#include <GL/glew.h>
#include<glm/glm.hpp>

#include "ResourceManager.h"
#include "InputManager.h"
#include "Renderer.h"

#include "../Resource/UILayout.h"
#include "../Resource/ShaderProgram.hpp"
#include "../Resource/Texture.h"
#include "../Resource/Font.h"

struct UIQuads
{
	static unsigned int m_iQuadVAO, m_iTextVAO, m_iTextVBO;

	static void SetupQuad()
	{
		const GLfloat positions[] =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f
		};

		const GLfloat uvs[] =
		{
			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,
			1.0, 1.0,
			1.0, 0.0,
			0.0, 0.0
		};

		GLuint positionsVBO = 0;
		glGenBuffers(1, &positionsVBO);
		if (!positionsVBO)
		{
			throw std::exception();
		}
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint uvVBO = 0;
		glGenBuffers(1, &uvVBO);
		if (!uvVBO)
		{
			throw std::exception();
		}
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &m_iQuadVAO);
		if (!m_iQuadVAO)
		{
			throw std::exception();
		}
		glBindVertexArray(m_iQuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}

	static void SetupTextQuad()
	{
		glGenVertexArrays(1, &m_iTextVAO);
		glGenBuffers(1, &m_iTextVBO);
		glBindVertexArray(m_iTextVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_iTextVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	static void SetupUIQuads()
	{
		SetupQuad();
		SetupTextQuad();
	}
};

/**
*Stores the screen position and scale for UI elements on the screen.
*/
struct UIRect
{
	/**
	*Stores the parent UI rect.
	*/
	std::shared_ptr<UIRect> m_parentRect;
	/**
	*Stores the layer of the UI rect.
	*/
	unsigned int m_iLayer = 0;
	glm::vec4 m_rect;

	UIRect();
	UIRect(unsigned int _layer, glm::vec4 _rect);

	/**
	*Returns the screen rect matrix.
	*/
	glm::mat4 GetUIMatrix();
	/**
	*Used to determine if the mouse is in the screen rect.
	*/
	bool IsMouseInRect();
	void SetLayer(unsigned int _newLayer) { m_iLayer = _newLayer; }
	void SetParent(std::shared_ptr<UIRect> _newParent) { m_parentRect = _newParent; }
	void SetRect(const glm::vec4 _newRect) { m_rect = _newRect; }
};
/**
*UI element types inherit from this base class.
*/
struct UIElement
{
	std::string m_sID;
	std::shared_ptr<UIRect> m_elementRect;

	UIElement(const std::string& _ID)
	{
		m_sID = _ID;
	}

	virtual void OnUpdate() {};
	virtual void OnRender() {};
};

struct TexturedUIElement : public UIElement
{
	TexturedUIElement(const std::string& _ID) : UIElement(_ID) {}
	std::shared_ptr<Texture> m_baseTexture, m_hoverTexture, m_activeTexture;
	virtual void TextureSwap();
	virtual void ToggleTextureSwap(const bool _toggleValue);

	void SetBaseTexture(std::string _baseTexLoc);
	void SetHoverTexture(std::string _hoverTexLoc);
	void SetActiveTexture(std::string _activeTexLoc);
};


/**
*Stores the relevant information for a UI box.
*/
struct UIBox : public TexturedUIElement
{
	/**
	*Initializes a UI box with a screen rect.
	*/
	UIBox(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect);
	/**
	*Renders a UI box onto the screen.
	*/
	void OnRender();
};
/**
*Stores the relevant information for a UI button. When clicked, it can call a function.
*/
struct UIButton : public TexturedUIElement
{
private:

	bool m_bPressed = false;

public:
	/**
	*Initializes a button with a screen position but no textures.
	*/
	UIButton(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect);

	/**
	*Determines whether the button is currently clicked on this frame.
	*/
	void OnUpdate();
	/**
	*Renders a UI button onto the screen.
	*/
	void OnRender();
	/**
	*Returns whether the button is currently clicked on.
	*/
	bool ClickedOn();
};
/**
*Renders text onto the screen as a UI element.
*/
struct UILabel : public UIElement
{
	std::shared_ptr<Font> m_labelFont;
	std::string m_sText;
	glm::vec3 m_colour = glm::vec3(1.0f, 0.0f, 0.0f);
	float m_fScale = 1.0f;

	UILabel(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect, float _labelScale, std::shared_ptr<Font> _font);
	/**
	*Renders the text onto the screen.
	*/
	void OnRender();
};
/**
*Stores the relevant information for a toggle tick box.
*/
struct UIToggle : public TexturedUIElement
{
	bool m_bToggleValue = false;

	UIToggle(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect);

	void OnUpdate();
	void OnRender();
};
/**
*Stores the relevant information for a slider.
*/
struct UISlider : public TexturedUIElement
{
private:

	std::shared_ptr<Texture> m_handleTexture;
	std::shared_ptr<UIRect> m_handleRect;

	bool m_bCurrentlyDragged = false;

public:

	float m_fMaxValue = 0, m_fCurrentValue = 0;
	float m_fCurrentPercentage = 0;

	UISlider(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect);

	void OnUpdate();
	void OnRender();
};

struct UIEditBox : public TexturedUIElement
{
private:

	std::shared_ptr<UILabel> m_editLabel;

public:
	bool m_bCurrentlyEditing = false;

	std::string m_sEditText;
	int m_iMaxCharacters;

	UIEditBox(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect, std::shared_ptr<Font> _font);

	void OnUpdate();
	void OnRender();
};

/**
*Stores a list of UI elements as keeps them together as a group.
*/
struct UIWindow : public TexturedUIElement
{

private:

	int m_iMaxLayers = 0;
	bool m_bCurrentlyDragged = false;
	std::vector<std::shared_ptr<UIElement>> m_vUIElements;

public:

	/**
	*Determines whether the window can be moved around the screen.
	*/
	bool m_bDraggable = false;

	/**
	*Initializes the UI window with parameters.
	*/
	UIWindow(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect);

	template<typename T>
	/**
	*Adds a UI element to the window without any parameters
	*/
	std::shared_ptr<T> AddUIElement()
	{
		std::shared_ptr<T> element = std::make_shared<T>();
		element->elementRect->parentRect = m_elementRect;
		m_vUIElements.push_back(element);

		return element;
	}
	template<typename T, typename ... Args>
	/**
	*Adds a UI element to the window with parameters
	*/
	std::shared_ptr<T> AddUIElement(Args&&... _args)
	{
		std::shared_ptr<T> element = std::make_shared<T>(std::forward<Args>(_args)...);
		element->elementRect->parentRect = m_elementRect;
		m_vUIElements.push_back(element);

		if (element->elementRect->layer > m_iMaxLayers)
		{
			m_iMaxLayers = element->elementRect->layer;
		}

		return element;
	}

	/**
	*Updates the UI window group
	*/
	void OnUpdate();
	/**
	*Renders the UI window group
	*/
	void OnRender();
};


/**
*User written user interfaces can inherit from this class.
*/
struct UISystem
{
private:

	std::shared_ptr<UILayout> m_sUILayoutFile;
	/**
	*Determines the max amount of layers in the UI system.
	*/
	int m_iMaxLayers = 0;

	template<typename T>
	void SetUITextures(std::shared_ptr<T> _UIElement, std::vector<std::string> _info)
	{
		if (_info.size() >= 5)
			_UIElement->SetBaseTexture(_info.at(4));
		if (_info.size() >= 6)
			_UIElement->SetHoverTexture(_info.at(5));
		if (_info.size() >= 7)
			_UIElement->SetActiveTexture(_info.at(6));
	}

	template<typename T>
	std::shared_ptr<T> InitializeUIElement(std::vector<std::string> _info)
	{
		std::vector<std::string> rectString = SplitString(_info.at(3), ',');
		glm::vec4 newElementRect = glm::vec4(std::stof(rectString.at(0)), std::stof(rectString.at(1)), std::stof(rectString.at(2)), std::stof(rectString.at(3)));

		std::shared_ptr<T> newElement = AddUIElement<T>(std::stoi(_info.at(1)), _info.at(2), newElementRect);
		SetUITextures(newElement, _info);
			
		return newElement;
	}

public:

	UISystem(const std::string& _UILayoutFile)
	{
		m_sUILayoutFile = ResourceManager::LoadResource<UILayout>(_UILayoutFile);
	}
	/**
	*Determines whether the UI system is active. When false, updates, event handling and rendering is disabled.
	*/
	bool m_bActive = true;
	/**
	*Stores all the UI system elements.
	*/
	std::vector<std::shared_ptr<UIElement>> m_vUIElements;

	template<typename T>
	/**
	*Adds a UI element to the system without any parameters
	*/
	std::shared_ptr<T> AddUIElement()
	{
		std::shared_ptr<T> element = std::make_shared<T>();
		m_vUIElements.push_back(element);

		return element;
	}

	template<typename T, typename ... Args>
	/**
	*Adds a UI element to the system with parameters
	*/
	std::shared_ptr<T> AddUIElement(Args&&... _args)
	{
		std::shared_ptr<T> element = std::make_shared<T>(std::forward<Args>(_args)...);
		m_vUIElements.push_back(element);

		if (element->m_elementRect->m_iLayer > m_iMaxLayers)
		{
			m_iMaxLayers = element->m_elementRect->m_iLayer;
		}

		return element;
	}

	template<typename T>
	std::shared_ptr<T> GetElementByID(const std::string& _ID)
	{
		for (int i = 0; i < m_vUIElements.size(); i++)
		{
			if (m_vUIElements.at(i)->m_sID == _ID)
			{
				return std::dynamic_pointer_cast<T>(m_vUIElements.at(i));
			}
		}
	}

	void InitializeUI();
	virtual void InitializeSpecificElements() {};
	/**
	*This can be used to handle what happens when UI elements are interacted with.
	*/
	virtual void HandleEvents() {};

	/**
	*Updates all the UI elements.
	*/
	void Update();
	/**
	*Renders all the UI elements.
	*/
	void Render();
};

#endif