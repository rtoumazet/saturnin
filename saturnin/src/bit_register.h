//
// bit_register.h
// Saturnin
//
// Copyright (c) 2023 Renaud Toumazet
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	bit_register.h
///
/// \brief	Register bits manipulation.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/utilities.h>

namespace saturnin {

// Forward declarations
template<typename MASKED_DATA, typename MASKED_CLASS>
class Masked;
template<typename REG_DATA, typename REG_CLASS>
class Reg;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Pos
///
/// \brief	Position in a register.
///
/// \author	Runik
/// \date	09/03/2023
///
/// \tparam	DATA 	Data type.
/// \tparam	CLASS	Class type.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DATA, typename CLASS>
class Pos {
  public:
    // Friends
    template<typename BITS_DATA, typename BITS_CLASS>
    friend class Bits;
    template<typename MASKED_DATA, typename MASKED_CLASS>
    friend class Masked;
    template<typename SHIFT_DATA, typename SHIFT_CLASS>
    friend class Shifted;
    template<typename ENUM_DATA, typename ENUM_CLASS, typename ENUM_ENUM>
    friend class Enum;
    template<typename REG_DATA, typename REG_CLASS>
    friend class Reg;

    using PosType = Pos<DATA, CLASS>;

    // Constructors
    explicit constexpr Pos() : pos_(static_cast<DATA>(0)) {}
    explicit constexpr Pos(const DATA pos) : pos_(pos) {}
    Pos(const PosType& other) = default;
    ~Pos()                    = default;

    // Reset
    auto operator()(const DATA pos) -> PosType& {
        pos_ = pos;
        return *this;
    }

    // Assignments
    auto operator=(const DATA data) -> PosType {
        pos_ = data;
        return *this;
    }

    // Accessor
    constexpr auto pos() const -> DATA { return pos_; }

    // Comparison
    constexpr auto operator==(const PosType other) -> bool { return other.pos_ == pos_; }

  protected:
    DATA pos_;

}; // template<typename DATA, typename CLASS> class Pos

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Bits
///
/// \brief	Single bit of a register.
///
/// \author	Runik
/// \date	09/03/2023
///
/// \tparam	DATA 	Data type.
/// \tparam	CLASS	Class type.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DATA, typename CLASS>
class Bits {
  public:
    // Friends
    template<typename MASKED_DATA, typename MASKED_CLASS>
    friend class Masked;
    template<typename REG_DATA, typename REG_CLASS>
    friend class Reg;

    using PosType    = Pos<DATA, CLASS>;
    using BitsType   = Bits<DATA, CLASS>;
    using MaskedType = Masked<DATA, CLASS>;
    using RegType    = Reg<DATA, CLASS>;

    // Constructors
    explicit constexpr Bits() : bits_(static_cast<DATA>(0)) {}
    explicit constexpr Bits(const DATA bits) : bits_(bits) {}
    constexpr Bits(const DATA bits, const PosType pos) : bits_(bits << pos.pos_) {}
    explicit Bits(const MaskedType mskd) : bits_(mskd.bits_) {}
    explicit Bits(const RegType& reg) : bits_(reg.data_) {}
    Bits(const BitsType& other) = default;
    ~Bits()                     = default;

    // Reset
    auto operator()(const DATA bits) -> BitsType& {
        bits_ = bits;
        return *this;
    }

    // Assignments
    auto operator=(const DATA bits) -> BitsType {
        bits_ = bits;
        return *this;
    }

    auto operator=(const MaskedType mskd) -> BitsType {
        bits_ = mskd.bits_;
        return *this;
    }

    auto operator=(const RegType& reg) -> BitsType {
        bits_ = reg.data_;
        return *this;
    }

    // Accessor
    constexpr auto bits() const -> DATA { return bits_; }

    // Bitwise operators
    constexpr auto operator|(const BitsType other) -> BitsType { return BitsType(bits_ | other.bits_); }
    constexpr auto operator|(const MaskedType& mskd) -> BitsType { return BitsType(bits_ | mskd.bits_); }

    auto operator|=(const BitsType other) -> BitsType {
        bits_ |= other.bits_;
        return *this;
    }

    constexpr auto operator|=(const MaskedType& mskd) -> BitsType {
        bits_ |= mskd.bits_;
        return *this;
    }

    constexpr auto operator-(const BitsType other) -> BitsType { return BitsType(bits_ & ~other.bits_); }

    constexpr auto operator-(const MaskedType& mskd) -> BitsType { return BitsType(bits_ & ~mskd.bits_); }

    auto operator-=(const BitsType other) -> BitsType {
        bits_ = bits & ~other.bits_;
        return *this;
    }

    constexpr auto operator-=(const MaskedType& mskd) -> BitsType {
        bits_ = bits & ~mskd.bits_;
        return *this;
    }

    // Comparisons
    constexpr bool operator==(const BitsType other) const { return other.bits_ == bits_; }

  protected:
    DATA bits_;

}; // template<typename DATA, typename CLASS> class Bits

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Masked
///
/// \brief	Continuous bit span of a register, including a mask.
///
/// \author	Runik
/// \date	09/03/2023
///
/// \tparam	DATA 	Data type.
/// \tparam	CLASS	Class type.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DATA, typename CLASS>
class Masked {
  public:
    // Friends
    template<typename BITS_DATA, typename BITS_CLASS>
    friend class Bits;
    template<typename REG_DATA, typename REG_CLASS>
    friend class Reg;

    using PosType    = Pos<DATA, CLASS>;
    using BitsType   = Bits<DATA, CLASS>;
    using MaskedType = Masked<DATA, CLASS>;
    using RegType    = Reg<DATA, CLASS>;

    // Constructors / destructors
    explicit constexpr Masked() : mask_(static_cast<DATA>(0)), bits_(static_cast<DATA>(0)) {}
    constexpr Masked(const DATA mask, const DATA bits, const PosType pos) : mask_(mask << pos.pos_), bits_(bits << pos.pos_) {}
    constexpr Masked(const DATA mask, const DATA bits) : mask_(mask), bits_(bits) {}
    explicit constexpr Masked(const BitsType bits) : mask_(bits.bits_), bits_(bits.bits_) {}
    Masked(const MaskedType& other) = default;
    ~Masked()                       = default;

    // Reset
    auto operator()(const DATA mask, const DATA bits) -> MaskedType& {
        mask_ = mask;
        bits_ = bits;
        return *this;
    }

    // Assignments
    auto operator=(const BitsType& bits) -> MaskedType& {
        mask_ = bits.bits_;
        bits_ = bits.bits_;
        return *this;
    }

    auto operator=(const RegType& reg) -> MaskedType& {
        bits_ = reg.data_;
        // mask_ isn't modified
        return *this;
    }

    // Accessors
    constexpr auto mask() const -> DATA { return mask_; }
    constexpr auto bits() const -> DATA { return bits_; }

    // Bitwise operators
    constexpr auto operator|(const MaskedType other) -> MaskedType {
        return MaskedType(mask_ | other.mask_, bits_ | other.bits_);
    }

    constexpr auto operator|(const BitsType bits) -> MaskedType { return MaskedType(mask_ | bits.bits_, bits_ | bits.bits_); }

    auto operator|=(const MaskedType other) -> MaskedType {
        mask_ |= other.mask_;
        bits_ |= other.bits_;
        return *this;
    }

    auto operator|=(const BitsType bits) -> MaskedType {
        mask_ |= bits.bits_;
        bits_ |= bits.bits_;
        return *this;
    }

    constexpr auto operator-(const MaskedType other) -> MaskedType {
        return MaskedType(mask_ & ~other.mask_, bits_ & ~other.bits_);
    }

    constexpr auto operator-(const BitsType bits) -> MaskedType { return MaskedType(mask_ & ~bits.bits_, bits_ & ~bits.bits_); }

    auto operator-=(const MaskedType other) -> MaskedType {
        mask_ = mask_ & ~other.mask_;
        bits_ = bits & ~other.bits_;
        return *this;
    }

    auto operator-=(const BitsType bits) -> MaskedType {
        mask_ = mask_ & ~bits.bits_;
        bits_ = mask_ & ~bits.bits_;
        return *this;
    }

    // Comparisons
    constexpr bool operator==(const MaskedType other) const { return other.mask_ == mask_ && other.bits_ == bits_; }
    constexpr bool operator<=>(const MaskedType other) const { return bits_ <=> other.bits_; }

  protected:
    DATA mask_;
    DATA bits_;

}; // template<typename DATA, typename CLASS> class Masked

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Shifted
///
/// \brief	Right shifting of bits of a register.
///
/// \author	Runik
/// \date	09/03/2023
///
/// \tparam	DATA 	Data type.
/// \tparam	CLASS	Class type.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DATA, typename CLASS>
class Shifted {
  public:
    // Friends
    template<typename REG_DATA, typename REG_CLASS>
    friend class Reg;

    using PosType     = Pos<DATA, CLASS>;
    using ShiftedType = Shifted<DATA, CLASS>;

    // Constructors / destructors
    explicit constexpr Shifted() : mask_(static_cast<DATA>(0)), pos_(static_cast<DATA>(0)) {}
    constexpr Shifted(const DATA mask, const PosType pos) : mask_(mask << pos.pos_), pos_(pos) {}
    Shifted(const ShiftedType& other) = default;
    ~Shifted()                        = default;

    // Reset
    auto operator()(const DATA mask, const DATA pos) -> ShiftedType& {
        mask_ = mask;
        pos_  = pos;
        return *this;
    }

    // Accessors
    auto mask() const -> u32 { return mask_; }
    auto pos() const -> PosType { return pos_; }

  protected:
    u32     mask_;
    PosType pos_;

}; // template<typename DATA, typename CLASS> class Shifted

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Enum
///
/// \brief	Right shifting of bits of a register to a scoped enum.
///
/// \author	Runik
/// \date	30/03/2023
///
/// \tparam	DATA 	Data type.
/// \tparam	CLASS	Class type.
/// \tparam	ENUM 	Scoped enum type.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DATA, typename CLASS, typename ENUM>
class Enum {
  public:
    // Friends
    template<typename REG_DATA, typename REG_CLASS>
    friend class Reg;

    using PosType  = Pos<DATA, CLASS>;
    using EnumType = Enum<DATA, CLASS, ENUM>;

    // Constructors / destructors
    explicit constexpr Enum() : mask_(static_cast<DATA>(0)), pos_(static_cast<DATA>(0)) {}
    constexpr Enum(const DATA mask, const PosType pos) : mask_(mask << pos.pos_), pos_(pos) {}
    Enum(const EnumType& other) = default;
    ~Enum()                     = default;

    // Reset
    auto operator()(const DATA mask, const DATA pos) -> EnumType& {
        mask_ = mask;
        pos_  = pos;
        return *this;
    }

    // Accessors
    auto mask() const -> u32 { return mask_; }
    auto pos() const -> PosType { return pos_; }

  protected:
    u32     mask_;
    PosType pos_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Reg
///
/// \brief	A register.
///
/// \author	Runik
/// \date	09/03/2023
///
/// \tparam	DATA 	Data type.
/// \tparam	CLASS	Class type.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DATA, typename CLASS>
class Reg {
  public:
    // Friends
    template<typename BITS_DATA, typename BITS_CLASS>
    friend class Bits;
    template<typename MASKED_DATA, typename MASKED_CLASS>
    friend class Masked;

    using BitsType   = Bits<DATA, CLASS>;
    using MaskedType = Masked<DATA, CLASS>;
    using RegType    = Reg<DATA, CLASS>;

    // Constructors
    Reg<DATA, CLASS>() = default;
    explicit constexpr Reg<DATA, CLASS>(const DATA data) : data_(data) {}
    explicit constexpr Reg<DATA, CLASS>(const BitsType bits) : data_(bits.bits()) {}
    explicit constexpr Reg<DATA, CLASS>(const MaskedType mskd) : data_(mskd.bits()) {}
    Reg(const RegType& other) = default;
    ~Reg()                    = default;

    // Zeroing all bits
    static constexpr auto zero() -> BitsType { return BitsType(0); }

    // Assignments
    void operator=(const DATA data) { data_ = data; }

    void operator=(const BitsType bits) { data_ = bits.bits_; }
    void wrt(const BitsType bits) { data_ = bits.bits_; }

    void operator=(const MaskedType& mskd) { data_ = mskd.bits_; }
    void wrt(const MaskedType& mskd) { data_ = mskd.bits_; }

    // Accessors
    auto data() const -> DATA { return data_; }

    // Bitwise operators
    void operator|=(const BitsType bits) { data_ |= bits.bits_; }
    void set(const BitsType bits) { data_ |= bits.bits_; }

    void operator|=(const MaskedType& mskd) { data_ |= mskd.bits_; }
    void set(const MaskedType& mskd) { data_ |= mskd.bits_; }

    void operator-=(const BitsType bits) { data_ &= ~bits.bits_; }
    void clr(const BitsType bits) { data_ &= ~bits.bits_; }

    void operator-=(const MaskedType& mskd) { data_ &= ~mskd.bits_; }
    void clr(const MaskedType& mskd) { data_ &= ~mskd.bits_; }

    void operator^=(const BitsType bits) { data_ ^= bits.bits_; }
    void flp(const BitsType bits) { data_ ^= bits.bits_; }

    void operator^=(const MaskedType& mskd) { data_ ^= mskd.bits_; }
    void flp(const MaskedType& mskd) { data_ ^= mskd.bits_; }

    void operator/=(const MaskedType& mskd) { data_ = (data_ & ~mskd.mask_) | mskd.bits_; }
    void upd(const MaskedType& mskd) { data_ = (data_ & ~mskd.mask_) | mskd.bits_; }
    template<typename ENUM>
    void upd(const Enum<DATA, CLASS, ENUM> e, const ENUM v) {
        static_assert(std::is_enum_v<ENUM>);
        data_ = (data_ & ~e.mask_) | (utilities::toUnderlying(v) << e.pos_.pos_);
    }

    // Extractors
    auto operator&(BitsType bits) const -> BitsType { return BitsType(data_ & bits.bits_); }
    auto operator&(MaskedType mskd) const -> MaskedType { return MaskedType(data_ & mskd.mask_, mskd.mask_); }
    auto operator>>(const Shifted<DATA, CLASS> shft) const -> DATA { return (data_ & shft.mask_) >> shft.pos_.pos_; }
    auto shifted(const Shifted<DATA, CLASS> shft) const -> DATA { return (data_ & shft.mask_) >> shft.pos_.pos_; }
    template<typename ENUM>
    auto operator>>(const Enum<DATA, CLASS, ENUM> shft) const -> ENUM {
        static_assert(std::is_enum_v<ENUM>);
        return utilities::toEnum<ENUM>((data_ & shft.mask_) >> shft.pos_.pos_);
    }
    template<typename ENUM>
    auto shifted(const Enum<DATA, CLASS, ENUM> shft) const -> ENUM {
        static_assert(std::is_enum_v<ENUM>);
        return utilities::toEnum<ENUM>((data_ & shft.mask_) >> shft.pos_.pos_);
    }

    //  Comparisons
    bool is(const DATA data) const { return data == data_; }
    bool is(const BitsType bits) const { return data_ == bits.bits_; }
    bool is(const MaskedType mskd) const { return data_ == mskd.bits_; }

    bool all(const DATA data) const { return (data & data_) == data_; }
    bool all(const BitsType bits) const { return (data_ & bits.bits_) == bits.bits_; }
    bool all(const BitsType mask, const BitsType bits) const { return (data_ & mask.bits_) == bits.bits_; }
    bool all(const MaskedType mskd) const { return (data_ & mskd.mask_) == mskd.bits_; }

    bool any(const DATA data) const { return static_cast<bool>(data & data_); }
    bool any(const BitsType bits) const { return static_cast<bool>(data_ & bits.bits_); }

    bool operator<(const MaskedType& mskd) const { return (data_ & mskd.mask_) < mskd.bits_; }
    bool operator<=(const MaskedType& mskd) const { return (data_ & mskd.mask_) <= mskd.bits_; }
    bool operator>(const MaskedType& mskd) const { return (data_ & mskd.mask_) > mskd.bits_; }
    bool operator>=(const MaskedType& mskd) const { return (data_ & mskd.mask_) >= mskd.bits_; }

  protected:
    DATA data_;

}; // template<typename BITS, typename MASKED> class Reg

// Macro for generating functions returning Bits.
// PosType and BitsType must have been 'using' defined in the class.
#define GENERATE_BITS_RANGE(CLASS, CONSTEXPR_NAME, RUNTIME_NAME, DATA)                                     \
    template<unsigned BIT_NUM>                                                                             \
    static constexpr BitsType CONSTEXPR_NAME() {                                                           \
        static_assert(BIT_NUM < sizeof(DATA) * 8, CLASS "::" #CONSTEXPR_NAME "<BIT_NUM> is out of range"); \
        return BitsType(1, pos_t(BIT_NUM));                                                                \
    }                                                                                                      \
                                                                                                           \
    static const BitsType RUNTIME_NAME(const unsigned bit_num) { return BitsType(1, PosType(bit_num)); }   \
                                                                                                           \
    static bool RUNTIME_NAME##_valid(const unsigned bit_num) { return bit_num < sizeof(DATA) * 4; }

// Macro for generating functions returning Masked.
// ShiftedType and MaskedType must have been 'using' defined in the class.
#define GENERATE_MASKED_RANGE(CLASS, CONSTEXPR_NAME, RUNTIME_NAME, MASK, POS, LIMIT)                                       \
    static constexpr ShiftedType CONSTEXPR_NAME##_SHFT = ShiftedType(MASK, POS);                                           \
                                                                                                                           \
    template<unsigned BITS>                                                                                                \
    static constexpr MaskedType CONSTEXPR_NAME() {                                                                         \
        static_assert(BITS <= (LIMIT), CLASS "::" #CONSTEXPR_NAME "<BITS> is out of range");                               \
        return MaskedType(MASK, BITS, POS);                                                                                \
    }                                                                                                                      \
                                                                                                                           \
    static const MaskedType RUNTIME_NAME(const unsigned bits) { return MaskedType(MASK << POS.pos(), bits << POS.pos()); } \
                                                                                                                           \
    static bool RUNTIME_NAME##_valid(const unsigned bits) { return bits <= (LIMIT); }

#define GENERATE_BIT_WITH_ENUM(NAME, POS, MASK, SCOPED_ENUM)                  \
    static constexpr auto NAME##_pos  = PosType(POS);                         \
    static constexpr auto NAME        = BitsType(1, NAME##_pos);              \
    static constexpr auto NAME##_mask = MASK;                                 \
    static constexpr auto NAME##_shft = ShiftedType(NAME##_mask, NAME##_pos); \
    static constexpr auto NAME##_enum = EnumType<SCOPED_ENUM>(NAME##_mask, NAME##_pos);

#define GENERATE_BIT_WITHOUT_ENUM(NAME, POS, MASK)               \
    static constexpr auto NAME##_pos  = PosType(POS);            \
    static constexpr auto NAME        = BitsType(1, NAME##_pos); \
    static constexpr auto NAME##_mask = MASK;                    \
    static constexpr auto NAME##_shft = ShiftedType(NAME##_mask, NAME##_pos);

#define GENERATE_USING(NAME, TYPE)           \
    using PosType     = Pos<TYPE, NAME>;     \
    using MaskedType  = Masked<TYPE, NAME>;  \
    using BitsType    = Bits<TYPE, NAME>;    \
    using ShiftedType = Shifted<TYPE, NAME>; \
    template<typename E>                     \
    using EnumType = Enum<TYPE, NAME, E>;

} // namespace saturnin
