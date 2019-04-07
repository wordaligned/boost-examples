// Allocate objects in a memory mapped file using Boost.Interprocess

#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <boost/filesystem.hpp>

#include <unordered_set>

#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

namespace ip = boost::interprocess;
namespace fs = boost::filesystem;

// Typedefs for allocating objects into the mmapped segment
typedef ip::managed_mapped_file::segment_manager SegmentManager;
typedef ip::allocator<void, SegmentManager> VoidAllocator;
typedef ip::allocator<char, SegmentManager> CharAllocator;
typedef ip::basic_string<char, std::char_traits<char>, CharAllocator> String;

// A SNOMED concept has an 8-byte numeric identifier
// Concepts have ancestors (defined via is-A relationships)
struct Concept
{
	uint64_t id_;
	std::vector<uint64_t> ancestors_;
};

typedef std::vector<Concept> Concepts;

// Fake some concept data. In reality there are ~400K concepts
// each having an average of around 8 ancestors.
Concepts LoadConcepts()
{
	return Concepts {
		{ 1, {} },
		{ 2, { 1 } },
		{ 3, { 2, 1 } }
	};
}

typedef std::pair<uint64_t, uint64_t> ConceptAncestor;

std::ostream &
operator<<(std::ostream & out, ConceptAncestor const & link)
{
	return out << '(' << link.second << '>' << link.first << ')';
}

struct Terminology
{
	typedef ip::allocator<ConceptAncestor, SegmentManager> Allocator;

	typedef std::unordered_set<
		ConceptAncestor,
		boost::hash<ConceptAncestor>,
		std::equal_to<ConceptAncestor>, Allocator> Ancestors;

	Ancestors ancestors;

	Terminology(VoidAllocator & alloc)
		: ancestors(alloc)
	{}

	void StoreAncestors(Concepts const & concepts)
	{
		for (auto concept : concepts)
			for (auto ancestor : concept.ancestors_)
				ancestors.insert({concept.id_, ancestor});
	}
};


void ReadExistingMMF(std::string const & mmf_name)
{
	auto fp = std::make_unique<ip::managed_mapped_file>
		(ip::open_read_only, mmf_name.c_str());

	if (auto [version, size] = fp->find<String>("Version"); version)
	{
		std::cout << "Version: " << *version << '\n';
	}
	if (auto terminology = fp->find<Terminology>(ip::unique_instance).first)
	{
		auto const & ancestors = terminology->ancestors;

		std::cout << "Found terminology\n";
		std::cout << "Concept-Ancestors\n";
		for (auto const ancestor : ancestors)
			std::cout << '\t' << ancestor << '\n';
	}
}

void WriteNewMMF(std::string const & mmf_name)
{
	{
		// Note: managed_mapped_file uses the default char type, memory allocation
		// and synchronisation strategy and index.
		auto fp = std::make_unique<ip::managed_mapped_file>
			(ip::create_only, mmf_name.c_str(), 1000000);

		// To allocate objects in the file, use its allocator
		VoidAllocator allocator = fp->get_segment_manager();

		// Add version string
		fp->construct<String>("Version")("ACCU 2019", allocator);

		// Create a Terminology instance - there's just one of these
		auto terminology = fp->construct<Terminology>(ip::unique_instance)(allocator);

		// Now add contents to the terminology
		auto const concepts = LoadConcepts();
		terminology->StoreAncestors(concepts);

		fp->flush();
	}
	ip::managed_mapped_file::shrink_to_fit(mmf_name.c_str());
}

// Example showing how to read and write a MMF.
// The file name is supplied on the command line:
// if the file exists, read it, otherwise, write it.
int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " MMF_NAME\n";
	}
	else if (fs::path mmf_name{argv[1]}; fs::exists(mmf_name))
	{
		ReadExistingMMF(mmf_name.string());
	}
	else
	{
		WriteNewMMF(mmf_name.string());
	}
	return 0;
}
