#include "BuiltinCmd.h"
#include "InputDevice.h"
#include <zoo/Utils.h>
#include <zoo/Component.h>
#include <zooCmd/CmdManager.h>
#include <zooCmd_osg/OsgEarthContext.h>
#include "OsgComponentImpls.h"
#include "BuiltinEventHandler.h"
#include "WindowCaptureCallback.h"

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
	usage->addCommandProcedureCall("home(string view_name)", "�������λ");
	usage->addCommandProcedureCall("focus(int ent_id, int ent_breed, int scene_id)", "���������õ�������������ʵ��");
	usage->addCommandProcedureCall("screenshot(string view_name, int count)", "����: count�ǽ���֡��, count==0��ʾ���ý���, count<0��ʾ���޴����Ľ���");
	usage->addCommandProcedureCall("posin(string view_name, bool show)", "��ʾ��ǰ�������������ϵ�к�����ģ�͵Ľ���λ��(Ctrl+Left Mouse Button)");
	usage->addCommandProcedureCall("rotate(string view_name, int ent_id, int ent_breed, int scene_id)", "ʹ����̬���ģ�ͽ�����ת");
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
	std::function<osgViewer::View*(string)> getView = [](string viewName) -> osgViewer::View*
	{
		osgViewer::View* pView = ServiceLocator<OsgDevice>::getService()->getView(viewName);
		if (!pView)
		{
			CmdManager::setTipMessage("��ͼ[" + viewName + "]�����ڣ�");
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
		string viewname;
		if (cmdarg.read("home", viewname))
		{
			SignalTrigger::connect(subcmd, [viewname, getView]
			{
				osgViewer::View* pView = getView(viewname);
				if (pView)
					pView->getCameraManipulator()->home(2);
			});
			break;
		}

		int id, breed, sceneId;
		if (cmdarg.read("focus", id, breed, sceneId))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneId, getEnt]
			{
				Entity* pEnt = getEnt(id, breed, sceneId);
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
		if (cmdarg.read("screenshot", viewname, count))
		{
			SignalTrigger::connect(subcmd, [this, viewname, count, getView]
			{
				osgViewer::View* pView = getView(viewname);
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
		if (cmdarg.read("posin", viewname, show))
		{
			SignalTrigger::connect(subcmd, [this, viewname, show, getView]
			{
				osgViewer::View* pView = getView(viewname);
				if (!pView)
					return;

				if (show)
				{
					if (!_showCursorWPosHandler)
					{
						_showCursorWPosHandler = new ShowCursorWPosHandler;
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

		if (cmdarg.read("rotate", viewname, id, breed, sceneId))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneId, viewname, getView, getEnt]
			{
				osgViewer::View* pView = getView(viewname);
				if (!pView)
					return;

				Entity* pEnt = getEnt(id, breed, sceneId);
				if (!pEnt)
					return;
			});
			break;
		}

		if (cmdarg.read("exit"))
		{
			SignalTrigger::connect(subcmd, []
			{
				ServiceLocator<OsgDevice>::getService()->as<InputAdapter>()->setDone(true);
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
