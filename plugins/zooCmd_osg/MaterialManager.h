#ifndef __MATERIAL_MANAGER_H__
#define __MATERIAL_MANAGER_H__

#include <zoo/Singleton.h>
#include <UniversalGlobalServices.h>
#include <component/WarComponents.h>
#include <zooCmd_osg/OsgMaterial.h>
#include <zooCmd_osg/OsgEarthSupport.h>
//------------------------------------------------------------------------
// ����: ���ʹ������������Ĳ��ʶ�����Ը��������ģ���������������ʹ��
// ����: shamozhishu
// ����: 2020.9.7
//------------------------------------------------------------------------
class MaterialManager : public zoo::Singleton<MaterialManager>, public OsgMaterialManager, public MaterialList
{
public:
	MaterialManager();
	~MaterialManager();
	vector<string> getMaterialList() const;
	bool switchMaterial(string materialName, Material* material);
	bool compileMaterial(string materialFile, Material* material);
	OsgMaterial* attach(Material* material, osg::Node* node);
	void detach(const string& materialName, osg::Node* node);
	void addMaterial(OsgMaterial* osgMaterial);
	void removeMaterial(OsgMaterial* osgMaterial);

private:
	unordered_map<string, OsgMaterial*> _materials;
};

#endif // __MATERIAL_MANAGER_H__
