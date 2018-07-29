#include "ComponentFactory.h"

using namespace Engine;

Manager<Mesh>* ComponentFactory::mMeshManager = nullptr;
Manager<Texture>* ComponentFactory::mTextureManager = nullptr;