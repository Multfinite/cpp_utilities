#ifndef UTILITIES_PATHFINDING_BFS_HPP
#define UTILITIES_PATHFINDING_BFS_HPP

#include "pathfinding.hpp"

#define VERBOSE_PF 0
#if VERBOSE_PF == 1
#include <iostream>
using std::cout; using std::endl; using std::hex; using std::dec;
#endif

namespace Utilities::Pathfinding
{
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
        using queue_type = std::list<edge_node_type*>;

        graph_type const& Graph;

        graph_node_type GraphNode;
        TCostEvaluator Evaluator;

        BFS(graph_type& graph, TCostEvaluator&& evaluator)
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

    private:
        inline void enqueue(queue_type& ways, vertex_type const& v)
        {
            for(auto ptr : v.OutcomingEdges)
            {
                edge_type& e = reference_cast<edge_type>((edge_ptr) ptr);
                edge_node_type& en  = GraphNode.node_of(e);
                if(en.State != ProcessingState::Unexplored) continue;
                en.State = ProcessingState::Discovered;
                ways.push_back(&en);
#if VERBOSE_PF == 1
                cout << "   " << hex << &e << " (" << &en << ") queued." << dec << endl;
#endif
            }
        }
        inline void check(
                            edge_type const& edge, edge_node_type& edgeNode
                          , vertex_type const& f, vertex_node_type& fNode
                          , vertex_type const& t, vertex_node_type& tNode)
        {
#if VERBOSE_PF == 1
            cout
                    << "   " << __FUNCTION__ << endl
                    << hex << "   " << &f << " (" << &fNode << ")" << dec << "  "
                    << hex << "   " << &t << " (" << &tNode << ")" << dec << "  "
                    << endl;
#endif

            const cost_type fNodeCost = fNode.Entry.has_value() ? fNode.Entry.value().Cost : 0;
            const cost_type fullCost = fNodeCost + edgeNode.Cost.value();
            if (!tNode.Entry.has_value())
                tNode.Entry.emplace(f, edge, fullCost);
            else if (fullCost < tNode.Entry.value().Cost)
                tNode.Entry.value().Cost = fullCost;
        }
    public:
        template<typename TPathType = typename vertex_node_type::path_type_default>
        TPathType operator()(vertex_type const& from, vertex_type const& to)
        {
            vertex_node_type& toNode = GraphNode.node_of(to);
            vertex_node_type& fromNode = GraphNode.node_of(from);
            fromNode.Entry = nullopt;
#if VERBOSE_PF == 1
            cout
                    << hex << &from << " (" << &fromNode << ")" << dec << "    " //<< from.Context
                    << "        "
                    << hex << &to << " (" << &toNode << ")" << dec << "  " //<< to.Context
                    << endl;
#endif
            queue_type ways; enqueue(ways, from);
            while (!ways.empty())
            {
                edge_node_type& edgeNode = *ways.front(); ways.pop_front();
                edge_type const& edge = edgeNode.Owner;

                vertex_type const& f = edge.From;
                vertex_node_type& fNode = GraphNode.node_of(f);

                vertex_type const& t = edge.To;
                vertex_node_type& tNode = GraphNode.node_of(t);
#if VERBOSE_PF == 1
                cout
                        << hex << &f << dec << "    " //<< f.Context
                        << "    {" << hex << &edge  << " (" << &edgeNode << ")" << dec << "}----->    "
                        << hex << &t << dec << "    " //<< t.Context
                        << endl;
#endif
                if(edge_type::Bidirectional)
                {
                    enqueue(ways, t); enqueue(ways, f);
                    check(edge, edgeNode, f, fNode, t, tNode);
                    if(&f != &from) check(edge, edgeNode, t, tNode, f, fNode);
                }
                else
                {
                    enqueue(ways, t);
                    check(edge, edgeNode, f, fNode, t, tNode);
                }

                edgeNode.State = ProcessingState::Explored;
            }
             return toNode.template Build<TPathType>();
        }
    };
}

#undef VERBOSE_PF
#endif // UTILITIES_PATHFINDING_BFS_HPP
