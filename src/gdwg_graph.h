#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <initializer_list>
#include <type_traits>
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
namespace gdwg {
	template<typename N, typename E>
	class TestHelper;
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
		// using iterator = typename std::vector<std::unique_ptr<edge>>::const_iterator;
		class iterator {
		 public:
			using value_type = struct {
				N from;
				N to;
				std::optional<E> weight;
			};
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;
			// Iterator constructor
			iterator();
			// Iterator source
			auto operator*() const -> reference;
			// Iterator traversal
			auto operator++() -> iterator&;
			auto operator++(int) -> iterator;
			auto operator--() -> iterator&;
			auto operator--(int) -> iterator;
			// Iterator comparison
			auto operator==(iterator const& other) const -> bool;

		 private:
			typename std::map<N, std::vector<std::unique_ptr<edge>>>::const_iterator map_it;
			typename std::vector<std::unique_ptr<edge>>::const_iterator vec_it;
			const graph* g;
			iterator(const graph* graph, bool end = false);
			iterator(const graph* graph,
			         typename std::map<N, std::vector<std::unique_ptr<edge>>>::const_iterator map_it,
			         typename std::vector<std::unique_ptr<edge>>::const_iterator vec_it);

			friend class graph;
		};

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

		[[nodiscard]] auto edges(N const& src, N const& dst) const -> std::vector<std::unique_ptr<edge>>;

		[[nodiscard]] auto find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) const ->
		    typename std::vector<std::unique_ptr<edge>>::const_iterator;

		[[nodiscard]] auto connections(N const& src) -> std::vector<N>;

	 protected:
		std::set<N> nodes_;
		std::map<N, std::vector<std::unique_ptr<edge>>> edges_;
		auto has_incoming_edges(N const& node) const -> bool;
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
			else if constexpr (std::is_floating_point_v<T>) {
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << value;
				return oss.str();
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
// Implementation of the iterator class
template<typename N, typename E>
gdwg::graph<N, E>::iterator::iterator()
: map_it()
, vec_it()
, g(nullptr) {}

template<typename N, typename E>
gdwg::graph<N, E>::iterator::iterator(const graph* graph, bool end)
: g(graph) {
	if (end) {
		map_it = g->edges_.end();
	}
	else {
		map_it = g->edges_.begin();
		if (map_it != g->edges_.end()) {
			vec_it = map_it->second.begin();
		}
	}
}
template<typename N, typename E>
gdwg::graph<N, E>::iterator::iterator(const graph* graph,
                                      typename std::map<N, std::vector<std::unique_ptr<edge>>>::const_iterator map_it,
                                      typename std::vector<std::unique_ptr<edge>>::const_iterator vec_it)
: g(graph)
, map_it(map_it)
, vec_it(vec_it) {}
template<typename N, typename E>
auto gdwg::graph<N, E>::iterator::operator*() const -> reference {
	return reference{map_it->first, (*vec_it)->get_nodes().second, (*vec_it)->get_weight()};
}
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
	for (const auto& [src, edges] : other.edges_) {
		for (const auto& e : edges) {
			if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
				edges_[src].push_back(std::make_unique<weighted_edge<N, E>>(*we));
			}
			else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
				edges_[src].push_back(std::make_unique<unweighted_edge<N, E>>(*ue));
			}
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
		for (const auto& [src, edges] : other.edges_) {
			for (const auto& e : edges) {
				if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
					edges_[src].push_back(std::make_unique<weighted_edge<N, E>>(*we));
				}
				else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
					edges_[src].push_back(std::make_unique<unweighted_edge<N, E>>(*ue));
				}
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
	return to_string(this->src_) + " -> " + to_string(this->dst_) + " | W | " + to_string(weight_);
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::print_edge() const noexcept -> std::string {
	return to_string(this->src_) + " -> " + to_string(this->dst_) + " | U";
}
template<typename N, typename E>
auto gdwg::graph<N, E>::insert_node(N const& value) -> bool {
	auto result = nodes_.insert(value);
	return result.second;
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
	for (const auto& e : edges_[src]) {
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
		edges_[src].push_back(std::make_unique<weighted_edge<N, E>>(src, dst, *weight));
	}
	else {
		edges_[src].push_back(std::make_unique<unweighted_edge<N, E>>(src, dst));
	}
	// Sort edges after insertion
	std::sort(edges_[src].begin(), edges_[src].end(), [](const std::unique_ptr<edge>& a, const std::unique_ptr<edge>& b) {
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
	auto new_edges = std::map<N, std::vector<std::unique_ptr<edge>>>{};
	// Iterate through current edges and replace old_data with new_data
	for (auto& [src, edges] : edges_) {
		for (const auto& e : edges) {
			auto nodes = e->get_nodes();
			N new_src = (nodes.first == old_data) ? new_data : nodes.first;
			N new_dst = (nodes.second == old_data) ? new_data : nodes.second;
			bool duplicate = false;
			for (const auto& edge : new_edges[new_src]) {
				if (edge->get_nodes() == std::make_pair(new_src, new_dst) && edge->get_weight() == e->get_weight()) {
					duplicate = true;
					break;
				}
			}
			if (!duplicate) {
				if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
					new_edges[new_src].push_back(
					    std::make_unique<weighted_edge<N, E>>(new_src, new_dst, *we->get_weight()));
				}
				else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
					new_edges[new_src].push_back(std::make_unique<unweighted_edge<N, E>>(new_src, new_dst));
				}
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
	for (auto& [src, edges] : edges_) {
		edges.erase(std::remove_if(
		                edges.begin(),
		                edges.end(),
		                [&](auto const& e) { return e->get_nodes().first == value or e->get_nodes().second == value; }),
		            edges.end());
	}
	// remove the node
	nodes_.erase(value);
	return true;
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
[[nodiscard]] auto gdwg::graph<N, E>::is_connected(N const& src, N const& dst) -> bool {
	if (!is_node(src) or !is_node(dst)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the "
		                         "graph");
	}
	auto it = edges_.find(src);
	if (it != edges_.end()) {
		for (const auto& e : it->second) {
			if (e->get_nodes() == std::make_pair(src, dst)) {
				return true;
			}
		}
	}
	return false;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::erase_edge(N const& src, N const& dst, std::optional<E> weight) -> bool {
	if (!is_node(src) || !is_node(dst)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the "
		                         "graph");
	}
	auto it = std::remove_if(edges_[src].begin(), edges_[src].end(), [&](const std::unique_ptr<edge>& e) {
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
	if (it != edges_[src].end()) {
		edges_[src].erase(it, edges_[src].end());
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
	auto it = edges_.find(src);
	if (it != edges_.end()) {
		for (const auto& e : it->second) {
			if (e->get_nodes() == std::make_pair(src, dst)) {
				if (auto we = dynamic_cast<weighted_edge<N, E>*>(e.get())) {
					result.push_back(std::make_unique<weighted_edge<N, E>>(*we));
				}
				else if (auto ue = dynamic_cast<unweighted_edge<N, E>*>(e.get())) {
					result.push_back(std::make_unique<unweighted_edge<N, E>>(*ue));
				}
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
[[nodiscard]] auto gdwg::graph<N, E>::connections(N const& src) -> std::vector<N> {
	if (!is_node(src)) {
		throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the graph");
	}
	auto it = edges_.find(src);
	if (it == edges_.end()) {
		return {};
	}
	auto connected_nodes = std::set<N>();
	for (const auto& e : it->second) {
		connected_nodes.insert(e->get_nodes().second);
	}
	return std::vector<N>(connected_nodes.begin(), connected_nodes.end());
}
template<typename N, typename E>
auto gdwg::graph<N, E>::iterator::operator++() -> iterator& {
	if (g == nullptr) {
		return *this;
	}
	++vec_it;
	while (map_it != g->edges_.end() && (vec_it == map_it->second.end() || !g->has_incoming_edges(map_it->first))) {
		++map_it;
		// skip empty edges and nodes without incoming edges
		while (map_it != g->edges_.end() && (map_it->second.empty() || !g->has_incoming_edges(map_it->first))) {
			++map_it;
		}
		if (map_it != g->edges_.end()) {
			vec_it = map_it->second.begin();
		}
	}
	return *this;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::has_incoming_edges(N const& node) const -> bool {
	for (const auto& [src, edges] : edges_) {
		for (const auto& e : edges) {
			if (e->get_nodes().second == node && e->get_nodes().first != node) {
				return true;
			}
		}
	}
	return false;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::iterator::operator++(int) -> iterator {
	iterator temp = *this;
	++(*this);
	return temp;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::iterator::operator--() -> iterator& {
	if (g == nullptr) {
		return *this;
	}
	if (vec_it == map_it->second.begin()) {
		if (map_it == g->edges_.begin()) {
			*this = iterator(g, true);
			return *this;
		}
		--map_it;
		vec_it = map_it->second.end();
	}
	--vec_it;
	while (map_it != g->edges_.begin() && (map_it->second.empty() || !g->has_incoming_edges(map_it->first))) {
		--map_it;
		vec_it = map_it->second.end();
		if (!map_it->second.empty() && g->has_incoming_edges(map_it->first)) {
			vec_it = map_it->second.end();
		}
	}
	return *this;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::iterator::operator--(int) -> iterator {
	iterator temp = *this;
	--(*this);
	return temp;
}
template<typename N, typename E>
auto gdwg::graph<N, E>::iterator::operator==(const iterator& other) const -> bool {
	return g == other.g and map_it == other.map_it and vec_it == other.vec_it;
}
#endif // GDWG_GRAPH_H
