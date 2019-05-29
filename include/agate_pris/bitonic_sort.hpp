#ifndef AGATE_PRIS_BITONIC_SORT_HPP
#define AGATE_PRIS_BITONIC_SORT_HPP

#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/subrange.hpp>

#include <boost/container/small_vector.hpp>
#include <boost/container/vector.hpp>
#include <boost/optional.hpp>

#include <functional>
#include <type_traits>

namespace agate_pris
{
    namespace bitonic_sort_detail
    {
        template < typename It, typename SizeType, typename Compare >
        void f(It const& begin, SizeType const& distance, SizeType const& coverage, Compare const& comp)
        {
            // Compare stepped element and swap ascending or descending
            auto initial_comparison_step = coverage / 4;
            auto comparison_step = initial_comparison_step;

            while (comparison_step > 0)
            {
                // Swap ascending
                for (auto j = 0; j < (coverage / 2) / (comparison_step * 2); ++j)
                {
                    for (SizeType i = 0; i < comparison_step; ++i)
                    {
                        if (j * (comparison_step * 2) + i + comparison_step >= distance)
                            break;
                        auto const lhs = ranges::next(begin, j * (comparison_step * 2) + i);
                        auto const rhs = ranges::next(begin, j * (comparison_step * 2) + i + comparison_step);
                        if (std::logical_not<> {}(comp(*lhs, *rhs)))
                        {
                            if ((*lhs) && (*rhs))
                            {
                                using std::swap;
                                swap(**lhs, **rhs);
                            }
                            else
                            {
                                using std::iter_swap;
                                iter_swap(lhs, rhs);
                            }
                        }
                    }
                }

                // Swap descending
                for (auto j = 0; j < (coverage / 2) / (comparison_step * 2); ++j)
                {
                    for (SizeType i = 0; i < comparison_step; ++i)
                    {
                        if (j * (comparison_step * 2) + coverage / 2 + i + comparison_step >= distance)
                            break;
                        auto const lhs = ranges::next(begin, j * (comparison_step * 2) + coverage / 2 + i);
                        auto const rhs = ranges::next(begin, j * (comparison_step * 2) + coverage / 2 + i + comparison_step);
                        if (comp(*lhs, *rhs))
                        {
                            if ((*lhs) && (*rhs))
                            {
                                using std::swap;
                                swap(**lhs, **rhs);
                            }
                            else
                            {
                                using std::iter_swap;
                                iter_swap(lhs, rhs);
                            }
                        }
                    }
                }

                comparison_step /= 2;
            }
        }
    }

    template < typename Range, typename Compare >
    void bitonic_sort_by(Range& range, Compare&& comp)
    {
        using size_type_t = ranges::size_type_t< Range >;
        using value_t = ranges::value_type_t< Range >;

        if (ranges::empty(range))
            return;

        auto v = [&range] {
            using v_t = boost::container::small_vector< std::add_pointer_t< value_t >, 64 >;
            using s_t = typename v_t::size_type;
            auto const distance = ranges::distance(range);
            auto size = s_t { 1 };
            while (size < distance)
                size *= 2;

            auto v = boost::container::vector< std::add_pointer_t< value_t > > { size };
            auto it = ranges::begin(range);
            auto i = s_t { 0 };
            while (it != ranges::end(range))
            {
                v[i] = &*it;
                ++it;
                ++i;
            }
            return v;
        }();

        auto coverage = size_type_t { 4 };
        do
        {
            auto new_comp = [comp](auto const lhs, auto const rhs) {
                if (!lhs)
                    return false;
                if (!rhs)
                    return true;
                return comp(*lhs, *rhs);
            };
            for (size_type_t i = 0; i < v.size(); i += coverage)
            {
                auto const b = ranges::next(ranges::begin(v), i);
                auto const d = i + coverage > v.size() ? v.size() - i : coverage;
                bitonic_sort_detail::f(b, d, coverage, new_comp);
            }
            if (coverage / 2 >= v.size())
                break;
            coverage *= 2;
        } while (true);
    }
}

#endif
