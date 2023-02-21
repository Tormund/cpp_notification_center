#include <unordered_map>
#include <vector>
#include <functional>
#include <type_traits>
#include <utility>
#include <tuple>
#include <iostream>

template <class Notification>
class NotificationCenter
{
protected:
    template<typename Function, typename Tuple, size_t ... I>
    static auto call(Function f, Tuple t, std::index_sequence<I ...>)
    {
        return f(std::get<I>(t) ...);
    }

    template<typename Function, typename Tuple>
    static auto call(Function f, Tuple t)
    {
        static constexpr auto size = std::tuple_size<Tuple>::value;
        return call(f, t, std::make_index_sequence<size>{});
    }

    template <class... Args>
    int typeHash(Args... args)
    {
        return typeid(std::function<void(Args...)>).hash_code();
    }
    template <class... Args>
    int typeHash(std::function<void(Args...)> args)
    {
        return typeid(args).hash_code();
    }
    struct ObserverVisitor
    {
        int _typeHash;
        std::function<void(void*)> _setter;
    };
public:
    template <class ...Args>
    void post(Notification const & n, Args... args)
    {
        if (notifications.find(n) == notifications.end())
            return;

        std::vector<ObserverVisitor> observersToCall;
        observersToCall.reserve(notifications[n].size());

        for (auto const & [obsId, observers]:notifications[n])
        {
            for (auto const & handler : observers)
            {
                observersToCall.push_back(handler);
            }
        }

        constexpr std::size_t argc = sizeof...(Args);
        if constexpr (argc > 0)
        {
            auto _packedArgs = std::make_tuple(args...);
            const auto argType = typeHash(args...);
            for (auto const & handler : observersToCall)
            {
                if (handler._typeHash != argType)
                {
                    // todo raise exception?
                    std::cout<<"invalid type hashes. Required: " << argType << ",but handler has - " << handler._typeHash << "\n";
                    continue;
                }
                handler._setter(&_packedArgs);
            }
        }
        else
        {
            const auto argType = typeHash();
            for (auto const & handler : observersToCall)
            {
                if (handler._typeHash != argType)
                {
                    // todo raise exception?
                    std::cout<<"invalid type hashes. Required: " << argType << ",but handler has - " << handler._typeHash << "\n";
                    continue;
                }
                handler._setter(nullptr);
            }
        }

    }

    template<class F>
    void addObserver(void* observerId, Notification const & notification, F f)
    {
        addObserver(reinterpret_cast<intptr_t>(observerId), notification, std::function(f));
    }

    template<class F>
    void addObserver(intptr_t observerId, Notification const & notification, F f)
    {
        addObserver(observerId, notification, std::function(f));
    }

    template<class... Args>
    void addObserver(void* observerId, Notification const & notification, std::function<void(Args...)> cb)
    {
        addObserver(reinterpret_cast<intptr_t>(observerId), notification, cb);
    }

    template<class... Args>
    void addObserver(intptr_t observerId, Notification const & notification, std::function<void(Args...)> cb)
    {
        int typHash = typeid(std::function<void(Args...)>).hash_code();
        constexpr bool isVoidfunc = std::is_same<std::function<void(Args...)>, std::function<void(void)>>::value;
        notifications[notification][observerId].push_back(ObserverVisitor{
            typHash, [cb](void* _packedArgs){
                if constexpr (!isVoidfunc)
                {
                    std::tuple<Args...> pargs = *static_cast<std::tuple<Args...>*>(_packedArgs);
                    call(cb, pargs);
                }
                else
                {
                    cb();
                }
            }
        });
    }

    void removeObserver(void * observerPtr)
    {
        removeObserver(reinterpret_cast<intptr_t>(observerPtr));
    }

    void removeObserver(intptr_t observerId)
    {
        std::vector<Notification> toRemove {};
        for (auto & [n, observers]: notifications)
        {
            observers.erase(observerId);
            if (notifications[n].size() == 0)
                toRemove.push_back(n);
        }
        for (auto & n: toRemove)
        {
            notifications.erase(n);
        }
    }

    void removeObserver(void * observerPtr, Notification const & notification)
    {
        removeObserver(reinterpret_cast<intptr_t>(observerPtr), notification);
    }

    void removeObserver(intptr_t observerId, Notification const & notification)
    {
        notifications[notification].erase(observerId);
        if (notifications[notification].size() == 0)
        {
            notifications.erase(notification);
        }
    }

private:
    template <class T> using Observers = std::unordered_map<intptr_t, std::vector<T>>;
    Observers<ObserverVisitor> observers;
    std::unordered_map<Notification, Observers<ObserverVisitor>> notifications;
};