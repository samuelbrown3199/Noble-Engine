#include "Font.h"

#include "../Core/Application.h"

Font::Font()
{
	fontPixelSize = 48;
}

void Font::OnLoad()
{
	FT_Face font;

	if (FT_New_Face(ResourceManager::m_fontLibrary, m_sResourcePath.c_str(), 0, &font))
	{
		std::cout << "Filaed to load font " << m_sResourcePath << "!" << std::endl;
	}

	FT_Set_Pixel_Sizes(font, 0, fontPixelSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(font, c, FT_LOAD_RENDER))
		{
			std::cout << "Failed to load glyph" << std::endl;
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
			font->glyph->bitmap.width, 
			font->glyph->bitmap.rows, 
			0, 
			GL_RED, GL_UNSIGNED_BYTE, 
			font->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		Character character = {
			texture,
			glm::ivec2(font->glyph->bitmap.width, font->glyph->bitmap.rows),
			glm::ivec2(font->glyph->bitmap_left, font->glyph->bitmap_top),
			(unsigned int)font->glyph->advance.x
		};
		characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(font);
}