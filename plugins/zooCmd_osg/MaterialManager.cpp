#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
	auto it = _materials.begin();
	auto itEnd = _materials.end();
	for (; it != itEnd; ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

vector<string> MaterialManager::getMaterialList() const
{
	vector<string> matList(_materials.size() + 1);
	matList[0] = "Default";
	auto it = _materials.begin();
	auto itEnd = _materials.end();
	for (int i = 1; it != itEnd; ++it, ++i)
		matList[i] = it->first;
	return matList;
}

void MaterialManager::getMaterialConfigInfo(string materialName, Material* material)
{
	material->_uniforms.clear();

	for (int i = 0; i < Material::TexUnitNum; ++i)
		material->_textureFiles[i] = make_pair("", "");

	auto it = _materials.find(materialName);
	if (it != _materials.end())
	{
		OsgMaterial* pOsgMaterial = it->second;
		if (pOsgMaterial)
			pOsgMaterial->getMaterialConfigInfo(material);
	}
}

OsgMaterial* MaterialManager::attach(Material* material, osg::Node* node)
{
	if (!material || !node || material->_currentUseMatName == "Default")
		return nullptr;

	auto it = _materials.find(material->_currentUseMatName);
	if (it == _materials.end())
	{
		zoo_warning("�����ڵĲ���[%s]��Ĭ��ʹ��ȱʡ���ʣ�", material->_currentUseMatName.c_str());
		return nullptr;
	}

	OsgMaterial* pOsgMaterial = it->second;
	if (pOsgMaterial)
	{
		if (pOsgMaterial->reform(node))
		{
			osg::StateSet* ss = new osg::StateSet;
			node->setStateSet(ss);
			pOsgMaterial->setupStateSet(material, ss);
		}

		osgFX::Effect* effect = pOsgMaterial->createEffect(material);
		if (effect)
		{
			osg::Group* parent = node->getParent(0);
			if (parent)
			{
				effect->addChild(node);
				parent->removeChild(node);
				parent->addChild(effect);
			}
		}
	}

	return pOsgMaterial;
}

void MaterialManager::detach(const string& materialName, osg::Node* node)
{
	auto it = _materials.find(materialName);
	if (it == _materials.end())
	{
		if (materialName != "Default")
			zoo_warning("Ҫ����Ĳ���[%s]�����ڣ�", materialName.c_str());
		return;
	}

	osgFX::Effect* effect = dynamic_cast<osgFX::Effect*>(node->getParent(0));
	if (effect)
	{
		osg::Group* parent = effect->getParent(0);
		parent->addChild(node);
		parent->removeChild(effect);
	}

	OsgMaterial* pOsgMaterial = it->second;
	if (pOsgMaterial)
		pOsgMaterial->restore(node);
}

void MaterialManager::addMaterial(OsgMaterial* osgMaterial)
{
	auto it = _materials.find(osgMaterial->getMaterialName());
	if (it != _materials.end())
		delete it->second;
	_materials[osgMaterial->getMaterialName()] = osgMaterial;
}

void MaterialManager::removeMaterial(OsgMaterial* osgMaterial)
{
	auto it = _materials.find(osgMaterial->getMaterialName());
	if (it != _materials.end())
	{
		delete it->second;
		_materials.erase(it);
	}
}
