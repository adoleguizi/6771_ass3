#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <type_traits>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	// 前向声明graph模板，以便在edge中使用
	template<typename N, typename W>
	class graph;

	template<typename N, typename W>
	class edge {
	 public:
		virtual ~edge() = default;

		virtual std::optional<W> get_weight() const = 0;

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

	template<typename N, typename W>
	class graph {
	 public:
		using edge = gdwg::edge<N, W>;
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

	 private:
		std::vector<N> nodes_;
		std::vector<std::unique_ptr<edge>> edges_;
	};
	template<typename N, typename W>
	class weighted_edge : public edge<N, W> {
	 public:
		weighted_edge(N src, N dst, W weight)
		: edge<N, W>(src, dst)
		, weight_(weight) {}
		~weighted_edge() noexcept override {
			// 由于此类不管理直接的资源，此处不需要执行特定操作
		}
		auto get_weight() const noexcept -> std::optional<W> override;

	 private:
		W weight_;
	};
	template<typename N, typename W>
	class unweighted_edge : public edge<N, W> {
	 public:
		unweighted_edge(N src, N dst)
		: edge<N, W>(src, dst) {}
		~unweighted_edge() noexcept override = default; // virtual destructor

		std::optional<W> get_weight() const noexcept override;
	};

} // namespace gdwg

template<typename N, typename W>
gdwg::graph<N, W>::graph()
: nodes_()
, edges_() {
	// Constructor
}

template<typename N, typename W>
template<typename InputIterator>
gdwg::graph<N, W>::graph(InputIterator first, InputIterator last) {
	static_assert(
	    std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIterator>::iterator_category>::value,
	    "InputIt must be an input iterator");

	// Check if the dereferenced iterator type is convertible to N
	static_assert(std::is_convertible<typename std::iterator_traits<InputIterator>::value_type, N>::value,
	              "The dereferenced iterator type must be convertible to N");
	nodes_.assign(first, last);
	// might modify for insert edge
}
template<typename N, typename W>
gdwg::graph<N, W>::graph(graph&& other) noexcept
: nodes_(std::exchange(other.nodes_, {}))
, // using std::exchange to set other.nodes_ to empty
edges_(std::exchange(other.edges_, {})) // using std::exchange to set other.edges_ to empty
{
	// Move constructor
}
template<typename N, typename W>
auto gdwg::graph<N, W>::operator=(graph&& other) noexcept -> graph& {
	if (this != &other) {
		nodes_ = std::exchange(other.nodes_, {});
		edges_ = std::exchange(other.edges_, {});
	}
	return *this;
}
template<typename N, typename W>
gdwg::graph<N, W>::graph(graph const& other) {
	nodes_ = other.nodes_;
	// might modify for insert edge
	for (auto const& edge : other.edges_) {
		edges_.push_back(std::make_unique<gdwg::edge<N, W>>(*edge));
	}
}
template<typename N, typename W>
auto gdwg::graph<N, W>::operator=(graph const& other) -> graph& {
	if (this != &other) {
		// clear the current nodes and edges
		nodes_.clear();
		edges_.clear();
		// deep copy nodes
		nodes_ = other.nodes_;
		// deep copy edges
		for (const auto& edge : other.edges_) {
			edges_.push_back(std::make_unique<gdwg::edge<N, W>>(*edge));
		}
	}
	return *this;
}
template<typename N, typename W>
auto gdwg::weighted_edge<N, W>::get_weight() const noexcept -> std::optional<W> {
	return weight_;
}
template<typename N, typename W>
auto gdwg::unweighted_edge<N, W>::get_weight() const noexcept -> std::optional<W> {
	return std::nullopt;
}
#endif // GDWG_GRAPH_H
