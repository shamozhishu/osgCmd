/************************************************************************\
* ����: ����ϵͳ
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __OSG_MATERIAL_H__
#define __OSG_MATERIAL_H__

#include <osgFX/Effect>
#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthSupport.h>
#include <component/war/WarComponents.h>
//------------------------------------------------------------------------
// ����: OSGʵ�ֵĲ�����(�ڴ��ͷŽ��ɲ��ʹ���������)
// ����: shamozhishu
// ����: 2020.9.7
//------------------------------------------------------------------------
class OsgMaterial
{
public:
	virtual ~OsgMaterial() {}
	/* ����: ��ȡ��������
	 * ����: ��
	 * ����ֵ: ��������
	 * ����: shamozhishu
	 * ����: 2020.9.8
	 */
	virtual string getMaterialName() const = 0;
	/* ����: ˢ�²���Ч��, ��Ҫ��Uniform��ʵʱ����
	 * ����: ��Ҫ���µ�Uniforms
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.28
	 */
	virtual void refresh(const map<string, vector<double>>& uniforms) { (void)uniforms; }
	/* ����: ����ڵ���Ҫ����, ����д�ó�Ա����, ���������ģ��������ߺ͸����ߵĶ�������
	 * ����: Ҫ����Ľڵ�
	 * ����ֵ: ����TRUE��ʾ����ִ��setupStateSet(), ����FALSE��ִ��setupStateSet().
	 * ����: shamozhishu
	 * ����: 2020.9.10
	 */
	virtual bool reform(osg::Node* node) { return true; }
	/* ����: �ָ������Ľڵ㵽��ʼ״̬
	 * ����: Ҫ�ָ��Ľڵ�
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.10
	 */
	virtual void restore(osg::Node* node) {}
	/* ����: �������ͻ�����ʹ������Щ��ɫ����ͳһֵ��
	 * ����: ���õ���Ϣ�ɸò�������
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.11
	 */
	virtual void getMaterialConfigInfo(Material* material) = 0;
	/* ����: ������Ⱦ״̬��
	 * ����: 1)��ȡ�ⲿ�Ĳ���������Ϣ,���ڲ��ʵĳ�ʼ������;2)���ʹ�������Ⱦ״̬��
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.7
	 */
	virtual void setupStateSet(Material* material, osg::StateSet* stateset) { (void)material; (void)stateset; }
	/* ����: ����Ч���ڵ�
	 * ����: ��ȡ�ⲿ�Ĳ���������Ϣ,���ڴ���Ч��ʱʹ��
	 * ����ֵ: ����Ч���ڵ����
	 * ����: shamozhishu
	 * ����: 2020.9.7
	 */
	virtual osgFX::Effect* createEffect(Material* material) { (void)material; return nullptr; }
};
//------------------------------------------------------------------------
// ����: ���ʹ���������
// ����: shamozhishu
// ����: 2020.9.7
//------------------------------------------------------------------------
class OsgMaterialManager : public Service
{
	ZOO_SERVICE(OsgMaterialManager)
public:
	/* ����: ���Ӳ���
	 * ����: 1)ѡ��Ҫ���ӵĲ���,2)����Ӱ��Ľڵ�
	 * ����ֵ: �ɹ����ض�Ӧ��osg����,ʧ�ܷ���NULL
	 * ����: shamozhishu
	 * ����: 2020.9.10
	 */
	virtual OsgMaterial* attach(Material* material, osg::Node* node) = 0;
	/* ����: �������
	 * ����: 1)ѡ��Ҫ����Ĳ���,2)����Ӱ��Ľڵ�
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.10
	 */
	virtual void detach(const string& materialName, osg::Node* node) = 0;
	/* ����: ��Ӳ��ʶ���
	 * ����: �û��Զ���Ĳ��ʶ���
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.10
	 */
	virtual void addMaterial(OsgMaterial* osgMaterial) = 0;
	/* ����: �Ƴ����ʶ���
	 * ����: �û��Զ���Ĳ��ʶ���
	 * ����ֵ: ��
	 * ����: shamozhishu
	 * ����: 2020.9.10
	 */
	virtual void removeMaterial(OsgMaterial* osgMaterial) = 0;
};

#endif // __OSG_MATERIAL_H__
