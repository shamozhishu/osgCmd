/************************************************************************\
* ����: �ڽ�����
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __BUILTIN_MESH_H__
#define __BUILTIN_MESH_H__

#include <zooCmd_osg/OsgMesh.h>

class DefaultMesh : public OsgMesh
{
public:
	DefaultMesh();
	string getMeshName() const;
	void getMeshConfigInfo(Mesh* mesh);
	osg::Node* setupMeshModel(Mesh* mesh);

protected:
	string _resourceFile;
};
//------------------------------------------------------------------------
// ����: ��������
// ����: shamozhishu
// ����: 2020.9.16
//------------------------------------------------------------------------
class SphereMesh : public OsgMesh
{
public:
	SphereMesh();
	string getMeshName() const;
	void getMeshConfigInfo(Mesh* mesh);
	osg::Node* setupMeshModel(Mesh* mesh);

protected:
	float _radius;
	osg::Vec3 _center;
};
//------------------------------------------------------------------------
// ����: ��������
// ����: shamozhishu
// ����: 2020.9.16
//------------------------------------------------------------------------
class BoxMesh : public OsgMesh
{
public:
	BoxMesh();
	string getMeshName() const;
	void getMeshConfigInfo(Mesh* mesh);
	osg::Node* setupMeshModel(Mesh* mesh);

protected:
	osg::Vec3 _center;
	float _lengthX, _lengthY, _lengthZ;
};
//------------------------------------------------------------------------
// ����: ���������
// ����: shamozhishu
// ����: 2021/01/03
//------------------------------------------------------------------------
class BillboardMesh : public OsgMesh
{
public:
	BillboardMesh();
	string getMeshName() const;
	void getMeshConfigInfo(Mesh* mesh);
	osg::Node* setupMeshModel(Mesh* mesh);

protected:
	string _resourceFile;
};

#endif // __BUILTIN_MESH_H__
