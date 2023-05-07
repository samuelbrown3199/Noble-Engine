#pragma once
#ifndef TILECHUNK_H
#define TILECHUNK_H

#include "Engine/ECS/Component.hpp"
#include "Engine/Resource/Texture.h"
#include "Engine/Core/ResourceManager.h"
#include "Engine/Systems/Transform.h"

#include <GL/glew.h>
#include<cmath>

struct TileChunkLayer
{
	unsigned int** m_aTileMap = nullptr;
	unsigned int m_iVertexVBO, m_iTextureVBO, m_iChunkVAO = -1;
	std::shared_ptr<Texture> m_tileMapTexture;

	glm::mat4 m_layerTransformMat = glm::mat4(1.0f);

	bool m_bDirty = true;
	bool m_bRender = false;

	void UpdateTile(int _tile, glm::ivec2 _tilePos)
	{
		m_aTileMap[_tilePos.x][_tilePos.y] = _tile;
		m_bDirty = true;
	}

	int GetTile(glm::ivec2 _tilePos)
	{
		return m_aTileMap[_tilePos.x][_tilePos.y];
	}
};

struct TileChunk : public ComponentData<TileChunk>
{
	std::vector<TileChunkLayer> m_vTileChunkLayers;
	const static unsigned int m_iChunkWidth;
	const static unsigned int m_iTileSpriteSheetWidth;

	Transform* m_chunkTransform = nullptr;
	bool m_bInitialized = false;

	void OnInitialize()
	{
		AddLayer("GameData\\Textures\\Tiles\\FloorTiles.png", true);
		AddLayer("GameData\\Textures\\Tiles\\WallTiles.png", false);
		AddLayer("GameData\\Textures\\Tiles\\CableTiles.png", false);
		AddLayer("GameData\\Textures\\Tiles\\PipeTiles.png", false);
		AddLayer("GameData\\Textures\\Tiles\\GasLiquidTiles.png", false);
		AddLayer("GameData\\Textures\\Tiles\\FloorTiles.png", false); //Ceiling / Fog of War
	}

	void UpdateTile(int _layer, int _tile, glm::vec2 _pos, bool _worldPos)
	{
		if (!m_bInitialized)
			return;

		if (m_vTileChunkLayers.empty())
			return;

		glm::ivec2 tilePos = _pos;
		if (_worldPos)
		{
			if (m_chunkTransform == nullptr)
				return;

			glm::ivec2 tilePos = glm::vec2(ceil(_pos.x / m_chunkTransform->m_scale.x), ceil(_pos.y / m_chunkTransform->m_scale.y));
		}

		if (tilePos.x < 0 || tilePos.y < 0 || tilePos.x > m_iChunkWidth - 1 || tilePos.y > m_iChunkWidth - 1)
			return;

		m_vTileChunkLayers.at(_layer).UpdateTile(_tile, tilePos);
	}

	int GetTile(int _layer, glm::vec2 _pos, bool _worldPos)
	{
		if (!m_bInitialized)
			return -1;

		glm::ivec2 tilePos = _pos;
		if (_worldPos)
		{
			if (m_chunkTransform == nullptr)
				return -1;

			glm::ivec2 tilePos = glm::vec2(ceil(_pos.x / m_chunkTransform->m_scale.x), ceil(_pos.y / m_chunkTransform->m_scale.y));
		}

		if (tilePos.x < 0 || tilePos.y < 0 || tilePos.x > m_iChunkWidth - 1 || tilePos.y > m_iChunkWidth - 1)
			return -1;

		return m_vTileChunkLayers.at(_layer).GetTile(tilePos);
	}

private:
	void AddLayer(std::string _textureLoc, bool _render)
	{
		TileChunkLayer newLayer;
		newLayer.m_tileMapTexture = ResourceManager::LoadResource<Texture>(_textureLoc);
		newLayer.m_bRender = _render;

		newLayer.m_aTileMap = new unsigned int* [m_iChunkWidth];
		for (int i = 0; i < m_iChunkWidth; i++)
			newLayer.m_aTileMap[i] = new unsigned int[m_iChunkWidth];

		newLayer.m_bDirty = true;
		m_vTileChunkLayers.push_back(newLayer);
	}
};

#endif