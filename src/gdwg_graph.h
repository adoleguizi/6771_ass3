#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <initializer_list>
#include <type_traits>
#include <unordered_set>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	// 前向声明graph模板，以便在edge中使用
	template<typename N, typename E>
	class graph;

	template<typename N, typename E>
	class weighted_edge;
	template<typename N, typename E>
	class unweighted_edge;
	template<typename N, typename E>
	class edge {
	 public:
		virtual ~edge() = 0;

		virtual std::optional<E> get_weight() const = 0;
		// is_weighted() const noexcept -> bool;
		[[nodiscard]] virtual auto is_weighted() const noexcept -> bool = 0;
		// get_nodes() const noexcept -> std::pair<N, N>;
		virtual auto get_nodes() const noexcept -> std::pair<N, N> = 0;
		// print_edge() const noexcept -> std::string;
		virtual auto print_edge() const noexcept -> std::string = 0;

	 private:
		N src_;
		N dst_;
		// You may need to add data members and member functions
		template<typename NW, typename EW>
		friend class graph;
		friend class weighted_edge<N, E>;
		friend class unweighted_edge<N, E>;
		edge(N src, N dst)
		: src_(src)
		, dst_(dst) {}
	};
	template<typename N, typename E>
	inline edge<N, E>::~edge() = default;

	template<typename N, typename E>
	class graph {
	 public:
		using edge = gdwg::edge<N, E>;
		graph();
		// Your member functions go here
		template<typename InputIterator>
		graph(InputIterator first, InputIterator last);

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

	 private:
		std::unordered_set<N> nodes_;
		std::vector<std::unique_ptr<edge>> edges_;
	};
	template<typename N, typename E>
	class weighted_edge : public edge<N, E> {
	 public:
		weighted_edge(N src, N dst, E weight)
		: edge<N, E>(src, dst)
		, weight_(weight) {}
		~weighted_edge() noexcept override = default;
		weighted_edge(const weighted_edge& other)
		: edge<N, E>(other.src_, other.dst_)
		, weight_(other.weight_) {}
		weighted_edge& operator=(const weighted_edge& other) {
			if (this != &other) {
				edge<N, E>::operator=(other);
				weight_ = other.weight_;
			}
			return *this;
		}
		auto get_weight() const noexcept -> std::optional<E> override;
		auto is_weighted() const noexcept -> bool override;
		auto get_nodes() const noexcept -> std::pair<N, N> override;
		auto print_edge() const noexcept -> std::string override;

	 private:
		template<typename T>
		std::string to_string(const T& value) const {
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
		: edge<N, E>(src, dst) {}
		~unweighted_edge() noexcept override = default; // virtual destructor

		unweighted_edge(const unweighted_edge& other)
		: edge<N, E>(other.src_, other.dst_) {}
		unweighted_edge& operator=(const unweighted_edge& other) {
			if (this != &other) {
				edge<N, E>::operator=(other);
			}
			return *this;
		}
		auto get_weight() const noexcept -> std::optional<E> override;
		auto is_weighted() const noexcept -> bool override;
		auto get_nodes() const noexcept -> std::pair<N, N> override;
		auto print_edge() const noexcept -> std::string override;

	 private:
		template<typename T>
		std::string to_string(const T& value) const {
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
gdwg::graph<N, E>::graph()
: nodes_()
, edges_() {
	// Constructor
}

template<typename N, typename E>
template<typename InputIterator>
gdwg::graph<N, E>::graph(InputIterator first, InputIterator last) {
	static_assert(
	    std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIterator>::iterator_category>::value,
	    "InputIt must be an input iterator");

	// Check if the dereferenced iterator type is convertible to N
	static_assert(std::is_convertible<typename std::iterator_traits<InputIterator>::value_type, N>::value,
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
#endif // GDWG_GRAPH_H
