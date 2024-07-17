#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H
#include <memory>
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
#endif // GDWG_GRAPH_H
