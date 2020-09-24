#ifndef __MESH_MANAGER_H__
#define __MESH_MANAGER_H__

#include <zoo/Singleton.h>
#include <zooCmd_osg/OsgMesh.h>
#include <UniversalGlobalServices.h>
#include <zooCmd_osg/OsgEarthSupport.h>
//------------------------------------------------------------------------
// ����: ��������������������������Ը��������ģ�����ʹ��
// ����: shamozhishu
// ����: 2020.9.16
//------------------------------------------------------------------------
class MeshManager : public zoo::Singleton<MeshManager>, public OsgMeshManager, public MeshList
{
public:
	MeshManager();
	~MeshManager();
	vector<string> getMeshList() const;
	void getMeshConfigInfo(string meshName, Mesh* mesh);
	bool attach(Mesh* mesh, osg::Group* meshNode);
	void detach(osg::Group* meshNode);
	void addMesh(OsgMesh* osgMesh);
	void removeMesh(OsgMesh* osgMesh);

private:
	unordered_map<string, OsgMesh*> _meshs;
};

#endif // __MESH_MANAGER_H__
