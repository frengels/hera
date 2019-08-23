#pragma once

namespace hera
{
template<typename T>
concept metafunction = requires
{
    typename T::type;
}
&&std::is_trivial_v<T>&& std::is_empty_v<T>;
} // namespace hera