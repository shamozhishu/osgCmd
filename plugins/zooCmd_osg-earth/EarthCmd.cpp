#include "EarthCmd.h"
#include "EarthControls.h"
#include <zoo/Utils.h>
#include <zoo/Sigslot.h>
#include <zooCmd/CmdManager.h>
#include "LocateModelHandler.h"
#include "MeasureDistHandler.h"
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth;

ZOO_REGISTER(EarthCmd)
ZOO_REFLEX_IMPLEMENT(EarthCmd);

EarthCmd::EarthCmd()
	: _osgEarthContext(nullptr)
{
}

EarthCmd::~EarthCmd()
{
}

bool EarthCmd::init()
{
	_osgEarthContext = ServiceLocator<OsgEarthContext>::getService();
	_lonLatHandler = new LonLatDistHandler;
	return true;
}

void EarthCmd::handle(const zooCmd::Event& evt)
{
	if (evt.getTopic() == zooCmd_osg::EVENT_RESET_OSGEARTH_CONTEXT)
	{
		EarthControls::destroy();
		osgViewer::View* pView = _osgEarthContext->getOpView();
		if (pView)
			pView->removeEventHandler(_lonLatHandler.get());
	}
}

void EarthCmd::help(CmdUsage* usage)
{
	usage->setDescription("earth command: osgearth��ص�������");
	usage->addCommandProcedureCall("lla(bool show)", "��ʾ����γ�Ⱥͺ���");
	usage->addCommandProcedureCall("fly(float longitude, float latitude, float distance)", "���ӵ�ת�Ƶ�ָ������γ�Ⱥ͸߶ȴ�");
	usage->addCommandProcedureCall("dist()", "���ز��");
	usage->addCommandProcedureCall("locate(string model, float height, float scale, bool repeat)", "�ڵ����Ϸ���ģ��");
}

void EarthCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	std::function<bool(osgViewer::View*&, osgEarth::MapNode*&, osgEarth::Util::EarthManipulator*&)> testError = [this]
	(osgViewer::View*& pView, osgEarth::MapNode*& pMapNode, osgEarth::Util::EarthManipulator*& pManipulator)
	{
		pView = _osgEarthContext->getOpView();
		if (!pView)
		{
			CmdManager::setTipMessage("��ͼ�����ڣ�");
			return true;
		}

		pMapNode = _osgEarthContext->getOpMapNode();
		pManipulator = _osgEarthContext->getOpManipulator();
		if (!pMapNode || !pManipulator)
		{
			CmdManager::setTipMessage("���ڳ��������һ����Ч�ĵ��������");
			return true;
		}

		return false;
	};

	do
	{
		bool show;
		if (cmdarg.read("lla", show))
		{
			SignalTrigger::connect(subcmd, [this, show, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				if (show)
				{
					_lonLatHandler->setMapNode(pMapNode);
					_lonLatHandler->setEarthManipulator(pManipulator);
					pView->addEventHandler(_lonLatHandler.get());
				}
				else
				{
					pView->removeEventHandler(_lonLatHandler.get());
					EarthControls::getIns()->removeLabelTextDisplay(lla_label_);
					EarthControls::getIns()->removeLabelTextDisplay(ipt_label_);
				}
			});
			break;
		}

		float lon, lat, dist;
		if (cmdarg.read("fly", lon, lat, dist))
		{
			SignalTrigger::connect(subcmd, [this, lon, lat, dist, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				Viewpoint vp = pManipulator->getViewpoint();
				vp.focalPoint().mutable_value().x() = lon;
				vp.focalPoint().mutable_value().y() = lat;
				vp.range() = dist;
				pManipulator->setViewpoint(vp, 2);
			});
			break;
		}

		if (cmdarg.read("dist"))
		{
			SignalTrigger::connect(subcmd, [this, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				osg::ref_ptr<MeasureDistHandler> measureDistanceHandler = new MeasureDistHandler(pMapNode, pManipulator);
				pView->addEventHandler(measureDistanceHandler.get());
				CmdManager::getSingleton().block(true);
				EarthControls::getIns()->removeLabelTextDisplay(dist_label_);
				pView->removeEventHandler(measureDistanceHandler.get());
			});
			break;
		}

		string model; float height, scale; bool repeat;
		if (cmdarg.read("locate", model, height, scale, repeat))
		{
			SignalTrigger::connect(subcmd, [this, model, height, scale, repeat, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + model);
				if (!node)
				{
					CmdManager::setTipMessage("ģ���ļ�[" + model + "]����ʧ�ܣ�");
					return;
				}

				osg::ref_ptr<LocateModelHandler> locateModelHandler = new LocateModelHandler(pMapNode, node, height, scale, repeat);
				pView->addEventHandler(locateModelHandler.get());
				CmdManager::getSingleton().block(true);
				pView->removeEventHandler(locateModelHandler.get());
			});
			break;
		}
	} while (0);
}
