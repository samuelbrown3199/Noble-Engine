#include "Engine/ECS/System.hpp"
#include "TileChunk.h"

struct TileChunkSystem : public System<TileChunk>
{
	void CreateChunkMeshData(TileChunk* comp);

	void OnUpdate(TileChunk* comp);
	void OnRender(TileChunk* comp);
};