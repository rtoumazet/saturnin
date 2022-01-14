////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	bitfield.h
///
/// \brief	Bitfield declarations.
///
/// Original code by Evan Teran : https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <cstddef>
#include <type_traits>

namespace saturnin {

namespace {

template<size_t LastBit>
struct MinimumTypeHelper {
    typedef typename std::conditional<
        LastBit == 0,
        void,
        typename std::conditional<
            LastBit <= 8,
            u8,
            typename std::conditional<
                LastBit <= 16,
                u16,
                typename std::conditional<LastBit <= 32, u32, typename std::conditional<LastBit <= 64, u64, void>::type>::type>::
                type>::type>::type type;
};

} // namespace

template<size_t Index, size_t Bits = 1>
class BitField {
  private:
    enum { Mask = (1u << Bits) - 1u };

    typedef typename MinimumTypeHelper<Index + Bits>::type T;

  public:
    template<class T2>
    BitField& operator=(T2 value) {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

              operator T() const { return (value_ >> Index) & Mask; }
    explicit  operator bool() const { return value_ & (Mask << Index); }
    BitField& operator++() { return *this = *this + 1; }
    T         operator++(int) {
        T r = *this;
        ++*this;
        return r;
    }
    BitField& operator--() { return *this = *this - 1; }
    T         operator--(int) {
        T r = *this;
        --*this;
        return r;
    }
    BitField& operator&=(const T& data) { return *this = *this & data; }
    BitField& operator|=(const T& data) { return *this = *this | data; }
    BitField& operator^=(const T& data) { return *this = *this ^ data; }

  private:
    T value_;
};

template<size_t Index>
class BitField<Index, 1> {
  private:
    enum { Bits = 1, Mask = 0x01 };

    typedef typename MinimumTypeHelper<Index + Bits>::type T;

  public:
    BitField& operator=(bool value) {
        value_ = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    explicit operator bool() const { return value_ & (Mask << Index); }

  private:
    T value_;
};

} // namespace saturnin
