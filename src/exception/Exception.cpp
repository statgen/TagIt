#include "include/Exception.h"

const unsigned int Exception::MESSAGE_BUFFER_SIZE = 16384u;

Exception::Exception(const char* source, int source_line, const char* text_message, ... ): exception() {
	va_list arguments;

	va_start(arguments, text_message);
	add_message(source, source_line, text_message, arguments);
	va_end(arguments);
}

Exception::~Exception() throw() {
	list<message*>::iterator trace_it;

	trace_it = trace.begin();
	while (trace_it != trace.end()) {
		if ((*trace_it)->text != NULL) {
			free((*trace_it)->text);
			(*trace_it)->text = NULL;
			(*trace_it)->source = NULL;
		}
		delete *trace_it;

		trace_it++;
	}

	trace.clear();
}

void Exception::format_message_text(char** text, const char* text_template, va_list arguments) {
	if (*text != NULL) {
		free(*text);
		*text = NULL;
	}

	*text = (char*)malloc(MESSAGE_BUFFER_SIZE * sizeof(char));
	if (*text != NULL) {
		if (vsprintf(*text, text_template, arguments) < 0) {
			free(*text);
			*text = NULL;
		}
	}
}

void Exception::add_message(const char* source, int source_line, const char* text_message, va_list arguments) {
	message* msg = new message();

	format_message_text(&(msg->text), text_message, arguments);
	msg->source = source;
	msg->source_line = source_line;

	trace.push_front(msg);
}

void Exception::add_message(const char* source, int source_line, const char* text_message, ... ) {
	va_list arguments;

	va_start(arguments, text_message);
	add_message(source, source_line, text_message, arguments);
	va_end(arguments);
}

const char* Exception::what() const throw() {
	stringstream string_stream;
	list<message*>::const_iterator trace_it;

	string_stream << setfill(' ');
	trace_it = trace.begin();
	while (trace_it != trace.end()) {
		if ((*trace_it)->source != NULL) {
			string_stream << (*trace_it)->source;
			if ((*trace_it)->source_line != 0) {
				string_stream << " (" << (*trace_it)->source_line << ")";
			}
			string_stream << ": ";
		} else if ((*trace_it)->source_line != 0) {
			string_stream << "(" << (*trace_it)->source_line << "): ";
		}

		if ((*trace_it)->text != NULL) {
			string_stream << (*trace_it)->text;
		} else {
			string_stream << "Exception.";
		}

		string_stream << endl;
		trace_it++;
	}

	string_stream.flush();
	trace_string = string_stream.str();

	return trace_string.c_str();
}
