#pragma once

#include <zooCmd/Support.h>

namespace zooCmd {

class _zooCmdExport AppUsage
{
public:
    static AppUsage* instance();
    AppUsage() {}
    AppUsage(const std::string& commandLineUsage);

    typedef std::map<std::string,std::string> UsageMap;

    /** The AppName is often displayed when logging errors, and frequently incorporated into the Description (below). */
    void setAppName(const std::string& name) { _appName = name; }
    const std::string& getAppName() const { return _appName; }

    /** If non-empty, the Description is typically shown by the Help Handler
        * as text on the Help display (which also lists keyboard abbreviations. */
    void setDescription(const std::string& desc) { _description = desc; }
    const std::string& getDescription() const { return _description; }

    enum Type
    {
        NO_HELP = 0x0,
        COMMAND_LINE_OPTION = 0x1,
        ENVIRONMENTAL_VARIABLE = 0x2,
        KEYBOARD_MOUSE_BINDING = 0x4,
        HELP_ALL = KEYBOARD_MOUSE_BINDING|ENVIRONMENTAL_VARIABLE|COMMAND_LINE_OPTION
    };

    void addUsageExplanation(Type type,const std::string& option,const std::string& explanation);

    void setCommandLineUsage(const std::string& explanation) { _commandLineUsage=explanation; }
    const std::string& getCommandLineUsage() const { return _commandLineUsage; }

    void addCommandLineOption(const std::string& option,const std::string& explanation, const std::string &defaultValue = "");

    void setCommandLineOptions(const UsageMap& usageMap) { _commandLineOptions=usageMap; }
    const UsageMap& getCommandLineOptions() const { return _commandLineOptions; }

    void setCommandLineOptionsDefaults(const UsageMap& usageMap) { _commandLineOptionsDefaults=usageMap; }
    const UsageMap& getCommandLineOptionsDefaults() const { return _commandLineOptionsDefaults; }

    void addEnvironmentalVariable(const std::string& option,const std::string& explanation, const std::string& defaultValue = "");

    void setEnvironmentalVariables(const UsageMap& usageMap) { _environmentalVariables=usageMap; }
    const UsageMap& getEnvironmentalVariables() const { return _environmentalVariables; }

    void setEnvironmentalVariablesDefaults(const UsageMap& usageMap) { _environmentalVariablesDefaults=usageMap; }
    const UsageMap& getEnvironmentalVariablesDefaults() const { return _environmentalVariablesDefaults; }

    void addKeyboardMouseBinding(const std::string& prefix, int key, const std::string& explanation);
    void addKeyboardMouseBinding(int key, const std::string& explanation);
    void addKeyboardMouseBinding(const std::string& option,const std::string& explanation);

    void setKeyboardMouseBindings(const UsageMap& usageMap) { _keyboardMouse=usageMap; }
    const UsageMap& getKeyboardMouseBindings() const { return _keyboardMouse; }

    void getFormattedString(std::string& str, const UsageMap& um,unsigned int widthOfOutput=80,bool showDefaults=false,const UsageMap& ud=UsageMap());
    void write(std::ostream& output,const UsageMap& um,unsigned int widthOfOutput=80,bool showDefaults=false,const UsageMap& ud=UsageMap());
    void write(std::ostream& output,unsigned int type=COMMAND_LINE_OPTION, unsigned int widthOfOutput=80,bool showDefaults=false);
    void writeEnvironmentSettings(std::ostream& output);

protected:
	std::string _appName;
	std::string _description;
	UsageMap    _keyboardMouse;
	std::string _commandLineUsage;
	UsageMap    _commandLineOptions;
	UsageMap    _environmentalVariables;
	UsageMap    _commandLineOptionsDefaults;
	UsageMap    _environmentalVariablesDefaults;
};

}
