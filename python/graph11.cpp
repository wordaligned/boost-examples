#include "graph.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(graph11, m)
{
	m.doc() = "pybind11 example plugin";

	// Reflect enums
	py::enum_<FeatureType>(m, "FeatureType")
		.value("fNull", fNull)
		.value("fID", fID)
		.value("fLanguage", fLanguage)
		.value("fContext", fContext)
		.value("fEncode", fEncode)
		.value("fSpelling", fSpelling)
		.value("fAcronym", fAcronym)
		;
	py::enum_<SymbolValue>(m, "SymbolValue")
		.value("vNull", vNull)
		.value("ven_US", ven_US)
		.value("vEn_GB", vEn_GB)
		.value("ven_AU", ven_AU)
		.value("vAND", vAND)
		.value("vOR", vOR)
		.value("vNOT", vNOT)
		;
	py::enum_<EdgeType>(m, "EdgeType")
		.value("eNull", eNull)
		.value("eHasContext", eHasContext)
		.value("eHasExtent", eHasExtent)
		.value("eHasPart", eHasPart)
		.value("eConsumes", eConsumes)
		;
	py::enum_<NodeType>(m, "NodeType")
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
	py::class_<Edge>(m, "Edge")
		.def(py::init<EdgeType, uint32_t, uint32_t>())
		.def_readwrite("type_", &Edge::type_)
		.def_readwrite("from_", &Edge::from_)
		.def_readwrite("to_", &Edge::to_);

	py::class_<Node>(m, "Node")
		.def(py::init<uint32_t, NodeType, uint32_t, uint32_t>())
		.def_readwrite("id_", &Node::id_)
		.def_readwrite("type_", &Node::type_)
		.def_readwrite("start_", &Node::start_)
		.def_readwrite("end_", &Node::end_)
		.def_readwrite("labels_", &Node::labels_);

	py::class_<Graph>(m, "Graph")
		.def(py::init<>())
		.def_readwrite("text_", &Graph::text_)
		.def_readwrite("edges_", &Graph::edges_)
		.def_readwrite("nodes_", &Graph::nodes_);

	py::class_<Feature>(m, "Feature")
		.def(py::init<>())
		.def_readwrite("type_", &Feature::type_)
		.def_readwrite("value_", &Feature::value_);

	// Reflect variant type
	py::class_<FeatureValue>(m, "FeatureValue");

	// Reflect collections
	py::class_<std::vector<Feature>>(m, "Features");

	py::class_<std::vector<Edge>>(m, "Edges");

	py::class_<std::vector<Node>>(m, "Nodes");

	// Reflect functions
	m.def("Encode", &Encode);
}
