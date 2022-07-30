#include "Texture.h"
#include "../Useful.h"
#include "../Core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::OnLoad()
{
    unsigned char* data = stbi_load(m_sResourcePath.c_str(), &m_iWidth, &m_iHeight, NULL, 4);
    if (!data)
    {
        Logger::LogError(FormatString("Failed to load texture file %s", m_sResourcePath.c_str()), 1);
        throw std::exception();
    }

    //create and bind a texture
    glGenTextures(1, &m_iTextureID);
    if (!m_iTextureID)
    {
        Logger::LogError("Failed to create texture ID", 1);
        throw std::exception();
    }
    glBindTexture(GL_TEXTURE_2D, m_iTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //upload the image daya to the bound texture unit in the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //free the loaded data because we now have a copy on the GPU
    free(data);
    //generate mipmap so the texture can be mapped correctly
    glGenerateMipmap(GL_TEXTURE_2D);
    //unbind the texture becase we are done operating on it
    glBindTexture(GL_TEXTURE_2D, 0);
}