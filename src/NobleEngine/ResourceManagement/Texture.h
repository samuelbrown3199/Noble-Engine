#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../Core/Resource.h"

#include<GL/glew.h>

namespace NobleResources
{
    /**
    *Stores a texture for use in the engine.
    */
    struct Texture : public NobleCore::Resource
    {
        GLuint textureID;
        int w = 0;
        int h = 0;

        void OnLoad();
    };
}

#endif // !TEXTURE_H_