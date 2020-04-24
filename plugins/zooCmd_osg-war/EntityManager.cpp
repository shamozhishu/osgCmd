#include "EntityManager.h"
#include "WarCmd.h"
#include <zoo/Log.h>
#include <zoo/UserData.h>
#include <SharedObject/DoF.h>

EntityManager::EntityManager()
{
	_entNames[ENTITY_WEAPON] = "Weapon";
	_entNames[ENTITY_EFFECT] = "Effect";
	_entNames[ENTITY_REDARMY] = "RedArmy";
	_entNames[ENTITY_BLUEARMY] = "BluArmy";
	_entNames[ENTITY_ALLYARMY] = "AllyArmy";
	_entNames[ENTITY_STATIONARY] = "Stationary";
	_entNames[ENTITY_WARREPORTER] = "WarReporter";
}

EntityManager::~EntityManager()
{
}

void EntityManager::updateEntitiesDoF()
{
	ENTITY ent;
	auto it = _entities.begin();
	auto itEnd = _entities.end();
	for (; it != itEnd; ++it)
	{
		ent = it->second;
		ent._entTransform->setMatrix(osg::Matrix::scale(ent._dof->getScaleX(), ent._dof->getScaleY(), ent._dof->getScaleZ())
			* osg::Matrix::rotate(osg::DegreesToRadians(ent._dof->getRoll()), osg::Y_AXIS)
			* osg::Matrix::rotate(osg::DegreesToRadians(ent._dof->getPitch()), osg::X_AXIS)
			* osg::Matrix::rotate(osg::DegreesToRadians(ent._dof->getHeading()), osg::Z_AXIS)
			* osg::Matrix::translate(ent._dof->getPosX(), ent._dof->getPosY(), ent._dof->getPosZ()));
	}
}

ENTITY EntityManager::findEnt(int id, ENTITY_TYPE type)
{
	ENTITY ent;
	long long i64ID = type;
	i64ID = i64ID << 32 | id;

	auto it = _entities.find(i64ID);
	if (it != _entities.end())
		ent = it->second;

	return ent;
}

bool EntityManager::isEmpty() const
{
	return _entities.size() == 0;
}

void EntityManager::onCreateWeapon(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_WEAPON);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
		ent.m_lineSeg = new osgUtil::LineSegmentIntersector(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0.0, 0.0, 0.0));
	}
}

void EntityManager::onCreateEffect(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_EFFECT);
	if (ent._dof)
	{
		ent._switchNode = new osg::Switch;
		ent._switchNode->addChild(ent._entNode);
		ent._switchNode->setAllChildrenOff();
		ent._entGroup->addChild(ent._switchNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
	}
}

void EntityManager::onCreateRedArmy(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_REDARMY);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onCreateBlueArmy(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_BLUEARMY);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onCreateAllyArmy(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_ALLYARMY);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onCreateStationary(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_STATIONARY);
	if (ent._dof)
	{
		const char* vertexShader =
		{
			"void main(void) {\n"
			"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
			"   gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;\n"
			"}\n"
		};

		const char* fragShader =
		{
			"uniform sampler2D baseTexture;\n"
			"void main(void) {\n"
			"   vec2 coord = gl_TexCoord[0].xy;\n"
			"   vec4 C = texture2D(baseTexture, coord)\n;"
			"	gl_FragColor = C;\n"
			"}\n"
		};

		osg::ref_ptr<osg::Program> program = new  osg::Program();
		program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShader));
		program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragShader));
		ent._entNode->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);

		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onChangeParentDoF(const UserData& userdata)
{
	int mark = any_cast<int>(userdata.getData());
	int id = any_cast<int>(userdata.getData("id"));
	int group = any_cast<int>(userdata.getData("group"));
	int parent_id = any_cast<int>(userdata.getData("parent_id"));
	int parent_group = any_cast<int>(userdata.getData("parent_group"));

	ENTITY ent, parentEnt;

	long long i64ID = group;
	i64ID = i64ID << 32 | id;
	auto it = _entities.find(i64ID);
	if (it != _entities.end())
		ent = it->second;

	i64ID = parent_group;
	i64ID = i64ID << 32 | parent_id;
	it = _entities.find(i64ID);
	if (it != _entities.end())
		parentEnt = it->second;

	if (ent._dof)
	{
		switch (mark)
		{
		case 0:
			if (parentEnt._dof)
			{
				WarCmd::getSingleton().getMainNode()->removeChild(ent._entTransform);
				parentEnt._entTransform->addChild(ent._entTransform);
			}
			else
			{
				WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
			}
			break;
		case 1:
			if (parentEnt._dof)
				parentEnt._entTransform->removeChild(ent._entTransform);
			else
				WarCmd::getSingleton().getMainNode()->removeChild(ent._entTransform);
			break;
		}
	}
}

ENTITY EntityManager::addEntity(const UserData& userdata, ENTITY_TYPE entType)
{
	ENTITY ent;
	int id = any_cast<int>(userdata.getData("id"));
	ent._dof = SharedObjectPool::findObject<DoF>(id, entType);
	if (!ent._dof)
	{
		zoo_error("Entity[id:%d][type:%s]: DoF does not exist.", id, _entNames[entType]);
		return ent;
	}

	string filename = any_cast<string>(userdata.getData("model_file"));
	filename = ZOOCMD_DATA_DIR + filename;
	ent._entNode = osgDB::readNodeFile(filename);
	if (!ent._entNode)
	{
		zoo_error("Model file [%s] load failed", filename.c_str());
		ent._dof = nullptr;
		return ent;
	}

	ent._entGroup = new osg::Group;
	ent._entTransform = new osg::MatrixTransform;

	long long i64ID = entType;
	i64ID = i64ID << 32 | id;
	_entities[i64ID] = ent;
	return ent;
}
