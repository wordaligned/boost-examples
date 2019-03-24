#pragma once

#include <string>
#include <vector>
#include <variant>

#define FEATURETYPES \
	X(fNull) \
	X(fID) \
	X(fLanguage) \
	X(fContext) \
	X(fEncode) \
	X(fSpelling) \
	X(fAcronym)

#define X(x) x,
enum FeatureType { FEATURETYPES };
#undef X

#define X(x) case x: return #x;
inline char const *
FeatureTypeToString(FeatureType f) { switch(f) { FEATURETYPES } }
#undef X

enum SymbolValue {
	vNull,
	ven_US,
	vEn_GB,
	ven_AU,
	vAND,
	vOR,
	vNOT
};

enum EdgeType {
	eNull,
	eHasContext,
	eHasExtent,
	eHasPart,
	eConsumes
};

enum NodeType {
	nNull,
	nDocument,
	nHeading,
	nSection,
	nSegment,
	nUnknown,
	nAcronym,
	nEncoding
};

struct Feature;

typedef std::variant<std::string, SymbolValue, bool, int64_t, double, std::vector<Feature>> FeatureValue;

// Name value pairs used for annotations
// Note: these can be recursive structures
struct Feature
{
	FeatureType type_;
	FeatureValue value_;
};

// Graph edge - typed link between annotation nodes
struct Edge 
{
	EdgeType type_;
	uint32_t from_, to_;
	Edge(EdgeType t, uint32_t from, uint32_t to)
		: type_(t), from_(from), to_(to) {}
};

// Graph node - a region of text having a unique ID, a type and optional labels
struct Node 
{
	uint32_t id_;
	NodeType type_;
	uint32_t start_, end_;
	std::vector<Feature> labels_;

	Node(uint32_t id, NodeType t, uint32_t s, uint32_t e)
		: id_(id), type_(t), start_(s), end_(e) {}
};

// The annotation graph.
struct Graph
{
	std::string text_;
	std::vector<Edge> edges_;
	std::vector<Node> nodes_;
};

// Encode function
inline int Encode(Graph & graph)
{
	throw std::runtime_error("NYI!");
}
