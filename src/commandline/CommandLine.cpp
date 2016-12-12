#include "include/CommandLine.h"

const char* CommandLine::HELP = "--help";

CommandLine::CommandLine() :
	commands_dictionary(100u, auxiliary::str_jenkins_hash_case_insensitive, auxiliary::bool_str_equal_to_case_insensitive),
	command_line(100u, auxiliary::str_jenkins_hash_case_insensitive, auxiliary::bool_str_equal_to_case_insensitive) {

}

CommandLine::~CommandLine() {
	command_line_it = command_line.begin();
	while (command_line_it != command_line.end()) {
		delete command_line_it->second;
		++command_line_it;
	}
	command_line.clear();
}

void CommandLine::initialize_commands_dictionary() {
	commands_dictionary.insert(HELP);
}

void CommandLine::read_command_line(int args, char** argv) throw (Exception) {
	vector<const char*>* arguments = NULL;

	initialize_commands_dictionary();

	for (int arg = 1; arg < args; ++arg) {
		commands_dictionary_it = commands_dictionary.find(argv[arg]);
		if (commands_dictionary_it != commands_dictionary.end()) {
			if (command_line.find(argv[arg]) != command_line.end()) {
				throw Exception(NULL, 0, "Incorrect command line: multiple '%s' commands.\nSpecify '%s' for the command line description.", argv[arg], HELP);
			}
			arguments = new vector<const char*>();
			command_line.insert(pair<const char*, vector<const char*>*>(argv[arg], arguments));
		} else if (arguments != NULL) {
			arguments->push_back(argv[arg]);
		} else {
			throw Exception(NULL, 0, "Incorrect command line: unrecognized '%s' command.\nSpecify '%s' for the command line description.", argv[arg], HELP);
		}
	}

	if (command_line.size() == 0u) {
		throw Exception(NULL, 0, "No commands were specified.\nSpecify '%s' for the command line description.", HELP);
	}
}
