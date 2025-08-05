#ifndef UTILITIES_SPACE_NODE_HPP
#define UTILITIES_SPACE_NODE_HPP

#include "tree_node.hpp"

namespace Utilities
{    
    template<typename TVector>
    class SpaceNode : public TreeNode
    {
        using node = SpaceNode<TVector>;
       /*!
        * @brief It should be any type which applies arithemtic operations even numeric types.
        * @brief Default construction should give ZERO object.
        */
        using vector_type = TVector;

        vector_type _position{}, _globalPosition{};
    public:
        struct events_t
        {
            event_t<node, vector_type /* old position */> PositionChanged;

            events_t(node& node) : PositionChanged(node) {}
        } Event;

        GETTER_V_DEFAULT(global_position, _globalPosition)
        SETTER_V(global_position, _globalPosition)
        {
            auto old = _globalPosition;
            _globalPosition = value;
            _position = _globalPosition - (parent() ? parent_as<node>()->_globalPosition : vector_type{});
            for(TreeNode* chlid : *this)
                if(node* n = dynamic_cast<node*>(chlid))
                    n->set_position(n->_position);
            Event.PositionChanged(std::move(old));
        }

        GETTER_V_DEFAULT(position, _position)
        SETTER_V(position, _position)
        {
            auto old = _position;
            _position = value;
            _globalPosition = _position + (parent() ? parent_as<node>()->_globalPosition : vector_type{});
            for(TreeNode* chlid : *this)
                if(node* n = dynamic_cast<node*>(chlid))
                    n->set_position(n->_position);
            Event.PositionChanged(std::move(old));
        }
    private:
        typename decltype(TreeNode::Event.ParentChanged)::subscription_t __onParentChanged;
    public:
        SpaceNode() : TreeNode(), Event(*this)
        {
            __onParentChanged = TreeNode::Event.ParentChanged += [this](TreeNode& sender, TreeNode* old)
            {
                set_global_position(_globalPosition);
            };
        }
    };
}

#endif // UTILITIES_SPACE_NODE_HPP
