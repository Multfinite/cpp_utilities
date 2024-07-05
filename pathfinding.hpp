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
    enum struct ProcessingState : unsigned char
    {
        /*!
          * @brief Did not appear in processing yet.
          */
        Unexplored = 0,
        /*!
          * @brief Contained in queue to be processed.
          */
        Discovered = 1,
        /*!
         * @brief It's excluded from processing. FFU.
         */
        Excluded = 254,
        /*!
          * @brief Fully processed
          */
        Explored = 255
    };

    /*!
     * @brief Contains a way how current VertexNode is reached ("Parent")
     */
    template<typename TVertex, typename TEdge, typename TCostType>
#if HAS_CONCEPTS
        requires IsVertex<TVertex> && IsEdge<TEdge>
#endif
    struct PathEntry
    {
        using cost_type = TCostType;
        using vertex_type = TVertex;
        using edge_type = TEdge;
        using path_entry = PathEntry<vertex_type, edge_type, cost_type>;

        vertex_type const& From;
        edge_type const& Direction;
        /*!
         * @brief Total cost from root to this
         */
        cost_type Cost;

        PathEntry(vertex_type const& from, edge_type const& direction) : From(from), Direction(direction), Cost() {}
        PathEntry(vertex_type const& from, edge_type const& direction, const cost_type& cost) : From(from), Direction(direction), Cost(cost) {}
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
        using path_entry = PathEntry<vertex_type, edge_type, cost_type>;
        using path_type_default = std::vector<std::reference_wrapper<path_entry>>;

        // create and std::set here and store ALL variants - [root vertex, it'is path entry]
        // also, it is cache for single root. Need to make storing for any count of roots.
        /*!
         * @brief If entry is empty then it mean that current node is root node.
         */
        optional<path_entry> Entry;

        VertexNode(vertex_type& owner) : linked_node(owner) { clear(); }
        VertexNode(vertex_type& owner, size_t index) : linked_node(owner, index) { clear(); }
        ~VertexNode() = default;

        template<typename TPathType = path_type_default>
        inline TPathType Build() const
        {
            std::list<std::reference_wrapper<path_entry>> path;
            vertex_node_type* v = const_cast<vertex_node_type*>(this);
            /*
             * NOTE: there are no protection against call with the same vertex in pathfinding (like `operator()(v, v)`)
             * as result it will cause endless cycle with memory allocation!
            */
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
            Entry.reset();
            //Entry = nullopt;
        }

        inline bool operator==(vertex_node_type const& other) const { return &this == &other; }
        inline bool operator!=(vertex_node_type const& other) const { return &this != &other; }
    };

    template<typename TEdge, typename TCostType = double>
#if HAS_CONCEPTS
        requires IsEdge<TEdge> && IsNumericType<TCostType>
#endif
    struct EdgeNode : public __linked_node<TEdge>
    {
        using linked_node = __linked_node<TEdge>;
        using edge_type = TEdge;
        using edge_node_type = EdgeNode<TEdge>;
        using vertex_type = typename edge_type::vertex_type;
        using cost_type = TCostType;

        /*!
         * @brief Atomic transferring cost of this edge
         */
        optional<cost_type> Cost;
        /*!
          * @brief Processing state (flags) for any pathfinding.
          * @note early it was located in VertexNode but we don't walk via vertexes when we walk for path of vertexes via edges. So it was moved here by logical reasons.
          */
        ProcessingState State;

        EdgeNode(edge_type& owner) : linked_node(owner) { clear(); }
        EdgeNode(edge_type& owner, size_t index) : linked_node(owner, index) { clear(); }
        ~EdgeNode() = default;

        void clear()
        {
            Cost = nullopt;
            State = ProcessingState::Unexplored;
        }

        inline bool operator==(edge_node_type const& other) const { return &this == &other; }
        inline bool operator!=(edge_node_type const& other) const { return &this != &other; }
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

        using vertex_nodes_type = std::vector<std::reference_wrapper<vertex_node_type>>;
        inline vertex_node_type& node_of(vertex_type const& v) const
        {
            return v.__linking.template as_ref<vertex_node_type>(this->Index);
        }
        inline vertex_nodes_type nodes_of(vertex_type const& v...) const
        {
            vertex_nodes_type nodes; nodes.reserve(sizeof(v));
            for(auto const& i : v)
                nodes.push_back(i.__linking.template as_ref<vertex_node_type>(this->Index));
            return nodes;
        }
        template<typename InputIt>
        inline vertex_nodes_type nodes_of(InputIt b, InputIt e) const
        {
            vertex_nodes_type nodes; nodes.reserve(e - b);
            for(; b != e; b++)
                nodes.push_back(b->__linking.template as_ref<vertex_node_type>(this->Index));
            return nodes;
        }

        using edge_nodes_type = std::vector<std::reference_wrapper<edge_node_type>>;
        inline edge_node_type& node_of(edge_type const& v) const
        {
            return v.__linking.template as_ref<edge_node_type>(this->Index);
        }
        inline edge_nodes_type nodes_of(edge_type const& v...) const
        {
            edge_nodes_type nodes; nodes.reserve(sizeof(v));
            for(auto const& i : v)
                nodes.push_back(i.__linking.template as_ref<edge_node_type>(this->Index));
            return nodes;
        }
        template<typename InputIt>
        inline edge_nodes_type nodes_of(InputIt b, InputIt e) const
        {
            edge_nodes_type nodes; nodes.reserve(e - b);
            for(; b != e; b++)
                nodes.push_back(b->__linking.template as_ref<edge_node_type>(this->Index));
            return nodes;
        }
    };

    template<typename TGraphNode>
    struct DefaultCostEvaluator
    {
        using graph_node_type = TGraphNode;
        using cost_type = typename TGraphNode::cost_type;
        using edge_node_type = typename TGraphNode::edge_node_type;

        cost_type operator()(graph_node_type const& g, edge_node_type const& e) { return 1; }
    };
}

#endif // UTILITIES_PATHFINDING_HPP
