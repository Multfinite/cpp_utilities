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

        Vertex() = default;
        Vertex(data_type&& context) : Context(context) {}
        template<typename ...TArgs>
        Vertex(TArgs&& ...args) : Context(args...) {}

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

        Edge(vertex_type& from, vertex_type& to, data_type&& context) :
            From(from), To(to), Context(context)
        { link(); }
         template<typename ...TArgs>
        Edge(vertex_type& from, vertex_type& to, TArgs&&... args) :
          From(from), To(to), Context(args...)
        { link(); }
        ~Edge() { unlink(); }
    private:
        inline void link()
        {
            From.Neighbors.push_back(&To); From.OutcomingNeighbors.push_back(&To);
            From.Edges.push_back(this); From.OutcomingEdges.push_back(this);

            To.Neighbors.push_back(&From); To.IncomingNeighbors.push_back(&From);
            To.Edges.push_back(this); To.IncomingEdges.push_back(this);
        }
        inline void unlink()
        {
            From.Neighbors.remove(&To);  From.OutcomingNeighbors.remove(&To);
            From.Edges.remove(this); From.OutcomingEdges.remove(this);

            To.Neighbors.remove(&From); To.IncomingNeighbors.remove(&From);
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

        EdgeBidirectional(vertex_type& from, vertex_type& to, data_type&& context) :
            From(from), To(to), Context(context)
        { link(); }
        template<typename ...TArgs>
        EdgeBidirectional(vertex_type& from, vertex_type& to, TArgs&& ...args) :
            From(from), To(to), Context(args...)
        { link(); }
        ~EdgeBidirectional() { unlink(); }
    private:
        inline void link()
        {
            From.Neighbors.push_back(&To);  From.IncomingNeighbors.push_back(&To); From.OutcomingNeighbors.push_back(&To);
            From.Edges.push_back(this); From.IncomingEdges.push_back(this); From.OutcomingEdges.push_back(this);

            To.Neighbors.push_back(&From); To.IncomingNeighbors.push_back(&From); To.OutcomingNeighbors.push_back(&From);
            To.Edges.push_back(this); To.IncomingEdges.push_back(this); To.OutcomingEdges.push_back(this);
        }
        inline void unlink()
        {
            From.Neighbors.remove(&To);  From.IncomingNeighbors.remove(&To); From.OutcomingNeighbors.remove(&To);
            From.Edges.remove(this); From.IncomingEdges.remove(this); From.OutcomingEdges.remove(this);

            To.Neighbors.remove(&From); To.IncomingNeighbors.remove(&From); To.OutcomingNeighbors.remove(&From);
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
        using vertex_data_type = typename vertex_type::data_type;
        using edge_type = TEdge;
        using edge_data_type = typename edge_type::data_type;
        using vertexes_type = std::list<vertex_type>;
        using edges_type = std::list<edge_type>;

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

        static edge_type& EdgeTo(vertex_type& from, vertex_type& to) { return reference_cast(*find(to.IncomingNeighbors, &from)); }
        static edge_type& EdgeTo(vertex_type const& from, vertex_type const& to) { return reference_cast(*find(to.IncomingNeighbors, &const_cast<vertex_type&>(from))); }

        edge_type& Between(vertex_type& from, vertex_type& to, const vertex_data_type& context)
        {
            try
            {
                auto it = find(to.IncomingEdges, &from);
                return *((edge_type*) *it);
            }
            catch (const Exceptions::item_not_found_exception&)
            {
                return Edges.emplace_back(from, to, context);
            }
        }
        edge_type& Between(vertex_type const& from, vertex_type const& to, const vertex_data_type& context) { return Between(const_cast<vertex_type&>(from), const_cast<vertex_type&>(to), context); }

        template<typename ...TArgs>
        edge_type& Between(vertex_type& from, vertex_type& to, TArgs ...args)
        {
            try
            {
                auto it = find(to.IncomingEdges, &from);
                return *((edge_type*) *it);
            }
            catch (const Exceptions::item_not_found_exception&)
            {
                return Edges.emplace_back(from, to, args...);
            }
        }

        template<typename ...TArgs>
        edge_type& Between(vertex_type const& from, vertex_type const& to, TArgs ...args)
        {
            return Between(const_cast<vertex_type&>(from), const_cast<vertex_type&>(to), args...);
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
