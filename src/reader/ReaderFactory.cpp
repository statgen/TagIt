#include "include/ReaderFactory.h"

ReaderFactory::ReaderFactory() {

}

ReaderFactory::~ReaderFactory() {

}

bool ReaderFactory::is_gzip(const char* file_name) throw (Exception) {
	ifstream ifile_stream;
	char characters[2] = {'\x00', '\x00'};

	ifile_stream.exceptions(ifstream::failbit | ifstream::badbit);
	ifile_stream.setf(ifstream::skipws);

	try {
		ifile_stream.open(file_name, ios::binary);
	} catch (ifstream::failure &e) {
		throw Exception(__FILE__, __LINE__, "Error while opening '%s' file.", file_name);
	}

	try {
		ifile_stream.read(characters, 2);
	} catch (ifstream::failure &e) {
		if (!ifile_stream.eof()) {
			throw Exception(__FILE__, __LINE__, "Error while reading '%s' file.", file_name);
		}
	}

	try {
		ifile_stream.close();
	} catch (ifstream::failure &e) {
		throw Exception(__FILE__, __LINE__, "Error while closing '%s' file.", file_name);
	}

	if ((characters[0] == '\x1F') && (characters[1] == '\x8B')) {
		return true;
	}

	return false;
}

Reader* ReaderFactory::create(const char* file_name) throw (Exception) {
	Reader* reader = NULL;

	try {
		if (is_gzip(file_name)) {
			reader = new GzipReader();
		} else {
			reader = new TextReader();
		}
		reader->set_file_name(file_name);
	} catch (Exception &e) {
		e.add_message(__FILE__, __LINE__, "Error while initializing reading facilities for '%s' file.", file_name);
		throw;
	}

	return reader;
}
