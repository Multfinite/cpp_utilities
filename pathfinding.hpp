#ifndef UTILITIES_PATHFINDING_HPP
#define UTILITIES_PATHFINDING_HPP

#include "graph.hpp"
#include "optional.hpp"

#include <list>
#include <vector>

namespace Utilities::Pathfinding
{
    /*!
     * @brief Don't put nodes to a containers to mark them as being in some stte. Use this enum instead which valua attached to node itself
     */
    enum struct VertexState : unsigned char
    {
        /*!
          * @brief Vertex did not appear yet.
          */
        Unhandled = 0,
        /*!
          * @brief This vertex is contained in queue to be processed.
          */
        Queued = 1,
        /*!
          * @brief All neigbors are processed.
          */
        Explored = 255
    };

    /*!
     * @brief Contains a way how current VertexNode is reached ("Parent")
     */
    template<typename TVertex, typename TEdge>
#if HAS_CONCEPTS
        requires IsVertex<TVertex> && IsEdge<TEdge>
#endif
    struct PathEntry
    {
        using vertex_type = TVertex;
        using edge_type = TEdge;
        using path_entry = PathEntry<vertex_type, edge_type>;

        vertex_type const& From;
        edge_type const& Direction;

        PathEntry(vertex_type const& from, edge_type const& direction) : From(from), Direction(direction) {}
    };

    template<typename TVertex, typename TEdge, typename TCostType = double>
#if HAS_CONCEPTS
        requires IsVertex<TVertex>&& IsNumericType<TCostType>
#endif
    struct VertexNode : public __linked_node<TVertex>
    {
        using linked_node = __linked_node<TVertex>;
        using vertex_type = TVertex;
        using edge_type = TEdge;
        using cost_type = TCostType;
        using vertex_node_type = VertexNode<vertex_type, edge_type, cost_type>;
        using path_entry = PathEntry<vertex_type, edge_type>;
        using path_type_default = std::vector<std::reference_wrapper<path_entry>>;

        /*!
         * @brief If entry is empty then it mean that current node is root node.
         */
        optional<path_entry> Entry;
        VertexState State;
        /*!
         * @brief Total cost from root to this
         */
        optional<cost_type> Cost;

        VertexNode(vertex_type& owner) : linked_node(owner) { clear(); }
        VertexNode(vertex_type& owner, size_t index) : linked_node(owner, index) { clear(); }
        ~VertexNode() = default;

        template<typename TPathType = path_type_default>
        inline TPathType Build() const
        {
            std::list<std::reference_wrapper<path_entry>> path;
            vertex_node_type* v = const_cast<vertex_node_type*>(this);
            while (v->Entry.has_value())
            {
                path_entry& entry = v->Entry.value();
                path.push_front(entry); // Current node is destination. Then: N; N-1 N; N-2 N-1 N; ... ; 0 1 2 ... N-2 N-1 N.
                v = entry.From.__linking.template as_ptr<vertex_node_type>(this->Index);
            }
            return TPathType(path.begin(), path.end()); // list are best way to create a sequence with valid direction without reverse. But best way to store items it is vector.
        }

        inline void clear()
        {
            Entry = nullopt;
            State = VertexState::Unhandled;
            Cost = nullopt;
        }

        inline bool operator==(vertex_node_type const& other) const { return &this == &other; }
        inline bool operator!=(vertex_node_type const& other) const { return &this != &other; }
    };

    template<typename TEdge, typename TCostType = double>
#if HAS_CONCEPTS
        requires IsVertex<TVertex> && IsEdge<TEdge> && IsNumericType<TCostType>
#endif
    struct EdgeNode : public __linked_node<TEdge>
    {
        using linked_node = __linked_node<TEdge>;
        using edge_type = TEdge;
        using vertex_type = typename edge_type::vertex_type;
        using cost_type = TCostType;

        /*!
         * @brief Atomic transferring cost of this edge
         */
        optional<cost_type> Cost;

        EdgeNode(edge_type& owner) : linked_node(owner) { clear(); }
        EdgeNode(edge_type& owner, size_t index) : linked_node(owner, index) { clear(); }
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
        using linked_node = __linked_node<TGraph>;
        using cost_type = TCostType;
        using graph_type = TGraph;
        using vertex_type = typename graph_type::vertex_type;
        using edge_type = typename graph_type::edge_type;
        using vertex_node_type = VertexNode<vertex_type, edge_type>;
        using edge_node_type = EdgeNode<edge_type>;

        std::list<vertex_node_type> Vertexes;
        std::list<edge_node_type> Edges;

        GraphNode(graph_type& owner) : linked_node(owner) { init(); }
        GraphNode(graph_type& owner, size_t index) : linked_node(owner, index) { init(); }
        ~GraphNode() = default;

        void init()
        {
            //Vertexes.reserve(this->Owner.Vertexes.size());
            for (auto& v : this->Owner.Vertexes)
                Vertexes.emplace_back(v, this->Index);

            //Edges.reserve(this->Owner.Edges.size());
            for (auto& e : this->Owner.Edges)
                Edges.emplace_back(e, this->Index);
        }

        void clear()
        {
            Edges.clear();
            Vertexes.clear();
        }

        inline vertex_node_type& node_of(vertex_type const& v) const
        {
            //return *reinterpret_cast<vertex_node_type*>(v.__linking.Items.at(this->Index));
            return v.__linking.template as_ref<vertex_node_type>(this->Index);
        }
        inline edge_node_type& node_of(edge_type const& v) const
        {
            //*reinterpret_cast<edge_node_type*>(v.__linking.Items.at(this->Index));
            return v.__linking.template as_ref<edge_node_type>(this->Index);
        }
    };

    template<typename TGraphNode>
#if HAS_CONCEPTS
        requires IsVertex<TVertex>&& IsNumericType<TCostType>
#endif
    struct DefaultCostEvaluator
    {
        using graph_node_type = TGraphNode;
        using cost_type = typename TGraphNode::cost_type;
        using edge_node_type = typename TGraphNode::edge_node_type;

        cost_type operator()(graph_node_type const& g, edge_node_type const& e) { return 1; }
    };

    template<
        typename TGraph,
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
        using graph_node_type = GraphNode<TGraph, cost_type>;
        using vertex_node_type = typename graph_node_type::vertex_node_type;
        using edge_node_type = typename graph_node_type::edge_node_type;

        graph_type const& Graph;

        graph_node_type GraphNode;
        TCostEvaluator Evaluator;

        BFS(graph_type& graph, TCostEvaluator const& evaluator)
            : Graph(graph), GraphNode(graph), Evaluator(evaluator)
        {
            for (auto& e : GraphNode.Edges)
            {
                e.clear();
                e.Cost = Evaluator(GraphNode, e);
            }
        }
        BFS(graph_type& graph) : BFS(graph, TCostEvaluator{}) {}
        ~BFS() = default;

        template<typename TPathType = typename vertex_node_type::path_type_default>
        TPathType operator()(vertex_type const& from, vertex_type const& to)
        {
            vertex_node_type& nodeTo = GraphNode.node_of(to);
            vertex_node_type& nodeFrom = GraphNode.node_of(from);
            nodeFrom.State = VertexState::Explored;
            nodeFrom.Cost = 0;

            std::list<vertex_node_type*> reachable;
            reachable.push_back(&nodeFrom);
            while (!reachable.empty())
            {
                vertex_node_type& nodeCurrent = *reachable.front(); reachable.pop_front();
                vertex_type const& current = nodeCurrent.Owner;
                if (&nodeCurrent == &nodeTo)
                    return nodeCurrent.template Build<TPathType>();

                reachable.remove(&nodeCurrent);
                nodeCurrent.State = VertexState::Explored;

                for (void* pEdge : current.OutcomingEdges)
                {
                    auto const& edge = reference_cast<edge_type>((edge_ptr) pEdge);
                    auto& nodeEdge = GraphNode.node_of(edge);
                    auto const& adjacent = edge.To;
                    auto& nodeAdjacent = GraphNode.node_of(adjacent);

                    if (nodeAdjacent.State == VertexState::Unhandled)
                    {
                        nodeAdjacent.State = VertexState::Queued;
                        reachable.push_back(&nodeAdjacent);
                    }
                    const bool isCostPresent = nodeAdjacent.Cost.has_value();
                    const cost_type fullCost = nodeCurrent.Cost.value() + nodeEdge.Cost.value();
                    if (!isCostPresent || fullCost < nodeAdjacent.Cost.value())
                    {
                        nodeAdjacent.Entry.emplace(current, edge);
                        nodeAdjacent.Cost = fullCost;
                    }
                }
            }
            return TPathType{};
        }
    };
}

#endif // UTILITIES_PATHFINDING_HPP
