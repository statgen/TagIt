#ifndef ARRAYGRAPH_H_
#define ARRAYGRAPH_H_

#include <cstdlib>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <functional>

#include "../../exception/include/Exception.h"

using namespace std;

class ArrayGraph {
public:
	struct node {
		unsigned int* heads;
		unsigned int* tails;
		unsigned int n_heads;
		unsigned int n_tails;
		double weight;
	};

	node* nodes;
	unsigned int n;

	ArrayGraph(unsigned int n) throw (Exception);
	ArrayGraph(ArrayGraph&& graph);

	virtual ~ArrayGraph();

	bool add_edge(unsigned int tail_id, unsigned int head_id) throw (Exception);
	void remove_edge(unsigned int tail_id, unsigned int head_id);
	bool find_edge(unsigned int tail_id, unsigned int head_id);

	void cut_node(unsigned int node_id, unordered_set<unsigned int>& altered_nodes);

	void load_weights(double weight);
	void load_weight(unsigned int node_id, double weight);

	void check_order_consistency() throw (Exception);

	unsigned long int get_n_edges();
	unsigned long int get_n_connections();

	void print();
	void print(unsigned int node_id);

	double get_memory_usage();
};

#endif
