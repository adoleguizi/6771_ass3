#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <initializer_list>
#include <type_traits>
#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
namespace gdwg {
	template<typename N, typename E>
	class graph;

	template<typename N, typename E>
	class edge {
	 public:
		virtual ~edge() = default;

		virtual std::optional<E> get_weight() const = 0;
		// is_weighted() const noexcept -> bool;
		[[nodiscard]] virtual auto is_weighted() const noexcept -> bool = 0;
		// get_nodes() const noexcept -> std::pair<N, N>;
		virtual auto get_nodes() const noexcept -> std::pair<N, N> = 0;
		// print_edge() const noexcept -> std::string;
		virtual auto print_edge() const noexcept -> std::string = 0;
		// operator ==
		virtual auto operator==(edge const& other) const noexcept -> bool = 0;

	 protected:
		edge(const edge& other) = default;
		edge& operator=(const edge& other) = default;
		edge(edge&& other) noexcept = default;
		edge& operator=(edge&& other) noexcept = default;
		edge() = default;

	 private:
		template<typename NW, typename EW>
		friend class graph;
	};

	template<typename N, typename E>
	class graph {
	 public:
		using edge = gdwg::edge<N, E>;
		using iterator = typename std::vector<std::unique_ptr<edge>>::const_iterator;

		// auto get_edges() -> const std::vector<std::unique_ptr<edge>>& {
		// 	return edges_;
		// }
		graph();
		// Your member functions go here
		template<typename InputIt>
		graph(InputIt first, InputIt last);

		graph(std::initializer_list<N> il)
		: graph(il.begin(), il.end()) { // delegate to range constructor
		}

		graph(graph&& other) noexcept;
		// move assignment opeartor=
		auto operator=(graph&& other) noexcept -> graph&;
		// copy constructor
		graph(graph const& other);
		// copy assignment operator=
		auto operator=(graph const& other) -> graph&;
		// modifiers
		auto insert_node(N const& val) -> bool;
		// accessors is_node in graph
		[[nodiscard]] auto is_node(N const& value) const noexcept -> bool;

		auto insert_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool;
		// replace_node()
		auto replace_node(N const& old_data, N const& new_data) -> bool;
		// replace_edge()
		auto merge_replace_node(N const& old_data, N const& new_data) -> void;

		[[nodiscard]] auto empty() -> bool;

		auto erase_node(N const& value) -> bool;

		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool;

		auto erase_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool;

		[[nodiscard]] auto nodes() const noexcept -> std::vector<N>;
		// get all edges from src to dst according to the order
		[[nodiscard]] auto edges(N const& src, N const& dst) const -> std::vector<std::unique_ptr<edge>>;
		// find the edge from src to dst return iterator
		[[nodiscard]] auto find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) const ->
		    typename std::vector<std::unique_ptr<edge>>::const_iterator;

	 protected:
		std::set<N> nodes_;
		std::vector<std::unique_ptr<edge>> edges_;
		auto get_edges() const -> const std::vector<std::unique_ptr<edge>>& {
			return edges_;
		}
		// std::map<N, std::vector<std::unique_ptr<edge>>> edges_;
	};
	template<typename N, typename E>
	class weighted_edge : public edge<N, E> {
	 public:
		weighted_edge(N src, N dst, E weight)
		: edge<N, E>()
		, src_(src)
		, dst_(dst)
		, weight_(weight) {}
		auto get_weight() const noexcept -> std::optional<E> override;
		auto is_weighted() const noexcept -> bool override;
		auto get_nodes() const noexcept -> std::pair<N, N> override;
		auto print_edge() const noexcept -> std::string override;
		auto operator==(edge<N, E> const& other) const noexcept -> bool override;

	 private:
		N src_;
		N dst_;
		template<typename T>
		auto to_string(const T& value) const -> std::string {
			if constexpr (std::is_same_v<T, std::string>) {
				return value;
			}
			else {
				return std::to_string(value);
			}
		}
		E weight_;
	};
	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		unweighted_edge(N src, N dst)
		: edge<N, E>()
		, src_(src)
		, dst_(dst) {}
		auto get_weight() const noexcept -> std::optional<E> override;
		auto is_weighted() const noexcept -> bool override;
		auto get_nodes() const noexcept -> std::pair<N, N> override;
		auto print_edge() const noexcept -> std::string override;
		auto operator==(edge<N, E> const& other) const noexcept -> bool override;

	 private:
		N src_;
		N dst_;
		template<typename T>
		auto to_string(const T& value) const -> std::string {
			if constexpr (std::is_same_v<T, std::string>) {
				return value;
			}
			else {
				return std::to_string(value);
			}
		}
	};

} // namespace gdwg
template<typename N, typename E>
class TestableGraph : public gdwg::graph<N, E> {
 public:
	using gdwg::graph<N, E>::get_edges; // make protected member get_edges public
};
template<typename N, typename E>
gdwg::graph<N, E>::graph()
: nodes_()
, edges_() {
	// Constructor
}

template<typename N, typename E>
template<typename InputIt>
gdwg::graph<N, E>::graph(InputIt first, InputIt last) {
	static_assert(
	    std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value,
	    "InputIt must be an input iterator");

	// Check if the dereferenced iterator type is convertible to N
	static_assert(std::is_convertible<typename std::iterator_traits<InputIt>::value_type, N>::value,
	              "The dereferenced iterator type must be convertible to N");
	for (auto it = first; it != last; ++it) {
		insert_node(*it);
	}
	// might modify for insert edge
}
template<typename N, typename E>
gdwg::graph<N, E>::graph(graph&& other) noexcept
: nodes_(std::exchange(other.nodes_, {}))
, // using std::exchange to set other.nodes_ to empty
edges_(std::exchange(other.edges_, {})) // using std::exchange to set other.edges_ to empty
{
	// Move constructor
}
template<typename N, typename E>
auto gdwg::graph<N, E>::operator=(graph&& other) noexcept -> graph& {
	if (this != &other) {
		nodes_ = std::exchange(other.nodes_, {});
		edges_ = std::exchange(other.edges_, {});
	}
	return *this;
}
template<typename N, typename E>
gdwg::graph<N, E>::graph(graph const& other) {
	nodes_ = other.nodes_;
	// might modify for insert edge
	for (const auto& e : other.edges_) {
		if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
			edges_.push_back(std::make_unique<weighted_edge<N, E>>(*we));
		}
		else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
			edges_.push_back(std::make_unique<unweighted_edge<N, E>>(*ue));
		}
	}
}
template<typename N, typename E>
auto gdwg::graph<N, E>::operator=(graph const& other) -> graph& {
	if (this != &other) {
		// clear the current nodes and edges
		nodes_.clear();
		edges_.clear();
		// deep copy nodes
		nodes_ = other.nodes_;
		// deep copy edges
		for (const auto& edge : other.edges_) {
			if (auto we = dynamic_cast<weighted_edge<N, E>*>(edge.get())) {
				edges_.push_back(std::make_unique<weighted_edge<N, E>>(*we));
			}
			else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(edge.get())) {
				edges_.push_back(std::make_unique<unweighted_edge<N, E>>(*ue));
			}
		}
	}
	return *this;
}
template<typename N, typename E>
auto gdwg::weighted_edge<N, E>::get_weight() const noexcept -> std::optional<E> {
	return weight_;
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::get_weight() const noexcept -> std::optional<E> {
	return std::nullopt;
}
template<typename N, typename E>
auto gdwg::weighted_edge<N, E>::is_weighted() const noexcept -> bool {
	return true;
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::is_weighted() const noexcept -> bool {
	return false;
}
template<typename N, typename E>
auto gdwg::weighted_edge<N, E>::get_nodes() const noexcept -> std::pair<N, N> {
	return std::make_pair(this->src_, this->dst_);
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::get_nodes() const noexcept -> std::pair<N, N> {
	return std::make_pair(this->src_, this->dst_);
}
template<typename N, typename E>
auto gdwg::weighted_edge<N, E>::print_edge() const noexcept -> std::string {
	return to_string(this->src_) + " -> " + to_string(this->dst_) + " | W |  " + to_string(weight_);
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::print_edge() const noexcept -> std::string {
	return to_string(this->src_) + " -> " + to_string(this->dst_) + " | U";
}
template<typename N, typename E>
auto gdwg::graph<N, E>::insert_node(N const& value) -> bool {
	auto result = nodes_.insert(value);
	return result.second; // 如果插入成功，返回 true；否则返回 false
}
template<typename N, typename E>
[[nodiscard]] auto gdwg::graph<N, E>::is_node(N const& value) const noexcept -> bool {
	return nodes_.find(value) != nodes_.end();
}
template<typename N, typename E>
auto gdwg::graph<N, E>::insert_edge(N const& src, N const& dst, std::optional<E> weight) -> bool {
	if (!is_node(src) or !is_node(dst)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does not "
		                         "exist");
	}
	// no same weight in edge
	for (const auto& e : edges_) {
		if (e->get_nodes() == std::make_pair(src, dst)) {
			if (weight and e->get_weight() == weight) {
				return false;
			}
			else if (!weight and e->get_weight() == std::nullopt) {
				return false;
			}
		}
	}
	if (weight) {
		edges_.push_back(std::make_unique<weighted_edge<N, E>>(src, dst, *weight));
	}
	else {
		edges_.push_back(std::make_unique<unweighted_edge<N, E>>(src, dst));
	}
	return true;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::replace_node(N const& old_data, N const& new_data) -> bool {
	if (!is_node(old_data)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
	}
	if (is_node(new_data)) {
		return false;
	}
	// Replace the node
	nodes_.erase(old_data);
	nodes_.insert(new_data);
	// no need to update the edges, since only the node data is changed
	return true;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::merge_replace_node(N const& old_data, N const& new_data) -> void {
	if (!is_node(old_data) || !is_node(new_data)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they don't "
		                         "exist in the graph");
	}
	std::vector<std::unique_ptr<edge>> new_edges;
	// Iterate through current edges and replace old_data with new_data
	for (const auto& e : edges_) {
		auto nodes = e->get_nodes();
		N src = (nodes.first == old_data) ? new_data : nodes.first;
		N dst = (nodes.second == old_data) ? new_data : nodes.second;

		bool duplicate = false;
		for (const auto& edge : new_edges) {
			if (edge->get_nodes() == std::make_pair(src, dst) and edge->get_weight() == e->get_weight()) {
				duplicate = true;
				break;
			}
		}
		if (!duplicate) {
			if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
				new_edges.push_back(std::make_unique<weighted_edge<N, E>>(src, dst, *we->get_weight()));
			}
			else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
				new_edges.push_back(std::make_unique<unweighted_edge<N, E>>(src, dst));
			}
		}
	}
	edges_ = std::move(new_edges);
	nodes_.erase(old_data);
}
template<typename N, typename E>
[[nodiscard]] auto gdwg::graph<N, E>::empty() -> bool {
	return nodes_.empty();
}
template<typename N, typename E>
auto gdwg::graph<N, E>::erase_node(N const& value) -> bool {
	if (!is_node(value)) {
		return false;
	}
	// first remove the edge that contains the node might src or dest
	auto it = edges_.begin();
	while (it != edges_.end()) {
		auto nodes = (*it)->get_nodes();
		if (nodes.first == value or nodes.second == value) {
			it = edges_.erase(it);
		}
		// if the node is not in the edge, just move to the next edge oterwise deadloop
		else {
			++it;
		}
	}
	// remove the node
	nodes_.erase(value);
	return true;
}
template<typename N, typename E>
[[nodiscard]] auto gdwg::graph<N, E>::is_connected(N const& src, N const& dst) -> bool {
	if (!is_node(src) || !is_node(dst)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the "
		                         "graph");
	}
	for (const auto& e : edges_) {
		if (e->get_nodes() == std::make_pair(src, dst)) {
			return true;
		}
	}
	return false;
}
template<typename N, typename E>
auto gdwg::weighted_edge<N, E>::operator==(edge<N, E> const& other) const noexcept -> bool {
	if (auto o = dynamic_cast<weighted_edge const*>(&other)) {
		return this->src_ == o->src_ and this->dst_ == o->dst_ and this->weight_ == o->weight_;
	}
	return false;
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::operator==(edge<N, E> const& other) const noexcept -> bool {
	if (auto o = dynamic_cast<unweighted_edge const*>(&other)) {
		return this->src_ == o->src_ and this->dst_ == o->dst_;
	}
	return false;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::erase_edge(N const& src, N const& dst, std::optional<E> weight) -> bool {
	if (!is_node(src) || !is_node(dst)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the "
		                         "graph");
	}

	auto it = std::remove_if(edges_.begin(), edges_.end(), [&](const std::unique_ptr<edge>& e) {
		auto nodes = e->get_nodes();
		if (nodes.first == src and nodes.second == dst) {
			if (weight and e->is_weighted()) {
				return e->get_weight() == weight;
			}
			else if (!weight and !e->is_weighted()) {
				return true;
			}
		}
		return false;
	});
	if (it != edges_.end()) {
		edges_.erase(it, edges_.end());
		return true;
	}
	return false;
}
template<typename N, typename E>
[[nodiscard]] auto gdwg::graph<N, E>::nodes() const noexcept -> std::vector<N> {
	auto result = std::vector<N>(nodes_.begin(), nodes_.end());
	std::sort(result.begin(), result.end());
	return result;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::edges(N const& src, N const& dst) const -> std::vector<std::unique_ptr<edge>> {
	if (!is_node(src) or !is_node(dst)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the graph");
	}
	// return copy of the edges
	auto result = std::vector<std::unique_ptr<edge>>();
	for (const auto& e : edges_) {
		if (e->get_nodes() == std::make_pair(src, dst)) {
			if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
				result.push_back(std::make_unique<weighted_edge<N, E>>(*we));
			}
			else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
				result.push_back(std::make_unique<unweighted_edge<N, E>>(*ue));
			}
		}
	}
	// first unweighted edge then weighted edge in acsedning order
	std::sort(result.begin(), result.end(), [](const std::unique_ptr<edge>& a, const std::unique_ptr<edge>& b) {
		auto a_nodes = a->get_nodes();
		auto b_nodes = b->get_nodes();
		if (a_nodes.first != b_nodes.first) {
			return a_nodes.first < b_nodes.first;
		}
		if (a_nodes.second != b_nodes.second) {
			return a_nodes.second < b_nodes.second;
		}
		auto weighted_a = dynamic_cast<weighted_edge<N, E>*>(a.get());
		auto weighted_b = dynamic_cast<weighted_edge<N, E>*>(b.get());
		if (!weighted_a) {
			return true;
		}
		if (!weighted_b) {
			return false;
		}
		return *weighted_a->get_weight() < *weighted_b->get_weight();
	});
	return result;
}
template<typename N, typename E>
[[nodiscard]] auto gdwg::graph<N, E>::find(N const& src, N const& dst, std::optional<E> weight) const -> iterator {
	return std::find_if(edges_.cbegin(), edges_.cend(), [&src, &dst, &weight](const std::unique_ptr<edge>& e) {
		if (e->get_nodes() != std::make_pair(src, dst)) {
			return false;
		}
		if (weight.has_value()) {
			return e->get_weight() == weight;
		}
		else {
			return !e->get_weight().has_value();
		}
	});
}
#endif // GDWG_GRAPH_H
