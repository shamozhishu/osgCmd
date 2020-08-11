#include "BuiltinCmd.h"
#include "InputDevice.h"
#include <zoo/Component.h>
#include <zooCmd/CmdManager.h>
#include "OsgComponentImpls.h"

using namespace zooCmd;

ZOO_REFLEX_IMPLEMENT(BuiltinCmd);

void BuiltinCmd::help(CmdUsage* usage)
{
	usage->setDescription("�ڽ�����: ���������ܵķ�װ");
	usage->addCommandProcedureCall("home(string view_name)", "�������λ");
	usage->addCommandProcedureCall("focus(int ent_id, int ent_breed, int scene_id)", "���������õ�������������ʵ��");
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
	do
	{
		string viewname;
		if (cmdarg.read("home", viewname))
		{
			SignalTrigger::connect(subcmd, [viewname]
			{
				osgViewer::View* pView = ServiceLocator<OsgDevice>::getService()->getView(viewname);
				if (!pView)
				{
					CmdManager::setTipMessage("��ͼ[" + viewname + "]�����ڣ�");
					return;
				}

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
