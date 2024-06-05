#ifndef UTILITIES_EVENTS_HPP
#define UTILITIES_EVENTS_HPP

#include <list>
#include <functional>

namespace Utilities
{
	template<typename ... TArgs>
	class Event
	{
	public:
		using CallbackType = std::function<void(TArgs... args)>;
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

	template<typename TObject, typename ... TArgs>
	class ObjectEvent
	{
	public:
		using CallbackType = std::function<void(TObject& sender, TArgs... args)>;
	private:
		TObject* _sender;
		std::list<CallbackType> _callbacks;
	public:
		ObjectEvent(TObject* sender) :
			_sender(sender)
		{ }
		ObjectEvent(TObject& sender) :
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

		void operator()(TArgs... args) const
		{
			TObject& sender = (TObject&) _sender;
			for (auto& callback : _callbacks)
				callback(sender, args...);
		}
		void operator+=(CallbackType callback) { _callbacks.push_back(callback); }
		void operator-=(CallbackType callback) { _callbacks.remove(callback); }
	};
}

#endif // UTILITIES_EVENTS_HPP
