#pragma once

#ifndef EMBIT_CHAIN_HPP
#    define EMBIT_CHAIN_HPP

#    include "CString.hpp"
#    include "Core.hpp"
#    include "Filter.hpp"
#    include "Map.hpp"
#    include "Take.hpp"

namespace embit {
template<class, class>
class ChainView;

template<class V>
constexpr auto chain(V&& view) -> ChainView<decltype(std::begin(view)), decltype(std::end(view))>;

template<class Iterator, class Sentinel>
class ChainView : public View<Iterator, Sentinel> {
    using Base = View<Iterator, Sentinel>;

public:
    using iterator = typename Base::iterator;
    using const_iterator = iterator;

    constexpr ChainView(Iterator first, Sentinel last) : View<Iterator, Sentinel>(std::move(first), std::move(last)) {
    }

    ChainView() = default;

    template<class Func>
    constexpr ChainView<detail::FilterIterator<Iterator, Sentinel, Func>, DefaultSentinel> filter(Func f) const {
        return chain(embit::filter(*this, f));
    }

    template<class Func>
    constexpr ChainView<detail::MapIterator<Iterator, Sentinel, Func>, DefaultSentinel> map(Func f) const {
        return chain(embit::map(*this, f));
    }

    constexpr ChainView<detail::TakeIterator<Iterator, Sentinel>, DefaultSentinel>
    take(const IterDiffType<Iterator> amount) const {
        return chain(embit::take(*this, amount));
    }

    constexpr ChainView<detail::TakeIterator<Iterator, Sentinel>, DefaultSentinel>
    slice(const IterDiffType<Iterator> from, const IterDiffType<Iterator> to) const {
        return chain(embit::slice(*this, from, to));
    }

    constexpr ChainView<Iterator, DefaultSentinel> skip(const IterDiffType<Iterator> amount) const {
        return chain(embit::skip(*this, amount));
    }

    template<class V = ChainView<Iterator, Sentinel>, class B = BeginIter<V>, class E = EndIter<V>>
    detail::EnableIf<!std::is_same<B, E>::value, View<std::reverse_iterator<B>, E>> reverse() const {
        return chain(embit::reverse(*this));
    }

    template<class V = ChainView<Iterator, Sentinel>, class B = BeginIter<V>, class E = EndIter<V>>
    detail::EnableIf<std::is_same<B, E>::value, View<std::reverse_iterator<B>, std::reverse_iterator<B>>> reverse(V&& view) const {
        return chain(embit::reverse(*this));
    }

    template<class UnaryOp>
    constexpr const ChainView<Iterator, Sentinel>& forEach(UnaryOp op) const {
        for (auto&& val : *this) {
            op(std::forward<decltype(val)>(val));
        }
        return *this;
    }
};

template<class V>
constexpr auto chain(V&& view) -> ChainView<decltype(std::begin(view)), decltype(std::end(view))> {
    return { std::begin(view), std::end(view) };
}
} // namespace embit

#endif // EMBIT_CHAIN_HPP
