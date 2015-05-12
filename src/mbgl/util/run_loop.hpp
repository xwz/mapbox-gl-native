#ifndef MBGL_UTIL_RUN_LOOP
#define MBGL_UTIL_RUN_LOOP

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <functional>
#include <queue>
#include <mutex>

namespace {

template <::std::size_t...>
struct index_sequence {};

template <::std::size_t N, ::std::size_t... I>
struct integer_sequence : integer_sequence<N - 1, N - 1, I...> {};

template <::std::size_t... I>
struct integer_sequence<0, I...> {
    using type = index_sequence<I...>;
};

}

namespace mbgl {
namespace util {

class RunLoop : private util::noncopyable {
public:
    RunLoop(uv_loop_t*);
    ~RunLoop();

    void stop();

    // Invoke fn() in the runloop thread.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        auto invokable = util::make_unique<Invoker<Fn, Args...>>(
            std::move(fn), std::forward_as_tuple(std::forward<Args>(args)...));
        withMutex([&] { queue.push(std::move(invokable)); });
        async.send();
    }

    // Return a function that always calls the given function on the current RunLoop.
    template <class... Args>
    auto bind(std::function<void (Args...)> fn) {
        return [this, fn = std::move(fn)] (Args&&... args) mutable {
            invoke(std::move(fn), std::move(args)...);
        };
    }

    // Invoke fn() in the runloop thread, then invoke callback(result) in the current thread.
    template <class Fn, class R>
    void invokeWithResult(Fn&& fn, std::function<void (R)> callback) {
        invoke([fn = std::move(fn), callback = current.get()->bind(callback)] () mutable {
            callback(fn());
        });
    }

    // Invoke fn() in the runloop thread, then invoke callback() in the current thread.
    template <class Fn>
    void invokeWithResult(Fn&& fn, std::function<void ()> callback) {
        invoke([fn = std::move(fn), callback = current.get()->bind(callback)] () mutable {
            fn();
            callback();
        });
    }

    uv_loop_t* get() { return async.get()->loop; }

    static uv::tls<RunLoop> current;

private:
    // A movable type-erasing invokable entity wrapper. This allows to store arbitrary invokable
    // things (like std::function<>, or the result of a movable-only std::bind()) in the queue.
    // Source: http://stackoverflow.com/a/29642072/331379
    struct Message {
        virtual void operator()() = 0;
        virtual ~Message() = default;
    };

    template <class F, class... Args>
    struct Invoker : Message {
        using P = std::tuple<Args...>;

        Invoker(F&& f, P&& p)
          : func(std::move(f)),
            params(std::move(p)) {
        }

        void operator()() override {
            invoke(typename integer_sequence<sizeof...(Args)>::type());
        }

        template<std::size_t... I>
        void invoke(index_sequence<I...>) {
             func(std::forward<Args>(std::get<I>(params))...);
        }

        F func;
        P params;
    };

    using Queue = std::queue<std::unique_ptr<Message>>;

    void withMutex(std::function<void()>&&);
    void process();

    Queue queue;
    std::mutex mutex;
    uv::async async;
};

}
}

#endif
