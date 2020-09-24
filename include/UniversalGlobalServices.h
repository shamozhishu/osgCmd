#ifndef __UNIVERSAL_GLOBAL_SERVICES_H__
#define __UNIVERSAL_GLOBAL_SERVICES_H__

#include <zoo/ServiceLocator.h>

class CoordTransformUtil : public zoo::Service
{
	ZOO_SERVICE(CoordTransformUtil)
public:
	virtual bool convertLLHToXYZ(double longitude, double latitude, double height, double& X, double& Y, double& Z) = 0;
	virtual bool convertXYZToLLH(double X, double Y, double Z, double& longitude, double& latitude, double& height) = 0;
};

class MeshList : public zoo::Service
{
	ZOO_SERVICE(MeshList)
public:
	/* ����: ��ȡ�����б�
	 * ����: ��
	 * ����ֵ: �����б�,�������û��Զ���������������
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual vector<string> getMeshList() const = 0;
	/* ����: ���û��Զ���������������ȡ������������Ϣ
	 * ����: 1)ָ��Ҫ��ȡ��һ���û��Զ����������Ϣ;2)������Ϣ��ŵ�λ��
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual void getMeshConfigInfo(string meshName, struct Mesh* mesh) = 0;
};

class MaterialList : public zoo::Service
{
	ZOO_SERVICE(MaterialList)
public:
	/* ����: ��ȡ�����б�
	 * ����: ��
	 * ����ֵ: �����б�,�������û��Զ�������в��ʶ���
	 * ����: shamozhishu
	 * ����: 2020.9.11
	 */
	virtual vector<string> getMaterialList() const = 0;
	/* ����: ���û��Զ���Ĳ��ʶ�������ȡ������������Ϣ
	 * ����: 1)ָ��Ҫ��ȡ��һ���û��Զ�����ʵ���Ϣ;2)������Ϣ��ŵ�λ��
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.11
	 */
	virtual void getMaterialConfigInfo(string materialName, struct Material* material) = 0;
};

#endif // __UNIVERSAL_GLOBAL_SERVICES_H__
