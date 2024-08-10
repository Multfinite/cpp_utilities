#ifndef UTILITIES_EVENTS_HPP
#define UTILITIES_EVENTS_HPP

#include <list>
#include <functional>
#include <utility>
#include <stdexcept>

namespace Utilities
{    
        template<typename ...TArgs>
	class Event
	{
	public:
                using CallbackType = std::function<void(TArgs&& ...args)>;
	private:
		std::list<CallbackType> _callbacks;
	public:
		Event() = default;
		~Event() { _callbacks.clear(); }

                void operator()(TArgs... args) const
		{
			for (auto& callback : _callbacks)
				callback(args...);
		}
		void operator+=(CallbackType callback) { _callbacks.push_back(callback); }
		void operator-=(CallbackType callback) { _callbacks.remove(callback); }
	};

        template<typename TObject, typename ...TArgs>
	class ObjectEvent
	{
	public:
                struct sender_must_be_valid_reference_error : std::runtime_error
                {
                    sender_must_be_valid_reference_error() :  std::runtime_error("nullptr was passed.") {}
                };

                using CallbackType = std::function<void(TObject& sender, TArgs&& ...args)>;
	private:
                TObject* _sender;
		std::list<CallbackType> _callbacks;
	public:
                ObjectEvent(TObject* sender) :
                        _sender(sender)
                {
                    if(!_sender)
                        throw sender_must_be_valid_reference_error{};
                }
                ObjectEvent(TObject& sender) noexcept :
                        _sender(&sender)
		{ }

		ObjectEvent(ObjectEvent const& other) noexcept :
			_sender(other._sender), _callbacks(other._callbacks) { }
		ObjectEvent& operator=(ObjectEvent const& other) noexcept
		{
			_sender = other._sender;
			_callbacks = other._callbacks;;
			return *this;
		}

		ObjectEvent(ObjectEvent&& other) noexcept :
			_sender(std::exchange(other._sender, nullptr)),
			_callbacks(std::exchange(other._callbacks, {})) { }
		ObjectEvent& operator=(ObjectEvent&& other) noexcept
		{
			_sender = std::exchange(other._sender, nullptr);
			_callbacks = std::exchange(other._callbacks, {});
			return *this;
		}

		~ObjectEvent() { _callbacks.clear(); }

                inline void operator()(TArgs&& ...args) const noexcept
		{
                    for (auto& callback : _callbacks)
                            callback(*_sender, args...);
		}
                inline void operator+=(CallbackType callback) noexcept { _callbacks.push_back(callback); }
                inline void operator-=(CallbackType callback) noexcept { _callbacks.remove(callback); }
	};
}

#endif // UTILITIES_EVENTS_HPP
