#include "BuiltinCmd.h"
#include "RenderDevice.h"
#include <zoo/Utils.h>
#include <zoo/Component.h>
#include <zooCmd/CmdManager.h>
#include "BuiltinEventHandler.h"
#include "WindowCaptureCallback.h"
#include <zooCmd_osg/OsgEarthContext.h>
#include <zooCmd_osg/OsgComponentImpls.h>

using namespace zooCmd;

ZOO_REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	_wndCaptureCB = new zooCmd_osg::WindowCaptureCallback(0, zooCmd_osg::WindowCaptureCallback::READ_PIXELS, zooCmd_osg::WindowCaptureCallback::END_FRAME, GL_BACK);
	_wndCaptureCB->setCaptureOperation(new zooCmd_osg::WriteToFile(ZOO_DATA_ROOT_DIR + "tmp/screenshot/screen_shot", "jpg"));
	return true;
}

void BuiltinCmd::help(CmdUsage* usage)
{
	usage->setDescription("�ڽ�����: osg�������");
	usage->addCommandProcedureCall("home(int view_id)", "�������λ");
	usage->addCommandProcedureCall("focus(int ent_id, int ent_breed, int scene_id)", "���������õ�������������ʵ��");
	usage->addCommandProcedureCall("screenshot(int view_id, int count)", "����: count�ǽ���֡��, count==0��ʾ���ý���, count<0��ʾ���޴����Ľ���");
	usage->addCommandProcedureCall("posin(int view_id, int scene_id, bool show)", "��ʾ��ǰ�������������ϵ�к�����ģ�͵Ľ���λ��(Ctrl+Left Mouse Button)");
	usage->addCommandProcedureCall("rotate(int view_id, int ent_id, int ent_breed, int scene_id)", "ʹ����̬���ģ�ͽ�����ת");
	usage->addCommandProcedureCall("exit()", "�˳�����");
	usage->addCommandProcedureCall("bool(bool b) // bool& b", "���Բ�������ֵ");
	usage->addCommandProcedureCall("int(int i) // int& i", "��������������ֵ");
	usage->addCommandProcedureCall("float(float f) // float& f", "���Ե����ȸ���������ֵ");
	usage->addCommandProcedureCall("double(double d) // double& d", "����˫���ȸ���������ֵ");
	usage->addCommandProcedureCall("string(string str) // string& str", "�����ַ�������ֵ");
	usage->addCommandProcedureCall("tips(string tip)", "������ʾ��Ϣ");
}

void BuiltinCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	std::function<osgViewer::View*(int)> getView = [](int viewID) -> osgViewer::View*
	{
		osgViewer::View* pView = ServiceLocator<OsgDevice>::getService()->getView(viewID);
		if (!pView)
		{
			CmdManager::setTipMessage("��ͼ[" + std::to_string(viewID) + "]�����ڣ�");
			return nullptr;
		}

		return pView;
	};

	std::function<Entity*(int, int, int)> getEnt = [](int id, int breed, int sceneId) -> Entity*
	{
		Spawner* pSpawner = Spawner::find(sceneId);
		if (!pSpawner)
		{
			CmdManager::setTipMessage("����[" + std::to_string(sceneId) + "]�����ڣ�");
			return nullptr;
		}

		Entity* pEnt = pSpawner->gain(id, breed);
		if (!pEnt)
		{
			CmdManager::setTipMessage("ʵ��[" + std::to_string(id) + ":" + std::to_string(breed) + "]�����ڣ�");
			return nullptr;
		}

		return pEnt;
	};

	do
	{
		int viewid;
		if (cmdarg.read("home", viewid))
		{
			SignalTrigger::connect(subcmd, [viewid, getView]
			{
				osgViewer::View* pView = getView(viewid);
				if (pView)
					pView->getCameraManipulator()->home(2);
			});
			break;
		}

		int id, breed, sceneid;
		if (cmdarg.read("focus", id, breed, sceneid))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneid, getEnt]
			{
				Entity* pEnt = getEnt(id, breed, sceneid);
				if (!pEnt)
					return;

				if (pEnt->isSpawner())
				{
					pEnt->getComponentImpl<CameraImpl>()->_manipulatorMgr->home(2);
					return;
				}

				osg::Node* pFocusNode = pEnt->getComponentImpl<DoFImpl>()->_transWorld;
				pEnt->getSpawner()->getComponentImpl<CameraImpl>()->_manipulatorMgr->focus(pEnt->getSpawner()->getComponent<Camera>()->_manipulatorKey, pFocusNode);
			});
			break;
		}

		int count;
		if (cmdarg.read("screenshot", viewid, count))
		{
			SignalTrigger::connect(subcmd, [this, viewid, count, getView]
			{
				osgViewer::View* pView = getView(viewid);
				if (!pView)
					return;

				if (count == 0)
				{
					pView->getCamera()->setFinalDrawCallback(nullptr);
				}
				else
				{
					_wndCaptureCB->setFramesToCapture(count);
					pView->getCamera()->setFinalDrawCallback(_wndCaptureCB);
				}
			});
			break;
		}

		bool show;
		if (cmdarg.read("posin", viewid, sceneid, show))
		{
			SignalTrigger::connect(subcmd, [this, viewid, sceneid, show, getView]
			{
				osgViewer::View* pView = getView(viewid);
				if (!pView)
					return;

				OsgEarthContext* context = Spawner::find(sceneid) ? Spawner::find(sceneid)->getContext<OsgEarthContext>() : nullptr;
				if (!context)
				{
					CmdManager::setTipMessage("����[" + std::to_string(sceneid) + "]�����ڣ�");
					return;
				}

				if (show)
				{
					if (!_showCursorWPosHandler)
					{
						_showCursorWPosHandler = new ShowCursorWPosHandler(context);
						pView->addEventHandler(_showCursorWPosHandler.get());
					}
				}
				else
				{
					if (_showCursorWPosHandler)
					{
						pView->removeEventHandler(_showCursorWPosHandler.get());
						_showCursorWPosHandler = nullptr;
					}
				}
			});
			break;
		}

		if (cmdarg.read("rotate", viewid, id, breed, sceneid))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneid, viewid, getView, getEnt]
			{
				osgViewer::View* pView = getView(viewid);
				if (!pView)
					return;

				Entity* pEnt = getEnt(id, breed, sceneid);
				if (!pEnt)
					return;
			});
			break;
		}

		if (cmdarg.read("exit"))
		{
			SignalTrigger::connect(subcmd, []
			{
				ServiceLocator<OsgDevice>::getService()->as<RenderAdapter>()->setDone(true);
				exit(0);
			});
			break;
		}

		bool b;
		if (cmdarg.read("bool", b))
		{
			returnValue.setData("b", false);
			SignalTrigger::connect(subcmd, [b]
			{
				CmdManager::getSingleton().setReturnValue("b", b);
				zoo_info("return value is %s", b ? "true" : "false");
			});
			break;
		}

		int i;
		if (cmdarg.read("int", i))
		{
			returnValue.setData("i", 0);
			SignalTrigger::connect(subcmd, [i]
			{
				CmdManager::getSingleton().setReturnValue("i", i);
				zoo_info("return value is %d", i);
			});
			break;
		}

		float f;
		if (cmdarg.read("float", f))
		{
			returnValue.setData("f", 0.0f);
			SignalTrigger::connect(subcmd, [f]
			{
				CmdManager::getSingleton().setReturnValue("f", f);
				zoo_info("return value is %.3f", f);
			});
			break;
		}

		double d;
		if (cmdarg.read("double", d))
		{
			returnValue.setData("d", 0.0);
			SignalTrigger::connect(subcmd, [d]
			{
				CmdManager::getSingleton().setReturnValue("d", d);
				zoo_info("return value is %.6f", d);
			});
			break;
		}

		string str;
		if (cmdarg.read("string", str))
		{
			returnValue.setData("str", string(""));
			SignalTrigger::connect(subcmd, [str]
			{
				CmdManager::getSingleton().setReturnValue("str", string("\"") + str + "\"");
				zoo_info("return value is %s", (string("\"") + str + "\"").c_str());
			});
			break;
		}

		if (cmdarg.read("tips", str))
		{
			SignalTrigger::connect(subcmd, [str]
			{
				CmdManager::getSingleton().setTipMessage(str);
				zoo_info("tip message is %s", (string("\"") + str + "\"").c_str());
			});
			break;
		}
	} while (0);
}
