#include "WarCommander.h"
#include "Battlefield.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <QCoreApplication>
#include <QFile>
#include <zoo/Log.h>
#include <zoo/Utils.h>
#include "LuaExportClass.h"

using namespace zoo;

WarCommander::WarCommander(string cmd, string table)
	: _cmd(cmd)
	, _battlefieldTable(nullptr)
	, _currentBattlefield(nullptr)
{
	new DatabaseCSV();
	new Input();
	string csvTablePath = table;
	size_t pos = csvTablePath.find_last_of(L'/', csvTablePath.length());
	csvTablePath = csvTablePath.substr(0, pos + 1);
	DatabaseCSV::getSingleton().init(csvTablePath);
	_battlefieldTable = DatabaseCSV::getSingleton().getTable(table);
	connect(&_tickTimer, SIGNAL(timeout()), this, SLOT(tick()));
	_tickTimer.start(20);
}

WarCommander::~WarCommander()
{
	_tickTimer.stop();
	SAFE_DELETE(_currentBattlefield);
	delete Input::getSingletonPtr();
	delete DatabaseCSV::getSingletonPtr();
}

const char* WarCommander::getCmd() const
{
	return _cmd.c_str();
}

bool WarCommander::enterBattlefield(int id)
{
	if (_currentBattlefield && _currentBattlefield->getID() == id)
		return true;

	if (!_battlefieldTable)
	{
		zoo_error("ս��CSV������");
		return false;
	}

	SAFE_DELETE(_currentBattlefield);
	_currentBattlefield = new Battlefield(id, _battlefieldTable);
	if (!_currentBattlefield->load())
	{
		SAFE_DELETE(_currentBattlefield);
		zoo_error("���س���[%d]ʧ��", id);
		return false;
	}

	return true;
}

void WarCommander::exitCurBattlefield()
{
	if (_currentBattlefield)
	{
		delete _currentBattlefield;
		_currentBattlefield = nullptr;
	}
}

void WarCommander::saveCurBattlefield()
{
	if (!_currentBattlefield)
	{
		zoo_warning("��ǰû��Ҫ�����ս��");
		return;
	}

	_currentBattlefield->save();
}

Battlefield* WarCommander::getCurBattlefield()
{
	return _currentBattlefield;
}

void WarCommander::tick()
{
	zooCmd_Refresh();

	Input::getSingleton().poll();
	if (_currentBattlefield)
		_currentBattlefield->stepping();

	zooCmd_Render();
}
