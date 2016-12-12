#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <limits>
#include <list>
#include <cstdarg>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

class Exception: public exception {
private:
	static const unsigned int MESSAGE_BUFFER_SIZE;

	struct message {
		char* text;
		const char* source;
		int source_line;
		message(): text(NULL), source(NULL), source_line(numeric_limits<int>::min()) {};
	};

	list<message*> trace;
	mutable string trace_string;

	void format_message_text(char** text, const char* text_template, va_list arguments);
	void add_message(const char* source, int source_line, const char* text_message, va_list arguments);

public:
	Exception(const char* source, int source_line, const char* text_message, ... );
	virtual ~Exception() throw();
	void add_message(const char* source, int source_line, const char* text_message, ... );
	virtual const char* what() const throw();
};

#endif
