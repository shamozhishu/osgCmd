#include "BuiltinCmd.h"
#include "InputDevice.h"
#include <zoo/Utils.h>
#include <zoo/Component.h>
#include <zooCmd/CmdManager.h>
#include "OsgComponentImpls.h"

using namespace zooCmd;

ZOO_REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	_screenshotHandler = new osgViewer::ScreenCaptureHandler(new osgViewer::ScreenCaptureHandler::WriteToFile(ZOO_DATA_ROOT_DIR + "tmp/screenshot/screen_shot", "jpg"), 0);
	_screenshotHandler->setKeyEventTakeScreenShot(0);
	_screenshotHandler->setKeyEventToggleContinuousCapture(0);
	return true;
}

void BuiltinCmd::help(CmdUsage* usage)
{
	usage->setDescription("�ڽ�����: osg�������");
	usage->addCommandProcedureCall("home(string view_name)", "�������λ");
	usage->addCommandProcedureCall("focus(int ent_id, int ent_breed, int scene_id)", "���������õ�������������ʵ��");
	usage->addCommandProcedureCall("screenshot(string view_name, int count)", "����: count�ǽ���֡��, count==0��ʾ���ý���, count<0��ʾ���޴����Ľ���");
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
	std::function<bool(string, osgViewer::View*&)> hasView = [](string viewName, osgViewer::View*& outView)
	{
		outView = ServiceLocator<OsgDevice>::getService()->getView(viewName);
		if (!outView)
		{
			CmdManager::setTipMessage("��ͼ[" + viewName + "]�����ڣ�");
			return false;
		}

		return true;
	};

	do
	{
		string viewname;
		if (cmdarg.read("home", viewname))
		{
			SignalTrigger::connect(subcmd, [viewname, hasView]
			{
				osgViewer::View* pView = nullptr;
				if (!hasView(viewname, pView))
					return;

				pView->getCameraManipulator()->home(2);
			});
			break;
		}

		int id, breed, sceneId;
		if (cmdarg.read("focus", id, breed, sceneId))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneId]
			{
				Spawner* pSpawner = Spawner::find(sceneId);
				if (!pSpawner)
				{
					CmdManager::setTipMessage("����[" + std::to_string(sceneId) + "]�����ڣ�");
					return;
				}

				Entity* pEnt = pSpawner->gain(id, breed);
				if (!pEnt)
				{
					CmdManager::setTipMessage("ʵ��[" + std::to_string(id) + ":" + std::to_string(breed) + "]�����ڣ�");
					return;
				}

				if (pEnt->isSpawner())
				{
					pSpawner->getComponentImpl<CameraImpl>()->_manipulatorMgr->home(2);
					return;
				}

				osg::Node* pFocusNode = pEnt->getComponentImpl<DoFImpl>()->_transWorld;
				pSpawner->getComponentImpl<CameraImpl>()->_manipulatorMgr->focus(pSpawner->getComponent<Camera>()->_manipulatorKey, pFocusNode);
			});
			break;
		}

		int count;
		if (cmdarg.read("screenshot", viewname, count))
		{
			SignalTrigger::connect(subcmd, [this, viewname, count, hasView]
			{
				osgViewer::View* pView = nullptr;
				if (!hasView(viewname, pView))
					return;

				if (count == 0)
				{
					_screenshotHandler->stopCapture();
				}
				else
				{
					_screenshotHandler->setFramesToCapture(count);
					pView->addEventHandler(_screenshotHandler);
					_screenshotHandler->startCapture();
				}
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
