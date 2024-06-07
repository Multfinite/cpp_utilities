#ifndef UTILITIES_GRAPH_HPP
#define UTILITIES_GRAPH_HPP

#include <list>
#include <vector>
#include <iterator>
#include <memory>

#include "algorithm.hpp"
#include "templates.hpp"
#include "macro.hpp"
#include "linking_node.hpp"

namespace Utilities
{
#if HAS_CONCEPTS
    template<typename T>
    concept IsVertex =
        HasLinkingNode<T> &&
        //std::is_same<decltype(T::__linked), typename T::linked_type>::value &&
        requires(T x)
    {
        typename T::data_type;
        typename T::linked_type;
        x.Context;
        std::is_same<decltype(x.__linked), typename T::linked_type>::value;
        x.clear();
    };

    template<typename T>
    concept IsEdgeCachedVertex =
        std::ranges::range<typename T::edges_type> &&
        std::ranges::range<typename T::vertexes_type> &&
       IsVertex<T> && requires(T x)
    {
        typename T::edges_type;
        typename T::vertexes_type;
        x.Edges; x.IncomingEdges; x.OutcomingEdges;
        x.Neighbors; x.IncomingNeighbors; x.OutcomingNeighbors;
    };

    template<typename T>
    concept IsEdge = IsVertex<typename T::vertex_type> &&
        HasLinkingNode<T> &&
        //std::is_same<decltype(T::__linked), typename T::linked_type>::value &&
        requires(T x, const typename T::vertex_type& v_, const typename T::data_type& d_)
    {
        typename T::data_type;
        typename T::vertex_type;
        typename T::linked_type;
        x.Context;
        std::is_same<decltype(x.__linked), typename T::linked_type>::value;
        x.To;
        x.From;
        x = T(v_, v_);
        x = T(v_, v_, d_);
    };

    template<typename T>
    concept IsCachingEdge = IsEdgeCachedVertex<typename T::vertex_type> && IsEdge<T>;

    template<typename T>
    concept IsGraph =
        HasLinkingNode<T> &&
        IsVertex<typename T::vertex_type> &&
        IsEdge<typename T::edge_type> &&
        requires(T x)
    {
        x.NodeVertexes;
        x.NodeEdges;
        x.NodeGraph;
    };

    template<typename T>
    concept IsCachingGraph =
        HasLinkingNode<T> &&
        IsEdgeCachedVertex<typename T::vertex_type> &&
        IsCachingEdge<typename T::edge_type> &&
        IsGraph<T>;
#endif

    template<typename TData>
    struct Vertex
    {
        using data_type = TData;
        using vertex_type = Vertex<data_type>;
        using edge_ptr = void*;
        using vertex_ptr = vertex_type*;
        using edges_type = std::list<edge_ptr>;
        using vertexes_type = std::list<vertex_ptr>;

        __linking_node __linking;

        data_type Context;
        edges_type Edges, IncomingEdges, OutcomingEdges;
        vertexes_type Neighbors, IncomingNeighbors, OutcomingNeighbors;

        Vertex(const data_type& context) : Context(context) {}
        Vertex() : Vertex(data_type{}) {}

        void clear()
        {
            clear(
              Edges, IncomingEdges, OutcomingEdges,
              Neighbors, IncomingNeighbors, OutcomingNeighbors
            );
        }
    };

    /*!
     * @brief one-directional edge
     */
    template<typename TVertex, typename TData>
#if HAS_CONCEPTS
        requires IsEdgeCachedVertex<TVertex>
#endif
    struct Edge
    {
        using data_type = TData;
        using vertex_type = TVertex;

        __linking_node __linking;

        data_type Context;
        vertex_type& From;
        vertex_type& To;

        Edge(const vertex_type& from, const vertex_type& to, const data_type& context) :
            From(from), To(to), Context(context)
        {
            From.Neighbors.push_back(&to); From.OutcomingNeighbors.push_back(&to);
            From.Edges.push_back(this); From.OutcomingEdges.push_back(this);

            To.Neighbors.push_back(&from); To.IncomingNeighbors.push_back(&from);
            To.Edges.push_back(this); To.IncomingEdges.push_back(this);
        }
        Edge(const vertex_type& from, const vertex_type& to) : Edge(from, to, data_type{}) {}
        ~Edge()
        {
             auto to = To; auto from = From;

            From.Neighbors.remove(&to);  From.OutcomingNeighbors.remove(&to);
            From.Edges.remove(this); From.OutcomingEdges.remove(this);

            To.Neighbors.remove(&from); To.IncomingNeighbors.remove(&from);
            To.Edges.remove(this); To.IncomingEdges.remove(this);
        }
    };

    /*!
     * @brief Bidirectional edge
     */
    template<typename TVertex, typename TData>
#if HAS_CONCEPTS
        requires IsEdgeCachedVertex<TVertex>
#endif
    struct EdgeBidirectional
    {
        using data_type = TData;
        using vertex_type = TVertex;

        __linking_node __linking;

        data_type Context;
        vertex_type& From;
        vertex_type& To;

        EdgeBidirectional(const vertex_type& from, const vertex_type& to, const data_type& context) :
            From(from), To(to), Context(context)
        {
            From.Neighbors.push_back(&to);  From.IncomingNeighbors.push_back(&to); From.OutcomingNeighbors.push_back(&to);
            From.Edges.push_back(this); From.IncomingEdges.push_back(this); From.OutcomingEdges.push_back(this);

            To.Neighbors.push_back(&from); To.IncomingNeighbors.push_back(&from); To.OutcomingNeighbors.push_back(&from);
            To.Edges.push_back(this); To.IncomingEdges.push_back(this); To.OutcomingEdges.push_back(this);
        }
        EdgeBidirectional(const vertex_type& from, const vertex_type& to) : EdgeBidirectional(from, to, data_type{}) {}
        ~EdgeBidirectional()
        {
            auto to = To; auto from = From;

            From.Neighbors.remove(&to);  From.IncomingNeighbors.remove(&to); From.OutcomingNeighbors.remove(&to);
            From.Edges.remove(this); From.IncomingEdges.remove(this); From.OutcomingEdges.remove(this);

            To.Neighbors.remove(&from); To.IncomingNeighbors.remove(&from); To.OutcomingNeighbors.remove(&from);
            To.Edges.remove(this); To.IncomingEdges.remove(this); To.OutcomingEdges.remove(this);
        }
    };

    template<typename TVertex, typename TEdge, typename TData>
#if HAS_CONCEPTS
        requires IsEdgeCachedVertex<TVertex> && IsCachingEdge<TEdge>
#endif
    class Graph
    {
    public:
        using graph_type = Graph<TVertex, TEdge, TData>;
        using data_type = TData;
        using vertex_type = TVertex;
        using edge_type = TEdge;
        using edge_data_type = typename edge_type::data_type;
        using vertexes_type = std::vector<vertex_type>;
        using edges_type = std::vector<edge_type>;

        __linking_node __linking;

        data_type Context;
        vertexes_type Vertexes;
        edges_type Edges;

        Graph() = default;
        Graph(size_t vertices)
        {
            Vertexes.resize(vertices);
        }
        Graph(const graph_type& other)
        {
            clone(other, *this);
        }

        static edge_type& EdgeTo(vertex_type& from, vertex_type& to)
        {
            return *find(to.IncomingNeighbors, from);
        }
        edge_type& Between(const vertex_type& from, const vertex_type& to)
        {
            return Between(from, to, edge_data_type{});
        }
        template<typename ...TArgs>
        edge_type& Between(const vertex_type& from, const vertex_type& to, TArgs ...args)
        {
            try
            {
                return *find(to.IncomingNeighbors, from);
            }
            catch (const Exceptions::item_not_found_exception&)
            {
                return Edges.emplace_back(from, to, args...);
            }
        }

        void RemoveEdgesOf(vertex_type& vertex)
        {
             for(auto& edge : vertex.Edges())
                 Edges.remove(&edge);
             vertex.clear();
        }

        void clear()
        {
            __linking.clear();
            Edges.clear();
            Vertexes.clear();
        }
    };
}

#endif // UTILITIES_GRAPH_HPP
