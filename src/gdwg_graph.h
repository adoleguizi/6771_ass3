#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <type_traits>
#include <memory>
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
	 private:
		// You may need to add data members and member functions
		friend class graph<N, W>;
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

	 private:
		std::vector<N> nodes_;
		std::vector<std::unique_ptr<edge>> edges_;
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
#endif // GDWG_GRAPH_H
