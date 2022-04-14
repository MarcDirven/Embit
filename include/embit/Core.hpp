#pragma once

#ifndef EMBIT_VIEW_HPP
#    define EMBIT_VIEW_HPP

#    include <iterator>

#    if defined(__has_cpp_attribute)
#        define EMBIT_HAS_ATTRIBUTE(ATTR) __has_cpp_attribute(ATTR)
#    else
#        define EMBIT_HAS_ATTRIBUTE(ATTR) 0
#    endif // __has_cpp_attribute

#    if (defined(__GNUC__)) && !(defined(__clang__))
#        define EMBIT_GCC_VERSION __GNUC__
#    endif // GNU/clang

#    if defined(_MSVC_LANG)
#        define EMBIT_MSVC _MSVC_LANG
#    endif // _MSVC_LANG

#    if (defined(EMBIT_MSVC) && (EMBIT_MSVC >= 201103L) && (EMBIT_MSVC < 201402L)) ||                                            \
        ((__cplusplus >= 201103L) && (__cplusplus < 201402L))
#        define EMBIT_HAS_CXX_11
#    endif // end has cxx 11

#    if (__cplusplus >= 201300) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201300))
#        define EMBIT_CONSTEXPR_CXX_14 constexpr
#    else
#        define EMBIT_CONSTEXPR_CXX_14 inline
#    endif // has cxx 14

#    if (__cplusplus >= 201703L) || ((defined(EMBIT_MSVC)) && (EMBIT_MSVC >= 201703L))
#        define EMBIT_HAS_CXX_17
#        define EMBIT_CONSTEXPR_CXX_17 constexpr
#    else
#        define EMBIT_CONSTEXPR_CXX_17 inline
#    endif // Has cxx 17

#    if (__cplusplus > 201703L) || ((defined(EMBIT_MSVC) && (EMBIT_MSVC > 201703L)))
#        define EMBIT_HAS_CXX_20
#        if defined(__cpp_constexpr_dynamic_alloc) && defined(__cpp_lib_constexpr_dynamic_alloc) &&                              \
            defined(__cpp_lib_constexpr_string) && defined(__cpp_lib_constexpr_vector) &&                                        \
            defined(__cpp_lib_constexpr_algorithms)
#            define EMBIT_CONSTEXPR_CXX_20 constexpr
#        else
#            define EMBIT_CONSTEXPR_CXX_20 inline
#        endif // cpp constexpr new/algo
#    else
#        define EMBIT_CONSTEXPR_CXX_20 inline
#    endif // Has cxx 20

#    ifdef __cpp_inline_variables
#        define EMBIT_INLINE_VARIABLE inline
#    else
#        define EMBIT_INLINE_VARIABLE
#    endif

#    if EMBIT_HAS_ATTRIBUTE(no_unique_address)
#        define EMBIT_NO_UNIQUE_ADDRESS [[no_unique_address]]
#    else
#        define EMBIT_NO_UNIQUE_ADDRESS
#    endif

namespace embit {
template<class, class>
class View;
namespace detail {
#    ifdef EMBIT_HAS_CXX_11
template<class C>
using Decay = typename std::decay<C>::type;

template<class C>
using RemoveRef = typename std::remove_reference<C>::type;

template<class C>
using AddConst = typename std::add_const<C>::type;

template<bool Check, class True, class False>
using Conditional = typename std::conditional<Check, True, False>::type;
#    else
template<class T>
using Decay = std::decay_t<T>;
} // detail

template<class I>
using IterDiffType = typename std::iterator_traits<I>::difference_type;

template<class I>
using IterCat = typename std::iterator_traits<I>::iterator_category;

template<class I>
using IterRef = typename std::iterator_traits<I>::reference;

template<class V>
using BeginIter = decltype(std::begin(std::declval<V>()));

template<class V>
using EndIter = decltype(std::end(std::declval<V>()));

template<class V>
using ViewRef = typename std::iterator_traits<BeginIter<V>>::reference;

template<class V>
using ViewValueType = typename std::iterator_traits<BeginIter<V>>::value_type;

EMBIT_NO_UNIQUE_ADDRESS
constexpr EMBIT_INLINE_VARIABLE static struct DefaultSentinel {
} defaultSentinel;

template<class I, class S>
constexpr View<detail::Decay<I>, detail::Decay<S>> view(I&& first, S&& last);

template<class V>
constexpr IterDiffType<BeginIter<V>> distance(V&& view);

namespace detail {
template<class>
class CStringIterator;

template<class I>
struct IsBidirectionalIter : std::is_convertible<IterCat<I>, std::bidirectional_iterator_tag> {};

template<class V>
struct IsBidirectionalView : IsBidirectionalIter<BeginIter<V>> {};

template<class I>
struct IsRandomAccessIter : std::is_convertible<IterCat<I>, std::random_access_iterator_tag> {};

template<class V>
struct IsRandomAccessView : IsRandomAccessIter<BeginIter<V>> {};

template<bool B>
struct EnableIfImpl {};

template<>
struct EnableIfImpl<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using EnableIf = typename EnableIfImpl<B>::template type<T>;

template<class T>
using RemoveRef = std::remove_reference_t<T>;

template<class T>
using AddConst = typename std::add_const_t<T>;

template<bool Check, class True, class False>
using Conditional = std::conditional_t<Check, True, False>;
#    endif

template<class, class = int>
struct HasBack : std::false_type {};

template<class T>
struct HasBack<T, decltype((void)std::declval<Decay<T>>().back(), 0)> : std::true_type {};

template<class, class = int>
struct HasDistance : std::false_type {};

template<class T>
struct HasDistance<T, decltype((void)std::declval<Decay<T>>().distance(), 0)> : std::true_type {};

template<class, class = int>
struct HasToCommon : std::false_type {};

template<class T>
struct HasToCommon<T, decltype((void)std::declval<Decay<T>>().toCommon(), 0)> : std::true_type {};

template<class, class = int>
struct HasSize : std::false_type {};

template<class T>
struct HasSize<T, decltype((void)std::declval<Decay<T>>().size(), 0)> : std::true_type {};

template<class, class = int>
struct HasSwapView : std::false_type {};

template<class T>
struct HasSwapView<T, decltype((void)std::declval<Decay<T>>().swapView(), 0)> : std::true_type {};

template<class, class = int>
struct HasInsert : std::false_type {};

template<class C>
struct HasInsert<
    C, decltype((void)std::declval<Decay<C>>().insert(std::declval<BeginIter<C>>(), std::declval<Decay<C>::value_type>()), 0)>
    : std::true_type {};

template<class V>
EnableIf<!std::is_same<BeginIter<V>, EndIter<V>>::value, View<std::reverse_iterator<BeginIter<V>>, EndIter<V>>>
reverse(V&& view) {
    auto begin = std::begin(view);
    begin.swapView();
    return embit::view(std::make_reverse_iterator(begin), std::end(view));
}

template<class V, class B = BeginIter<V>>
EnableIf<std::is_same<BeginIter<V>, EndIter<V>>::value, View<std::reverse_iterator<BeginIter<V>>, EndIter<V>>> reverse(V&& view) {
    return embit::view(std::reverse_iterator<B>(std::end(view)), std::reverse_iterator<B>(std::begin(view)));
}

#    ifndef __cpp_if_constexpr
template<class V>
constexpr EnableIf<!HasToCommon<BeginIter<V>>::value, V> toCommon(V&& view) {
    return view;
}

template<class V, class B = BeginIter<V>>
constexpr auto toCommon(V&& view) -> EnableIf<HasToCommon<B>::value, decltype(std::begin(view).toCommon())> {
    return std::begin(view).toCommon();
}

template<class I, class S>
constexpr EnableIf<IsRandomAccessIter<I>::value, IterDiffType<I>> distance(I first, S last) {
    return last - first;
}

template<class I, class S>
constexpr EnableIf<!IsRandomAccessIter<I>::value, IterDiffType<I>> distance(I first, S last) {
    IterDiffType<I> i;
    for (i = 0; first != last; ++i, ++first) {
    }
    return i;
}

template<class V, class I = BeginIter<V>>
constexpr EnableIf<IsRandomAccessView<V>::value || HasSize<V>::value, IterDiffType<I>> distance(V&& view) {
    return static_cast<IterDiffType<I>>(view.size());
}

template<class V, class I = BeginIter<V>>
constexpr EnableIf<!IsRandomAccessView<V>::value && !HasSize<V>::value, IterDiffType<I>> distance(V&& view) {
    return detail::distance(std::begin(view), std::end(view));
}

template<class T>
constexpr AddConst<T>& asConst(T& t) noexcept {
    return t;
}

template<class T>
constexpr AddConst<T>& asConst(T&& t) = delete;

template<class C>
EnableIf<HasInsert<C>::value, std::insert_iterator<C>> getOutputIterator(C& c) {
    return std::inserter(c, std::begin(c));
}

template<class C>
auto getOutputIterator(C& c) -> EnableIf<!HasInsert<C>::value, decltype(std::begin(c))> {
    return std::begin(c);
}

#    else
template<class V>
constexpr decltype(auto) toCommon(V&& view) {
    if constexpr (!std::is_same_v<BeginIter<V>, EndIter<V>>) {
        return std::begin(view).toCommon();
    }
    else {
        return view;
    }
}

template<class I, class S>
constexpr IterDiffType<I> distance(I first, S last) {
    if constexpr (IsRandomAccessIter<I>::value) {
        return last - first;
    }
    else {
        IterDiffType<I> i = 0;
        for (; first != last; ++i, ++first) {
        }
        return i;
    }
}

template<class V>
constexpr IterDiffType<BeginIter<V>> distance(V&& view) {
    using C = IterDiffType<BeginIter<V>>;
    if constexpr (IsRandomAccessView<V>::value || (HasSize<V>::value && !HasDistance<V>::value)) {
        return static_cast<C>(view.size());
    }
    else {
        return detail::distance(std::begin(view), std::end(view));
    }
}

template<class C>
auto getOutputIterator(C& c) {
    if constexpr (HasInsert<C>::value) {
        return std::inserter(c, std::begin(c));
    }
    else {
        return std::begin(c);
    }
}

template<class V>
auto reverse(V&& view) {
    using B = BeginIter<V>;
    using E = EndIter<V>;
    if constexpr (std::is_same_v<B, E>) {
        return embit::view(std::reverse_iterator<B>(std::end(view)), std::reverse_iterator<B>(std::begin(view)));
    }
    else {
        auto begin = std::begin(view);
        begin.swapView();
        return embit::view(std::make_reverse_iterator(begin), std::end(view));
    }
}
#    endif
} // namespace detail

template<class V>
auto reverse(V&& v) -> decltype(detail::reverse(v));

template<class V, class OutputStream, class Char = char>
void format(V&& v, OutputStream& stream, const Char* delimiter);

template<class Iterator, class S>
class View {
    Iterator _first{};
    S _last{};

public:
    using iterator = Iterator;
    using const_iterator = iterator;

private:
    using Traits = std::iterator_traits<Iterator>;

public:
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;

private:
#    ifdef __cpp_if_constexpr
    EMBIT_CONSTEXPR_CXX_17 reference backImpl() const {
        if constexpr (detail::HasBack<Iterator>::value) {
            return _first.back();
        }
        else {
            return *--_last;
        }
    }
#    else
    template<class I = Iterator>
    EMBIT_CONSTEXPR_CXX_20 detail::EnableIf<!detail::HasBack<I>::value, reference> backImpl() const {
        return *--_last;
    }

    template<class I = Iterator>
    EMBIT_CONSTEXPR_CXX_17 detail::EnableIf<detail::HasBack<I>::value, reference> backImpl() const {
        return _first.back();
    }
#    endif
public:
    constexpr View(Iterator first, S last) : _first(std::move(first)), _last(std::move(last)) {
    }

    View() = default;

    constexpr Iterator begin() const {
        return _first;
    }

    constexpr S end() const {
        return _last;
    }

    template<class OutputIterator>
    constexpr void copy(OutputIterator out) const {
        for (auto&& i : *this) {
            *out++ = i;
        }
    }

    template<class OutputIterator, class UnaryExpr>
    constexpr void transform(OutputIterator out, UnaryExpr&& f) const {
        for (auto&& i : *this) {
            *out++ = f(std::forward<decltype(i)>(i));
        }
    }

    template<class OutputIterator>
    constexpr void move(OutputIterator out) const {
        this->transform(out, [](reference r) { return std::move(r); });
    }

    template<class Container, class... ContainerArgs>
    constexpr Container collectAs(ContainerArgs&&... args) const {
        Container c(std::forward<ContainerArgs>(args)...);
        const auto outputIter = detail::getOutputIterator(c);
        copy(outputIter);
        return c;
    }

    template<class Container, class UnaryExpr, class... ContainerArgs>
    constexpr Container transformCollectAs(UnaryExpr&& expr, ContainerArgs&&... args) const {
        Container c(std::forward<ContainerArgs>(args)...);
        const auto outputIter = detail::getOutputIterator(c);
        this->transform(outputIter, std::forward<UnaryExpr>(expr));
        return c;
    }

    template<class T, class BinaryExpr>
    constexpr T foldl(BinaryExpr&& expr, T init) const {
        for (const auto& item : *this) {
            init = expr(std::move(init), item);
        }
        return init;
    }

    template<class T, class BinaryExpr>
    constexpr detail::Decay<T> foldr(BinaryExpr&& expr, T&& init) const {
        return embit::reverse(*this).foldl(std::forward<BinaryExpr>(expr), std::forward<T>(init));
    }

    constexpr reference front() const {
        return *_first;
    }

    constexpr reference back() const {
        static_assert(detail::IsBidirectionalIter<Iterator>::value, "View must be at least bidirectional to get last element");
        return backImpl();
    }

    constexpr bool empty() const {
        return _first == _last;
    }

    constexpr IterDiffType<Iterator> distance() const {
        return embit::distance(*this);
    }
};

template<class I, class S>
constexpr View<detail::Decay<I>, detail::Decay<S>> view(I&& first, S&& last) {
    return { std::forward<I>(first), std::forward<S>(last) };
}

template<class V>
constexpr View<BeginIter<V>, EndIter<V>> view(V&& view) {
    return embit::view(std::begin(view), std::end(view));
}

template<class V>
EMBIT_CONSTEXPR_CXX_17 View<BeginIter<V>, EndIter<V>> skip(V&& view, const IterDiffType<BeginIter<V>> offset) {
    return embit::view(std::next(std::begin(view), offset), offset);
}

template<class V>
constexpr IterDiffType<BeginIter<V>> distance(V&& view) {
    return detail::distance(view);
}

template<class I>
EMBIT_CONSTEXPR_CXX_17 bool operator==(const std::reverse_iterator<I>& it, DefaultSentinel s) noexcept {
    return it.base() == s;
}

template<class I>
EMBIT_CONSTEXPR_CXX_17 bool operator!=(const std::reverse_iterator<I>& it, DefaultSentinel s) noexcept {
    return !(it == s);
}

template<class V>
constexpr auto toCommon(V&& view) -> decltype(detail::toCommon(view)) {
    return detail::toCommon(view);
}

template<class V>
auto reverse(V&& view) -> decltype(detail::reverse(view)) {
    static_assert(detail::IsBidirectionalView<V>::value, "View must be bidirectional iterable");
    return detail::reverse(view);
}


} // namespace embit

#endif // EMBIT_VIEW_HPP
