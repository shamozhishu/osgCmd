#include "WarSimulator.h"
#include "LuaScript.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>

SimState::SimState()
	: _script(new LuaScript)
	, _scriptValid(false)
{
}

SimState::~SimState()
{
	SAFE_DELETE(_script);
}

bool SimState::init(string scriptFile)
{
	_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + scriptFile);
	return _scriptValid;
}

SimState* SimState::handle(ESimState simState)
{
	if (_scriptValid)
		_script->executeGlobalFunction("Handle");

	return &WarSimulator::_SimStates[simState];
}

void SimState::update()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Update");
}

void SimState::enter()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Enter");
}

void SimState::exit()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Exit");
}

SimState WarSimulator::_SimStates[Count_];
WarSimulator::WarSimulator()
	: _state(nullptr)
{
}

void WarSimulator::update()
{
	if (_state)
		_state->update();
}

void WarSimulator::handle(ESimState simState)
{
	if (_state)
	{
		_state->exit();
		_state = _state->handle(simState);
	}

	if (_state)
		_state->enter();
}