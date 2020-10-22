#ifndef __UNIVERSAL_GLOBAL_SERVICES_H__
#define __UNIVERSAL_GLOBAL_SERVICES_H__

#include <zoo/ServiceLocator.h>

class CoordTransformUtil : public zoo::Service
{
	ZOO_SERVICE(CoordTransformUtil)
public:
	class Converter
	{
	public:
		virtual ~Converter() {}
		template<typename T> T* to() { return dynamic_cast<T*>(this); }
	};

	virtual bool convertLLHToXYZ(Converter* converter, double longitude, double latitude, double height, double& X, double& Y, double& Z) = 0;
	virtual bool convertXYZToLLH(Converter* converter, double X, double Y, double Z, double& longitude, double& latitude, double& height) = 0;
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
	/* ����: �л������Ҵ��û��Զ���������������ȡ������������Ϣ
	 * ����: 1)ָ��Ҫ�л�������;2)��ȡ���û��Զ��������������Ϣ���洢����ǰ������
	 * ����ֵ: �л��ɹ����
	 * ����: shamozhishu
	 * ����: 2020.9.16
	 */
	virtual bool switchMesh(string meshName, struct Mesh* mesh) = 0;
	/* ����: ����ָ��������ű��ļ�
	 * ����: 1)ָ��������ű��ļ���;2)���Զ�̬��������������ŵ�λ��
	 * ����ֵ: �Ƿ����ɹ�
	 * ����: shamozhishu
	 * ����: 2020.10.20
	 */
	virtual bool compileMesh(string meshFile, struct Mesh* mesh) = 0;
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
	/* ����: �л����ʲ��Ҵ��û��Զ���Ĳ��ʶ�������ȡ������������Ϣ
	 * ����: 1)ָ��Ҫ�л��Ĳ���;2)��ȡ���û��Զ�����ʵ�������Ϣ���洢����ǰ������
	 * ����ֵ: �л��ɹ����
	 * ����: shamozhishu
	 * ����: 2020.9.11
	 */
	virtual bool switchMaterial(string materialName, struct Material* material) = 0;
	/* ����: ����ָ���Ĳ��ʽű��ļ�
	 * ����: 1)ָ���Ĳ��ʽű��ļ���;2)���Զ�̬����Ĳ��ʲ�����ŵ�λ��
	 * ����ֵ: �Ƿ����ɹ�
	 * ����: shamozhishu
	 * ����: 2020.10.20
	 */
	virtual bool compileMaterial(string materialFile, struct Material* material) = 0;
};

#endif // __UNIVERSAL_GLOBAL_SERVICES_H__
