#include "MeshManager.h"
#include <zoo/Utils.h>

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
	auto it = _meshs.begin();
	auto itEnd = _meshs.end();
	for (; it != itEnd; ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

vector<string> MeshManager::getMeshList() const
{
	vector<string> meshList(_meshs.size());
	auto it = _meshs.begin();
	auto itEnd = _meshs.end();
	for (int i = 0; it != itEnd; ++it, ++i)
		meshList[i] = it->first;
	return meshList;
}

bool MeshManager::switchMesh(string meshName, Mesh* mesh)
{
	mesh->_params.clear();

	auto it = _meshs.find(meshName);
	if (it != _meshs.end())
	{
		OsgMesh* pOsgMesh = it->second;
		if (pOsgMesh)
			pOsgMesh->getMeshConfigInfo(mesh);
	}
	else
	{
		meshName = "Default";
	}

	if (mesh->_currentUseMeshName != meshName)
	{
		mesh->_currentUseMeshName = meshName;
		mesh->getParent()->dirtyBit().addState(Mesh::Changed_);
		return true;
	}

	return false;
}

bool MeshManager::compileMesh(string meshFile, Mesh* mesh)
{
	return false;
}

bool MeshManager::attach(Mesh* mesh, osg::Group* meshNode)
{
	if (!mesh || !meshNode)
		return false;

	auto it = _meshs.find(mesh->_currentUseMeshName);
	if (it == _meshs.end())
	{
		zoo_warning("�����ڵ�����[%s]��", mesh->_currentUseMeshName.c_str());
		return false;
	}

	OsgMesh* pOsgMesh = it->second;
	if (pOsgMesh)
	{
		osg::ref_ptr<osg::Node> node = pOsgMesh->setupMeshModel(mesh);
		if (node)
			meshNode->addChild(node);
	}

	return true;
}

void MeshManager::detach(osg::Group* meshNode)
{
	meshNode->removeChildren(0, meshNode->getNumChildren());
}

void MeshManager::addMesh(OsgMesh* osgMesh)
{
	auto it = _meshs.find(osgMesh->getMeshName());
	if (it != _meshs.end())
		delete it->second;
	_meshs[osgMesh->getMeshName()] = osgMesh;
}

void MeshManager::removeMesh(OsgMesh* osgMesh)
{
	auto it = _meshs.find(osgMesh->getMeshName());
	if (it != _meshs.end())
	{
		delete it->second;
		_meshs.erase(it);
	}
}
