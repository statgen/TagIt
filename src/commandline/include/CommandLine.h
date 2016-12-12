#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "../../auxiliary/include/auxiliary.h"
#include "../../exception/include/Exception.h"

using namespace std;

class CommandLine {
protected:
	unordered_set<const char*, unsigned long int(*)(const char*), bool(*)(const char*, const char*)> commands_dictionary;
	unordered_set<const char*, unsigned long int(*)(const char*), bool(*)(const char*, const char*)>::iterator commands_dictionary_it;

	unordered_map<const char*, vector<const char*>*, unsigned long int(*)(const char*), bool(*)(const char*, const char*)> command_line;
	unordered_map<const char*, vector<const char*>*, unsigned long int(*)(const char*), bool(*)(const char*, const char*)>::iterator command_line_it;

	virtual void initialize_commands_dictionary();

public:
	static const char* HELP;

	CommandLine();
	virtual ~CommandLine();

	void read_command_line(int args, char** argv) throw (Exception);

	virtual void parse_command_line() throw (Exception) = 0;
};

#endif
