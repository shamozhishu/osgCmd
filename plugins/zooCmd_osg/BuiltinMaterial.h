/************************************************************************\
* ����: �ڽ�����
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __BUILTIN_MATERIAL_H__
#define __BUILTIN_MATERIAL_H__

#include <zooCmd_osg/OsgMaterial.h>

//------------------------------------------------------------------------
// ����: ��͹��ͼ
// ����: shamozhishu
// ����: 2020.9.10
//------------------------------------------------------------------------
class BumpMapping : public OsgMaterial
{
public:
	BumpMapping();
	string getMaterialName() const;
	bool reform(osg::Node* node);
	void restore(osg::Node* node);
	void getMaterialConfigInfo(Material* material);
	void setupStateSet(Material* material, osg::StateSet* stateset);

protected:
	string _colorImgPath;
	string _normalImgPath;
};

#endif // __BUILTIN_MATERIAL_H__
