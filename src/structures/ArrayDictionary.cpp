#include "include/ArrayDictionary.h"

ArrayDictionary::ArrayDictionary(unsigned int bucket_size, unsigned int heap_initial_size, unsigned int heap_increment) throw (Exception):
	bucket_size(bucket_size), heap_size(0u), heap_initial_size(heap_initial_size), heap_increment(heap_increment),
	idx(bucket_size, auxiliary::str_jenkins_hash_case_insensitive, auxiliary::bool_str_equal_to_case_insensitive),
	names(NULL), n(0u) {

	allocate();
}

ArrayDictionary::~ArrayDictionary() {
	deallocate();
}

void ArrayDictionary::allocate() throw (Exception) {
	heap_size = heap_initial_size;
	names = (char**)malloc(heap_size * sizeof(char*));
	if (names == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	for(unsigned int i = 0u; i < heap_size; ++i) {
		names[i] = NULL;
	}
}

void ArrayDictionary::reallocate() throw (Exception) {
	char** new_names = NULL;

	heap_size += heap_increment;
	new_names = (char**)realloc(names, heap_size * sizeof(char*));
	if (new_names == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory re-allocation.");
	}
	names = new_names;
	new_names = NULL;

	for (unsigned int i = n; i < heap_size; ++i) {
		names[i] = NULL;
	}
}

void ArrayDictionary::deallocate() {
	if (names != NULL) {
		for (unsigned int i = 0u; i < heap_size; ++i) {
			if (names[i] != NULL) {
				free(names[i]);
				names[i] = NULL;
			}
		}

		free(names);
		names = NULL;
		heap_size = 0u;
		n = 0u;
	}


	idx.clear();
}

unsigned int ArrayDictionary::insert(const char* name) throw (Exception) {
	unsigned int id = 0u;

	idx_it = idx.find(name);
	if (idx_it == idx.end()) {
		if (n >= heap_size) {
			reallocate();
		}

		names[n] = (char*)malloc((strlen(name) + 1u) * sizeof(char));
		if (names[n] == NULL) {
			throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
		}
		names[n][0u] = '\0';
		strcpy(names[n], name);

		idx.insert(pair<const char*, unsigned int>(names[n], n));

		id = n;
		++n;
	} else {
		id = idx_it->second;
	}

	return id;
}

long int ArrayDictionary::find(const char* name) {
	idx_it = idx.find(name);

	if (idx_it != idx.end()) {
		return (long int)idx_it->second;
	}

	return -1;
}

void ArrayDictionary::remove(const char* name) {
	idx_it = idx.find(name);

	if (idx_it != idx.end()) {
		free(names[idx_it->second]);
		names[idx_it->second] = NULL;
		idx.erase(idx_it);
	}
}

void ArrayDictionary::load(const char* input_file) throw (Exception) {
	Reader* reader = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 0u;

	try {
		reader = ReaderFactory::create(input_file);
		reader->open();

		while ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);

			insert(line);
		}

		if (line_length == 0) {
			throw Exception(__FILE__, __LINE__, "Line %d is empty in '%s' file.", line_number + 1u, input_file);
		}

		reader->close();
		delete reader;
		reader = NULL;
	} catch (Exception &e1) {
		if (reader != NULL) {
			try {
				reader->close();
			} catch (Exception &e2) {}
			delete reader;
		}
		e1.add_message(__FILE__, __LINE__, "Error while reading '%s' file.", input_file);
		throw;
	}
}

bool ArrayDictionary::intersect(ArrayDictionary& dictionary) {
	if (dictionary.n < n) {
		for (unsigned int i = 0u; i < dictionary.n; ++i) {
			if (idx.count(dictionary.names[i]) > 0) {
				return true;
			}
		}
	} else {
		for (unsigned int i = 0u; i < n; ++i) {
			if (dictionary.idx.count(names[i]) > 0) {
				return true;
			}
		}
	}

	return false;
}

double ArrayDictionary::get_memory_usage() {
	double memory_usage = 0.0;

//	Memory used by char** data:
	for (unsigned int i = 0u; i < heap_size; ++i) {
		if (names[i] != NULL) {
			memory_usage += (strlen(names[i]) + 1u) * sizeof(char);
		}
	}
	memory_usage += heap_size * sizeof(char*);

//	Minimal memory used by idx:
	memory_usage += idx.size() * sizeof(const char*);

	return memory_usage / 1048576.0;
}
