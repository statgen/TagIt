#include "include/Tagger.h"

const double Tagger::EPSILON = 0.000000001;

const char Tagger::NONE = 0;
const char Tagger::TAG = 1;

Tagger::Tagger(ArrayDictionary* markers, vector<ArrayGraph>* graphs) throw (Exception) :
		markers(markers), graphs(graphs), summaries(NULL), n(markers->n) {

	summaries = (summary*)malloc(n * sizeof(summary));
	if (summaries == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	for (unsigned int i = 0u; i < n; ++i) {
		summaries[i].weights_all = 0.0;
		summaries[i].weights_unique = 0.0;
		summaries[i].flag = NONE;
	}
}

Tagger::~Tagger() {
	if (summaries != NULL) {
		free(summaries);
		summaries = NULL;
	}
}

void Tagger::compute_weights(unsigned int node_id) {
	max_weights.clear();

	summaries[node_id].weights_all = 0.0;
	summaries[node_id].weights_unique = 0.0;

	for (auto&& graph : *graphs) {

		for (unsigned int i = 0u, tagged_node_id = 0u; i < graph.nodes[node_id].n_heads; ++i) {
			tagged_node_id = graph.nodes[node_id].heads[i];

			summaries[node_id].weights_all += graph.nodes[tagged_node_id].weight;

			max_weights_it = max_weights.find(tagged_node_id);
			if (max_weights_it != max_weights.end()) {
				max_weights_it->second = max(max_weights_it->second, graph.nodes[tagged_node_id].weight);
			} else {
				max_weights.emplace(tagged_node_id, graph.nodes[tagged_node_id].weight);
			}
		}
	}

	for (max_weights_it = max_weights.begin(); max_weights_it != max_weights.end(); ++max_weights_it) {
		summaries[node_id].weights_unique += max_weights_it->second;
	}
}

bool Tagger::get_candidate_tag(unsigned int* node_id, bool weights_all) {
	double max_weight = 0.0;

	if (weights_all) {
		for (unsigned int i = 0u; i < n; ++i) {
			if (summaries[i].flag == TAG) {
				continue;
			}

			if (auxiliary::fcmp(max_weight, summaries[i].weights_all, EPSILON) < 0) {
				max_weight = summaries[i].weights_all;
				*node_id = i;
			}
		}
	} else {
		for (unsigned int i = 0u; i < n; ++i) {
			if (summaries[i].flag == TAG) {
				continue;
			}

			if (auxiliary::fcmp(max_weight, summaries[i].weights_unique, EPSILON) < 0) {
				max_weight = summaries[i].weights_unique;
				*node_id = i;
			}
		}
	}

	return max_weight != 0.0;
}

void Tagger::tag(ArrayDictionary& fix_markers, bool weights_all) {
	unsigned int tag_id;
	long int lookup_tag_id;

	double old_weights_all = 0.0;
	double old_weights_unique = 0.0;

	unordered_set<unsigned int> altered;

	for (unsigned int i = 0u; i < n; ++i) {
		compute_weights(i);
	}

	for (unsigned int i = 0u; i < fix_markers.n; ++i) {
		if ((lookup_tag_id = markers->find(fix_markers.names[i])) < 0) {
			continue;
		}

		tag_id = (long int)lookup_tag_id;

		old_weights_all = summaries[tag_id].weights_all;
		old_weights_unique = summaries[tag_id].weights_unique;

		for (auto&& graph : *graphs) {
			graph.cut_node(tag_id, altered);
		}

		summaries[tag_id].flag = TAG;
		for (auto id: altered) {
			compute_weights(id);
		}

		compute_weights(tag_id);
		if ((auxiliary::fcmp(old_weights_all, summaries[tag_id].weights_all,EPSILON) != 0) ||
				(auxiliary::fcmp(old_weights_unique, summaries[tag_id].weights_unique, EPSILON) != 0)) {
			throw Exception(__FILE__, __LINE__, "Tagging went wrong (%g, %g, %g, %g).",
					old_weights_all, old_weights_unique, summaries[tag_id].weights_all, summaries[tag_id].weights_unique);
		}

		altered.clear();
	}

	while (get_candidate_tag(&tag_id, weights_all)) {
		old_weights_all = summaries[tag_id].weights_all;
		old_weights_unique = summaries[tag_id].weights_unique;

		for (auto&& graph : *graphs) {
			graph.cut_node(tag_id, altered);
		}

		summaries[tag_id].flag = TAG;
		for (auto id: altered) {
			compute_weights(id);
		}

		compute_weights(tag_id);
		if ((auxiliary::fcmp(old_weights_all, summaries[tag_id].weights_all,EPSILON) != 0) ||
				(auxiliary::fcmp(old_weights_unique, summaries[tag_id].weights_unique, EPSILON) != 0)) {
			throw Exception(__FILE__, __LINE__, "Tagging went wrong (%g, %g, %g, %g).",
					old_weights_all, old_weights_unique, summaries[tag_id].weights_all, summaries[tag_id].weights_unique);
		}

		altered.clear();
	}
}

unsigned int Tagger::get_n_tags() {
	unsigned int n_tags = 0u;

	for (unsigned int i = 0u; i < n; ++i) {
		if (summaries[i].flag == TAG) {
			++n_tags;
		}
	}

	return n_tags;
}

unsigned int Tagger::get_n_tagged() {
	unordered_set<unsigned int> tagged_union;

	for (unsigned int i = 0u; i < n; ++i) {
		if (summaries[i].flag == TAG) {
			continue;
		}

		for (auto&& graph : *graphs) {
			if (graph.nodes[i].tails > 0) {
				tagged_union.insert(i);
				break;
			}
		}
	}

	return tagged_union.size();
}

unsigned int Tagger::get_n_tagged(ArrayGraph& graph) {
	unsigned int n_tagged = 0u;

	for (unsigned int i = 0u; i < n; ++i) {
		if (summaries[i].flag == TAG) {
			continue;
		}

		if (graph.nodes[i].n_tails > 0) {
			++n_tagged;
		}
	}

	return n_tagged;
}

void Tagger::print_summary(const char* output_file, bool gzip, vector<const char*>& labels) throw (Exception) {
	Writer* writer = NULL;

	double weight = 0.0;
	vector<double> weights;
	double weights_all = 0.0;
	double weights_unique = 0.0;

	try {
		writer = WriterFactory::create(gzip ? WriterFactory::GZIP : WriterFactory::TEXT);
		writer->set_file_name(output_file);
		writer->open();

		writer->write("%s\t%s\t%s", "MARKER", "WEIGHT.ALL", "WEIGHT.UNIQUE");
		if (labels.empty()) {
			for (unsigned int i = 1u; i <= graphs->size(); ++i) {
				writer->write("\t%u.WEIGHT", i);
			}
		} else {
			for (auto&& label : labels) {
				writer->write("\t%s.WEIGHT", label);
			}
		}
		writer->write("\n");

		for (unsigned int i = 0u; i < n; ++i) {
			max_weights.clear();

			weights.clear();
			weights_all = 0.0;
			weights_unique = 0.0;

			for (auto&& graph : *graphs) {
				weight = 0.0;
				for (unsigned int j = 0u, tagged_node_id = 0u; j < graph.nodes[i].n_heads; ++j) {
					tagged_node_id = graph.nodes[i].heads[j];
					weight += graph.nodes[tagged_node_id].weight;
					max_weights_it = max_weights.find(tagged_node_id);
					if (max_weights_it != max_weights.end()) {
						max_weights_it->second = max(max_weights_it->second, graph.nodes[tagged_node_id].weight);
					} else {
						max_weights.emplace(tagged_node_id, graph.nodes[tagged_node_id].weight);
					}
				}
				weights_all += weight;
				weights.push_back(weight);
			}

			for (max_weights_it = max_weights.begin(); max_weights_it != max_weights.end(); ++max_weights_it) {
				weights_unique += max_weights_it->second;
			}

			writer->write("%s\t%g\t%g", markers->names[i], weights_all, weights_unique);
			for (auto&& weight : weights) {
				writer->write("\t%g", weight);
			}
			writer->write("\n");
		}

		writer->close();
		delete writer;
		writer = NULL;
	} catch (Exception &e1) {
		if (writer != NULL) {
			try {
				writer->close();
			} catch (Exception &e2) {}
			delete writer;
		}

		 e1.add_message(__FILE__, __LINE__, "Error while writing '%s' file.", output_file);
		 throw;
	}
}

void Tagger::print_tags(const char* output_file, bool gzip, vector<const char*>& labels) throw (Exception) {
	Writer* writer = NULL;
	double weight = 0.0;

	try {
		writer = WriterFactory::create(gzip ? WriterFactory::GZIP : WriterFactory::TEXT);
		writer->set_file_name(output_file);
		writer->open();

		writer->write("%s\t%s\t%s", "MARKER", "WEIGHT.ALL", "WEIGHT.UNIQUE");
		if (labels.empty()) {
			for (unsigned int i = 1u; i <= graphs->size(); ++i) {
				writer->write("\t%u.WEIGHT", i);
			}
		} else {
			for (auto&& label : labels) {
				writer->write("\t%s.WEIGHT", label);
			}
		}
		writer->write("\n");

		for (unsigned int i = 0u; i < n; ++i) {
			if (summaries[i].flag != TAG) {
				continue;
			}

			writer->write("%s\t%g\t%g", markers->names[i], summaries[i].weights_all, summaries[i].weights_unique);
			for (auto&& graph : *graphs) {
				weight = 0.0;
				for (unsigned int j = 0u, tagged_node_id = 0u; j < graph.nodes[i].n_heads; ++j) {
					tagged_node_id = graph.nodes[i].heads[j];
					weight += graph.nodes[tagged_node_id].weight;
				}
				writer->write("\t%g", weight);
			}
			writer->write("\n");
		}

		writer->close();
		delete writer;
		writer = NULL;
	} catch (Exception &e1) {
		if (writer != NULL) {
			try {
				writer->close();
			} catch (Exception &e2) {}
			delete writer;
		}

		 e1.add_message(__FILE__, __LINE__, "Error while writing '%s' file.", output_file);
		 throw;
	}
}

void Tagger::print_tagged(const char* output_file, bool gzip, vector<const char*>& labels) throw (Exception) {
	Writer* writer = NULL;

	unordered_set<unsigned int> tagged_union;

	try {
		for (unsigned int i = 0u; i < n; ++i) {
			if (summaries[i].flag == TAG) {
				continue;
			}

			for (auto&& graph : *graphs) {
				if (graph.nodes[i].tails > 0) {
					tagged_union.insert(i);
					break;
				}
			}
		}

		writer = WriterFactory::create(gzip ? WriterFactory::GZIP : WriterFactory::TEXT);
		writer->set_file_name(output_file);
		writer->open();

		writer->write("%s", "MARKER");
		if (labels.empty()) {
			for (unsigned int i = 1u; i <= graphs->size(); ++i) {
				writer->write("\t%u", i);
			}
		} else {
			for (auto&& label : labels) {
				writer->write("\t%s", label);
			}
		}
		writer->write("\n");

		for (auto tagged_id : tagged_union) {
			writer->write("%s", markers->names[tagged_id]);
			for (auto&& graph : *graphs) {
				writer->write("\t%d", graph.nodes[tagged_id].tails > 0 ? 1 : 0);
			}
			writer->write("\n");
		}

		writer->close();
		delete writer;
		writer = NULL;
	} catch (Exception &e1) {
		if (writer != NULL) {
			try {
				writer->close();
			} catch (Exception &e2) {}
			delete writer;
		}

		 e1.add_message(__FILE__, __LINE__, "Error while writing '%s' file.", output_file);
		 throw;
	}
}
