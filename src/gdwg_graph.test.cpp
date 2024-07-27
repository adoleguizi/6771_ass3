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
TEST_CASE("Move constructor: New object contains moved nodes and edges") {
	auto g1 = gdwg::graph<std::string, int>{};
	g1.insert_node("A");
	g1.insert_node("B");
	g1.insert_edge("A", "B", 1);

	auto g2 = std::move(g1);

	CHECK(g2.is_node("A"));
	CHECK(g2.is_node("B"));
	CHECK(g2.is_connected("A", "B"));
}
TEST_CASE("Move constructor: Original object is empty after move") {
	auto g1 = gdwg::graph<std::string, int>{};
	g1.insert_node("A");
	g1.insert_node("B");
	g1.insert_edge("A", "B", 1);
	auto g2 = std::move(g1);
	CHECK(g1.empty());
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
	gdwg::unweighted_edge<std::string, int> e("A", "B");
	auto result = e.print_edge();
	std::string expected = "A -> B | U";
	CHECK(result == expected);
}
TEST_CASE("Print weighted edge") {
	gdwg::weighted_edge<std::string, int> e("A", "B", 5);
	auto result = e.print_edge();
	std::string expected = "A -> B | W | 5";
	CHECK(result == expected);
}
TEST_CASE("Print edge with different types") {
	gdwg::unweighted_edge<int, double> e1(1, 2);
	auto result1 = e1.print_edge();
	std::string expected1 = "1 -> 2 | U";
	CHECK(result1 == expected1);

	gdwg::weighted_edge<int, double> e2(1, 2, 3.14);
	auto result2 = e2.print_edge();
	std::string expected2 = "1 -> 2 | W | 3.14";
	CHECK(result2 == expected2);
}
TEST_CASE("Test is_weighted for weighted edges") {
	gdwg::weighted_edge<int, double> we1(1, 2, 3.14);
	auto result1 = we1.is_weighted();
	CHECK(result1 == true);

	gdwg::weighted_edge<std::string, int> we2("A", "B", 10);
	auto result2 = we2.is_weighted();
	CHECK(result2 == true);
}

TEST_CASE("Test is_weighted for unweighted edges") {
	gdwg::unweighted_edge<int, double> ue1(1, 2);
	auto result1 = ue1.is_weighted();
	CHECK(result1 == false);

	gdwg::unweighted_edge<std::string, int> ue2("A", "B");
	auto result2 = ue2.is_weighted();
	CHECK(result2 == false);
}
TEST_CASE("Test get_nodes with weighted edge") {
	gdwg::weighted_edge<int, double> we1(1, 2, 3.14);
	auto result1 = we1.get_nodes();
	CHECK(result1 == std::make_pair(1, 2));

	gdwg::weighted_edge<std::string, int> we2("A", "B", 10);
	auto result2 = we2.get_nodes();
	CHECK(result2 == std::make_pair(std::string("A"), std::string("B")));
}
TEST_CASE("Test get_nodes with unweighted edge") {
	gdwg::unweighted_edge<int, double> ue1(1, 2);
	auto result1 = ue1.get_nodes();
	CHECK(result1 == std::make_pair(1, 2));

	gdwg::unweighted_edge<std::string, int> ue2("A", "B");
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
TEST_CASE("Connections with a non-existing node") {
	gdwg::graph<std::string, int> g;
	g.insert_node("A");
	CHECK_THROWS_WITH(g.connections("B"), "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the graph");
}
TEST_CASE("Connections with no outgoing edges") {
	gdwg::graph<std::string, int> g;
	g.insert_node("A");
	auto result = g.connections("A");
	CHECK(result.empty());
}
TEST_CASE("Connections with one outgoing edge") {
	gdwg::graph<std::string, int> g;
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	auto result = g.connections("A");
	std::vector<std::string> expected = {"B"};
	CHECK(result == expected);
}
TEST_CASE("Connections with duplicate outgoing edges") {
	gdwg::graph<std::string, int> g;
	g.insert_node("A");
	g.insert_node("B");
	g.insert_edge("A", "B", 1);
	g.insert_edge("A", "B", 2);
	auto result = g.connections("A");
	auto expected = std::vector<std::string>{"B"};
	CHECK(result == expected);
}
TEST_CASE("Connections with differently weighted outgoing edges and test order") {
	gdwg::graph<std::string, int> g;
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
	gdwg::graph<std::string, int> g;
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
	gdwg::graph<int, std::string>::iterator it1;
	gdwg::graph<int, std::string>::iterator it2;
	CHECK(it1 == it2); // Both are default constructed
}
TEST_CASE("Iterator specific element constructor and dereference") {
	gdwg::graph<int, std::string> g;
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
	gdwg::graph<int, int> g;
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
	gdwg::graph<int, std::string> g;
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
	gdwg::graph<int, std::string> g;
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
	gdwg::graph<int, int> g;
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
	gdwg::graph<int, int> g;
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
	gdwg::graph<int, int> g;
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
	gdwg::graph<int, std::string> g;
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
	gdwg::graph<int, std::string> g;
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
	gdwg::graph<int, int> g;
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
	gdwg::graph<int, int> g;
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
	gdwg::graph<int, std::string> g;
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
	gdwg::graph<int, std::string> g;
	g.insert_node(1);
	g.insert_edge(1, 1, "self-loop");
	auto it = g.begin();
	CHECK(it != g.end());
	CHECK((*it).from == 1);
	CHECK((*it).to == 1);
	CHECK((*it).weight == "self-loop");
}