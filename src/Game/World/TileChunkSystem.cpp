#include "TileChunkSystem.h"
#include "Engine/Core/Application.h"
#include "Engine/Useful.h"

std::weak_ptr<SystemBase> TileChunkSystem::self;
std::weak_ptr<SystemBase> TileChunk::componentSystem;
std::vector<TileChunk> TileChunk::componentData;

const unsigned int TileChunk::m_iChunkWidth = 16;
const unsigned int TileChunk::m_iTileSpriteSheetWidth = 16;

void TileChunkSystem::CreateChunkMeshData(TileChunk* comp)
{
	if (comp->m_chunkTransform == nullptr)
	{
		comp->m_chunkTransform = Application::GetEntity(comp->entityID)->AddComponent<Transform>();
	}

	for (int chunk = 0; chunk < comp->m_vTileChunkLayers.size(); chunk++)
	{
		TileChunkLayer* targetLayer = &comp->m_vTileChunkLayers.at(chunk);

		if (!targetLayer->m_bDirty)
			continue;

		if (targetLayer->m_layerTransformMat == glm::mat4(1.0f))
		{
			glm::vec3 layerPos = comp->m_chunkTransform->m_position;
			layerPos.z += (0.1*chunk);
			targetLayer->m_layerTransformMat = glm::translate(targetLayer->m_layerTransformMat, layerPos);
			targetLayer->m_layerTransformMat = glm::scale(targetLayer->m_layerTransformMat, comp->m_chunkTransform->m_scale);

			for (int x = 0; x < TileChunk::m_iChunkWidth; x++)
			{
				for (int y = 0; y < TileChunk::m_iChunkWidth; y++)
				{
					targetLayer->m_aTileMap[x][y] = 1;
				}
			}
		}

		float tileVertex[] =
		{
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f,
			-0.5f, -0.5f,
			0.5f, 0.5f,
			-0.5f, 0.5f
		};

		float chunkVertices[12 * (16 * 16)];
		int vert = 0;
		for (int x = 0; x < TileChunk::m_iChunkWidth; x++)
		{
			for (int y = 0; y < TileChunk::m_iChunkWidth; y++)
			{
				if (targetLayer->m_aTileMap[x][y] == 0)
					continue;

				for (int i = 0; i < 12; i++)
				{
					if (i % 2 == 0)
					{
						chunkVertices[vert + i] = tileVertex[i] + y;
					}
					else
					{
						chunkVertices[vert + i] = tileVertex[i] + x;
					}
				}
				vert += 12;
			}
		}

		float tileTex[] =
		{
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f
		};

		float chunkTex[12 * (16 * 16)];
		vert = 0;
		float texCoordWidth = 1.0f / TileChunk::m_iTileSpriteSheetWidth;
		for (int x = 0; x < TileChunk::m_iChunkWidth; x++)
		{
			for (int y = 0; y < TileChunk::m_iChunkWidth; y++)
			{
				int currentTileID = targetLayer->m_aTileMap[x][y];
				if (currentTileID == 0)
					continue;
				float cornerX = (float)(currentTileID - 1) / TileChunk::m_iTileSpriteSheetWidth;
				float cornerY = std::floor((float)currentTileID / TileChunk::m_iTileSpriteSheetWidth) * texCoordWidth;

				for (int i = 0; i < 12; i++)
				{
					chunkTex[vert + i] = (i + 1) % 2 == 0 ? cornerY : cornerX;
					chunkTex[vert + i] += ((int)tileTex[i] * texCoordWidth);
				}
				vert += 12;
			}
		}

		if (targetLayer->m_iChunkVAO == -1)
		{
			glGenVertexArrays(1, &targetLayer->m_iChunkVAO);
			glGenBuffers(1, &targetLayer->m_iVertexVBO);
			glGenBuffers(1, &targetLayer->m_iTextureVBO);
		}
		glBindVertexArray(targetLayer->m_iChunkVAO);
		glBindBuffer(GL_ARRAY_BUFFER, targetLayer->m_iVertexVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(chunkVertices), chunkVertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, targetLayer->m_iTextureVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(chunkTex), chunkTex, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);

		targetLayer->m_bDirty = false;
	}
	comp->m_bInitialized = true;
}

void TileChunkSystem::OnUpdate(TileChunk* comp)
{
	CreateChunkMeshData(comp);
}

void TileChunkSystem::OnRender(TileChunk* comp)
{
	Application::m_mainShaderProgram->UseProgram();

	glm::mat4 finalMat = Renderer::GenerateProjMatrix() * Renderer::GenerateViewMatrix();
	Application::m_mainShaderProgram->BindMat4("vpMat", finalMat);
	Application::m_mainShaderProgram->BindVector4("colour", glm::vec4(1.0f));

	for (int i = 0; i < comp->m_vTileChunkLayers.size(); i++)
	{
		if (!comp->m_vTileChunkLayers.at(i).m_bRender)
			continue;

		Application::m_mainShaderProgram->BindMat4("transMat", comp->m_vTileChunkLayers.at(i).m_layerTransformMat);
		glBindVertexArray(comp->m_vTileChunkLayers.at(i).m_iChunkVAO);
		glBindTexture(GL_TEXTURE_2D, comp->m_vTileChunkLayers.at(i).m_tileMapTexture->m_iTextureID);
		glDrawArrays(Renderer::GetRenderMode(), 0, 6 * (comp->m_iChunkWidth * comp->m_iChunkWidth));
		glBindVertexArray(0);
	}
}