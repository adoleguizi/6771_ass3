#include "gdwg_graph.h"

#include <catch2/catch.hpp>

TEST_CASE("basic test") {
	auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(g.is_node(n));
}
TEST_CASE("graph range constructor") {
	std::vector<int> nodes = {1, 2, 3, 4, 5};
	auto g = gdwg::graph<int, std::string>(nodes.begin(), nodes.end());
	for (auto n : nodes) {
		CHECK(g.is_node(n));
	}
}
// test copy constructor after defined inert_edge to copy
TEST_CASE("graph copy constructor") {
	auto g1 = gdwg::graph<int, std::string>{1, 2, 3};
	g1.insert_edge(1, 2);
	g1.insert_edge(2, 3, "weight1");

	auto g2 = g1;

	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_node(3));
	CHECK(g2.insert_edge(1, 2) == false);
	CHECK(g2.insert_edge(2, 3, "weight1") == false);
}