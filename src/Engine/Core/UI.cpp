#include "UI.h"

#include "Application.h"
#include "../Systems/SpriteSystem.h"
#include "../Useful.h"

unsigned int UIQuads::m_iQuadVAO;
unsigned int UIQuads::m_iTextVAO;
unsigned int UIQuads::m_iTextVBO;

UIRect::UIRect()
{
	m_rect = glm::vec4(0, 0, 0, 0);
}

UIRect::UIRect(unsigned int _layer, glm::vec4 _rect)
{
	m_iLayer = _layer;
	m_rect = _rect;
}

glm::mat4 UIRect::GetUIMatrix()
{
	glm::mat4 uiMat(1.0f);

	glm::vec2 uiPosition = glm::vec2(m_rect.x, m_rect.y);
	if (m_parentRect)
	{
		glm::vec2 parentPos = glm::vec2(m_parentRect->m_rect.x, m_parentRect->m_rect.y);
		uiPosition = parentPos + uiPosition;
	}

	uiMat = glm::translate(uiMat, glm::vec3(uiPosition, 0.0f));
	uiMat = glm::scale(uiMat, glm::vec3(m_rect.z, m_rect.w, 1.0f));

	return uiMat;
}

bool UIRect::IsMouseInRect()
{
	int xMousePos = InputManager::m_iMouseX;
	int yMousePos = InputManager::m_iMouseY;

	glm::vec2 uiPosition = glm::vec2(m_rect.x, m_rect.y);
	if (m_parentRect)
	{
		glm::vec2 parentPos = glm::vec2(m_parentRect->m_rect.x, m_parentRect->m_rect.y);
		uiPosition = parentPos + uiPosition;
	}

	if (xMousePos >= uiPosition.x && xMousePos <= uiPosition.x + m_rect.z)
	{
		if (yMousePos >= uiPosition.y && yMousePos <= uiPosition.y + m_rect.w)
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------//

void TexturedUIElement::TextureSwap()
{
	Application::m_uiShaderProgram->UseProgram();
	Application::m_uiShaderProgram->BindMat4("u_UIPos", m_elementRect->GetUIMatrix());
	Application::m_uiShaderProgram->BindMat4("u_Ortho", Renderer::GenerateOrthographicMatrix());

	glActiveTexture(GL_TEXTURE0);
	if (m_baseTexture)
	{
		glBindTexture(GL_TEXTURE_2D, m_baseTexture->m_iTextureID);
	}
	if (m_hoverTexture)
	{
		if (m_elementRect->IsMouseInRect())
		{
			glBindTexture(GL_TEXTURE_2D, m_hoverTexture->m_iTextureID);
			if (InputManager::GetMouseButton(0))
			{
				if(m_activeTexture)
					glBindTexture(GL_TEXTURE_2D, m_activeTexture->m_iTextureID);
			}
		}
	}
}

void TexturedUIElement::ToggleTextureSwap(const bool _toggleValue)
{
	Application::m_uiShaderProgram->UseProgram();
	Application::m_uiShaderProgram->BindMat4("u_UIPos", m_elementRect->GetUIMatrix());
	Application::m_uiShaderProgram->BindMat4("u_Ortho", Renderer::GenerateUIOrthographicMatrix());

	glActiveTexture(GL_TEXTURE0);
	if (m_baseTexture)
	{
		glBindTexture(GL_TEXTURE_2D, m_baseTexture->m_iTextureID);
	}
	
	if(_toggleValue)
	{
		if (m_activeTexture)
		{
			glBindTexture(GL_TEXTURE_2D, m_activeTexture->m_iTextureID);
		}
	}
}

void TexturedUIElement::SetBaseTexture(std::string _baseTexLoc)
{
	m_baseTexture = ResourceManager::LoadResource<Texture>(_baseTexLoc);
}

void TexturedUIElement::SetHoverTexture(std::string _hoverTexLoc)
{
	m_hoverTexture = ResourceManager::LoadResource<Texture>(_hoverTexLoc);
}

void TexturedUIElement::SetActiveTexture(std::string _activeTexLoc)
{
	m_activeTexture = ResourceManager::LoadResource<Texture>(_activeTexLoc);
}

//---------------------------------------------------------------------------//

UIBox::UIBox(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect) : TexturedUIElement(_ID)
{
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
}
void UIBox::OnRender()
{
	TextureSwap();
	glBindVertexArray(UIQuads::m_iQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in Box Rendering!! Error Code: " + errorCode, 1);
	}
}

//---------------------------------------------------------------------------//

UIButton::UIButton(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect) : TexturedUIElement(_ID)
{
	m_bPressed = false;
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
}

void UIButton::OnUpdate()
{
	if (m_elementRect->IsMouseInRect())
	{
		if (InputManager::GetMouseButtonDown(0))
		{
			m_bPressed = true;
		}
		else
		{
			m_bPressed = false;
		}
	}
}

void UIButton::OnRender()
{
	TextureSwap();
	glBindVertexArray(UIQuads::m_iQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in Button Rendering!! Error Code: " + errorCode, 1);
	}
}

bool UIButton::ClickedOn()
{
	return m_bPressed;
}

//---------------------------------------------------------------------------//

UILabel::UILabel(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect, float _labelScale, std::shared_ptr<Font> _font) : UIElement(_ID)
{
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
	m_labelFont = _font;
}
void UILabel::OnRender()
{
	float x = m_elementRect->m_rect.x;
	float y = m_elementRect->m_rect.y + m_labelFont->fontPixelSize;

	// activate corresponding render state	
	Application::m_uiTextProgram->UseProgram();
	Application::m_uiTextProgram->BindVector3("textColor", m_colour);
	Application::m_uiTextProgram->BindMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(m_elementRect->m_rect.z, m_elementRect->m_rect.w, 1.0f)));
	Application::m_uiTextProgram->BindMat4("projection", Renderer::GenerateUIOrthographicMatrix());
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(UIQuads::m_iTextVAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = m_sText.begin(); c != m_sText.end(); c++)
	{
		Character ch = m_labelFont->characters[*c];

		float xpos = x + ch.bearing.x * m_fScale;
		float ypos = y + (ch.size.y - ch.bearing.y) * m_fScale;

		float w = ch.size.x * m_fScale;
		float h = ch.size.y * m_fScale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos - h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos - h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos - h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, UIQuads::m_iTextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6)* m_fScale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in Label Rendering!! Error Code: " + errorCode, 2);
	}
}


//---------------------------------------------------------------------------//

UIToggle::UIToggle(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect) : TexturedUIElement(_ID)
{
	m_bToggleValue = false;
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
}

void UIToggle::OnUpdate()
{
	if (m_elementRect->IsMouseInRect())
	{
		if (InputManager::GetMouseButtonDown(0))
		{
			m_bToggleValue = !m_bToggleValue;
		}
	}
}
void UIToggle::OnRender()
{
	ToggleTextureSwap(m_bToggleValue);
	glBindVertexArray(UIQuads::m_iQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in Toggle Rendering!! Error Code: " + errorCode, 1);
	}
}

//---------------------------------------------------------------------------//

UISlider::UISlider(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect) : TexturedUIElement(_ID)
{
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
}

void UISlider::OnUpdate()
{
	float minPos = m_elementRect->m_rect.x;
	float maxPos = m_elementRect->m_rect.x + m_elementRect->m_rect.z - m_handleRect->m_rect.z;

	float percentageValue = ((minPos + maxPos) / 100);
	m_fCurrentPercentage = ((m_handleRect->m_rect.x) / percentageValue) / 100;

	if (!m_bCurrentlyDragged)
	{
		if (m_handleRect->IsMouseInRect() && m_elementRect->IsMouseInRect())
		{
			if (InputManager::GetMouseButton(0))
			{
				m_bCurrentlyDragged = true;
			}
		}
	}
	else
	{
		m_handleRect->m_rect.x = InputManager::m_iMouseX - (m_handleRect->m_rect.z / 2);
		if (!InputManager::GetMouseButton(0))
		{
			m_bCurrentlyDragged = false;
		}
	}

	if (m_handleRect->m_rect.x <= minPos)
	{
		m_handleRect->m_rect.x = minPos;
		m_fCurrentPercentage = 0;
	}
	if (m_handleRect->m_rect.x + m_handleRect->m_rect.z >= m_elementRect->m_rect.x + m_elementRect->m_rect.z)
	{
		m_handleRect->m_rect.x = maxPos;
		m_fCurrentPercentage = 1;
	}

	m_fCurrentValue = (m_fMaxValue * m_fCurrentPercentage);
}
void UISlider::OnRender()
{
	TextureSwap();
	glBindVertexArray(UIQuads::m_iQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	Application::m_uiShaderProgram->BindMat4("u_UIPos", m_handleRect->GetUIMatrix());
	if (m_handleTexture)
	{
		glBindTexture(GL_TEXTURE_2D, m_handleTexture->m_iTextureID);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in Slider Rendering!! Error Code: " + errorCode, 1);
	}
}

//---------------------------------------------------------------------------//

UIEditBox::UIEditBox(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect, std::shared_ptr<Font> _font) : TexturedUIElement(_ID)
{
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
	m_editLabel = std::make_shared<UILabel>(_layer + 1, _ID + "Label", _rect, 2, _font);
}

void UIEditBox::OnUpdate()
{
	if (m_elementRect->IsMouseInRect() && InputManager::GetMouseButtonDown(0))
	{
		m_bCurrentlyEditing = !m_bCurrentlyEditing;
	}

	if (m_bCurrentlyEditing)
	{
		InputManager::StartTextInput();

		if (!InputManager::m_sInputText.empty())
		{
			if(m_sEditText.length() < m_iMaxCharacters)
				m_sEditText += InputManager::m_sInputText;
		}

		if (InputManager::GetKeyDown(SDLK_BACKSPACE))
		{
			if(m_sEditText.length() != 0)
				m_sEditText.pop_back();
		}

		m_editLabel->m_sText = m_sEditText;
	}

	if (InputManager::GetKeyDown(SDLK_KP_ENTER))
	{
		InputManager::StopTextInput();
		m_bCurrentlyEditing = false;
	}
}

void UIEditBox::OnRender()
{
	ToggleTextureSwap(m_bCurrentlyEditing);
	glBindVertexArray(UIQuads::m_iQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
	m_editLabel->OnRender();

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in Edit Box Rendering!! Error Code: " + errorCode, 1);
		std::cout << "OpenGL Error in Edit Box Rendering!! Error Code: " << errorCode << std::endl;
	}
}

//---------------------------------------------------------------------------//

UIWindow::UIWindow(unsigned int _layer, const std::string& _ID, const glm::vec4& _rect) : TexturedUIElement(_ID)
{
	m_elementRect = std::make_shared<UIRect>(_layer, _rect);
	m_iMaxLayers = 0;
}


void UIWindow::OnUpdate()
{
	if (m_bDraggable)
	{
		bool canDrag = true;
		for (int i = 0; i < m_vUIElements.size(); i++)
		{
			if (m_vUIElements.at(i)->m_elementRect->IsMouseInRect())
			{
				canDrag = false;
				break;
			}
		}

		if (canDrag)
		{
			if (!m_bCurrentlyDragged)
			{
				if (m_elementRect->IsMouseInRect())
				{
					if (InputManager::GetMouseButton(0))
					{
						m_bCurrentlyDragged = true;
					}
				}
			}
			else
			{
				m_elementRect->m_rect = glm::vec4(InputManager::m_iMouseX - (m_elementRect->m_rect.x / 2), InputManager::m_iMouseY - (m_elementRect->m_rect.y / 2), m_elementRect->m_rect.z, m_elementRect->m_rect.w);
				if (!InputManager::GetMouseButton(0))
				{
					m_bCurrentlyDragged = false;
				}
			}
		}
	}

	for (int i = 0; i < m_vUIElements.size(); i++)
	{
		m_vUIElements.at(i)->OnUpdate();
	}
}
void UIWindow::OnRender()
{
	TextureSwap();
	glBindVertexArray(UIQuads::m_iQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);

	for (int i = 0; i <= m_iMaxLayers; i++)
	{
		for (int j = 0; j < m_vUIElements.size(); j++)
		{
			if (m_vUIElements.at(j)->m_elementRect->m_iLayer == i)
			{
				m_vUIElements.at(j)->OnRender();
			}
		}
	}

	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		std::cout << "OpenGL Error in Window Rendering!! Error Code: " << errorCode << std::endl;
	}
}

//---------------------------------------------------------------------------//

void UISystem::InitializeUI()
{
	std::vector<std::string> layoutLines = m_sUILayoutFile->m_vLayoutLines;
	for (int i = 0; i < layoutLines.size(); i++)
	{
		if (layoutLines.at(i).empty())
			continue;

		std::vector<std::string> splitLine = SplitString(layoutLines.at(i), ' ');
		if (splitLine.at(0) == "UIBOX")
		{
			InitializeUIElement<UIBox>(splitLine);
		}
		else if (splitLine.at(0) == "UIBUTTON")
		{
			InitializeUIElement<UIButton>(splitLine);
		}
		else
		{
			Logger::LogError(FormatString("Unknown UI type %s in %s", splitLine.at(0), m_sUILayoutFile->m_sResourcePath), 0);
			continue;
		}
	}
}

void UISystem::Update()
{
	bool foundCurrentEditText = false;
	for (int i = 0; i < m_vUIElements.size(); i++)
	{
		m_vUIElements.at(i)->OnUpdate();

		std::shared_ptr<UIEditBox> editBox = std::dynamic_pointer_cast<UIEditBox>(m_vUIElements.at(i));
		if (editBox != nullptr)
		{
			if (foundCurrentEditText)
			{
				editBox->m_bCurrentlyEditing = false;
				continue;
			}

			if (editBox->m_bCurrentlyEditing)
				foundCurrentEditText = true;
		}
	}
}
void UISystem::Render()
{
	glDisable(GL_DEPTH_TEST);

	for (int i = 0; i <= m_iMaxLayers; i++)
	{
		for (int j = 0; j < m_vUIElements.size(); j++)
		{
			if (m_vUIElements.at(j)->m_elementRect->m_iLayer == i)
			{
				m_vUIElements.at(j)->OnRender();
			}
		}
	}
	if (glGetError() != GL_NO_ERROR)
	{
		Logger::LogError("OpenGL Error in UI Rendering!!", 1);
	}

	glEnable(GL_DEPTH_TEST);
}