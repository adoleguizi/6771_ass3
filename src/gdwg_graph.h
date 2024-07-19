#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <type_traits>
#include <unordered_set>
#include <memory>
#include <optional>
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

	 private:
		N src_;
		N dst_;
		// You may need to add data members and member functions
		template<typename NW, typename EW>
		friend class graph;
		edge(N src, N dst)
		: src_(src)
		, dst_(dst) {}
	};

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
		~weighted_edge() noexcept override {
			// 由于此类不管理直接的资源，此处不需要执行特定操作
		}
		auto get_weight() const noexcept -> std::optional<E> override;
		auto is_weighted() const noexcept -> bool override;
		auto get_nodes() const noexcept -> std::pair<N, N> override;
		auto print_edge() const noexcept -> std::string override;

	 private:
		E weight_;
	};
	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		unweighted_edge(N src, N dst)
		: edge<N, E>(src, dst) {}
		~unweighted_edge() noexcept override = default; // virtual destructor

		auto get_weight() const noexcept -> std::optional<E> override;
		auto is_weighted() const noexcept -> bool override;
		auto get_nodes() const noexcept -> std::pair<N, N> override;
		auto print_edge() const noexcept -> std::string override;
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
	nodes_.assign(first, last);
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
	for (auto const& edge : other.edges_) {
		edges_.push_back(std::make_unique<gdwg::edge<N, E>>(*edge));
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
			edges_.push_back(std::make_unique<gdwg::edge<N, E>>(*edge));
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
	return std::to_string(this->src_) + " -> " + std::to_string(this->dst_) + " | W |  " + std::to_string(weight_);
}
template<typename N, typename E>
auto gdwg::unweighted_edge<N, E>::print_edge() const noexcept -> std::string {
	return std::to_string(this->src_) + " -> " + std::to_string(this->dst_) + " | U";
}
template<typename N, typename E>
auto gdwg::graph<N, E>::insert_node(N const& value) -> bool {
	auto result = nodes_.insert(value);
	return result.second; // 如果插入成功，返回 true；否则返回 false
}
#endif // GDWG_GRAPH_H
