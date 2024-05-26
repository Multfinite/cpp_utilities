#include "graph.hpp"
#include "pathfinding.hpp"

using namespace Utilities;

void test_graph()
{
	struct Graph2Data {};
	struct Vertex2Data {};
	struct Edge2Data {};
	using Vertex2 = Vertex<Vertex2Data>;
	using Edge2 = Edge<Vertex2, Edge2Data>;
	using Graph2 = Graph<Vertex2, Edge2, Graph2Data>;

	Graph2 g2;
	Vertex2 v2, v3;
	Edge2 e2 { v2, v3 };
}