#ifndef UTILITIES_SPACE_NODE_HPP
#define UTILITIES_SPACE_NODE_HPP

#include <macro.hpp>

#include <list>

namespace Utilities
{
    template<typename TVector>
    class SpaceNode
    {
        using node = SpaceNode<TVector>;
       /*!
        * @brief It should be any type which applies arithemtic operations even numeric types.
        * @brief Default construction should give ZERO object.
        */
        using vector_type = TVector;

        SpaceNode* _parent = nullptr;
        std::list<std::reference_wrapper<SpaceNode>> _chlidrens;
        vector_type _position, _globalPosition;

    public:
        bool is_root() const { return !_parent; }

        GETTER_V_DEFAULT(global_position, _globalPosition)
        SETTER_V(global_position, _globalPosition)
        {
            _globalPosition = value;
            _position = _globalPosition - (_parent ? _parent->_globalPosition : vector_type{});
            for(node& chlid : _chlidrens)
                chlid._globalPosition = chlid._position + _globalPosition;
        }

        GETTER_V_DEFAULT(position, _position)
        SETTER_V(position, _position)
        {
            _position = value;
            _globalPosition = _position + (_parent ? _parent->_globalPosition : vector_type{});
            for(node& chlid : _chlidrens)
                chlid._globalPosition = chlid._position + _globalPosition;
        }

        GETTER_V_DEFAULT(parent, _parent)
        SETTER_V(parent, _parent)
        {
            if(_parent) _parent->_chlidrens.remove(*this);
            _parent = value;
            if(_parent) _parent->_chlidrens.push_back(*this);
            set_position(_position);
        }

        auto begin() const { return _chlidrens.begin(); }
        auto end() const { return _chlidrens.end(); }

        auto cbegin() const { return _chlidrens.cbegin(); }
        auto cend() const { return _chlidrens.cend(); }
    };
}

#endif // UTILITIES_SPACE_NODE_HPP
