#ifndef WRITER_H_
#define WRITER_H_

#include "../../exception/include/Exception.h"
#include <cstdarg>
#include <cstring>

using namespace std;

class Writer {
protected:
	char* file_name;

	Writer();

public:
	virtual ~Writer();

	void set_file_name(const char* file_name) throw (Exception);
	const char* get_file_name();

	virtual void open() throw (Exception) = 0;
	virtual void close() throw (Exception) = 0;
	virtual void write(const char* format, ...) throw (Exception) = 0;
};

#endif
