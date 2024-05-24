#ifndef UTILITIES_GRAPH_HPP
#define UTILITIES_GRAPH_HPP

#include <list>
#include <vector>
#include <iterator>
#include <memory>

#include "algorithm.hpp"
#include "templates.hpp"
#include "macro.hpp"

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
    struct Edge
    {
        using data_type = TData;
        using vertex_type = TVertex;

        data_type Context;
        vertex_type& const From;
        vertex_type& const To;

        Edge(const vertex_type& from, const vertex_type& to, const data_type& context) :
            From(from), To(to), Context(context)
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

    /*!
     * @brief Bidirectional edge
     */
    template<typename TVertex, typename TData>
    struct EdgeBidirectional
    {

    };

    template<typename TVertex, typename TEdge>
    class Graph
    {
    public:
        using vertex_type = TVertex;
        using edge_type = TEdge;
        using edge_data_type = typename edge_type::data_type;
        using vertexes_type = std::vector<vertex_type>;
        using edges_type = std::vector<edge_type>;

        vertexes_type Vertexes;
        edges_type Edges;

        Graph() = default;
        Graph(size_t vertices)
        {
            Vertexes.resize(vertices);
        }

        static edge_type& EdgeTo(vertex_type& from, vertex_type& to)
        {
            return *find(to.IncomingNeighbors, from);
        }
        edge_type& Between(const vertex_type& from, const vertex_type& to)
        {
            return Between(from, to, default(edge_data_type));
        }
        edge_type& Between(const vertex_type& from, const vertex_type& to, const edge_data_type& context)
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

        void RemoveEdgesOf(vertex_type& vertex)
        {
             for(auto& edge : vertex.Edges())
                 Edges.remove(&edge);
             vertex.clear();
        }

        void clear()
        {
            Edges.clear();
            Vertexes.clear();
        }
    };

    enum struct PathfindingState : unsigned char
    {
        Unhandled = 0,
        Queued = 1,
        Explored = 255
    };

    template<typename TVertex>
    struct PathfindingNode
    {
        using vertex_type = TVertex;

        vertex_type* Previous;
        PathfindingState State;
        double Cost;

        PathfindingNode() { clear(); }

        template<typename TPathType = std::vector<std::reference_wrapper<vertex_type>>>
        TPathType Build()
        {
            TPathType path;
            auto* v = &this;
            while(v != nullptr)
            {
                path.pop_back(*v);
                v = v->Pathfinding->Previous;
            }
            return path;
        }

        void clear()
        {
            Previous = nullptr;
            State = PathfindingState::Unhandled;
            Cost = 0;
        }
    };

    template<typename TVertex>
    struct DFSSpec
    {
        using vertex_type = TVertex;
        using vertexes_type = std::list<std::reference_wrapper<vertex_type>>;

        vertex_type& Select(vertexes_type& reachable)
        {
            vertex_type& v = *reachable.front();
            reachable.pop_front();
            return v;
        }
        void Put(vertexes_type& reachable, vertex_type& v)
        {
            reachable.push_front(v);
        }
    };

    template<typename TVertex>
    struct BFSSpec
    {
        using vertex_type = TVertex;
        using vertexes_type = std::list<std::reference_wrapper<vertex_type>>;

        vertex_type& Select(vertexes_type& reachable)
        {
            vertex_type& v = *reachable.front();
            reachable.pop_front();
            return v;
        }
        void Put(vertexes_type& reachable, vertex_type& v)
        {
            reachable.push_back(v);
        }
    };

    /*!
     * @brief Pathfinding base algorithm
     * @arg TGraph::vertex_type - must have a field  { PathfindingNode Pathfinding; } in Context
     * @arg TPathType - must be the any container (see std spec) of TVertex
     */
    template<
            typename TSpec,
            typename TGraph,
            typename TPathType = std::vector<std::reference_wrapper<typename TGraph::vertex_type>>
    > class Pathfinding
    {
    public:
        using spec_type = TSpec;
        using graph_type = TGraph;
        using vertex_type = typename graph_type::vertex_type;
        using edge_type = typename graph_type::edge_type;
        using edge_ptr = edge_type*;
        using path_type = TPathType;
        using vertexes_type = std::list<std::reference_wrapper<vertex_type>>;

        double evaluate_cost(const vertex_type& v) { return 1; }
    private:
        TSpec _spec;
    public:
        Pathfinding() = default;
        Pathfinding(const TSpec& spec) : _spec(spec) {}

        template<typename TCostEvaluator>
        path_type Find(
                const graph_type& graph,
                const vertex_type& from, const vertex_type& to
                , TCostEvaluator evaluateCost
        ) {
            for(auto& v : graph.Vertexes)
                v.Context.Pathfinding.clear();

            vertexes_type reachable, unexplored;

            reachable.push_back(from);
            while(!reachable.empty())
            {
                auto& current = _spec.Select(reachable);
                if(current == to)
                    return current.Context.Pathfinding.Build();

                reachable.remove(current);
                current.Context.Pathfinding.State = PathfindingState::Explored;

                transform_if(current.OutcomingEdges.begin(), current.OutcomingEdges.end(), std::back_inserter(unexplored),
                [](edge_ptr pEdge)
                {
                    return reference_cast<edge_type>(static_cast<edge_type*>(pEdge));
                },
                [](edge_ptr pEdge)
                {
                   auto edge = reference_cast<edge_type>(static_cast<edge_type*>(pEdge));
                   return edge.Context.Pathfinding.State == PathfindingState::Unhandled;
                });

                for(auto& adjacent : unexplored)
                {
                    if(adjacent.Context.Pathdinfind.State == PathfindingState::Unhandled)
                    {
                        _spec.Put(reachable, adjacent);
                        adjacent.Context.Pathfinding.State = PathfindingState::Queued;
                    }
                    auto atomicCost = evaluateCost(adjacent);
                    if(current.Context.Pathfinding.Cost + atomicCost < adjacent.Context.Pathfinding.Cost)
                    {
                        adjacent.Context.Pathfinding.Previous = current;
                        adjacent.Context.Pathfinding.Cost = current.Context.Pathfinding.Cost + atomicCost;
                    }
                }
            }

            return path_type{};
        }
    };
}

#endif // UTILITIES_GRAPH_HPP
