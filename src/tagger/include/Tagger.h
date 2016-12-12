#ifndef TAGGER_H_
#define TAGGER_H_

#include <iostream>
#include <unordered_map>
#include <cmath>

#include "../../structures/include/ArrayDictionary.h"
#include "../../structures/include/ArrayGraph.h"
#include "../../writer/include/WriterFactory.h"

using namespace std;

class Tagger {
private:
	static const double EPSILON;

	static const char NONE;
	static const char TAG;

	struct summary {
		double weights_all;
		double weights_unique;
		char flag;
	};

	ArrayDictionary* markers;
	vector<ArrayGraph>* graphs;

	unordered_map<unsigned int, double> max_weights;
	unordered_map<unsigned int, double>::iterator max_weights_it;

	summary* summaries;
	unsigned int n;

	void compute_weights(unsigned int id);
	bool get_candidate_tag(unsigned int* node_id, bool weights_all);

public:
	Tagger(ArrayDictionary* markers, vector<ArrayGraph>* graphs) throw (Exception);
	virtual ~Tagger();

	void tag(ArrayDictionary& fix_markers, bool weights_all);

	unsigned int get_n_tags();
	unsigned int get_n_tagged();
	unsigned int get_n_tagged(ArrayGraph& graph);

	void print_summary(const char* output_file, bool gzip, vector<const char*>& labels) throw (Exception);
	void print_tags(const char* output_file, bool gzip, vector<const char*>& labels) throw (Exception);
	void print_tagged(const char* output_file, bool gzip, vector<const char*>& labels) throw (Exception);
};

#endif
