#ifndef UTILITIES_STRUCT_CHAIN4_HPP
#define UTILITIES_STRUCT_CHAIN4_HPP

#include "macro.hpp"

#include <list>

namespace Utilities::Struct
{
    /*!
     * @brief Structure with 4 linked sides: right, bottom, left, top
     */
    template<typename T> struct Chain4
    {
        using self_type = Chain4<T>;
        using value_type = T;
        using ptr_type = T*;
    private:
        ptr_type _top = nullptr; ptr_type _bottom = nullptr; ptr_type _right = nullptr; ptr_type _left = nullptr;
    public:
        constexpr GETTER_V_DEFAULT(top, _top)
        constexpr SETTER_V(top, _top)
        {
            if(_top) _top->_bottom = nullptr;
            _top = value;
            if(_top) _top->_bottom = this;
        }

        constexpr GETTER_V_DEFAULT(bottom, _bottom)
        constexpr SETTER_V(bottom, _bottom)
        {
            if(_bottom) _bottom->_top = nullptr;
            _bottom = value;
            if(_bottom) _bottom->_top = this;
        }

        constexpr GETTER_V_DEFAULT(left, _left)
        constexpr SETTER_V(left, _left)
        {
            if(_left) _left->_right = nullptr;
            _left = value;
            if(_left) _left->_right = this;
        }

        constexpr GETTER_V_DEFAULT(right, _right)
        constexpr SETTER_V(right, _right)
        {
            if(_right) _right->_left = nullptr;
            _right = value;
            if(_right) _right->_left = this;
        }

        using direction_ptr = decltype(&self_type::_top);
        using direction_getter_type = decltype(&self_type::top);
        using direction_setter_type = decltype(&self_type::set_top);

        constexpr bool is_on_direction(value_type const& other, direction_ptr direction) const noexcept
        {
            for(value_type const* w = this->*direction; w != nullptr; w = w->*direction)
            {
                if(w == &other)
                    return true;
            }
            return false;
        }

        constexpr bool above(value_type const& other) const noexcept { return is_on_direction(other, &self_type::_top); }
        constexpr bool below(value_type const& other) const noexcept { return is_on_direction(other, &self_type::_bottom); }
        constexpr bool at_right(value_type const& other) const noexcept { return is_on_direction(other, &self_type::_right); }
        constexpr bool at_left(value_type const& other) const noexcept { return is_on_direction(other, &self_type::_left); }

        constexpr std::list<ptr_type> seek_direction(value_type const& other, direction_ptr direction) const noexcept
        {
            std::list<ptr_type> path;
            for(ptr_type w = this->*direction; w != nullptr; w = w->*direction)
            {
                path.push_back(w);
                if(w == &other)
                    return path;
            }
            path.clear();
            return path;
        }
        constexpr std::list<ptr_type> seek_direction(value_type const& other, direction_getter_type direction) const noexcept
        {
            std::list<ptr_type> path;
            for(ptr_type w = (this->*direction)(); w != nullptr; w = (w->*direction)())
            {
                path.push_back(w);
                if(w == &other)
                    return path;
            }
            path.clear();
            return path;
        }
        constexpr std::list<ptr_type> seek_above(value_type const& other) const noexcept { return seek_direction(other, &self_type::_top); }
        constexpr std::list<ptr_type> seek_below(value_type const& other) const noexcept { return seek_direction(other, &self_type::_bottom); }
        constexpr std::list<ptr_type> seek_at_right(value_type const& other) const noexcept { return seek_direction(other, &self_type::_right); }
        constexpr std::list<ptr_type> seek_at_left(value_type const& other) const noexcept { return seek_direction(other, &self_type::_left); }

        template<typename Pred> constexpr std::list<ptr_type> seek_while_direction(Pred const& pred, direction_ptr direction) const noexcept
        {
            std::list<ptr_type> path;
            for(ptr_type w = this->*direction; w != nullptr; w = w->*direction)
            {
                path.push_back(w);
                if(pred(w))
                    return path;
            }
            path.clear();
            return path;
        }
        template<typename Pred> constexpr std::list<ptr_type> seek_while_direction(Pred const& pred, direction_getter_type direction) const noexcept
        {
            std::list<ptr_type> path;
            for(ptr_type w = (this->*direction)(); w != nullptr; w = (w->*direction)())
            {
                path.push_back(w);
                if(pred(w))
                    return path;
            }
            path.clear();
            return path;
        }
        template<typename Pred> constexpr std::list<ptr_type> seek_while_above(Pred const& pred) const noexcept { return seek_while_direction(pred, &self_type::_top); }
        template<typename Pred> constexpr std::list<ptr_type> seek_while_below(Pred const& pred) const noexcept { return seek_while_direction(pred, &self_type::_bottom); }
        template<typename Pred> constexpr std::list<ptr_type> seek_while_at_right(Pred const& pred) const noexcept { return seek_while_direction(pred, &self_type::_right); }
        template<typename Pred> constexpr std::list<ptr_type> seek_while_at_left(Pred const& pred) const noexcept { return seek_while_direction(pred, &self_type::_left); }

        constexpr ptr_type farest(direction_ptr direction) const noexcept
        {
            value_type const* w;
            for(w = this; w->*direction != nullptr; w = w->*direction) {}
            return const_cast<ptr_type>(w);
        }
        constexpr ptr_type farest(direction_getter_type direction) const noexcept
        {
            value_type const* w;
            for(w = this; (w->*direction)() != nullptr; w = (w->*direction)()) {}
            return const_cast<ptr_type>(w);
        }
        constexpr ptr_type top_most() const noexcept { return farest(&self_type::_top); }
        constexpr ptr_type bottom_most() const noexcept { return farest(&self_type::_bottom); }
        constexpr ptr_type right_most() const noexcept { return farest(&self_type::_right); }
        constexpr ptr_type left_most() const noexcept { return farest(&self_type::_left); }

        constexpr std::list<ptr_type> all_in_direction(direction_ptr direction) const noexcept
        {
            std::list<ptr_type> path;
            for(ptr_type w = w->*direction; w != nullptr; w = w->*direction) { path.push_back(w); }
            return path;
        }
        constexpr std::list<ptr_type> all_in_direction(direction_getter_type direction) const noexcept
        {
            std::list<ptr_type> path;
            for(ptr_type w = (w->*direction)(); w != nullptr; w = (w->*direction)()) { path.push_back(w); }
            return path;
        }
        constexpr std::list<ptr_type> all_above() const noexcept { return all_in_direction(&self_type::_top); }
        constexpr std::list<ptr_type> all_below() const noexcept { return all_in_direction(&self_type::_bottom); }
        constexpr std::list<ptr_type> all_right() const noexcept { return all_in_direction(&self_type::_right); }
        constexpr std::list<ptr_type> all_left() const noexcept { return all_in_direction(&self_type::_left); }

        constexpr bool is_corner(value_type const& a, value_type const& b) const noexcept
        {
            bool const aIsVertical = _top == &a || _bottom == &a;
            bool const bIsVertical = _top == &b || _bottom == &b;
            bool const aIsHorizontal = _right == &a || _left == &a;
            bool const bIsHorizontal = _right == &b || _left == &b;

            if(!aIsVertical && !aIsHorizontal && !bIsVertical && !bIsHorizontal)
                return false;
            if((aIsVertical && bIsVertical) || (aIsHorizontal && bIsHorizontal))
                return false;
            return true;
        }
        constexpr bool is_vertical(value_type const& a, value_type const& b) const noexcept
        {
            bool const aIsVertical = _top == &a || _bottom == &a;
            bool const bIsVertical = _top == &b || _bottom == &b;
            return aIsVertical && bIsVertical;
        }
        constexpr bool is_horizontal(value_type const& a, value_type const& b) const noexcept
        {
            bool const aIsHorizontal = _right == &a || _left == &a;
            bool const bIsHorizontal = _right == &b || _left == &b;
            return aIsHorizontal && bIsHorizontal;
        }
        constexpr bool on_vertical(value_type const& other) const noexcept { return _top == &other || _bottom == &other; }
        constexpr bool on_horizontal(value_type const& other) const noexcept { return _right == &other || _left == &other; }

        constexpr std::list<ptr_type> seek(value_type const& other) const noexcept
        {
            for(auto dir :{
                &self_type::_bottom
              , &self_type::_top
              , &self_type::_right
              , &self_type::_left
            }) {
                auto path = seek_direction(other, dir);
                if(!path.empty())
                    return path;
            }
            return {};
        }

        constexpr void detach()
        {
            for(auto setter : { &self_type::set_right, &self_type::set_bottom, &self_type::set_left, &self_type::set_top })
                (this->*setter)(nullptr);
        }

        constexpr size_t connections() const noexcept {
            size_t conns = 0;
            for(auto v : { _right, _bottom, _left, _top })
                if(v) ++conns;
            return conns;
        }

        static constexpr direction_ptr opposite_of(direction_ptr side) noexcept
        {
            direction_ptr oppositeSide = side;
            for(auto p : {
                std::make_pair(&self_type::_right, &self_type::_left)
              , std::make_pair(&self_type::_bottom, &self_type::_top)
              , std::make_pair(&self_type::_left, &self_type::_right)
              , std::make_pair(&self_type::_top, &self_type::_bottom)
            })
                if(p.first == side)
                    oppositeSide = p.second;
            return oppositeSide;
        }
        static constexpr direction_getter_type opposite_of(direction_getter_type side) noexcept
        {
            direction_getter_type oppositeSide = side;
            for(auto p : {
                std::make_pair(&self_type::right, &self_type::left)
              , std::make_pair(&self_type::bottom, &self_type::top)
              , std::make_pair(&self_type::left, &self_type::right)
              , std::make_pair(&self_type::top, &self_type::bottom)
            })
                if(p.first == side)
                    oppositeSide = p.second;
            return oppositeSide;
        }
        static constexpr direction_setter_type opposite_of(direction_setter_type side) noexcept
        {
            direction_setter_type oppositeSide = side;
            for(auto p : {
                std::make_pair(&self_type::set_right, &self_type::set_left)
              , std::make_pair(&self_type::set_bottom, &self_type::set_top)
              , std::make_pair(&self_type::set_left, &self_type::set_right)
              , std::make_pair(&self_type::set_top, &self_type::set_bottom)
            })
                if(p.first == side)
                    oppositeSide = p.second;
            return oppositeSide;
        }
        static constexpr direction_getter_type getter_of(direction_setter_type side) noexcept
        {
            direction_getter_type oppositeSide = side;
            for(auto p : {
                std::make_pair(&self_type::set_right, &self_type::right)
              , std::make_pair(&self_type::set_bottom, &self_type::bottom)
              , std::make_pair(&self_type::set_left, &self_type::left)
              , std::make_pair(&self_type::set_top, &self_type::top)
            })
                if(p.first == side)
                    oppositeSide = p.second;
            return oppositeSide;
        }
        static constexpr direction_setter_type setter_of(direction_getter_type side) noexcept
        {
            direction_setter_type oppositeSide = side;
            for(auto p : {
                std::make_pair(&self_type::right, &self_type::set_right)
              , std::make_pair(&self_type::bottom, &self_type::set_bottom)
              , std::make_pair(&self_type::left, &self_type::set_left)
              , std::make_pair(&self_type::top, &self_type::set_top)
            })
                if(p.first == side)
                    oppositeSide = p.second;
            return oppositeSide;
        }

        constexpr void insert(direction_getter_type side, value_type* wp) noexcept
        {
            auto setSide = setter_of(side);
            auto oppositeSide = opposite_of(side);
            auto setOppositeSide = setter_of(oppositeSide);

            ptr_type other = (this->*side)();
            (this->*setSide)(wp);
            (other->*setOppositeSide)(wp);
        }
    };
}

#endif // UTILITIES_STRUCT_CHAIN4_HPP
