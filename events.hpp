#ifndef UTILITIES_EVENTS_HPP
#define UTILITIES_EVENTS_HPP

#include <list>
#include <functional>
#include <utility>
#include <stdexcept>
#include <memory>

namespace Utilities
{
    template<typename ...TArgs>
    class static_event_t final
    {
    public:
        using callback_t = std::function<void(TArgs ...args)>;
        using callback_ptr = std::shared_ptr<callback_t>;
    private:
        std::list<callback_ptr> _callbacks;
    public:
        static_event_t() = default;
        ~static_event_t() { _callbacks.clear(); }

        void operator()(TArgs&&... args) const
        {
            for (auto& callback : _callbacks)
                (*callback)(args...);
        }

        callback_ptr operator+=(callback_t&& callback) noexcept {
            return _callbacks.emplace_back(callback);
        }
        callback_ptr operator+=(callback_t const& callback) noexcept {
            return _callbacks.emplace_back(callback);
        }
        callback_ptr operator+=(callback_ptr&& callback) noexcept {
            return _callbacks.push_back(callback);
        }
        callback_ptr operator+=(callback_ptr const& callback) noexcept {
            return _callbacks.push_back(callback);
        }
        void operator-=(callback_ptr&& callback) noexcept {
            _callbacks.remove(callback);
        }
        void operator-=(callback_ptr const& callback) noexcept {
            _callbacks.remove(callback);
        }
    };

    template<typename TObject, typename ...TArgs>
    class event_t final
    {
    public:
        struct sender_must_be_valid_reference_error : std::runtime_error
        {
            sender_must_be_valid_reference_error() :  std::runtime_error("nullptr was passed.") {}
        };

        using callback_t = std::function<void(TObject& sender, TArgs ...args)>;
        using callback_ptr = std::shared_ptr<callback_t>;
    private:
        TObject* _sender;
        std::list<callback_ptr> _callbacks;
    public:
        event_t(TObject* sender) : _sender(sender)
        {
            if(!_sender)
                throw sender_must_be_valid_reference_error{};
        }
        event_t(TObject& sender) noexcept : _sender(&sender) { }

        event_t(event_t const& other) noexcept : _sender(other._sender), _callbacks(other._callbacks) { }
        event_t& operator=(event_t const& other) noexcept
        {
            _sender = other._sender;
            _callbacks = other._callbacks;;
            return *this;
        }

        event_t(event_t&& other) noexcept :
            _sender(std::exchange(other._sender, nullptr)),
            _callbacks(std::exchange(other._callbacks, {})) { }
        event_t& operator=(event_t&& other) noexcept
        {
            _sender = std::exchange(other._sender, nullptr);
            _callbacks = std::exchange(other._callbacks, {});
            return *this;
        }

        ~event_t() { _callbacks.clear(); }

        void operator()(TArgs&&... args) const
        {
            for (auto& callback : _callbacks)
                (*callback)(*_sender, args...);
        }

        callback_ptr operator+=(callback_t&& callback) noexcept {
            return _callbacks.emplace_back(callback);
        }
        callback_ptr operator+=(callback_t const& callback) noexcept {
            return _callbacks.emplace_back(callback);
        }
        callback_ptr operator+=(callback_ptr&& callback) noexcept {
            return _callbacks.push_back(callback);
        }
        callback_ptr operator+=(callback_ptr const& callback) noexcept {
            return _callbacks.push_back(callback);
        }
        void operator-=(callback_ptr&& callback) noexcept {
            _callbacks.remove(callback);
        }
        void operator-=(callback_ptr const& callback) noexcept {
            _callbacks.remove(callback);
        }
    };
}

#endif // UTILITIES_EVENTS_HPP
