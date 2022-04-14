#pragma once

#ifndef EMBIT_FUNCTION_WRAPPER_HPP
#    define EMBIT_FUNCTION_WRAPPER_HPP

#    include "Core.hpp"

namespace embit {
namespace detail {
template<class>
struct AlwaysFalse : std::false_type {};

template<class Func>
class FunctionWrapper {
    mutable Func _func;
    bool _isConstructed{ false };

    constexpr explicit FunctionWrapper(std::false_type /*isDefaultConstructible*/) {
        static_assert(AlwaysFalse<Func>::value, "Please use std::function (if possible), an object with operator()() or a "
                                                "regular function instead of a lambda in this case, because "
                                                "lambda's are not default constructible pre C++20");
    }

    constexpr explicit FunctionWrapper(std::true_type /*isDefaultConstructible*/) : _func(), _isConstructed(true) {
    }

    template<class F>
    EMBIT_CONSTEXPR_CXX_20 void construct(F&& f) {
        ::new (static_cast<void*>(std::addressof(_func))) Func(static_cast<F&&>(f));
        _isConstructed = true;
    }

    constexpr void reset() noexcept {
        if (_isConstructed) {
            _func.~Func();
            _isConstructed = false;
        }
    }

#    ifdef __cpp_if_constexpr
    template<class F = Func>
    EMBIT_CONSTEXPR_CXX_20 void copy(const Func& f) {
        if constexpr (std::is_copy_assignable_v<F>) {
            _func = f;
        }
        else {
            reset();
            construct(f);
        }
    }

    template<class F = Func>
    EMBIT_CONSTEXPR_CXX_20 void move(Func&& f) {
        if constexpr (std::is_move_assignable_v<F>) {
            _func = std::move(f);
        }
        else {
            reset();
            construct(std::move(f));
        }
    }
#    else
    template<class F = Func>
    EMBIT_CONSTEXPR_CXX_20 EnableIf<std::is_copy_assignable<F>::value> copy(const Func& f) {
        _func = f;
    }

    template<class F = Func>
    EMBIT_CONSTEXPR_CXX_20 EnableIf<!std::is_copy_assignable<F>::value> copy(const Func& f) {
        reset();
        construct(f);
    }

    template<class F = Func>
    constexpr EnableIf<std::is_move_assignable<F>::value> move(Func&& f) {
        _func = std::move(f);
    }

    template<class F = Func>
    EMBIT_CONSTEXPR_CXX_20 EnableIf<!std::is_move_assignable<F>::value> move(Func&& f) {
        reset();
        construct(std::move(f));
    }
#    endif

public:
    constexpr explicit FunctionWrapper(const Func& func) : _func(func), _isConstructed(true) {
    }

    constexpr explicit FunctionWrapper(Func&& func) noexcept : _func(std::move(func)), _isConstructed(true) {
    }

    constexpr FunctionWrapper() : FunctionWrapper(std::is_default_constructible<Func>()) {
    }

    constexpr const Func& get() const noexcept {
        return _func;
    }

    EMBIT_CONSTEXPR_CXX_14 FunctionWrapper(FunctionWrapper&& other) noexcept :
        _func(std::move(other._func)),
        _isConstructed(true) {
        other._isConstructed = false;
    }

    constexpr FunctionWrapper(const FunctionWrapper& other) : _func(other._func), _isConstructed(true) {
    }

    EMBIT_CONSTEXPR_CXX_20 FunctionWrapper& operator=(const FunctionWrapper& other) {
        if (_isConstructed && other._isConstructed) {
            copy(other._func);
        }
        else if (other._isConstructed) {
            construct(other._func);
        }
        else if (_isConstructed) {
            reset();
        }
        return *this;
    }

    EMBIT_CONSTEXPR_CXX_20 FunctionWrapper& operator=(FunctionWrapper&& other) noexcept {
        if (_isConstructed && other._isConstructed) {
            move(std::move(other._func));
        }
        else if (other._isConstructed) {
            construct(std::move(other._func));
        }
        else if (_isConstructed) {
            reset();
        }
        return *this;
    }

    template<class... Args>
    constexpr auto operator()(Args&&... args) const noexcept(noexcept(_func(std::forward<Args>(args)...)))
        -> decltype(_func(std::forward<Args>(args)...)) {
        return _func(std::forward<Args>(args)...);
    }
};
} // namespace detail
} // namespace embit

#endif // EMBIT_FUNCTION_WRAPPER_HPP
