#include "Model.h"
#include "ModelLoader.hpp"

void Model::OnLoad()
{
	m_vaoID = LoadModel(m_sResourcePath, &m_drawCount);
}