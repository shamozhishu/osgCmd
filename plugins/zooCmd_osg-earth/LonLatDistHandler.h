#ifndef __LON_LAT_DIST_HANDLER_H__
#define __LON_LAT_DIST_HANDLER_H__

#include <zooCmd_osg/OsgDevice.h>

class EarthCmd;
class EarthControls;
class OsgEarthContext;
class LonLatDistHandler : public osgGA::GUIEventHandler
{
public:
	LonLatDistHandler(OsgEarthContext* context, EarthControls* controls);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);
	void setMapNode(osgEarth::MapNode* mapNode);
	void setEarthManipulator(osgEarth::Util::EarthManipulator* manipulator);

private:
	osg::NodePath _nodePath;
	EarthControls* _controls;
	OsgEarthContext* _context;
	osg::observer_ptr<osgEarth::MapNode> _mapNode;
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};

#endif // __LON_LAT_DIST_HANDLER_H__
