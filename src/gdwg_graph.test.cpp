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
TEST_CASE("graph copy constructor and assign operator") {
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
TEST_CASE("insert node and check node") {
	auto g = gdwg::graph<int, std::string>{};
	auto n1 = 1;
	auto n2 = 2;
	auto n3 = 3;

	g.insert_node(n1);
	g.insert_node(n2);
	g.insert_node(n3);

	CHECK(g.is_node(n1));
	CHECK(g.is_node(n2));
	CHECK(g.is_node(n3));
	CHECK(!g.is_node(4));
}
TEST_CASE("insert duplicate edges weight int and unweighted edge") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	CHECK(g.insert_edge(1, 2, 10) == false);

	g.insert_edge(2, 3);
	CHECK(g.insert_edge(2, 3) == false);
}
TEST_CASE("replace node with a new node") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	REQUIRE(g.is_node(1));
	REQUIRE(g.is_node(2));
	REQUIRE(g.is_node(3));

	// Replace existing node with a new node
	REQUIRE(g.replace_node(1, 4));
	CHECK(!g.is_node(1));
	CHECK(g.is_node(4));
}
TEST_CASE("replace non-existing node") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	REQUIRE(g.is_node(1));
	REQUIRE(g.is_node(2));
	REQUIRE(g.is_node(3));

	// Attempt to replace a non-existing node
	CHECK_THROWS_WITH(g.replace_node(5, 6), "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
}
TEST_CASE("merge_replace_node basic test") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_node("C");
	g.insert_edge("A", "B", 1);
	g.insert_edge("A", "C", 2);
	g.insert_edge("B", "C", 3);

	// Test merging "A" to "D" where "D" does not exist
	CHECK_THROWS_WITH(g.merge_replace_node("A", "D"),
	                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they don't exist in the "
	                  "graph");
	// Insert "D" node to make the merge valid
	g.insert_node("D");
	g.merge_replace_node("A", "D");
	CHECK(g.is_node("D"));
	CHECK(!g.is_node("A"));
	CHECK(g.is_node("B"));
	CHECK(g.is_node("C"));
	// Check that the edges are correctly replaced
	CHECK(g.insert_edge("D", "B", 1) == false);
	CHECK(g.insert_edge("D", "C", 2) == false);
	CHECK(g.insert_edge("B", "C", 3) == false);
}
TEST_CASE("merge_replace_node with duplicate edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_node("C");
	g.insert_edge("A", "B", 1);
	g.insert_edge("A", "C", 2);
	g.insert_edge("B", "B", 1);

	g.insert_node("D");

	g.merge_replace_node("A", "B");

	CHECK(g.is_node("B"));
	CHECK(!g.is_node("A"));
	CHECK(g.is_node("C"));

	CHECK(g.insert_edge("B", "B", 1) == false);
	CHECK(g.insert_edge("B", "C", 2) == false);
}
TEST_CASE("merge_replace_node with unweighted edges") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2);
	g.insert_edge(1, 3);
	g.insert_edge(2, 3);
	g.insert_node(4);

	g.merge_replace_node(1, 4);

	CHECK(g.is_node(4));
	CHECK(!g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(g.is_node(3));

	CHECK(g.insert_edge(4, 2) == false);
	CHECK(g.insert_edge(4, 3) == false);
	CHECK(g.insert_edge(2, 3) == false);
}
TEST_CASE("merge_replace_node with self loop") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "A", 1);
	g.insert_edge("A", "B", 2);

	g.insert_node("C");

	// Test merging "A" to "B"
	g.merge_replace_node("A", "B");

	CHECK(g.is_node("B"));
	CHECK(!g.is_node("A"));

	// Check that the edges are correctly replaced and duplicates removed
	CHECK(g.insert_edge("B", "B", 1) == false);
	CHECK(g.insert_edge("B", "B", 2) == false);
}
TEST_CASE("empty basic test") {
	auto g = gdwg::graph<std::string, int>{};
	CHECK(g.empty() == true);

	g.insert_node("A");
	CHECK(g.empty() == false);

	g.insert_node("B");
	CHECK(g.empty() == false);
	CHECK(g.insert_edge("A", "B", 1) == true);
}
TEST_CASE("erase_node existing node with edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);

	bool result = g.erase_node("A");
	CHECK(result == true);
	CHECK(g.is_node("A") == false);
	CHECK(g.is_node("B") == true);
}
TEST_CASE("erase_node non-existing node") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");

	bool result = g.erase_node("D");
	CHECK(result == false);
}
TEST_CASE("erase_node existing node without edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_node("C");

	bool result = g.erase_node("C");
	CHECK(result == true);
	CHECK(g.is_node("C") == false);
}
TEST_CASE("erase_node with self loop") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 1, 5);

	bool result = g.erase_node(1);
	CHECK(result == true);
	CHECK(g.is_node(1) == false);
}
TEST_CASE("erase_node from an empty graph") {
	auto g = gdwg::graph<int, int>{};

	bool result = g.erase_node(3);
	CHECK(result == false);
}
TEST_CASE("erase_node with incoming and outgoing edges") {
	auto g = gdwg::graph<int, double>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, 1.5);
	g.insert_edge(2, 3, 2.5);
	g.insert_edge(3, 1, 3.5);

	bool result = g.erase_node(2);
	CHECK(result == true);
	CHECK(g.is_node(2) == false);
	CHECK(g.is_node(1) == true);
	CHECK(g.is_node(3) == true);
	CHECK_THROWS_AS(g.insert_edge(2, 3, 2.5), std::runtime_error);
	CHECK_THROWS_AS(g.insert_edge(1, 2, 1.5), std::runtime_error);
}
TEST_CASE("is_connected with non-existing nodes") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");

	CHECK_THROWS_WITH(g.is_connected("A", "D"),
	                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
	CHECK_THROWS_WITH(g.is_connected("D", "A"),
	                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
	CHECK_THROWS_WITH(g.is_connected("D", "E"),
	                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
}
TEST_CASE("is_connected with empty graph") {
	auto g = gdwg::graph<int, double>{};

	CHECK_THROWS_WITH(g.is_connected(1, 2),
	                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
}

TEST_CASE("is_connected with nodes without edges") {
	auto g = gdwg::graph<int, double>{};
	g.insert_node(1);
	g.insert_node(2);

	CHECK(g.is_connected(1, 2) == false);
}
TEST_CASE("is_connected with nodes and edges") {
	auto g = gdwg::graph<int, double>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, 1.5);

	CHECK(g.is_connected(1, 2) == true);
}
TEST_CASE("is_connected with self loop") {
	auto g = gdwg::graph<int, double>{};
	g.insert_node(1);
	g.insert_edge(1, 1, 3.5);

	CHECK(g.is_connected(1, 1) == true);
}
TEST_CASE("not_connected with dst->src edge") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);

	CHECK(g.is_connected("B", "A") == false);
}