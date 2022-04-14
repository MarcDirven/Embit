#pragma once

#ifndef EMBIT_CONCAT_HPP
#    define EMBIT_CONCAT_HPP

#    include "Core.hpp"

namespace embit {
namespace detail {
template<class I1, class S1, class I2, class S2>
class ConcatIterator {
    I1 _iter1{};
    S1 _last1{};
    I2 _iter2{};
    S2 _last2{};

    using Traits = std::iterator_traits<I1>;

public:
    using value_type = typename Traits::value_type;
    using reference = typename Traits::reference;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::pointer;
    using iterator_category = std::forward_iterator_tag;

    constexpr ConcatIterator(I1 first1, S1 last1, I2 first2, S2 last2) :
        _iter1(std::move(first1)),
        _last1(std::move(last1)),
        _iter2(std::move(first2)),
        _last2(std::move(last2)) {
    }

    ConcatIterator() = default;

    constexpr reference operator*() const {
        if (_iter1 == _last1) {
            return *_iter2;
        }
        return *_iter1;
    }

    constexpr ConcatIterator& operator++() {
        if (_iter1 == _last1) {
            ++_iter2;
        }
        else {
            ++_iter1;
        }
        return *this;
    }

    constexpr ConcatIterator operator++(int) {
        auto tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr friend bool operator==(const ConcatIterator& a, DefaultSentinel) noexcept {
        return a._iter2 == a._last2;
    }

    constexpr friend bool operator!=(const ConcatIterator& a, DefaultSentinel s) noexcept {
        return !(a == s);
    }

    constexpr friend bool operator==(const ConcatIterator& a, const ConcatIterator& b) noexcept {
        if (a._iter1 != b._last1) {
            return false;
        }
        return a._iter2 == b._iter2;
    }

    constexpr friend bool operator!=(const ConcatIterator& a, const ConcatIterator& b) noexcept {
        return !(a == b);
    }
};
} // namespace detail

template<class I1, class S1, class I2, class S2>
class ConcatView;

template<class V1, class V2>
constexpr ConcatView<BeginIter<V1>, EndIter<V1>, BeginIter<V2>, EndIter<V2>> concat(V1&& view1, V2&& view2);

template<class I1, class S1, class I2, class S2>
class ConcatView : public embit::View<detail::ConcatIterator<I1, S1, I2, S2>, DefaultSentinel> {
public:
    using iterator = detail::ConcatIterator<I1, S1, I2, S2>;
    using const_iterator = iterator;

    constexpr ConcatView(I1 first1, S1 last1, I2 first2, S2 last2) :
        View<iterator, DefaultSentinel>(iterator(std::move(first1), std::move(last1), std::move(first2), std::move(last2)),
                                        defaultSentinel) {
    }

    ConcatView() = default;

    template<class V>
    constexpr ConcatView<iterator, DefaultSentinel, BeginIter<V>, EndIter<V>> concat(V&& view) const {
        return embit::concat(*this, view);
    }

    template<class V>
    constexpr auto constcat(V&& view) const -> decltype(concat(detail::asConst(view))) {
        return concat(detail::asConst(view));
    }
};

template<class V1, class V2>
constexpr ConcatView<BeginIter<V1>, EndIter<V1>, BeginIter<V2>, EndIter<V2>> concat(V1&& view1, V2&& view2) {
    static_assert(std::is_same<ViewValueType<V1>, ViewValueType<V2>>::value, "value types do not match");
    return { std::begin(view1), std::end(view1), std::begin(view2), std::end(view2) };
}

template<class V1, class V2>
constexpr auto constcat(V1&& view1, V2&& view2) -> decltype(concat(detail::asConst(view1), detail::asConst(view2))) {
    return concat(detail::asConst(view1), detail::asConst(view2));
}
} // namespace embit

#endif // EMBIT_CONCAT_HPP
