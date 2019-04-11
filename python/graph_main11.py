import sys
sys.path.append('build/Debug')

import graph11 as graph

# Build a graph
G = graph.Graph()

# Add text to the graph
text = "E: Rash on finger"
G.text_ = text

# Create a document
doc = graph.Node(0, graph.NodeType.nDocument, 0, len(text))

# Set the document ID
doc_id = graph.Feature()
doc_id.type_ = graph.FeatureType.fID
doc_id.value_ = "MRN-666"
doc.labels_.append(doc_id)

# Set the document language
language = graph.Feature()
language.type_ = graph.FeatureType.fLanguage
language.value_ = graph.SymbolValue.vEn_GB
doc.labels_.append(language)

# Add document to the graph
G.nodes_.append(doc)

# Add heading and section
heading = graph.Node(1, graph.NodeType.nHeading, 0, 1)
section = graph.Node(2, graph.NodeType.nSection, 3, len(text))

# Link the section to the heading
edge = graph.Edge(graph.EdgeType.eHasContext, 2, 1)
G.edges_.append(edge)
