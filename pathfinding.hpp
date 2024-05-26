#ifndef UTILITIES_PATHFINDING_HPP
#define UTILITIES_PATHFINDING_HPP

#include "graph.hpp";

#include <optional>

namespace Utilities::Pathfinding
{
    enum struct VertexState : unsigned char
    {
        Unhandled = 0,
        Queued = 1,
        Explored = 255
    };

    template<typename TVertex, typename TCostType = double>
        requires IsVertex<TVertex>&& IsNumericType<TCostType>
    struct VertexNode
    {
        using vertex_type = TVertex;
        using cost_type = TCostType;

        vertex_type* Previous;
        VertexState State;
        std::optional<cost_type> AtomicCost, Cost;

        VertexNode() { clear(); }

        template<typename TPathType = std::vector<std::reference_wrapper<vertex_type>>>
        TPathType Build()
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

        void clear()
        {
            Previous = nullptr;
            State = VertexState::Unhandled;
            Cost = std::nullopt;
            AtomicCost = std::nullopt;
        }
    };

    template<typename T>
    concept IsPathfinding = requires(T x)
    {
        x._linked_index;
        std::is_same<decltype(x._linked_index), size_t>::value;
        x.Graph;
        x.NodeVertexes;
        x.NodeEdges;
        x.NodeGraph;
    };

    template<typename T, typename TVertex, typename TCostType>
    concept IsCostEvaluator =
        IsVertex<TVertex> && IsNumericType<TCostType> &&
        requires(T x, TVertex v, TCostType c)
    {
        c = x(v);
    };

    template<typename TVertex, typename TCostType = double>
        requires IsVertex<TVertex>&& IsNumericType<TCostType>
    TCostType EvaluateCostDefault(const TVertex& v, const VertexNode<TVertex, TCostType>& n) { return 1; }

    template<
        typename TGraph,
        typename TPathType,
        typename TCostEvaluator
    > requires
        IsCachingGraph<TGraph>
    class BFS
    {
    private:
        size_t _linked_index = 0;
    public:
        using graph_type = typename TGraph;
        using vertex_type = typename graph_type::vertex_type;
        using vertex_ptr = vertex_type*;
        using edge_type = typename graph_type::edge_type;
        using edge_ptr = edge_type*;
        using path_type = TPathType;
        using vertexes_type = std::list<std::reference_wrapper<vertex_type>>;
        using vertex_node_type = VertexNode<vertex_type>;
        using edge_node_type = int;
        using graph_node_type = int;

        const graph_type& Graph;

        std::vector<vertex_node_type> NodeVertexes;
        std::vector<edge_node_type> NodeEdges;
        graph_node_type NodeGraph;

        TCostEvaluator Evaluator;

        inline vertex_node_type& node_of(vertex_type& v)
        {
            return v.__linking.as_ref<vertex_node_type>(_linked_index);
        }

        BFS(const graph_type& graph, const TCostEvaluator& evaluator)
            : Graph(graph), Evaluator(evaluator)
        {
            _linked_index = Graph.__linking.free_index();
            Graph.__linking.set(_linked_index, &NodeGraph);

            NodeVertexes.reserve(Graph.Vertexes.size());
            for (auto& v : Graph.Vertexes)
            {
                vertex_node_type& n = NodeVertexes.emplace_back(v);
                v.__linking.set(_linked_index, &n);
            }

            NodeEdges.reserve(Graph.Edges.size());
            for (auto& e : Graph.Edges)
            {
                edge_node_type& n = NodeEdges.emplace_back(e);
                e.__linking.set(_linked_index, &n);
            }
        }
        BFS(const graph_type& graph) : BFS(graph, default(TCostEvaluator)) {}
        ~BFS()
        {
            Graph.__linking.reset(_linked_index);
            for (auto& v : Graph.Vertexes)
                v.__linking.reset(_linked_index);
            for (auto& e : Graph.Edges)
                e.__linking.reset(_linked_index);
        }

        path_type operator()(const vertex_type& from, const vertex_type& to)
        {
            for (auto& n : NodeVertexes)
            {
                n.clear();
                n.AtomicCost = Evaluator(adjacent, nodeAdjacent);
            }

            node_of(from).State = VertexState::Explored;
            node_of(from).Cost = 0;

            vertexes_type reachable;
            reachable.push_back(from);
            while (!reachable.empty())
            {
                vertex_type& current = reachable.front(); reachable.pop_front();
                vertex_node_type& nodeCurrent = node_of(current);
                if (current == to)
                    return nodeCurrent.Build();

                reachable.remove(current);
                nodeCurrent.State = VertexState::Explored;

                for (auto& pAdjacent : current.OutcomingNeighbors)
                {
                    auto& adjacent = reference_cast<vertex_type>(pAdjacent);
                    auto& nodeAdjacent = node_of(adjacent);

                    if (nodeAdjacent.State == VertexState::Unhandled)
                    {
                        nodeAdjacent.State = VertexState::Queued;
                        reachable.push_back(adjacent);
                    }
                    constexpr bool isCostPresent = nodeAdjacent.Cost.has_value();
                    constexpr bool isShorter = nodeCurrent.Cost + nodeAdjacent.AtomicCost < nodeAdjacent.Cost;
                    if (!isCostPresent || isShorter)
                    {
                        nodeAdjacent.Previous = current;
                        nodeAdjacent.Cost = nodeCurrent.Cost + nodeAdjacent.AtomicCost;
                    }
                }
            }
            return path_type{};
        }
    };
}

#endif // UTILITIES_PATHFINDING_HPP