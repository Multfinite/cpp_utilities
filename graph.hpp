#ifndef UTILITIES_GRAPH_HPP
#define UTILITIES_GRAPH_HPP

#include <list>
#include <vector>
#include <iterator>
#include <memory>

#include "algorithm.hpp"

namespace Utilities
{
	template<typename TData>
	struct Vertex
	{
		using data_type = TData;		
		using vertex_type = Vertex<data_type>;
		using edge_ptr = void*;
		using vertex_ptr = vertex_type*;
		using edges_type = std::vector<edge_ptr>;
		using vertexes_type = std::vector<vertex_ptr>;

		data_type Context;
		edges_type Edges, IncomingEdges, OutcomingEdges;
		vertexes_type Neighbors, IncomingNeighbors, OutcomingNeighbors;

		Vertex(const data_type& context) : Context(context) {}
		Vertex() : Vertex(default(data_type)) {}
	};

	template<typename TVertex, typename TData>
	struct Edge
	{
		using data_type = TData;
		using vertex_type = TVertex;

		data_type Data;
		vertex_type& const From;
		vertex_type& const To;

		Edge(const vertex_type& from, const vertex_type& to, const data_type& data) :
			From(from), To(to), Data(data)
		{
			From.Neighbors.push_back(&to); From.OutcomingNeighbors.push_back(&to);
			From.Edges.push_back(this); From.OutcomingNeighbors.push_back(this);

			To.Neighbors.push_back(&from); To.IncomingNeighbors.push_back(&from);
			To.Edges.push_back(this); To.IncomingNeighbors.push_back(this);
		}
		Edge(const vertex_type& from, const vertex_type& to) : Edge(from, to, default(vertex_type)) {}
		~Edge()
		{
			From.Neighbors.remove(&to); From.OutcomingNeighbors.remove(&to);
			From.Edges.remove(this); From.OutcomingNeighbors.remove(this);

			To.Neighbors.remove(&from); To.IncomingNeighbors.remove(&from);
			To.Edges.remove(this); To.IncomingNeighbors.remove(this);
		}
	};

	template<typename TVertex, typename TEdge>
	class Graph
	{
	public:
		using vertex_type = TVertex;
		using edge_type = TEdge;
		using vertexes_type = std::vector<vertex_type>;
		using edges_type = std::vector<edge_type>;
		
		vertexes_type Vertexes;
		edges_type Edges;

		Graph() = default;
		Graph(size_t vertices)
		{
			Vertexes.resize(vertices);
		}

		edge_type& Between(const vertex_type& from, const vertex_type& to)
		{
			return Between(from, to, default(vertex_type::data_type));
		}
		edge_type& Between(const vertex_type& from, const vertex_type& to, const edge_type::data_type& context)
		{
			try
			{
				return *find(to.IncomingNeighbors, from);
			}
			catch (const Exceptions::item_not_found_exception&)
			{
				return Edges.emplace_back(from, to, context);
			}
		}

		void clear()
		{
			Edges.clear();
			Vertexes.clear();
		}
	};
}

#endif // UTILITIES_GRAPH_HPP
