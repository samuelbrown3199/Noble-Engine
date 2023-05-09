#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Resource.h"

#include<GL/glew.h>

/**
*Stores a texture for use in the engine.
*/
struct Texture : public Resource
{
    GLuint m_iTextureID;
    int m_iWidth = 0;
    int m_iHeight = 0;

    enum FilterMode
    {
        Point,
        Linear
    };

    FilterMode m_filterMode;

    void OnLoad();
};

#endif