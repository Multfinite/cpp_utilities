#ifndef UTILITIES_PATHFINDING_HPP
#define UTILITIES_PATHFINDING_HPP

#include "graph.hpp"
#include "optional.hpp"

namespace Utilities::Pathfinding
{
    enum struct VertexState : unsigned char
    {
        Unhandled = 0,
        Queued = 1,
        Explored = 255
    };

    template<typename TVertex, typename TCostType = double>
#if HAS_CONCEPTS
        requires IsVertex<TVertex>&& IsNumericType<TCostType>
#endif
    struct VertexNode : public __linked_node<TVertex>
    {
        using vertex_type = TVertex;
        using cost_type = TCostType;

        vertex_type* Previous;
        VertexState State;
        /*!
         * @brief Total cost from root to this
         */
        optional<cost_type> Cost;

        VertexNode(const vertex_type& owner) : __linked_node<TVertex>(owner) { clear(); }
        VertexNode(const vertex_type& owner, size_t index) : __linked_node<TVertex>(owner, index) { clear(); }
        ~VertexNode() = default;

        template<typename TPathType = std::vector<std::reference_wrapper<vertex_type>>>
        TPathType Build() const
        {
            TPathType path;
            auto* v = &this;
            while (v != nullptr)
            {
                path.pop_back(*v);
                v = v->Pathfinding->Previous;
            }
            return path;
        }

        vertex_type& Root() const
        {
            auto* v = &this;
            while (v != nullptr)
                v = v->Pathfinding->Previous;
            return *v;
        }

        void clear()
        {
            Previous = nullptr;
            State = VertexState::Unhandled;
            Cost = nullopt;
        }
    };

    template<typename TEdge, typename TCostType = double>
#if HAS_CONCEPTS
        requires IsVertex<TVertex> && IsEdge<TEdge> && IsNumericType<TCostType>
#endif
    struct EdgeNode : public __linked_node<TEdge>
    {
        using edge_type = TEdge;
        using vertex_type = typename edge_type::vertex_type;
        using cost_type = TCostType;

        /*!
         * @brief Atomic transferring cost of this edge
         */
        optional<cost_type> Cost;

        EdgeNode(const edge_type& owner) : __linked_node<TEdge>(owner) { clear(); }
        EdgeNode(const edge_type& owner, size_t index) : __linked_node<TEdge>(owner, index) { clear(); }
        ~EdgeNode() = default;

        void clear()
        {
            Cost = nullopt;
        }
    };

    template<typename TGraph, typename TCostType = double>
#if HAS_CONCEPTS
        requires IsGraph<TGraph> && IsNumericType<TCostType>
#endif
    struct GraphNode : public __linked_node<TGraph>
    {
        using cost_type = TCostType;
        using graph_type = TGraph;
        using vertex_type = typename graph_type::vertex_type;
        using edge_type = typename graph_type::edge_type;
        using vertex_node_type = VertexNode<vertex_type>;
        using edge_node_type = EdgeNode<edge_type>;

        std::vector<vertex_node_type> Vertexes;
        std::vector<edge_node_type> Edges;

        GraphNode(const graph_type& owner) : __linked_node<TGraph>(owner) { init(); }
        GraphNode(const graph_type& owner, size_t index) : __linked_node<TGraph>(owner, index) { init(); }
        ~GraphNode() = default;

        void init()
        {
            Vertexes.reserve(this->Owner.Vertexes.size());
            for (auto& v : this->Owner.Vertexes)
                Vertexes.emplace_back(v, this->Index);

            Edges.reserve(this->Owner.Edges.size());
            for (auto& e : this->Owner.Edges)
                Edges.emplace_back(e, this->Index);
        }

        void clear()
        {
            Edges.clear();
            Vertexes.clear();
        }

        inline vertex_node_type& node_of(const vertex_type& v) const
        {
            return v.__linking.template as_ref<vertex_node_type>(this->Index);
        }
        inline edge_node_type& node_of(const edge_type& v) const
        {
            return v.__linking.template as_ref<vertex_node_type>(this->Index);
        }
    };
#if HAS_CONCEPTS
    template<typename T>
    concept IsPathfinding = requires(T x)
    {
        x._linked_index;
        std::is_same<decltype(x._linked_index), size_t>::value;
        x.Graph;
        x.GraphNode;
    };

    template<typename T, typename TVertex, typename TCostType>
    concept IsCostEvaluator =
        IsVertex<TVertex> && IsNumericType<TCostType> &&
        requires(T x, TVertex v, TCostType c)
    {
        c = x(v);
    };
#endif

    template<typename TGraphNode, typename TEdgeNode = typename TGraphNode::edge_type, typename TCostType = typename TGraphNode::cost_type>
#if HAS_CONCEPTS
        requires IsVertex<TVertex>&& IsNumericType<TCostType>
#endif
    TCostType EvaluateCostDefault(const TGraphNode& g, const TEdgeNode& e) { return 1; }

    template<
        typename TGraph,
        typename TPathType,
        typename TCostEvaluator,
        typename TCostType = double
    >
#if HAS_CONCEPTS
    requires
        IsCachingGraph<TGraph>
#endif
    class BFS
    {
    public:
        using cost_type = TCostType;
        using graph_type = TGraph;
        using vertex_type = typename graph_type::vertex_type;
        using vertex_ptr = vertex_type*;
        using edge_type = typename graph_type::edge_type;
        using edge_ptr = edge_type*;
        using path_type = TPathType;
        using vertexes_type = std::list<std::reference_wrapper<vertex_type>>;
        using graph_node_type = GraphNode<TGraph, cost_type>;
        using vertex_node_type = typename graph_node_type::vertex_node_type;
        using edge_node_type = typename graph_node_type::edge_node_type;

        const graph_type& Graph;
        graph_node_type GraphNode;

        TCostEvaluator Evaluator;

        BFS(const graph_type& graph, const TCostEvaluator& evaluator)
            : Graph(graph), GraphNode(graph), Evaluator(evaluator)
        {}
        BFS(const graph_type& graph) : BFS(graph, TCostEvaluator{}) {}
        ~BFS() = default;

        path_type operator()(const vertex_type& from, const vertex_type& to)
        {
            for (auto& e : GraphNode.Edges)
            {
                e.clear();
                e.Cost = Evaluator(GraphNode, e);
            }

            GraphNode.node_of(from).State = VertexState::Explored;
            GraphNode.node_of(from).Cost = 0;

            vertexes_type reachable;
            reachable.push_back(from);
            while (!reachable.empty())
            {
                vertex_type& current = reachable.front(); reachable.pop_front();
                vertex_node_type& nodeCurrent = GraphNode.node_of(current);
                if (current == to)
                    return nodeCurrent.Build();

                reachable.remove(current);
                nodeCurrent.State = VertexState::Explored;

                for (auto& pEdge : current.OutcomingEdges)
                {
                    auto& edge = reference_cast<edge_type>(pEdge);
                    auto& nodeEdge = GraphNode.node_of(edge);
                    auto& adjacent = edge.To;
                    auto& nodeAdjacent = GraphNode.node_of(adjacent);

                    if (nodeAdjacent.State == VertexState::Unhandled)
                    {
                        nodeAdjacent.State = VertexState::Queued;
                        reachable.push_back(adjacent);
                    }
                    constexpr bool isCostPresent = nodeAdjacent.Cost.has_value();
                    constexpr bool isShorter = nodeCurrent.Cost + nodeEdge.Cost < nodeAdjacent.Cost;
                    if (!isCostPresent || isShorter)
                    {
                        nodeAdjacent.Previous = current;
                        nodeAdjacent.Cost = nodeCurrent.Cost + nodeEdge.Cost;
                    }
                }
            }
            return path_type{};
        }
    };
}

#endif // UTILITIES_PATHFINDING_HPP
