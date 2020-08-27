﻿#include "WeatherEffect.h"
#include <zooCmd_osg/OsgEarthUtils.h>
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgParticle;

WeatherEffect::WeatherEffect()
	: _islonlat(false)
	, _isLocal(false)
	, _weatherHeight(20000)
{
}

WeatherEffect::WeatherEffect(const WeatherEffect& copy, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/)
	: PrecipitationEffect(copy, copyop)
{
}

WeatherEffect::~WeatherEffect()
{
}

void WeatherEffect::update()
{
	PrecipitationEffect::update();
	if (_isLocal)
	{
		OsgEarthUtils* pOsgEarthUtils = ServiceLocator<OsgEarthUtils>::getService();
		osgEarth::MapNode* pMapNode = ServiceLocator<OsgEarthContext>::getService()->getOpMapNode();
		if (pOsgEarthUtils && pMapNode)
		{
			osg::Vec3d worldXYZ;
			bool succ = _islonlat ? pOsgEarthUtils->convertLatLongHeightToXYZ(_weatherPos.y(), _weatherPos.x(), _weatherPos.z(), worldXYZ.x(), worldXYZ.y(), worldXYZ.z()) : false;
			if (succ)
			{
				float meanPerimeter_2 = osg::PI * worldXYZ.length();
				float meanRange = 360.0 * _weatherPos.z() / meanPerimeter_2;
				_weatherRange = osg::Vec4(_weatherPos.x() - meanRange, _weatherPos.x() + meanRange, _weatherPos.y() - meanRange, _weatherPos.y() + meanRange);
			}
			else
			{
				OSG_INFO << "MapNode Is NULL!" << std::endl;
				_isLocal = false;
			}
		}
		else
		{
			OSG_INFO << "MapNode Is NULL!" << std::endl;
			_isLocal = false;
		}
	}
}

bool WeatherEffect::build(const osg::Vec3 eyeLocal, int i, int j, int k, float startTime,
	PrecipitationDrawableSet& pds, osg::Polytope& frustum, osgUtil::CullVisitor* cv) const
{
	osg::Vec3 position = _origin + osg::Vec3(float(i)*_du.x(), float(j)*_dv.y(), float(k)*_dw.z());

	double lon = 180, lat = 90, hei = 0;
	OsgEarthUtils* pOsgEarthUtils = ServiceLocator<OsgEarthUtils>::getService();
	osgEarth::MapNode* pMapNode = ServiceLocator<OsgEarthContext>::getService()->getOpMapNode();
	if (pOsgEarthUtils && pMapNode)
	{
		osg::Vec3d vecLL;
		pOsgEarthUtils->convertXYZToLatLongHeight(position.x(), position.y(), position.z(), vecLL.y(), vecLL.x(), vecLL.z());
		lon = vecLL.x();
		lat = vecLL.y();
		hei = vecLL.z();
	}
	
	if (_isLocal)
	{
		if (!(lon < _weatherRange.y() && lon > _weatherRange.x() && lat > _weatherRange.z() && lat < _weatherRange.w()))
			return false;
	}

	// 判断该高度是否允许产生天气效果
	if (hei > _weatherHeight)
		return false;

	build(eyeLocal, i, j, k, startTime, pds, frustum, cv);
}
