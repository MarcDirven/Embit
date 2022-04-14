#pragma once

#ifndef EMBIT_MAP_HPP
#    define EMBIT_MAP_HPP

#    include "FunctionWrapper.hpp"

namespace embit {
namespace detail {
template<class Iterator, class Sentinel, class Func>
class MapIterator {
    Iterator _iterator{};
    Sentinel _last{};
    mutable detail::FunctionWrapper<Func> _f{};

    using Traits = std::iterator_traits<Iterator>;

public:
    using reference = decltype(_f(*_iterator));
    using value_type = typename std::remove_reference<reference>::type;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::pointer;

    constexpr MapIterator(Iterator current, Sentinel last, Func f) :
        _iterator(std::move(current)),
        _last(std::move(last)),
        _f(std::move(f)) {
    }

    MapIterator() = default;

    constexpr reference operator*() const {
        return _f(*_iterator);
    }

    constexpr MapIterator& operator++() {
        ++_iterator;
        return *this;
    }

    constexpr MapIterator operator++(int) {
        auto tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr MapIterator& operator--() {
        --_iterator;
        return *this;
    }

    constexpr MapIterator operator--(int) {
        auto tmp(*this);
        --*this;
        return tmp;
    }

    constexpr friend bool operator==(const MapIterator& a, DefaultSentinel) noexcept {
        return a._iterator == a._last;
    }

    constexpr friend bool operator!=(const MapIterator& a, DefaultSentinel s) noexcept {
        return !(a == s);
    }

    constexpr friend bool operator==(const MapIterator& a, const MapIterator& b) noexcept {
        return a._iterator == b._iterator;
    }

    constexpr friend bool operator!=(const MapIterator& a, const MapIterator& b) noexcept {
        return !(a == b);
    }

#    ifdef __cpp_if_constexpr
    EMBIT_CONSTEXPR_CXX_20 void swapView() {
        if constexpr (!HasSwapView<Iterator>::value) {
            std::swap(_iterator, _last);
        }
        else {
            _iterator.swapView();
        }
    }

    EMBIT_CONSTEXPR_CXX_17 reference back() const {
        if constexpr (std::is_same_v<Iterator, Sentinel>) {
            return _f(*std::prev(_last));
        }
        else {
            return _iterator.back();
        }
    }

    constexpr auto toCommon() const {
        using It = MapIterator<Iterator, Iterator, Func>;
        if constexpr (std::is_same_v<Iterator, Sentinel>) {
            return view(It(_iterator, _last, _f.get()), It(_last, _last, _f.get()));
        }
        else {
            auto v = _iterator.toCommon();
            auto begin = std::begin(v);
            auto end = std::end(v);
            return view(It(std::move(begin), end, _f.get()), It(end, end, _f.get()));
        }
    }
#    else
    template<class I = Iterator>
    EnableIf<!HasSwapView<I>::value> swapView() {
        std::swap(_iterator, _last);
    }

    template<class I = Iterator>
    EnableIf<HasSwapView<I>::value> swapView() {
        _iterator.swapView();
    }

    template<class I = Iterator>
    EMBIT_CONSTEXPR_CXX_17 EnableIf<!HasBack<I>::value, reference> back() const {
        return _f(*std::prev(_last));
    }

    template<class I = Iterator>
    EMBIT_CONSTEXPR_CXX_17 EnableIf<HasBack<I>::value, reference> back() const {
        return _iterator.back();
    }

    template<class I = Iterator>
    constexpr EnableIf<!HasToCommon<I>::value, View<MapIterator<I, I, Func>, MapIterator<I, I, Func>>>
    toCommon() const {
        using It = MapIterator<I, I, Func>;
        return view(It(_iterator, _last, _f.get()), It(_last, _last, _f.get()));
    }

    template<class I = Iterator>
    constexpr EnableIf<HasToCommon<I>::value, View<MapIterator<I, I, Func>, MapIterator<I, I, Func>>>
    toCommon() const {
        using It = MapIterator<I, I, Func>;
        auto v = _iterator.toCommon();
        auto begin = std::begin(v);
        auto end = std::end(v);
        return embit::view(It(std::move(begin), end, _f.get()), It(end, end, _f.get()));
    }
#    endif
};
} // namespace detail

template<class Iterator, class Sentinel, class Func>
class MapView : public View<detail::MapIterator<Iterator, Sentinel, Func>, DefaultSentinel> {
public:
    using iterator = detail::MapIterator<Iterator, Sentinel, Func>;
    using const_iterator = iterator;

    constexpr MapView(Iterator first, Sentinel last, Func func) :
        View<iterator, DefaultSentinel>(iterator(std::move(first), std::move(last), std::move(func)), defaultSentinel) {
    }

    MapView() = default;
};

template<class V, class Func>
constexpr MapView<BeginIter<V>, EndIter<V>, detail::Decay<Func>> map(V&& view, Func&& f) {
    return { std::begin(view), std::end(view), std::forward<Func>(f) };
}
} // namespace embit
#endif