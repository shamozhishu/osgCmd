#ifndef __MATERIAL_SCRIPT_H__
#define __MATERIAL_SCRIPT_H__

#include <osgFX/Effect>
#include <zooCmd_osg/OsgMaterial.h>

//------------------------------------------------------------------------
// ����: ���ʼ���,���ڶ����ͨ��
// ����: shamozhishu
// ����: 2020.9.25
//------------------------------------------------------------------------
class MaterialTechnique : public osgFX::Technique
{
public:
	MaterialTechnique();
	MaterialTechnique(const char* name);
	const char *techniqueName();
	const char *techniqueDescription();
	void pushPass(osg::StateSet* ss);
	void refresh(const map<string, vector<double>>& uniforms);

protected:
	void define_passes();

private:
	string _name;
	vector<osg::ref_ptr<osg::StateSet>> _passes;
};

//------------------------------------------------------------------------
// ����: ����Ч��,���ٰ���һ�����ʼ���
// ����: shamozhishu
// ����: 2020.9.25
//------------------------------------------------------------------------
class MaterialEffect : public osgFX::Effect
{
public:
	MaterialEffect();
	MaterialEffect(const MaterialEffect& copy, const osg::CopyOp op = osg::CopyOp::SHALLOW_COPY);
	META_Effect(osgFX, MaterialEffect, "MaterialEffect", "My ShaderLab", "shamozhishu")
	void pushTechnique(MaterialTechnique* technique);
	void refresh(const map<string, vector<double>>& uniforms);

protected:
	bool define_techniques();

private:
	vector<osg::ref_ptr<MaterialTechnique>> _techniques;
};

//------------------------------------------------------------------------
// ����: ���ʽű�,ÿһ��material���ʽű������������һ����ǰ���͵Ķ���
// ����: shamozhishu
// ����: 2020.9.25
//------------------------------------------------------------------------
class MaterialScript : public OsgMaterial
{
public:
	MaterialScript(const string name);
	string getMaterialName() const;
	void refresh(const map<string, vector<double>>& uniforms);
	void getMaterialConfigInfo(Material* material);
	osgFX::Effect* createEffect(Material* material);
	void setMaterialEffect(MaterialEffect* effect);

protected:
	const string _materialName;
	osg::ref_ptr<MaterialEffect> _effect;
};

#endif // __MATERIAL_SCRIPT_H__
