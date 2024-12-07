#ifndef UTILITIES_TREE_NODE_HPP
#define UTILITIES_TREE_NODE_HPP

#include "macro.hpp"
#include "events.hpp"
#include <list>

namespace Utilities
{
    class TreeNode
    {
        TreeNode* _parent = nullptr;
        std::list<TreeNode*> _chlidrens;
    public:
        struct events_t
        {
            ObjectEvent<TreeNode, TreeNode* /*old parent */> ParentChanged;

            events_t(TreeNode& node) : ParentChanged(node) {}
        } Event;

        constexpr bool is_root() const { return !_parent; }

        constexpr GETTER_V_DEFAULT(parent, _parent)
        SETTER_V(parent, _parent)
        {
            auto* old = _parent;
            if(_parent) _parent->_chlidrens.remove(this);
            _parent = value;
            if(_parent) _parent->_chlidrens.push_back(this);
            Event.ParentChanged(std::move(old));
        }

        template<typename T>
        constexpr T* parent_as() const noexcept { return static_cast<T*>(_parent); }

        auto begin() const { return _chlidrens.begin(); }
        auto end() const { return _chlidrens.end(); }

        auto cbegin() const { return _chlidrens.cbegin(); }
        auto cend() const { return _chlidrens.cend(); }

        TreeNode() : Event(*this) {}
        virtual ~TreeNode() {} // need to use dynamic_cast
    };
}

#endif // UTILITIES_TREE_NODE_HPP
