#include "include/ArrayGraph.h"

ArrayGraph::ArrayGraph(unsigned int n) throw (Exception) : nodes(NULL), n(n) {
	nodes = (node*)malloc(n * sizeof(node));
	if (nodes == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	for (unsigned int i = 0u; i < n; ++i) {
		nodes[i].heads = NULL;
		nodes[i].tails = NULL;
		nodes[i].n_heads = 0u;
		nodes[i].n_tails = 0u;
		nodes[i].weight = 0.0;
	}
}

ArrayGraph::ArrayGraph(ArrayGraph&& graph) : nodes(graph.nodes), n(graph.n) {
	graph.nodes = NULL;
	graph.n = 0u;
}

ArrayGraph::~ArrayGraph() {
	if (nodes != NULL) {
		for (unsigned int i = 0u; i < n; ++i) {
			if (nodes[i].heads != NULL) {
				free(nodes[i].heads);
				nodes[i].heads = NULL;
			}

			if (nodes[i].tails != NULL) {
				free(nodes[i].tails);
				nodes[i].tails = NULL;
			}
		}

		free(nodes);
		nodes = NULL;
	}
}

bool ArrayGraph::add_edge(unsigned int tail_id, unsigned int head_id) throw (Exception) {
	unsigned int* new_edges = NULL;
	unsigned int* last = NULL;
	unsigned int* res = NULL;

	if (binary_search(nodes[tail_id].heads, nodes[tail_id].heads + nodes[tail_id].n_heads, head_id, [](unsigned int first, unsigned int second) {return first < second;})) {
		return false;
	}

	new_edges = (unsigned int*)realloc(nodes[tail_id].heads, (nodes[tail_id].n_heads + 1u) * sizeof(unsigned int));
	if (new_edges == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory re-allocation.");
	}
	nodes[tail_id].heads = new_edges;
	new_edges = NULL;

	last = nodes[tail_id].heads + nodes[tail_id].n_heads;
	res = std::upper_bound(nodes[tail_id].heads, last, head_id, [](unsigned int first, unsigned int second) {return first < second;});
	for (unsigned int* pos = last; pos > res; --pos) {
		*pos = *(pos - 1);
	}
	*res = head_id;
	nodes[tail_id].n_heads += 1u;

	new_edges = (unsigned int*)realloc(nodes[head_id].tails, (nodes[head_id].n_tails + 1u) * sizeof(unsigned int));
	if (new_edges == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory re-allocation.");
	}
	nodes[head_id].tails = new_edges;
	new_edges = NULL;

	last = nodes[head_id].tails + nodes[head_id].n_tails;
	res = std::upper_bound(nodes[head_id].tails, last, tail_id, [](unsigned int first, unsigned int second) {return first < second;});
	for (unsigned int* pos = last; pos > res; --pos) {
		*pos = *(pos - 1);
	}
	*res = tail_id;
	nodes[head_id].n_tails += 1u;

	return true;
}

void ArrayGraph::remove_edge(unsigned int tail_id, unsigned int head_id) {
	unsigned int* last = NULL;
	unsigned int* first = NULL;

	last = nodes[tail_id].heads + nodes[tail_id].n_heads;
	first = std::lower_bound(nodes[tail_id].heads, last, head_id, [](unsigned int first, unsigned int second) {return first < second;});
	if ((first != last) && (head_id == *first)) {
		for (unsigned int* pos = first + 1; pos < last; ++pos) {
			*(pos - 1) = *pos;
		}
		nodes[tail_id].n_heads -= 1u;
	}

	last = nodes[head_id].tails + nodes[head_id].n_tails;
	first = std::lower_bound(nodes[head_id].tails, last, tail_id, [](unsigned int first, unsigned int second) {return first < second;});
	if ((first != last) && (tail_id == *first)) {
		for (unsigned int* pos = first + 1; pos < last; ++pos) {
			*(pos - 1) = *pos;
		}
		nodes[head_id].n_tails -= 1u;
	}
}

bool ArrayGraph::find_edge(unsigned int tail_id, unsigned int head_id) {
	unsigned int* last = NULL;
	unsigned int* first = NULL;

	last = nodes[tail_id].heads + nodes[tail_id].n_heads;
	first = std::lower_bound(nodes[tail_id].heads, last, head_id, [](unsigned int first, unsigned int second) {return first < second;});
	if ((first == last) || (head_id != *first)) {
		return false;
	}

	last = nodes[head_id].tails + nodes[head_id].n_tails;
	first = std::lower_bound(nodes[head_id].tails, last, tail_id, [](unsigned int first, unsigned int second) {return first < second;});
	if ((first == last) || (tail_id != *first)) {
		return false;
	}

	return true;
}

void ArrayGraph::cut_node(unsigned int node_id, unordered_set<unsigned int>& altered_nodes) {
	unsigned int tail_id = 0u;

	while (nodes[node_id].n_tails > 0) {
		tail_id = nodes[node_id].tails[0];
		altered_nodes.insert(tail_id);
		remove_edge(tail_id, node_id);
	}

	for (unsigned int i = 0u, head_id = 0u; i < nodes[node_id].n_heads; ++i) {
		altered_nodes.insert(head_id);
		head_id = nodes[node_id].heads[i];

		while (nodes[head_id].n_heads > 0) {
			altered_nodes.insert(nodes[head_id].heads[0]);
			remove_edge(head_id, nodes[head_id].heads[0]);
		}

		while (nodes[head_id].n_tails > 1) {
			if (nodes[head_id].tails[0] != node_id) {
				altered_nodes.insert(nodes[head_id].tails[0]);
				remove_edge(nodes[head_id].tails[0], head_id);
			} else {
				altered_nodes.insert(nodes[head_id].tails[1]);
				remove_edge(nodes[head_id].tails[1], head_id);
			}
		}
	}
}

void ArrayGraph::load_weights(double weight) {
	for (unsigned int i = 0u; i < n; ++i) {
		nodes[i].weight = weight;
	}
}

void ArrayGraph::load_weight(unsigned int node_id, double weight) {
	nodes[node_id].weight = weight;
}

void ArrayGraph::check_order_consistency() throw (Exception) {
	for (unsigned int i = 0u; i < n; ++i) {
		for (long int j = 0; j < (long int)nodes[i].n_heads - 1; ++j) {
			if (nodes[i].heads[j] >= nodes[i].heads[j + 1]) {
				throw Exception(__FILE__, __LINE__, "Graph edges are not ordered.");
			}
		}

		for (long int j = 0; j < (long int)nodes[i].n_tails - 1; ++j) {
			if (nodes[i].tails[j] >= nodes[i].tails[j + 1]) {
				throw Exception(__FILE__, __LINE__, "Graph edges are not ordered.");
			}
		}
	}
}

unsigned long int ArrayGraph::get_n_edges() {
	unsigned long int n_edges = 0ul;

	for (unsigned int i = 0u; i < n; ++i) {
		n_edges += nodes[i].n_heads;
	}

	return n_edges;
}

unsigned long int ArrayGraph::get_n_connections() {
	unsigned long int n_bi_edges = 0ul;

	for (unsigned int i = 0u; i < n; ++i) {
		for (unsigned int j = 0u; j < nodes[i].n_heads; ++j) {
			if (find_edge(nodes[i].heads[j], i)) {
				++n_bi_edges;
			}
		}
	}

	return get_n_edges() - n_bi_edges / 2;
}

void ArrayGraph::print() {
	for (unsigned int i = 0u; i < n; ++i) {
		cout << "Node " << i << ": " << endl;
		cout << " TAILS:";
		for (unsigned int j = 0u; j < nodes[i].n_tails; ++j) {
			cout << " " << nodes[i].tails[j];
		}
		cout << endl;
		cout << " HEADS:";
		for (unsigned int j = 0u; j < nodes[i].n_heads; ++j) {
			cout << " " << nodes[i].heads[j];
		}
		cout << endl;
		cout << endl;
	}
}

void ArrayGraph::print(unsigned int node_id) {
	cout << "Node " << node_id << ": " << endl;
	cout << " TAILS:";
	for (unsigned int j = 0u; j < nodes[node_id].n_tails; ++j) {
		cout << " " << nodes[node_id].tails[j];
	}
	cout << endl;
	cout << " HEADS:";
	for (unsigned int j = 0u; j < nodes[node_id].n_heads; ++j) {
		cout << " " << nodes[node_id].heads[j];
	}
	cout << endl;
	cout << endl;
}

double ArrayGraph::get_memory_usage() {
	double memory_usage = 0.0;

	for (unsigned int i = 0u; i < n; ++i) {
		memory_usage += sizeof(unsigned int) * (nodes[i].n_heads + nodes[i].n_tails);
	}
	memory_usage += (sizeof(node) * n);

	return memory_usage / 1048576.0;
}
