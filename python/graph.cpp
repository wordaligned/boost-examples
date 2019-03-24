#include "graph.h"

#include <boost/python.hpp>
#pragma warning(disable: 4267)
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#pragma warning (default: 4267)

// Helper for std::visit - used to reflect std::variant FeatureValue
// See: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

namespace bp = boost::python;

// Equality operators required for bp::vector_indexing_suite
bool operator==(Edge const & L, Edge const & R) { return L.type_ == R.type_ && L.from_ == R.from_ && L.to_ == R.to_; }
bool operator==(Node const & L, Node const & R) { return L.id_ == R.id_; }
bool operator==(Feature const & L, Feature const & R) { return L.type_ == R.type_ && L.value_ == R.value_; }

namespace
{
	template<typename T>
	PyObject * convert_value(T const & t)
	{
		return bp::incref(bp::object(t).ptr());
	}

	// https://blind.guru/boost_python-and-boost_variant.html
	struct variant_to_object {
		static PyObject * convert(FeatureValue const & v) {
			return std::visit(overloaded{
							[](auto arg) { return convert_value(arg); }
				}, v);
		}
	};

	template <typename T>
	T get(Feature const & f) { return std::get<T>(f.value_); }

	template <typename T>
	void set(Feature & f, T const & v) { f.value_ = v; }
}


BOOST_PYTHON_MODULE(graph)
{
	// Reflect enums
#define X(x) .value(#x, x)
	bp::enum_<FeatureType>("FeatureType") FEATURETYPES;
#undef X

	bp::enum_<SymbolValue>("SymbolValue")
		.value("vNull", vNull)
		.value("ven_US", ven_US)
		.value("vEn_GB", vEn_GB)
		.value("ven_AU", ven_AU)
		.value("vAND", vAND)
		.value("vOR", vOR)
		.value("vNOT", vNOT)
		;
	bp::enum_<EdgeType>("EdgeType")
		.value("eNull", eNull)
		.value("eHasContext", eHasContext)
		.value("eHasExtent", eHasExtent)
		.value("eHasPart", eHasPart)
		.value("eConsumes", eConsumes)
		;
	bp::enum_<NodeType>("NodeType")
		.value("nNull", nNull)
		.value("nDocument", nDocument)
		.value("nHeading", nHeading)
		.value("nSection", nSection)
		.value("nSegment", nSegment)
		.value("nUnknown", nUnknown)
		.value("nAcronym", nAcronym)
		.value("nEncoding", nEncoding)
		;

	// Reflect structs
	bp::class_<Edge>("Edge", bp::init<EdgeType, uint32_t, uint32_t>())
		.def_readwrite("type_", &Edge::type_)
		.def_readwrite("from_", &Edge::from_)
		.def_readwrite("to_", &Edge::to_);

	bp::class_<Node>("Node", bp::init<uint32_t, NodeType, uint32_t, uint32_t>())
		.def_readwrite("id_", &Node::id_)
		.def_readwrite("type_", &Node::type_)
		.def_readwrite("start_", &Node::start_)
		.def_readwrite("end_", &Node::end_)
		.def_readwrite("labels_", &Node::labels_);

	bp::class_<Graph>("Graph")
		.def_readwrite("text_", &Graph::text_)
		.def_readwrite("edges_", &Graph::edges_)
		.def_readwrite("nodes_", &Graph::nodes_);

	// Reflect struct with variant member 
	bp::class_<Feature>("Feature")
		.def_readwrite("type_", &Feature::type_)
		.add_property("string_value", get<std::string>, set<std::string>)
		.add_property("symbol_value", get<SymbolValue>, set<SymbolValue>)
		.add_property("bool_value", get<bool>, set<bool>)
		.add_property("int_value", get<int64_t>, set<int64_t>)
		.add_property("float_value", get<double>, set<double>)
		.add_property("features_value", get<std::vector<Feature>>, set<std::vector<Feature>>);

	// Reflect collections
	bp::class_<std::vector<Feature>>("Features")
		.def(bp::vector_indexing_suite<std::vector<Feature>>());

	bp::class_<std::vector<Edge>>("Edges")
		.def(bp::vector_indexing_suite<std::vector<Edge>>());

	bp::class_<std::vector<Node>>("Nodes")
		.def(bp::vector_indexing_suite<std::vector<Node>>());

	// Reflect functions
	bp::def("Encode", Encode);
}
