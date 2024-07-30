#include "gdwg_graph.h"

#include <catch2/catch.hpp>

TEST_CASE("basic test") {
	auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(g.is_node(n));
}
TEST_CASE("graph range constructor") {
	auto nodes = std::vector<int>{1, 2, 3, 4, 5};
	auto g = gdwg::graph<int, std::string>(nodes.begin(), nodes.end());
	for (auto n : nodes) {
		CHECK(g.is_node(n));
	}
}
// // test copy constructor after defined inert_edge to copy
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
TEST_CASE("graph copy constructor with empty graph") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto g2 = g1;

	CHECK(g2.empty());
}
TEST_CASE("graph copy constructor and modifying original graph") {
	auto g1 = gdwg::graph<int, std::string>{1, 2, 3};
	g1.insert_edge(1, 2, "weight1");
	auto g2 = g1;
	g1.insert_edge(2, 3, "weight2");
	g1.erase_node(1);
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_node(3));
	CHECK(g2.is_connected(1, 2));
	CHECK_FALSE(g2.is_connected(2, 3));
}
TEST_CASE("Copy assignment with empty graph") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto g2 = gdwg::graph<int, std::string>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_node(3);
	g2.insert_edge(1, 2, "weight1");
	g2 = g1;
	CHECK(g2.empty());
}
TEST_CASE("Copy assignment with multiple nodes and edges") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_node(3);
	g1.insert_edge(1, 2, "weight1");
	g1.insert_edge(2, 3, "weight2");
	auto g2 = gdwg::graph<int, std::string>{};
	g2 = g1;
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_node(3));
	CHECK(g2.insert_edge(1, 2, "weight1") == false);
	CHECK(g2.insert_edge(2, 3, "weight2") == false);
	CHECK(g2.is_connected(1, 2));
	CHECK(g2.is_connected(2, 3));
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
TEST_CASE("Move constructor: empty graph") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto g2 = gdwg::graph<int, std::string>{std::move(g1)};
	CHECK(g2.empty());
	CHECK(g1.empty());
}
TEST_CASE("Move constructor: New object contains moved nodes and edges") {
	auto g1 = gdwg::graph<std::string, int>{};
	g1.insert_node("A");
	g1.insert_node("B");
	g1.insert_edge("A", "B", 1);
	auto it = g1.begin();
	auto g2 = std::move(g1);
	CHECK(g2.is_node("A"));
	CHECK(g2.is_node("B"));
	CHECK(g2.is_connected("A", "B"));
	CHECK(it != g2.end());
	CHECK((*it).from == "A");
	CHECK((*it).to == "B");
	CHECK((*it).weight == 1);
	CHECK((g1.empty()));
}
TEST_CASE("Move constructor: single edge, no nodes") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	g1.erase_node(1);
	g1.erase_node(2);

	auto g2 = gdwg::graph<int, std::string>{std::move(g1)};
	CHECK(g2.empty());
	CHECK(g1.empty());
}
TEST_CASE("Move constructor: basic functionality with iterators") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	auto it_g1 = g1.begin();
	auto g2 = gdwg::graph<int, std::string>{std::move(g1)};
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_connected(1, 2));
	CHECK(it_g1 == g2.begin());
	CHECK(it_g1 != g2.end());
	CHECK(g1.empty());
}
TEST_CASE("Move constructor: iterators from this are invalidated") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	auto it_g1 = g1.begin();
	auto g2 = gdwg::graph<int, std::string>{std::move(g1)};
	CHECK_THROWS_AS(*it_g1, std::exception);
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_connected(1, 2));
	CHECK(g1.empty());
}
TEST_CASE("Move assignment: basic functionality") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	auto g2 = gdwg::graph<int, std::string>{};
	g2.insert_node(3);
	g2.insert_node(4);
	g2.insert_edge(3, 4, "edge2");
	g2 = std::move(g1);
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_connected(1, 2));
	CHECK(g2.find(1, 2, "edge1") != g2.end());
	CHECK(g1.empty());
}
TEST_CASE("Move assignment: move to empty graph") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	auto g2 = gdwg::graph<int, std::string>{};
	g2 = std::move(g1);
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_connected(1, 2));
	CHECK(g2.find(1, 2, "edge1") != g2.end());
	CHECK(g1.empty());
}
TEST_CASE("Move assignment: move from empty graph to another empty graph") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto g2 = gdwg::graph<int, std::string>{};
	g2 = std::move(g1);
	CHECK(g1.empty());
	CHECK(g2.empty());
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
TEST_CASE("Print unweighted edge") {
	auto e = gdwg::unweighted_edge<std::string, int>("A", "B");
	auto result = e.print_edge();
	auto expected = std::string{"A -> B | U"};
	CHECK(result == expected);
}
TEST_CASE("Print weighted edge") {
	auto e = gdwg::weighted_edge<std::string, int>("A", "B", 5);
	auto result = e.print_edge();
	std::string expected = "A -> B | W | 5";
	CHECK(result == expected);
}
TEST_CASE("Print edge with different types") {
	auto e1 = gdwg::unweighted_edge<int, double>(1, 2);
	auto result1 = e1.print_edge();
	auto expected1 = std::string{"1 -> 2 | U"};
	CHECK(result1 == expected1);

	auto e2 = gdwg::weighted_edge<int, double>(1, 2, 3.14);
	auto result2 = e2.print_edge();
	auto expected2 = std::string{"1 -> 2 | W | 3.14"};
	CHECK(result2 == expected2);
}
TEST_CASE("Test is_weighted for weighted edges") {
	auto we1 = gdwg::weighted_edge<int, double>(1, 2, 3.14);
	auto result1 = we1.is_weighted();
	CHECK(result1 == true);

	auto we2 = gdwg::weighted_edge<std::string, int>("A", "B", 10);
	auto result2 = we2.is_weighted();
	CHECK(result2 == true);
}
TEST_CASE("Test is_weighted for unweighted edges") {
	auto ue1 = gdwg::unweighted_edge<int, double>(1, 2);
	auto result1 = ue1.is_weighted();
	CHECK(result1 == false);

	auto ue2 = gdwg::unweighted_edge<std::string, int>("A", "B");
	auto result2 = ue2.is_weighted();
	CHECK(result2 == false);
}
TEST_CASE("get_weight: weighted edge") {
	auto we = gdwg::weighted_edge<int, int>{1, 2, 5};
	CHECK(we.get_weight().has_value());
	CHECK(we.get_weight() == 5);
}
TEST_CASE("get_weight: unweighted edge") {
	auto ue = gdwg::unweighted_edge<int, int>{1, 2};
	CHECK(!ue.get_weight().has_value());
}
TEST_CASE("get_weight: mixed edges in graph") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, 5); // weighted edge
	g.insert_edge(1, 3); // unweighted edge
	// Ensure the graph is populated correctly
	CHECK(g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(g.is_node(3));
	auto it_weighted = g.find(1, 2, 5);
	CHECK(it_weighted != g.end());
	CHECK((*it_weighted).weight.has_value());
	CHECK((*it_weighted).weight == 5);
	auto it_unweighted = g.find(1, 3, std::nullopt);
	CHECK(it_unweighted != g.end());
	CHECK(!(*it_unweighted).weight.has_value());
}
TEST_CASE("Test get_nodes with weighted edge") {
	auto we1 = gdwg::weighted_edge<int, double>(1, 2, 3.14);
	auto result1 = we1.get_nodes();
	CHECK(result1 == std::make_pair(1, 2));

	auto we2 = gdwg::weighted_edge<std::string, int>("A", "B", 10);
	auto result2 = we2.get_nodes();
	CHECK(result2 == std::make_pair(std::string("A"), std::string("B")));
}
TEST_CASE("Test get_nodes with unweighted edge") {
	auto ue1 = gdwg::unweighted_edge<int, double>(1, 2);
	auto result1 = ue1.get_nodes();
	CHECK(result1 == std::make_pair(1, 2));

	auto ue2 = gdwg::unweighted_edge<std::string, int>("A", "B");
	auto result2 = ue2.get_nodes();
	CHECK(result2 == std::make_pair(std::string("A"), std::string("B")));
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
TEST_CASE("Different weighted edges are not equal (different destination)") {
	using weighted_edge = gdwg::weighted_edge<int, double>;
	weighted_edge e1(1, 2, 3.5);
	weighted_edge e3(1, 3, 3.5);
	CHECK(!e1.operator==(e3));
}

TEST_CASE("Different weighted edges are not equal (different weight)") {
	using weighted_edge = gdwg::weighted_edge<int, double>;
	weighted_edge e1(1, 2, 3.5);
	weighted_edge e4(1, 2, 4.5);
	CHECK(!e1.operator==(e4));
}
TEST_CASE("Same unweighted edges are equal") {
	using unweighted_edge = gdwg::unweighted_edge<int, double>;
	unweighted_edge e1(1, 2);
	unweighted_edge e2(1, 2);
	CHECK(e1.operator==(e2));
}
TEST_CASE("Different unweighted edges are not equal (different source)") {
	using unweighted_edge = gdwg::unweighted_edge<int, double>;
	unweighted_edge e1(1, 2);
	unweighted_edge e4(2, 1);
	CHECK(!e1.operator==(e4));
}
TEST_CASE("Different unweighted edges are not equal (different destination)") {
	using unweighted_edge = gdwg::unweighted_edge<int, double>;
	unweighted_edge e1(1, 2);
	unweighted_edge e3(1, 3);
	CHECK(!e1.operator==(e3));
}
TEST_CASE("Weighted edge and unweighted edge are not equal ") {
	using weighted_edge = gdwg::weighted_edge<int, double>;
	using unweighted_edge = gdwg::unweighted_edge<int, double>;
	weighted_edge e1(1, 2, 3.5);
	unweighted_edge e2(1, 2);
	CHECK(!e1.operator==(e2));
}
TEST_CASE("erase_edge non-existent nodes") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");

	CHECK_THROWS_WITH(g.erase_edge("A", "C"),
	                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the graph");
	CHECK_THROWS_WITH(g.erase_edge("C", "B"),
	                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the graph");
}
TEST_CASE("erase_edge basic test") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	CHECK(g.erase_edge("A", "B", 1) == true);
	CHECK(g.is_connected("A", "B") == false);
	CHECK(g.erase_edge("A", "B", 1) == false); // Edge already removed
}
TEST_CASE("erase_edge with multiple edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	g.insert_edge("A", "B", 2);

	CHECK(g.erase_edge("A", "B", 1) == true);
	CHECK(g.is_connected("A", "B") == true); // There is still one edge
	CHECK(g.erase_edge("A", "B", 2) == true);
	CHECK(g.is_connected("A", "B") == false); // All edges removed
}
TEST_CASE("erase_edge with reverse direction edge") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	CHECK(g.erase_edge("B", "A", 1) == false);
	CHECK(g.is_connected("A", "B") == true);
}
TEST_CASE("erase_edge with unweighted edge") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B");
	CHECK(g.erase_edge("A", "B") == true);
	CHECK(g.is_connected("A", "B") == false);
	CHECK(g.erase_edge("A", "B") == false); // Edge already removed
}
TEST_CASE("nodes function returns empty vector for empty graph") {
	auto g = gdwg::graph<int, int>{};
	auto nodes = g.nodes();
	CHECK(nodes.empty());
}

TEST_CASE("nodes function returns single node") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	auto nodes = g.nodes();
	CHECK(nodes.size() == 1);
	CHECK(nodes[0] == 1);
}
TEST_CASE("nodes function returns multiple nodes in sorted order") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(3);
	g.insert_node(1);
	g.insert_node(2);
	auto nodes = g.nodes();
	CHECK(nodes.size() == 3);
	CHECK(nodes[0] == 1);
	CHECK(nodes[1] == 2);
	CHECK(nodes[2] == 3);
}
TEST_CASE("nodes function with duplicate nodes") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(1); // Attempt to insert duplicate node
	auto nodes = g.nodes();
	CHECK(nodes.size() == 1);
	CHECK(nodes[0] == 1);
}
TEST_CASE("nodes function with string nodes") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("banana");
	g.insert_node("apple");
	g.insert_node("cherry");
	auto nodes = g.nodes();
	CHECK(nodes.size() == 3);
	CHECK(nodes[0] == "apple");
	CHECK(nodes[1] == "banana");
	CHECK(nodes[2] == "cherry");
}
TEST_CASE("edges function throws error for non-existing nodes") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");

	CHECK_THROWS_WITH(g.edges("A", "C"),
	                  "Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the graph");
	CHECK_THROWS_WITH(g.edges("C", "B"),
	                  "Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the graph");
	CHECK_THROWS_WITH(g.edges("C", "D"),
	                  "Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the graph");
}
TEST_CASE("edges function returns all edges from src to dst in correct order") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B"); // Unweighted edge
	g.insert_edge("A", "B", 2); // Weighted edge
	g.insert_edge("A", "B", 1); // Weighted edge
	g.insert_edge("A", "B", 3); // Weighted edge
	auto edges = g.edges("A", "B");
	CHECK(edges.size() == 4);
	CHECK(!edges[0]->get_weight().has_value()); // Unweighted edge
	CHECK(edges[1]->get_weight() == 1);
	CHECK(edges[2]->get_weight() == 2);
	CHECK(edges[3]->get_weight() == 3);
}
TEST_CASE("edges function handles self loops") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_edge("A", "A", 4);
	g.insert_edge("A", "A", 2);
	g.insert_edge("A", "A", 3);
	g.insert_edge("A", "A"); // Unweighted edge
	auto edges = g.edges("A", "A");
	CHECK(edges.size() == 4);
	CHECK(!edges[0]->get_weight().has_value()); // Unweighted edge
	CHECK(edges[1]->get_weight() == 2);
	CHECK(edges[2]->get_weight() == 3);
	CHECK(edges[3]->get_weight() == 4);
}
TEST_CASE("Find existing unweighted edge") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2);

	auto it = g.find(1, 2);
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK(!(*it).weight.has_value());
}
TEST_CASE("Find existing weighted edge") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge1");
	auto it = g.find(1, 2, "edge1");
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight.has_value());
	CHECK((*it).weight.value() == "edge1");
}
TEST_CASE("Find non-existing edge") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	auto it = g.find(1, 2);
	CHECK(it == g.end());
}
TEST_CASE("Find unweighted edge with weight parameter") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2);
	auto it = g.find(1, 2, "edge1");
	CHECK(it == g.end());
}
TEST_CASE("Find weighted edge with different weight parameter") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge1");
	auto it = g.find(1, 2, "edge2");
	CHECK(it == g.end());
}
TEST_CASE("Find in a complex graph with multiple edges and weights") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	g.insert_edge(2, 3, "edge3");
	g.insert_edge(3, 4, "edge4");
	g.insert_edge(1, 4);
	SECTION("Find existing weighted edge") {
		auto it = g.find(1, 2, "edge1");
		CHECK(it != g.end());
		CHECK((*it).from == 1);
		CHECK((*it).to == 2);
		CHECK((*it).weight.has_value());
		CHECK((*it).weight.value() == "edge1");
		it = g.find(2, 3, "edge3");
		CHECK(it != g.end());
		CHECK((*it).from == 2);
		CHECK((*it).to == 3);
		CHECK((*it).weight.has_value());
		CHECK((*it).weight.value() == "edge3");
	}
	SECTION("Find existing unweighted edge") {
		auto it = g.find(1, 4);
		CHECK(it != g.end());
		CHECK((*it).from == 1);
		CHECK((*it).to == 4);
		CHECK(!(*it).weight.has_value());
	}
	SECTION("Find non-existing edge") {
		auto it = g.find(2, 4);
		CHECK(it == g.end());

		it = g.find(3, 1);
		CHECK(it == g.end());
	}
	SECTION("Find weighted edge with wrong weight") {
		auto it = g.find(1, 2, "wrong_edge");
		CHECK(it == g.end());
	}
	SECTION("Find unweighted edge with weight parameter") {
		auto it = g.find(1, 4, "edge5");
		CHECK(it == g.end());
	}
}
TEST_CASE("Connections with a non-existing node") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	CHECK_THROWS_WITH(g.connections("B"), "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the graph");
}
TEST_CASE("Connections with no outgoing edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	auto result = g.connections("A");
	CHECK(result.empty());
}
TEST_CASE("Connections with one outgoing edge") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	auto result = g.connections("A");
	auto expected = std::vector<std::string>{"B"};
	CHECK(result == expected);
}
TEST_CASE("Connections with duplicate outgoing edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	g.insert_edge("A", "B", 2);
	auto result = g.connections("A");
	auto expected = std::vector<std::string>{"B"};
	CHECK(result == expected);
}
TEST_CASE("Connections with differently weighted outgoing edges and test order") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_node("C");
	g.insert_node("D");
	g.insert_edge("A", "C", 3);
	g.insert_edge("A", "B", 1);
	g.insert_edge("A", "D", 2);
	auto result = g.connections("A");
	auto expected = std::vector<std::string>{"B", "C", "D"};
	CHECK(result == expected);
}
TEST_CASE("Connections with unweighted and weighted outgoing edges") {
	auto g = gdwg::graph<std::string, int>{};
	g.insert_node("A");
	g.insert_node("B");
	g.insert_node("C");
	g.insert_node("D");
	g.insert_edge("A", "C");
	g.insert_edge("A", "B", 2);
	g.insert_edge("A", "D", 1);
	auto result = g.connections("A");
	auto expected = std::vector<std::string>{"B", "C", "D"};
	CHECK(result == expected);
}
TEST_CASE("Iterator default constructor and comparison") {
	auto it1 = gdwg::graph<int, std::string>::iterator();
	auto it2 = gdwg::graph<int, std::string>::iterator();
	CHECK(it1 == it2); // Both are default constructed
}
TEST_CASE("Iterator specific element constructor and dereference") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge");
	auto it = g.begin();
	auto edge = *it;
	CHECK(edge.from == 1);
	CHECK(edge.to == 2);
	CHECK(edge.weight == "edge");
}
TEST_CASE("Iterator pre-increment basic consistenct node graph") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, 1);
	g.insert_edge(2, 3, 2);
	auto it = g.begin();
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	++it;
	CHECK((*it).from == 2);
	CHECK((*it).to == 3);
}
TEST_CASE("Iterator post-increment") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(2, 3, "edge2");
	auto it = g.begin();
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	auto it2 = it++;
	CHECK((*it2).from == 1);
	CHECK((*it2).to == 2);
	CHECK((*it).from == 2);
	CHECK((*it).to == 3);
}
TEST_CASE("Iterator pre-increment with complex graph structure") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_node(5);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	g.insert_edge(2, 4, "edge3");
	g.insert_edge(3, 4, "edge4");
	g.insert_edge(4, 5, "edge5");
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == "edge1");
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == "edge2");
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 2);
	CHECK((*it).to == 4);
	CHECK((*it).weight == "edge3");
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 3);
	CHECK((*it).to == 4);
	CHECK((*it).weight == "edge4");
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 4);
	CHECK((*it).to == 5);
	CHECK((*it).weight == "edge5");
}
TEST_CASE("Iterator pre-increment with complex graph and refexive node") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_node(5);
	g.insert_edge(1, 2, 1);
	g.insert_edge(1, 3, 2);
	g.insert_edge(3, 2, 3);
	g.insert_edge(2, 4, 4);
	g.insert_edge(5, 5, 5);
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 1);
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == 2);
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 2);
	CHECK((*it).to == 4);
	CHECK((*it).weight == 4);
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 3);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 3);
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 5);
	CHECK((*it).to == 5);
	CHECK((*it).weight == 5);
	++it;
	CHECK(it == g.end());
}
TEST_CASE("Iterator pre-increment with complex graph(not line graph) and split node") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_edge(1, 2, 1);
	g.insert_edge(1, 3, 2);
	g.insert_edge(3, 4, 3);
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 1);
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == 2);
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 3);
	CHECK((*it).to == 4);
	CHECK((*it).weight == 3);
	++it;
	CHECK(it == g.end());
}
TEST_CASE("Iterator post-increment with complex graph and reflexive node") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_node(5);
	g.insert_edge(1, 2, 1);
	g.insert_edge(1, 3, 2);
	g.insert_edge(3, 2, 3);
	g.insert_edge(2, 4, 4);
	g.insert_edge(5, 5, 5);
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 1);
	auto it2 = it++;
	CHECK(it2 != g.end());
	CHECK((*it2).from == 1);
	CHECK((*it2).to == 2);
	CHECK((*it2).weight == 1);
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == 2);
	it2 = it++;
	CHECK(it2 != g.end());
	CHECK((*it2).from == 1);
	CHECK((*it2).to == 3);
	CHECK((*it2).weight == 2);
	CHECK(it != g.end());
	CHECK((*it).from == 2);
	CHECK((*it).to == 4);
	CHECK((*it).weight == 4);
	it2 = it++;
	CHECK(it2 != g.end());
	CHECK(it != g.end());
	CHECK((*it).from == 3);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 3);
	it2 = it++;
	CHECK(it2 != g.end());
	CHECK((*it2).from == 3);
	CHECK((*it2).to == 2);
	CHECK((*it2).weight == 3);
	it2 = it++;
	CHECK(it == g.end());
}
TEST_CASE("Iterator pre-decrement with empty nodes") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4); // No edges connected to node 4
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(2, 3, "edge2");
	auto it = g.end();
	--it;
	CHECK((*it).from == 2);
	CHECK((*it).to == 3);
	CHECK((*it).weight == "edge2");
	--it;
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == "edge1");
}
TEST_CASE("Iterator pre-decrement to begin") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(2, 3, "edge2");
	auto it = g.end();
	--it;
	CHECK((*it).from == 2);
	CHECK((*it).to == 3);
	CHECK((*it).weight == "edge2");
	--it;
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == "edge1");
	--it;
	CHECK(it == g.begin());
}
TEST_CASE("Iterator post-decrement with complex graph and reflexive node") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_node(5);
	g.insert_edge(1, 2, 1);
	g.insert_edge(1, 3, 2);
	g.insert_edge(3, 2, 3);
	g.insert_edge(2, 4, 4);
	g.insert_edge(5, 5, 5);
	auto it = g.end();
	--it;
	CHECK(it != g.end());
	CHECK((*it).from == 5);
	CHECK((*it).to == 5);
	CHECK((*it).weight == 5);
	auto it2 = it--;
	CHECK(it2 != g.end());
	CHECK((*it2).from == 5);
	CHECK((*it2).to == 5);
	CHECK((*it2).weight == 5);
	CHECK(it != g.end());
	CHECK((*it).from == 3);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 3);
	it2 = it--;
	CHECK(it2 != g.end());
	CHECK((*it2).from == 3);
	CHECK((*it2).to == 2);
	CHECK((*it2).weight == 3);
	CHECK(it != g.end());
	CHECK((*it).from == 2);
	CHECK((*it).to == 4);
	CHECK((*it).weight == 4);
	std::advance(it, -2);
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 1);
	it2 = it--;
	CHECK(it2 != g.end());
	CHECK(it2 == g.begin());
	CHECK((*it2).from == 1);
	CHECK((*it2).to == 2);
	CHECK((*it2).weight == 1);
	CHECK(it == g.begin());
}
TEST_CASE("Iterator post-decrement with complex graph and reflexive node should be 1->12 1->21 12->19 19->1 19->21") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(12);
	g.insert_node(19);
	g.insert_node(21);
	g.insert_node(67);
	g.insert_edge(1, 12, 3);
	g.insert_edge(12, 19, 3);
	g.insert_edge(1, 21, 3);
	g.insert_edge(19, 1, 3);
	g.insert_edge(19, 21, 3);
	auto it = g.end();
	--it;
	CHECK(it != g.end());
	CHECK((*it).from == 19);
	CHECK((*it).to == 21);
	CHECK((*it).weight == 3);
	auto it2 = it--;
	CHECK(it2 != g.end());
	CHECK((*it2).from == 19);
	CHECK((*it2).to == 21);
	CHECK((*it2).weight == 3);
	CHECK(it != g.end());
	CHECK((*it).from == 19);
	CHECK((*it).to == 1);
	CHECK((*it).weight == 3);
	std::advance(it, -3);
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 12);
	CHECK((*it).weight == 3);
	std::advance(it, 3);
	std::advance(it, -3);
	CHECK(it == g.begin());
}
TEST_CASE("Empty graph begin() and end()") {
	auto g = gdwg::graph<int, std::string>{};
	auto it = g.begin();
	CHECK(it == g.end());
}
TEST_CASE("Single node graph begin() and end()") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	auto it = g.begin();
	CHECK(it == g.end());
}
TEST_CASE("Single edge graph begin() and end()") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge1");
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == "edge1");
}
TEST_CASE("Graph with self-loop begin() and end()") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_edge(1, 1, "self-loop");
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 1);
	CHECK((*it).weight == "self-loop");
}
TEST_CASE("Graph with multiple edges from a single node begin() and end()") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == "edge1");
	++it;
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == "edge2");
	++it;
	CHECK(it == g.end());
}
TEST_CASE("Graphs with identical nodes and edges are equal") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_node(3);
	g1.insert_edge(1, 2, "edge1");
	g1.insert_edge(2, 3, "edge2");
	auto g2 = gdwg::graph<int, std::string>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_node(3);
	g2.insert_edge(1, 2, "edge1");
	g2.insert_edge(2, 3, "edge2");
	CHECK(g1 == g2);
}
TEST_CASE("Graphs with different nodes are not equal") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	auto g2 = gdwg::graph<int, std::string>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_node(3);
	CHECK(g1 != g2);
}
TEST_CASE("Graphs with different edges are not equal") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	auto g2 = gdwg::graph<int, std::string>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_edge(1, 2, "edge2");
	CHECK(g1 != g2);
}
TEST_CASE("Graphs with one having extra edges are not equal") {
	auto g1 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	auto g2 = gdwg::graph<int, std::string>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_edge(1, 2, "edge1");
	g2.insert_edge(2, 1, "edge2");
	CHECK(!(g1 == g2));
}
TEST_CASE("Graphs with edges in different order are equal") {
	auto g1 = gdwg::graph<int, std::string>{};
	auto g2 = gdwg::graph<int, std::string>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, "edge1");
	g1.insert_edge(2, 1, "edge2");
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_edge(2, 1, "edge2"); // Different order
	g2.insert_edge(1, 2, "edge1");
	CHECK(g1 == g2);
}
TEST_CASE("Erase edge: basic functionality") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge1");
	auto it = g.find(1, 2, "edge1");
	auto next_it = g.erase_edge(it);
	CHECK(g.find(1, 2, "edge1") == g.end());
	CHECK(next_it == g.end());
	CHECK(!g.is_connected(1, 2));
	CHECK(g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(!g.empty());
}
TEST_CASE("Insert and erase edge") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, 5);
	auto it = g.find(1, 2, 5);
	CHECK(it != g.end());
	std::cout << "Inserted edge (1, 2, 5)" << std::endl;
	g.erase_edge(1, 2, 5);
	it = g.find(1, 2, 5);
	CHECK(it == g.end());
	std::cout << "Erased edge (1, 2, 5)" << std::endl;
}
TEST_CASE("Erase edge: erase last edge") {
	auto g = gdwg::graph<int, int>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, 5);
	g.insert_edge(1, 3, 10);
	g.insert_edge(2, 3, 15);

	auto it = g.find(2, 3, 15);
	CHECK(it != g.end());

	auto next_it = g.erase_edge(it);
	CHECK(next_it == g.end());
	CHECK(g.find(2, 3, 15) == g.end());
}
TEST_CASE("Erase edge: erase middle edge") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	g.insert_edge(2, 3, "edge3");
	auto it = g.find(1, 2, "edge1");
	auto next_it = g.erase_edge(it);
	CHECK(g.find(1, 2, "edge1") == g.end());
	CHECK(next_it != g.end());
	CHECK(g.is_connected(1, 3));
	CHECK(g.is_connected(2, 3));
	CHECK(!g.empty());
	auto next_it_ = g.erase_edge(next_it);
	CHECK(g.find(1, 3, "edge2") == g.end());
	CHECK(next_it_ != g.end());
	auto next_it_1 = g.erase_edge(next_it_);
	CHECK(g.find(2, 3, "edge3") == g.end());
	CHECK(next_it_1 == g.end());
}
TEST_CASE("Erase edge: erase all edges from a node") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	auto it1 = g.find(1, 2, "edge1");
	auto it2 = g.find(1, 3, "edge2");
	g.erase_edge(it1);
	auto next_it = g.erase_edge(it2);
	CHECK(g.find(1, 2, "edge1") == g.end());
	CHECK(g.find(1, 3, "edge2") == g.end());
	CHECK(next_it == g.end());
	CHECK(!g.is_connected(1, 2));
	CHECK(!g.is_connected(1, 3));
	CHECK(g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(g.is_node(3));
	CHECK(!g.empty());
}
TEST_CASE("Erase edge: erase non-existing edge") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge1");
	auto it = g.find(2, 1, "edge2");
	CHECK(it == g.end());
	auto next_it = g.erase_edge(it);
	CHECK(next_it == g.end());
	CHECK(g.is_connected(1, 2));
	CHECK(!g.is_connected(2, 1));
	CHECK(g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(!g.empty());
}
TEST_CASE("Erase edge: erase range of edges") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	g.insert_edge(2, 3, "edge3");
	g.insert_edge(3, 1, "edge4");
	auto it_start = g.find(1, 2, "edge1");
	auto it_end = g.find(2, 3, "edge3");
	auto ret_it = g.erase_edge(it_start, it_end);
	// Check if the edges are correctly erased
	CHECK(g.find(1, 2, "edge1") == g.end());
	CHECK(g.find(1, 3, "edge2") == g.end());
	CHECK(ret_it == it_end);
}
TEST_CASE("Erase edge: erase single edge") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, "edge1");
	auto it_start = g.find(1, 2, "edge1");
	auto it_end = std::next(it_start);
	auto ret_it = g.erase_edge(it_start, it_end);
	CHECK(g.find(1, 2, "edge1") == g.end());
	CHECK(ret_it == it_end);
	CHECK(g.empty() == false);
}
TEST_CASE("Clear: erasing all nodes and edges") {
	auto g = gdwg::graph<int, std::string>{};
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, "edge1");
	g.insert_edge(1, 3, "edge2");
	g.insert_edge(2, 3, "edge3");
	g.clear();
	CHECK(g.empty() == true);
	CHECK(g.begin() == g.end());
}
TEST_CASE("operator==: empty graphs") {
	auto g1 = gdwg::graph<int, int>{};
	auto g2 = gdwg::graph<int, int>{};
	CHECK(g1 == g2);
}
TEST_CASE("operator==: graphs with same nodes but no edges") {
	auto g1 = gdwg::graph<int, int>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_node(3);
	auto g2 = gdwg::graph<int, int>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_node(3);
	CHECK(g1 == g2);
}
TEST_CASE("operator==: graphs with different nodes but same edges") {
	auto g1 = gdwg::graph<int, int>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, 5);
	auto g2 = gdwg::graph<int, int>{};
	g2.insert_node(1);
	g2.insert_node(3);
	g2.insert_edge(1, 3, 5);
	CHECK(!(g1 == g2));
}
TEST_CASE("operator==: graphs with different nodes and edges") {
	auto g1 = gdwg::graph<int, int>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, 5);
	auto g2 = gdwg::graph<int, int>{};
	g2.insert_node(3);
	g2.insert_node(4);
	g2.insert_edge(3, 4, 10);
	CHECK(!(g1 == g2));
}
TEST_CASE("operator==: graphs with different weighted edges") {
	auto g1 = gdwg::graph<int, int>{};
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, 5);
	auto g2 = gdwg::graph<int, int>{};
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_edge(1, 2);
	CHECK(!(g1 == g2));
}
TEST_CASE("Graph output: formatted output") {
	using graph = gdwg::graph<int, int>;
	auto const v = std::vector<std::tuple<int, int, std::optional<int>>>{
	    {2, 4, std::nullopt},
	    {2, 4, 2},
	    {2, 1, 1},
	    {4, 1, std::nullopt},
	    {1, 5, -1},
	    {3, 6, -8},
	    {4, 5, 3},
	};
	auto g = graph{};
	for (const auto& [from, to, weight] : v) {
		g.insert_node(from);
		g.insert_node(to);
		if (weight.has_value()) {
			g.insert_edge(from, to, weight.value());
		}
		else {
			g.insert_edge(from, to);
		}
	}
	g.insert_node(64);
	auto out = std::ostringstream{};
	out << g;
	auto const expected_output = std::string_view(R"(1 (
  1 -> 5 | W | -1
)
2 (
  2 -> 1 | W | 1
  2 -> 4 | U
  2 -> 4 | W | 2
)
3 (
  3 -> 6 | W | -8
)
4 (
  4 -> 1 | U
  4 -> 5 | W | 3
)
5 (
)
6 (
)
64 (
)
)");
	CHECK(out.str() == expected_output);
}