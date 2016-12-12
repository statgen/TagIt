#ifndef READER_H_
#define READER_H_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "../../auxiliary/include/auxiliary.h"
#include "../../exception/include/Exception.h"

using namespace std;

class Reader {
protected:
	char* file_name;
	bool compressed;

	Reader(char** buffer);

public:
	char* const* line;

	virtual ~Reader();

	void set_file_name(const char* file_name) throw (Exception);
	const char* get_file_name();

	virtual void open() throw (Exception) = 0;
	virtual void close() throw (Exception) = 0;
	virtual int read_line() throw (Exception) = 0;
	virtual void reset() throw (Exception) = 0;
	virtual bool eof() = 0;
	virtual bool is_compressed() = 0;
};

#endif
