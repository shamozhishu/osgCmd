/************************************************************************\
* ����: ������Դ
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __OSG_MESH_H__
#define __OSG_MESH_H__

#include <osg/Geode>
#include <zoo/ServiceLocator.h>
#include <component/war/WarComponents.h>

//------------------------------------------------------------------------
// ����: �����๹�캯���н���addMesh�����������������delete��������������ͷ��ڴ�
// ����: shamozhishu
// ����: 2020.9.16
//------------------------------------------------------------------------
class OsgMesh
{
public:
	virtual ~OsgMesh() {}
	/* ����: ��ȡ��������
	 * ����: ��
	 * ����ֵ: ��������
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual string getMeshName() const = 0;
	/* ����: �������ͻ������Ƿ�ʹ��ģ���ļ���������
	 * ����: ���õ���Ϣ�ɸò�������
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual void getMeshConfigInfo(Mesh* mesh) = 0;
	/* ����: ��������ģ��
	 * ����: ��ȡ�ⲿ������ģ��������Ϣ,��������ĳ�ʼ������
	 * ����ֵ: ���ذ���������ģ�͵�Ҷ�ӽڵ�
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual osg::Geode* setupMeshModel(Mesh* mesh) { (void)mesh; return nullptr; }
};
//------------------------------------------------------------------------
// ����: �������������
// ����: shamozhishu
// ����: 2020.9.16
//------------------------------------------------------------------------
class OsgMeshManager : public Service
{
	ZOO_SERVICE(OsgMeshManager)
public:
	/* ����: ��������
	 * ����: 1)ѡ��Ҫ���ӵ�����,2)������صĽڵ�
	 * ����ֵ: �ɹ�����true,ʧ�ܷ���false
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual bool attach(Mesh* mesh, osg::Group* meshNode) = 0;
	/* ����: �������
	 * ����: ������صĽڵ�
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual void detach(osg::Group* meshNode) = 0;
	/* ����: ����������
	 * ����: �û��Զ�����������
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual void addMesh(OsgMesh* osgMesh) = 0;
	/* ����: �Ƴ��������
	 * ����: �û��Զ�����������
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual void removeMesh(OsgMesh* osgMesh) = 0;
};

#endif // __OSG_MESH_H__
