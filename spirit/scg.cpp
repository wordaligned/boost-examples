// Using Spirit Qi, Phoenix and Karma to parse and generate (a subset of)
// SNOMED Compositional Grammar expressions.
#include <boost/format.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/include/adapted.hpp>
#include <boost/phoenix.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
#include <iostream>
#include <vector>

namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace karma = spirit::karma;
namespace qi = spirit::qi;

// Define the structures used to represent SNOMED expressions.
// Note: this is a reduced subset for expositional purposes.
struct Expression;

// A concept is an integer id and a descriptive term
struct Concept { uint64_t id_; std::string term_; };

// An attribute value is a Concept or -- recursively -- an Expression
typedef	boost::variant<
	Concept, boost::recursive_wrapper<Expression>> AttributeValue;

struct Attribute  { Concept name_; AttributeValue value_; };
struct Expression { Concept concept_; std::vector<Attribute> refinements_; };

// Adapt the structs for use with the Spirit grammar
BOOST_FUSION_ADAPT_STRUCT(Attribute, name_, value_)
BOOST_FUSION_ADAPT_STRUCT(Concept, id_, term_)
BOOST_FUSION_ADAPT_STRUCT(Expression, concept_, refinements_)

// Spirit.Karma SNOMED generator grammar
// Templated for use with different iterator types
// Operators: - optional, << sequence, | alternative, + one or more
template <typename OutIt>
struct scg_generator_grammar
	: karma::grammar<OutIt, Expression()> 
{
	scg_generator_grammar() : scg_generator_grammar::base_type(expression)
	{
		expression     %= concept << -refinements;
		term           %= '|' << +karma::char_ << '|';
		concept        %= karma::long_long << term;
		refinements    %= ':' << attribute % ',' | karma::eps;
		attribute      %= concept << '=' << attributeValue;
		attributeValue %= concept | '(' << expression << ')';
	}

	karma::rule<OutIt, std::string()> term;
	karma::rule<OutIt, Expression()> expression;
	karma::rule<OutIt, Concept()> concept;
	karma::rule<OutIt, std::vector<Attribute>()> refinements;
	karma::rule<OutIt, Attribute()> attribute;
	karma::rule<OutIt, AttributeValue()> attributeValue;
};

/* The composition grammar:
	 - is templated for use with different iterator types
	 - generates Expressions
	 - uses space_type as its skip parser */
template <typename Iter>
struct composition_grammar : qi::grammar<Iter, Expression(), ascii::space_type>
{
	composition_grammar() : composition_grammar::base_type(expression, "expression")
	{
		using namespace qi::labels;
		using phoenix::construct;
		using phoenix::val;

		/* Notes:
			 A "lexeme" is a primitive or atom
			 Operators: % list, | alternative, > expect, >> sequence,
			            - optional, + one or more
			 %= applies the parsed attribute directly (auto-rules) */
		term           %= qi::lexeme['|' > +(qi::char_ - '|') > '|'];
		concept        %= qi::ulong_long > term;
		attributeValue %= concept | '(' > expression > ')';
		attribute      %= concept > '=' > attributeValue;
		refinements    %= attribute % ',';
		expression     %= concept >> -(':' > refinements);

		// Name our rules for better reporting
		expression.name("expression");
		concept.name("concept");
		attribute.name("attribute");
		refinements.name("refinements");
		attributeValue.name("attribute value");
		term.name("term");

		// Quit and fail on error, printing an error message using phoenix
		qi::on_error<qi::fail>
			(expression,
			 std::cerr << val("ERROR! Expecting ") << spirit::_4 << val(" after ") <<
			 construct<std::string>(spirit::_1, spirit::_3) << val('\n'));
	}

	// The rules
	qi::rule<Iter, Expression(), ascii::space_type> expression;
	qi::rule<Iter, Concept(), ascii::space_type> concept;
	qi::rule<Iter, Attribute(), ascii::space_type> attribute;
	qi::rule<Iter, std::vector<Attribute>(), ascii::space_type> refinements;
	qi::rule<Iter, AttributeValue(), ascii::space_type> attributeValue;
	qi::rule<Iter, std::string(), ascii::space_type> term;
};

// Generate the SCG representation of the SNOMED expression
std::string generate(Expression const & expression)
{
	std::string scg;
	auto sink = std::back_inserter(scg);
	scg_generator_grammar<decltype(sink)> generator{};
	karma::generate(sink, generator, expression);
	return scg;
}

// Parse the SCG representation of a SNOMED expression
std::pair<bool, Expression> parse(std::string const & scg)
{
	auto begin = scg.begin(), end = scg.end(), it = begin;

	composition_grammar<decltype(it)> parser;
	auto skipper = spirit::ascii::space;

	Expression expression;

	bool ok = qi::phrase_parse(it, end, parser, skipper, expression);

	if (!ok)
	{
		auto const fmt = boost::format("Parse '%1%' failed") % scg;
		std::cerr << "ERROR: " << boost::str(fmt) << '\n';
	}
	else if (ok = it == end; !ok)
	{
		auto const fmt = boost::format
			("Parse terminated at: '%1%' leaving '%2%'")
			% std::string(begin, it) % std::string(it, end);
		std::cerr << "WARNING: " << boost::str(fmt) << '\n';
	}
	return {ok, ok ? expression : Expression{}};
}

// Comments start with a '#'
bool is_comment(std::string const & line)
{
	return !line.empty() && line[0] == '#';
}

int main()
{
	// Read lines from stdin
	for (std::string line, cg_example; std::getline(std::cin, line);)
	{
		if (!is_comment(line))
		{
			cg_example += line; // accumulate CG expressions

			if (line.empty())   // terminated by blank line
			{
				// Parse the line, regenerate the expression, clear accumulator
				if (auto const [ok, expression] = parse(cg_example); ok)
				{
					std::cout << generate(expression) << "\n\n";
				}
				cg_example.clear();
			}
		}
	}
	return 0;
}
