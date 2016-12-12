#ifndef ARRAYDICTIONARY_H_
#define ARRAYDICTIONARY_H_

#include <unordered_map>

#include "../../auxiliary/include/auxiliary.h"
#include "../../exception/include/Exception.h"
#include "../../reader/include/ReaderFactory.h"

using namespace std;

class ArrayDictionary {
private:
	unsigned int bucket_size;
	unsigned int heap_size;
	unsigned int heap_initial_size;
	unsigned int heap_increment;

	unordered_map<const char*, unsigned int, unsigned long int(*)(const char*), bool(*)(const char*, const char*)> idx;
	unordered_map<const char*, unsigned int, unsigned long int(*)(const char*), bool(*)(const char*, const char*)>::iterator idx_it;

	void allocate() throw (Exception);
	void reallocate() throw (Exception);
	void deallocate();

public:
	char** names;
	unsigned int n;

	ArrayDictionary(unsigned int bucket_size, unsigned int heap_initial_size, unsigned int heap_increment) throw (Exception);
	virtual ~ArrayDictionary();

	unsigned int insert(const char* name) throw (Exception);
	long int find(const char* name);
	void remove(const char* name);

	void load(const char* input_file) throw (Exception);
	bool intersect(ArrayDictionary& dictionary);

	double get_memory_usage();
};

#endif
