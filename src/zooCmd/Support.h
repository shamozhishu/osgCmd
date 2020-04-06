#pragma once

#include <zoo/Support.h>

#if defined(ZOOCMD_NONCLIENT_BUILD)
#	define _zooCmdExport __declspec(dllexport)
#else
#	if defined(__MINGW32__)
#		define _zooCmdExport
#	else
#		define _zooCmdExport __declspec(dllimport)
#	endif
#endif

using namespace zoo;

namespace zooCmd {

class Cmd;
class BuiltinCmd;
class CmdManager;
class AppUsage;
class CmdParser;
class InputAdapter;
template class _zooCmdExport ReflexFactory<>;
_zooCmdExport extern Interlock g_interlock;
_zooCmdExport extern std::string ansi_data_dir;
_zooCmdExport extern std::string utf8_data_dir;
_zooCmdExport extern std::map<int, int> g_keyboardMap;

}
