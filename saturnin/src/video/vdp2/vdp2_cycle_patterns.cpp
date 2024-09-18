//
// vdp2_cycle_patterns.cpp
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/vdp2/vdp2.h>

namespace saturnin::video {

using core::tr;

//--------------------------------------------------------------------------------------------------------------
// VRAM CYCLE PATTERNS methods
//--------------------------------------------------------------------------------------------------------------

auto Vdp2::isScreenDisplayed(ScrollScreen s) -> bool {
    // First check to ensure scroll screen must be displayed. If the screen cannot display, no vram access will be performed.

    // For normal scroll screens (NBG0 to NBG3) :
    // - in cell display format, required image data is pattern name data and character pattern data
    // - in bitmap format, required image data is bitmap pattern data (same as character pattern data)
    //
    // Pattern name data (PND) read access during 1 cycle must be set to a maximum of 2 banks, one being either VRAM-A0 or
    // VRAM-B0, the other being VRAM-A1 or VRAM-B1.
    // When the VRAM is not divided in 2 partitions, VRAM-A0 is used as VRAM-A and VRAM-B0 as VRAM-B.
    // Selectable timing in normal mode, :
    // - TO-T7 in bank VRAM-A0 or VRAM-B0, TO-T7 in bank VRAM-A1 or VRAM-B1 (case where both banks are splitted)
    // - TO-T7 in bank VRAM-A or VRAM-B (case where no bank is splitted)
    // In Hi-Res or Exclusive mode, selectable timing is reduced to T0-T3, the bank access is identical.

    using Bgon   = Vdp2Regs::Bgon;
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Zmctl  = Vdp2Regs::Zmctl;

    getScreen(s).is_display_enabled = false;

    switch (s) {
        using enum ScrollScreen;
        case nbg0: {
            if ((regs_.bgon >> Bgon::n0on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }

            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(regs_.zmctl >> Zmctl::n0zmqt_enum, regs_.zmctl >> Zmctl::n0zmhf_enum);

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if ((regs_.chctla >> Chctla::n0bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = regs_.chctla >> Chctla::n0chcn_enum;
                const auto required_bpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_bdp_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_bdp_reads < required_bpd_reads) { return false; }
            } else {
                // Character format needs character pattern data (cpd) and pattern name data (pnd).
                const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
                const auto current_pnd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg0_pattern_name_read, reduction);
                if (current_pnd_reads < required_pnd_reads) { return false; }

                const auto color_number = regs_.chctla >> Chctla::n0chcn_enum;
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }
            break;
        }
        case nbg1: {
            if ((regs_.bgon >> Bgon::n1on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(regs_.zmctl >> Zmctl::n1zmqt_enum, regs_.zmctl >> Zmctl::n1zmhf_enum);

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if ((regs_.chctla >> Chctla::n1bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = regs_.chctla >> Chctla::n1chcn_enum;
                const auto required_bpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_bdp_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_bdp_reads < required_bpd_reads) { return false; }
            } else {
                // Character format needs character pattern data (cpd) and pattern name data (pnd).
                const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
                const auto current_pnd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg1_pattern_name_read, reduction);
                if (current_pnd_reads < required_pnd_reads) { return false; }

                const auto color_number = regs_.chctla >> Chctla::n1chcn_enum;
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }

            break;
        }
        case nbg2: {
            if ((regs_.bgon >> Bgon::n2on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }

            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg2_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = regs_.chctlb >> Chctlb::n2chcn_enum;
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads
                = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg2_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }

            break;
        }
        case nbg3: {
            if ((regs_.bgon >> Bgon::n3on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg3_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = regs_.chctlb >> Chctlb::n3chcn_enum;
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads
                = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg3_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }
            break;
        }
        case rbg0: {
            if ((regs_.bgon >> Bgon::r0on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            core::Log::unimplemented(core::tr("VDP2 RBG0 display"));
            break;
        }
        case rbg1: {
            if ((regs_.bgon >> Bgon::r1on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            core::Log::unimplemented(core::tr("VDP2 RBG1 display"));
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Scroll screen not set"));
        }
    }

    getScreen(s).is_display_enabled = true;

    return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::isScreenDisplayLimitedByReduction(ScrollScreen s) const -> bool {
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Zmctl  = Vdp2Regs::Zmctl;

    switch (s) {
        using enum ScrollScreen;
        case nbg2: {
            const auto reduction    = getReductionSetting(regs_.zmctl >> Zmctl::n0zmqt_enum, regs_.zmctl >> Zmctl::n0zmhf_enum);
            const auto color_number = regs_.chctla >> Chctla::n0chcn_enum;

            if ((reduction == ReductionSetting::up_to_one_quarter)
                && (color_number == Vdp2Regs::CharacterColorNumber3Bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half)
                && (color_number == Vdp2Regs::CharacterColorNumber3Bits::palette_256)) {
                return true;
            }
            break;
        }
        case nbg3: {
            const auto reduction    = getReductionSetting(regs_.zmctl >> Zmctl::n1zmqt_enum, regs_.zmctl >> Zmctl::n1zmhf_enum);
            const auto color_number = regs_.chctla >> Chctla::n1chcn_enum;

            if ((reduction == ReductionSetting::up_to_one_quarter)
                && (color_number == Vdp2Regs::CharacterColorNumber2Bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half)
                && (color_number == Vdp2Regs::CharacterColorNumber2Bits::palette_256)) {
                return true;
            }
            break;
        }
        default: return false;
    }
    return false;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramAccessByCommand(const Vdp2Regs::VramAccessCommand command, const ReductionSetting reduction) const -> u8 {
    using Tvmd   = Vdp2Regs::Tvmd;
    using Cycxxl = Vdp2Regs::Cycxxl;
    using Cycxxu = Vdp2Regs::Cycxxu;

    auto is_normal_mode = ((regs_.tvmd >> Tvmd::hreso_enum) == Tvmd::HorizontalResolution::normal_320);
    is_normal_mode |= ((regs_.tvmd >> Tvmd::hreso_enum) == Tvmd::HorizontalResolution::normal_352);

    VramTiming bank_a0 = {regs_.cyca0l >> Cycxxl::t0_enum,
                          regs_.cyca0l >> Cycxxl::t1_enum,
                          regs_.cyca0l >> Cycxxl::t2_enum,
                          regs_.cyca0l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    VramTiming bank_a1 = {regs_.cyca1l >> Cycxxl::t0_enum,
                          regs_.cyca1l >> Cycxxl::t1_enum,
                          regs_.cyca1l >> Cycxxl::t2_enum,
                          regs_.cyca1l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    VramTiming bank_b0 = {regs_.cycb0l >> Cycxxl::t0_enum,
                          regs_.cycb0l >> Cycxxl::t1_enum,
                          regs_.cycb0l >> Cycxxl::t2_enum,
                          regs_.cycb0l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    VramTiming bank_b1 = {regs_.cycb1l >> Cycxxl::t0_enum,
                          regs_.cycb1l >> Cycxxl::t1_enum,
                          regs_.cycb1l >> Cycxxl::t2_enum,
                          regs_.cycb1l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;

    switch (command) {
        using enum Vdp2Regs::VramAccessCommand;
        case nbg0_character_pattern_data_read: {
            if ((regs_.chctla >> Chctla::n0bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = regs_.chctla >> Chctla::n0chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg1_character_pattern_data_read: {
            if ((regs_.chctla >> Chctla::n1bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = regs_.chctla >> Chctla::n1chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg2_character_pattern_data_read: {
            const auto cp_size = regs_.chctlb >> Chctlb::n2chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg3_character_pattern_data_read: {
            const auto cp_size = regs_.chctlb >> Chctlb::n3chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg0_pattern_name_read:
        case nbg1_pattern_name_read:
        case nbg2_pattern_name_read:
        case nbg3_pattern_name_read: {
            return getVramPatternNameDataReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
        }
        case nbg0_vertical_cell_scroll_table_data_read:
        case nbg1_vertical_cell_scroll_table_data_read: {
            core::Log::unimplemented(core::tr("VDP2 vertical cell scroll table data read"));
            break;
        }
        case cpu_read_write: {
            break;
        }
        default: core::Log::warning(Logger::vdp2, core::tr("VDP2 VRAM access command not allowed"));
    }

    constexpr auto not_found = u8{0x0};
    return not_found;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramBitmapReads(const VramTiming&                 bank_a0,
                              const VramTiming&                 bank_a1,
                              const VramTiming&                 bank_b0,
                              const VramTiming&                 bank_b1,
                              const Vdp2Regs::VramAccessCommand command) const -> u8 {
    using Ramctl      = Vdp2Regs::Ramctl;
    auto bitmap_reads = std::ranges::count(bank_a0, command);
    if ((regs_.ramctl >> Ramctl::vramd_enum) == Ramctl::VramMode::partition_in_2_banks) {
        bitmap_reads += std::ranges::count(bank_a1, command);
    }
    bitmap_reads += std::ranges::count(bank_b0, command);
    if ((regs_.ramctl >> Ramctl::vrbmd_enum) == Ramctl::VramMode::partition_in_2_banks) {
        bitmap_reads += std::ranges::count(bank_b1, command);
    }
    return static_cast<u8>(bitmap_reads);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramPatternNameDataReads(const VramTiming&                 bank_a0,
                                       const VramTiming&                 bank_a1,
                                       const VramTiming&                 bank_b0,
                                       const VramTiming&                 bank_b1,
                                       const Vdp2Regs::VramAccessCommand command) const -> u8 {
    // Pattern name data (PND) read access during 1 cycle must be set to a maximum of 2 banks, one being either
    // VRAM-A0 or VRAM-B0, the other being VRAM-A1 or VRAM-B1. When the VRAM is not divided in 2 partitions, VRAM-A0
    // is used as VRAM-A and VRAM-B0 as VRAM-B.
    // -------------------------------------------
    // | Partition | Partition |   Banks used    |
    // |  Bank A   |  Bank B   |                 |
    // |-----------------------------------------|
    // |    no     |    no     | A0, B0          |
    // |   yes     |    no     | A0, B0          |
    // |    no     |   yes     | A0, B0          |
    // |   yes     |   yes     | A0, B0 or A1,B1 |
    // -------------------------------------------
    using Ramctl   = Vdp2Regs::Ramctl;
    auto pnd_reads = ptrdiff_t{};
    if ((regs_.ramctl >> Ramctl::vramd_enum) == Ramctl::VramMode::partition_in_2_banks
        && ((regs_.ramctl >> Ramctl::vrbmd_enum) == Ramctl::VramMode::partition_in_2_banks)) {
        auto pnd_reads_bank_0 = std::ranges::count(bank_a0, command);
        pnd_reads_bank_0 += std::ranges::count(bank_b0, command);

        auto pnd_reads_bank_1 = std::ranges::count(bank_a1, command);
        pnd_reads_bank_1 += std::ranges::count(bank_b1, command);

        pnd_reads = (pnd_reads_bank_0 > pnd_reads_bank_1) ? pnd_reads_bank_0 : pnd_reads_bank_1;
    } else {
        pnd_reads += std::ranges::count(bank_a0, command);
        pnd_reads += std::ranges::count(bank_b0, command);
    }
    return static_cast<u8>(pnd_reads);
}

// static
auto Vdp2::getReductionSetting(Vdp2Regs::Zmctl::ZoomQuarter zq, Vdp2Regs::Zmctl::ZoomHalf zh) -> ReductionSetting {
    if (zq == Vdp2Regs::Zmctl::ZoomQuarter::up_to_one_quarter) { return ReductionSetting::up_to_one_quarter; }
    if (zh == Vdp2Regs::Zmctl::ZoomHalf::up_to_one_half) { return ReductionSetting::up_to_one_half; }
    return ReductionSetting::none;
};

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting                    r,
                                                      Vdp2Regs::CharacterColorNumber3Bits ccn) -> VramAccessNumber {
    switch (ccn) {
        using enum Vdp2Regs::CharacterColorNumber3Bits;
        case palette_16:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::one;
                case up_to_one_half: return VramAccessNumber::two;
                case up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case palette_256:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::two;
                case up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case palette_2048:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case rgb_32k:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case rgb_16m:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::eight;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting                    r,
                                                      Vdp2Regs::CharacterColorNumber2Bits ccn) -> VramAccessNumber {
    switch (ccn) {
        using enum Vdp2Regs::CharacterColorNumber2Bits;
        case palette_16:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::one;
                case up_to_one_half: return VramAccessNumber::two;
                case up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case palette_256:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::two;
                case up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case palette_2048:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case rgb_32k:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting                   r,
                                                      Vdp2Regs::CharacterColorNumber1Bit ccn) -> VramAccessNumber {
    switch (ccn) {
        using enum Vdp2Regs::CharacterColorNumber1Bit;
        case palette_16:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::one;
                case up_to_one_half: return VramAccessNumber::two;
                case up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case palette_256:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::two;
                case up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber {
    switch (r) {
        using enum ReductionSetting;
        case up_to_one_quarter: return VramAccessNumber::four;
        case up_to_one_half: return VramAccessNumber::two;
        default: return VramAccessNumber::one;
    }
};

// static
auto Vdp2::getPatternNameFromCharacterPattern(const Vdp2Regs::VramAccessCommand character_pattern)
    -> Vdp2Regs::VramAccessCommand {
    switch (character_pattern) {
        using enum Vdp2Regs::VramAccessCommand;
        case nbg0_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg0_pattern_name_read;
        }
        case nbg1_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg1_pattern_name_read;
        }
        case nbg2_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg2_pattern_name_read;
        }
        case nbg3_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg3_pattern_name_read;
        }
        default: return Vdp2Regs::VramAccessCommand::no_access;
    }
}

// static
void Vdp2::setPatternNameAccess(const VramTiming&                   bank,
                                const Vdp2Regs::VramAccessCommand   pattern,
                                std::array<bool, vram_timing_size>& pnd_access) {
    auto it = std::ranges::find(bank, pattern);
    while (it != bank.end()) {
        pnd_access[std::distance(bank.begin(), it)] = true;
        ++it;
        it = std::find(it, bank.end(), pattern);
    }
}

// static
void Vdp2::setCharacterPatternLimitations(const bool                                is_screen_mode_normal,
                                          const bool                                is_character_pattern_2_by_2,
                                          const std::array<bool, vram_timing_size>& pnd_access,
                                          std::array<bool, vram_timing_size>&       allowed_cpd_timing) {
    constexpr auto t0 = u8{0};
    constexpr auto t1 = u8{1};
    constexpr auto t2 = u8{2};
    constexpr auto t3 = u8{3};
    constexpr auto t4 = u8{4};
    constexpr auto t5 = u8{5};
    constexpr auto t6 = u8{6};
    constexpr auto t7 = u8{7};

    // For each pnd access set, allowed timing is set for cpd
    auto it = std::ranges::find(pnd_access, true);
    while (it != pnd_access.end()) {
        switch (std::distance(pnd_access.begin(), it)) {
            case t0: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true; // should be false
                    allowed_cpd_timing[t4] = true;
                    allowed_cpd_timing[t5] = true;
                    allowed_cpd_timing[t6] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                }
                break;
            }
            case t1: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                    allowed_cpd_timing[t5] = true;
                    allowed_cpd_timing[t6] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t2: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                    allowed_cpd_timing[t6] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    // Sattech #37 states that accessing t2 in high res mode while using 2*2 character pattern restricts access to
                    // t2 and t3
                    if (!is_character_pattern_2_by_2) { allowed_cpd_timing[t0] = true; }
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t3: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    // Sattech #37 states that accessing t3 in high res mode while using 2*2 character pattern restricts access to
                    // t3
                    if (!is_character_pattern_2_by_2) {
                        allowed_cpd_timing[t0] = true;
                        allowed_cpd_timing[t1] = true;
                    }
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t4: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t5: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t6: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t7: {
                if (is_screen_mode_normal) { allowed_cpd_timing[t3] = true; }
                break;
            }
        }

        ++it;
        it = std::find(it, pnd_access.end(), true);
    }
}

// static
auto Vdp2::getVramCharacterPatternDataReads(const VramTiming&                 bank_a0,
                                            const VramTiming&                 bank_a1,
                                            const VramTiming&                 bank_b0,
                                            const VramTiming&                 bank_b1,
                                            const Vdp2Regs::VramAccessCommand command,
                                            const ReductionSetting            reduction,
                                            const bool                        is_screen_mode_normal,
                                            const bool                        is_using_2_by_2_cp) -> u8 {
    // From the command we must use the linked Pattern Name Data. The limitations are based on the PND read position.
    // Step 1 : find PND reads for the current command
    const auto pnd = getPatternNameFromCharacterPattern(command);
    // const auto pnd_reads = getVramPatternNameDataReads(bank_a0, bank_a1, bank_b0, bank_b1, pnd);

    auto pnd_timing_access = std::array{false, false, false, false, false, false, false, false};
    setPatternNameAccess(bank_a0, pnd, pnd_timing_access);
    setPatternNameAccess(bank_b0, pnd, pnd_timing_access);
    setPatternNameAccess(bank_a1, pnd, pnd_timing_access);
    setPatternNameAccess(bank_b1, pnd, pnd_timing_access);

    // If there's no reduction, observe selection limits when CPD read access >= 2.
    // If reduction = 1/2 or 1/4, the behavior isn't clear from the docs ... I'll just
    // observe selection limits for every access.
    auto are_limitations_applied = bool{true};
    if (reduction == ReductionSetting::none) {
        auto unlimited_cpd_reads = std::ranges::count(bank_a0, command);
        unlimited_cpd_reads += std::ranges::count(bank_b0, command);
        unlimited_cpd_reads += std::ranges::count(bank_a1, command);
        unlimited_cpd_reads += std::ranges::count(bank_b1, command);
        if (unlimited_cpd_reads < 2) { are_limitations_applied = false; }
    }

    VramTiming limited_bank_a0 = {bank_a0};
    VramTiming limited_bank_b0 = {bank_b0};
    VramTiming limited_bank_a1 = {bank_a1};
    VramTiming limited_bank_b1 = {bank_b1};

    if (are_limitations_applied) {
        // Step 2 : apply selection limits on accessed timings
        // Some more restrictions can be applied when displaying NBG screens in high res when using 2*2 CP (cf sattechs #37 for
        // details)
        auto allowed_cpd_timing = std::array{false, false, false, false, false, false, false, false};

        // Timings limitations are disabled for now (cf https://github.com/rtoumazet/saturnin/issues/255)
        // setCharacterPatternLimitations(is_screen_mode_normal, is_using_2_by_2_cp, pnd_timing_access, allowed_cpd_timing);
        if (is_screen_mode_normal) {
            allowed_cpd_timing = std::array{true, true, true, true, true, true, true, true};
        } else {
            allowed_cpd_timing = std::array{true, true, true, true, false, false, false, false};
        }

        // Step 3 : get the reads
        // First access not available are changed to no access
        auto it = std::ranges::find(allowed_cpd_timing, false);
        while (it != allowed_cpd_timing.end()) {
            limited_bank_a0[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            limited_bank_b0[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            limited_bank_a1[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            limited_bank_b1[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            ++it;
            it = std::find(it, allowed_cpd_timing.end(), false);
        }
    }
    // Counting cpd access
    auto cpd_reads = std::ranges::count(limited_bank_a0, command);
    cpd_reads += std::ranges::count(limited_bank_b0, command);
    cpd_reads += std::ranges::count(limited_bank_a1, command);
    cpd_reads += std::ranges::count(limited_bank_b1, command);

    return static_cast<u8>(cpd_reads);
};

} // namespace saturnin::video
