/************************************************************************\
* ����: ��Ⱦ�������ӿ�
* https://github.com/shamozhishu
* Copyright (c) 2020-2021 authored by shamozhishu
\************************************************************************/
#ifndef __ZOOCMD_RENDER_ADAPTER_H__
#define __ZOOCMD_RENDER_ADAPTER_H__

namespace zooCmd {

class RenderAdapter
{
public:
	virtual ~RenderAdapter() {}
	virtual int   run() = 0;
	virtual bool  isDone() = 0;
	virtual void  setDone(bool done) = 0;
	virtual void  frame(double simulationTime) = 0;
	/* ����: ��ȡ���ڿؼ����ߵײ㴰�ھ����ָ��
	 * ����: windowID-����ID,wndHandle-ѡ���Ƿ��ȡ�ײ㴰�ھ��
	 * ����ֵ: ���ش��ڿؼ����ߵײ㴰�ھ����ָ��
	 * ����: shamozhishu
	 * ����: 2021.1.13
	 */
	virtual void* getWnd(int windowID, bool wndHandle = false) = 0;
	/* ����: ����ָ���ߴ�Ĵ��ڿؼ�
	 * ����: windowWidth-��,windowHeight-��,windowScale-���ű�,windowID-����ID
	 * ����ֵ: ���ط�װ�Ĵ��ڿؼ������ָ��
	 * ����: shamozhishu
	 * ����: 2021.1.13
	 */
	virtual void* setup(int windowWidth, int windowHeight, float windowScale, int windowID = 0) = 0;
};

}

#endif // __ZOOCMD_RENDER_ADAPTER_H__
