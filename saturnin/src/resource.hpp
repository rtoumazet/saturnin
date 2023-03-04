#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <span>   // span
#include <string> // string
#include <tuple>
#include <vector>
#include <saturnin/src/emulator_defs.h> // u8, u16, u32

class Resource {
  public:
    template<typename T>
    using span         = std::span<T>;
    using EmbeddedData = span<const std::uint8_t>;

  private:
    const EmbeddedData arr_view;
    const std::string  path;

  public:
    Resource() = default;
    template<typename Container>
    Resource(const Container& arr_, std::string path_) noexcept : arr_view(arr_), path(std::move(path_)) {}

    auto GetArray() const noexcept { return arr_view; }

    auto& GetPath() const noexcept { return path; }
};
