//
// vdp2_display.cpp
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
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/utilities.h> // toUnderlying

namespace saturnin::video {

namespace util = saturnin::utilities;
using core::tr;
using util::toUnderlying;

constexpr auto use_concurrent_read_for_cells = bool{false};
constexpr auto vdp2_vram_4mb_mask            = u16{0x3FFF};
constexpr auto vdp2_vram_8mb_mask            = u16{0x7FFF};
constexpr auto bits_in_a_byte                = u8{8};

//--------------------------------------------------------------------------------------------------------------
// DISPLAY methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::clearRenderData(const ScrollScreen s) { std::vector<video::Vdp2Part>().swap(vdp2_parts_[toUnderlying(s)]); }

void Vdp2::populateRenderData() {
    clearRenderData(ScrollScreen::rbg1);
    if (isScreenDisplayed(ScrollScreen::rbg1)) {
        updateScrollScreenStatus(ScrollScreen::rbg1);
        if (getScreen(ScrollScreen::rbg1).priority_number != 0) { readScrollScreenData(ScrollScreen::rbg1); }
    } else {
        // discardCache(ScrollScreen::rbg1);
    }

    clearRenderData(ScrollScreen::rbg0);
    if (isScreenDisplayed(ScrollScreen::rbg0)) {
        updateScrollScreenStatus(ScrollScreen::rbg0);
        if (getScreen(ScrollScreen::rbg0).priority_number != 0) { readScrollScreenData(ScrollScreen::rbg0); }
    } else {
        // discardCache(ScrollScreen::rbg0);
    }
    auto is_nbg_displayed
        = !(getScreen(ScrollScreen::rbg0).is_display_enabled && getScreen(ScrollScreen::rbg1).is_display_enabled);

    if (is_nbg_displayed) {
        clearRenderData(ScrollScreen::nbg0);
        if (isScreenDisplayed(ScrollScreen::nbg0)) {
            updateScrollScreenStatus(ScrollScreen::nbg0);
            if (getScreen(ScrollScreen::nbg0).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg0); }
        }

        clearRenderData(ScrollScreen::nbg1);
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg1)) {
            if (isScreenDisplayed(ScrollScreen::nbg1)) {
                updateScrollScreenStatus(ScrollScreen::nbg1);
                if (getScreen(ScrollScreen::nbg1).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg1); }
            }
        }

        clearRenderData(ScrollScreen::nbg2);
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg2)) {
            if (isScreenDisplayed(ScrollScreen::nbg2)) {
                updateScrollScreenStatus(ScrollScreen::nbg2);
                if (getScreen(ScrollScreen::nbg2).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg2); }
            }
        } else {
            // bg_[util::toUnderlying(ScrollScreen::nbg2)] = {};
        }

        if (uses_fbo) {
            // WIP
            if (canScrollScreenBeDisplayed(ScrollScreen::nbg3) && isScreenDisplayed(ScrollScreen::nbg3)) {
                updateScrollScreenStatus(ScrollScreen::nbg3);
                const auto isDirty             = isCacheDirty(ScrollScreen::nbg3);
                const auto priorityIsAboveZero = getScreen(ScrollScreen::nbg3).priority_number != 0;
                //    if (isDirty && priorityIsAboveZero) {
                //        discardCache(ScrollScreen::nbg3);
                //        clearRenderData(ScrollScreen::nbg3);
                //        readScrollScreenData(ScrollScreen::nbg3);
                //        // Data must be reloaded. Passing the status as 'to_clear' suffice on this side.
                //        modules_.opengl()->setFboStatus(getScreen(ScrollScreen::nbg3).priority_number,
                //                                        ScrollScreen::nbg3,
                //                                        FboStatus::to_clear);
                //    }

                //    if (!priorityIsAboveZero) {
                //        // Clear previously used data.
                //        modules_.opengl()->setFboStatus(ScrollScreen::nbg3, FboStatus::to_clear);
                //    }

                //    if (!isDirty && priorityIsAboveZero) {
                //        // Reuse previous data.
                //        modules_.opengl()->setFboStatus(getScreen(ScrollScreen::nbg3).priority_number,
                //                                        ScrollScreen::nbg3,
                //                                        FboStatus::reuse);
                //    }
            } else {
                //    // Clear previously used data.
                //    modules_.opengl()->setFboStatus(ScrollScreen::nbg3, FboStatus::to_clear);
            }
        } else {
            clearRenderData(ScrollScreen::nbg3);
            if (canScrollScreenBeDisplayed(ScrollScreen::nbg3)) {
                if (isScreenDisplayed(ScrollScreen::nbg3)) {
                    updateScrollScreenStatus(ScrollScreen::nbg3);
                    if (getScreen(ScrollScreen::nbg3).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg3); }
                }
            }
        }
    }
}

auto Vdp2::canScrollScreenBeDisplayed(const ScrollScreen s) const -> bool {
    const auto nbg0_color_nb = getScreen(ScrollScreen::nbg0).character_color_number;
    const auto nbg1_color_nb = getScreen(ScrollScreen::nbg1).character_color_number;
    switch (s) {
        using enum ScrollScreen;
        case nbg1: {
            return (nbg0_color_nb != ColorCount::rgb_16m);
        }
        case nbg2: {
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k, ColorCount::rgb_16m};
            return std::ranges::none_of(colors_preventing_display,
                                        [&nbg0_color_nb](const ColorCount cc) { return cc == nbg0_color_nb; });
        }
        case nbg3: {
            if (nbg0_color_nb == ColorCount::rgb_16m) { return false; }
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k};
            return std::ranges::none_of(colors_preventing_display,
                                        [&nbg1_color_nb](const ColorCount cc) { return cc == nbg1_color_nb; });
        }
        default: return true;
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::updateScrollScreenStatus(const ScrollScreen s) {
    using Pcnxx = Vdp2Regs::Pcnxx;
    using Plsz  = Vdp2Regs::Plsz;
    using Scin  = Vdp2Regs::Scin;
    using Scdn  = Vdp2Regs::Scdn;

    constexpr auto map_size_nbg = u8{2 * 2};
    constexpr auto map_size_rbg = u8{4 * 4};
    constexpr auto cell_size    = u8{8 * 8};

    const auto getCharacterColorNumber3Bits = [](const Vdp2Regs::CharacterColorNumber3Bits c, const ScreenModeType t) {
        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber3Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: {
                return (t == ScreenModeType::normal) ? ColorCount::rgb_32k : ColorCount::not_allowed;
            }
            case rgb_16m: return ColorCount::rgb_16m;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber2Bits = [](const Vdp2Regs::CharacterColorNumber2Bits c, const ScreenModeType t) {
        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber2Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: {
                return (t == ScreenModeType::exclusive) ? ColorCount::rgb_16m : ColorCount::rgb_32k;
            }
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber1Bit = [](const Vdp2Regs::CharacterColorNumber1Bit c) {
        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber1Bit;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumberRbg0 = [](const Vdp2Regs::CharacterColorNumber3Bits c, const ScreenModeType t) {
        if (t == ScreenModeType::exclusive) { return ColorCount::cannot_display; }

        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber3Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: return ColorCount::rgb_32k;
            case rgb_16m: {
                return (t == ScreenModeType::normal) ? ColorCount::rgb_16m : ColorCount::not_allowed;
            }
            default: return ColorCount::not_allowed;
        }
    };
    const auto getDotSize = [](const ColorCount cc) {
        const auto dot_size_4  = u8{4};
        const auto dot_size_8  = u8{8};
        const auto dot_size_16 = u8{16};
        const auto dot_size_32 = u8{32};
        switch (cc) {
            using enum ColorCount;
            case palette_16: return dot_size_4;
            case palette_256: return dot_size_8;
            case palette_2048: return dot_size_16;
            case rgb_32k: return dot_size_16;
            default: return dot_size_32;
        }
    };
    const auto getPageSize = [](const Pcnxx::PatternNameDataSize pnd_sz, const Vdp2Regs::CharacterSize ch_sz) {
        constexpr auto boundary_1_word_1_by_1_cell  = u16{0x2000};
        constexpr auto boundary_1_word_2_by_2_cells = u16{0x800};
        if (pnd_sz == Pcnxx::PatternNameDataSize::one_word) {
            return (ch_sz == Vdp2Regs::CharacterSize::one_by_one) ? boundary_1_word_1_by_1_cell : boundary_1_word_2_by_2_cells;
        }

        // The other case is 2 words.
        constexpr auto boundary_2_words_1_by_1_cell  = u16{0x4000};
        constexpr auto boundary_2_words_2_by_2_cells = u16{0x1000};
        return (ch_sz == Vdp2Regs::CharacterSize::one_by_one) ? boundary_2_words_1_by_1_cell : boundary_2_words_2_by_2_cells;
    };

    const auto getScrollScreenFormat = [](const Vdp2Regs::BitmapEnable be) {
        return (be == Vdp2Regs::BitmapEnable::cell_format) ? ScrollScreenFormat::cell : ScrollScreenFormat::bitmap;
    };

    const auto getBitmapSize = [](const Vdp2Regs::BitmapSize2Bits sz) {
        switch (sz) {
            using enum Vdp2Regs::BitmapSize2Bits;
            case size_512_by_256: return BitmapSize::size_512_by_256;
            case size_512_by_512: return BitmapSize::size_512_by_512;
            case size_1024_by_256: return BitmapSize::size_1024_by_256;
            case size_1024_by_512: return BitmapSize::size_1024_by_512;
            default: return BitmapSize::not_set;
        }
    };

    const auto getBitmapStartAddress = [](const u8 map_offset) {
        constexpr auto boundary = u32{0x20000};
        return vram_start_address + map_offset * boundary;
    };

    auto& screen         = getScreen(s);
    screen.scroll_screen = s;

    using Bgon   = Vdp2Regs::Bgon;
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Bmpna  = Vdp2Regs::Bmpna;
    using Bmpnb  = Vdp2Regs::Bmpnb;
    using Mpofn  = Vdp2Regs::Mpofn;
    using Mpofr  = Vdp2Regs::Mpofr;
    using Mpab   = Vdp2Regs::Mpab;
    using Mpcd   = Vdp2Regs::Mpcd;
    using Mpef   = Vdp2Regs::Mpef;
    using Mpgh   = Vdp2Regs::Mpgh;
    using Mpij   = Vdp2Regs::Mpij;
    using Mpkl   = Vdp2Regs::Mpkl;
    using Mpmn   = Vdp2Regs::Mpmn;
    using Mpop   = Vdp2Regs::Mpop;
    using Craofa = Vdp2Regs::Craofa;
    using Craofb = Vdp2Regs::Craofb;
    using Prina  = Vdp2Regs::Prina;
    using Prinb  = Vdp2Regs::Prinb;
    using Prir   = Vdp2Regs::Prir;

    switch (s) {
        using enum ScrollScreen;
        case nbg0:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n0caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n0tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prina >> Prina::n0prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n0mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n0plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn0 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn0 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn0 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn0 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn0 >> Pcnxx::pnb_enum, regs_.chctla >> Chctla::n0chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn0 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn0 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn0 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn0 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn0 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn0 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctla >> Chctla::n0chsz_enum;
            screen.character_color_number
                = getCharacterColorNumber3Bits(regs_.chctla >> Chctla::n0chcn_enum, tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(regs_.chctla >> Chctla::n0bmen_enum);

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size                      = getBitmapSize(regs_.chctla >> Chctla::n0bmsz_enum);
            screen.bitmap_palette_number            = static_cast<u8>(regs_.bmpna >> Bmpna::n0bmpx_shft);
            screen.bitmap_special_priority          = static_cast<u8>(regs_.bmpna >> Bmpna::n0bmpr_shft);
            screen.bitmap_special_color_calculation = static_cast<u8>(regs_.bmpna >> Bmpna::n0bmcc_shft);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer    = regs_.scxin0 >> Scin::nsci_shft;
            screen.screen_scroll_horizontal_fractional = static_cast<u8>(regs_.scxdn0 >> Scdn::nscd_shft);
            screen.screen_scroll_vertical_integer      = regs_.scyin0 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_fractional   = static_cast<u8>(regs_.scydn0 >> Scdn::nscd_shft);

            // Color offset
            screen.color_offset = getColorOffset(VdpLayer::nbg0);

            break;
        case nbg1:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n1caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n1tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);
            // Priority
            screen.priority_number = static_cast<u8>(regs_.prina >> Prina::n1prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n1mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n1plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn1 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn1 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn1 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn1 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn1 >> Pcnxx::pnb_enum, regs_.chctla >> Chctla::n1chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn1 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn1 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn1 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn1 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn1 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn1 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctla >> Chctla::n1chsz_enum;
            screen.character_color_number
                = getCharacterColorNumber2Bits(regs_.chctla >> Chctla::n1chcn_enum, tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(regs_.chctla >> Chctla::n1bmen_enum);

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size                      = getBitmapSize(regs_.chctla >> Chctla::n1bmsz_enum);
            screen.bitmap_palette_number            = static_cast<u8>(regs_.bmpna >> Bmpna::n1bmpx_shft);
            screen.bitmap_special_priority          = static_cast<u8>(regs_.bmpna >> Bmpna::n1bmpr_shft);
            screen.bitmap_special_color_calculation = static_cast<u8>(regs_.bmpna >> Bmpna::n1bmcc_shft);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer    = regs_.scxin1 >> Scin::nsci_shft;
            screen.screen_scroll_horizontal_fractional = static_cast<u8>(regs_.scxdn1 >> Scdn::nscd_shft);
            screen.screen_scroll_vertical_integer      = regs_.scyin1 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_fractional   = static_cast<u8>(regs_.scydn1 >> Scdn::nscd_shft);

            // Color offset
            screen.color_offset = getColorOffset(VdpLayer::nbg1);
            break;
        case nbg2:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n2caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n2tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prinb >> Prinb::n2prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n2mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n2plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn2 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn2 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn2 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn2 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn2 >> Pcnxx::pnb_enum, regs_.chctlb >> Chctlb::n2chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn2 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn2 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn2 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn2 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn2 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn2 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctlb >> Chctlb::n2chsz_enum;
            screen.character_color_number = getCharacterColorNumber1Bit(regs_.chctlb >> Chctlb::n2chcn_enum);
            screen.bitmap_size            = BitmapSize::not_set;
            screen.bitmap_start_address   = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = regs_.scxn2 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_integer   = regs_.scyn2 >> Scin::nsci_shft;

            // Color offset
            screen.color_offset = getColorOffset(VdpLayer::nbg2);
            break;

        case nbg3:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n3caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n3tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prinb >> Prinb::n3prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n3mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n3plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn3 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn3 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn3 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn3 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn3 >> Pcnxx::pnb_enum, regs_.chctlb >> Chctlb::n3chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn3 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn3 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn3 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn3 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn3 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn3 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctlb >> Chctlb::n3chsz_enum;
            screen.character_color_number = getCharacterColorNumber1Bit(regs_.chctlb >> Chctlb::n3chcn_enum);
            screen.bitmap_size            = BitmapSize::not_set;
            screen.bitmap_start_address   = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = regs_.scxn3 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_integer   = regs_.scyn3 >> Scin::nsci_shft;

            // Color offset
            screen.color_offset = getColorOffset(VdpLayer::nbg3);
            break;

        case rbg0:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofb >> Craofb::r0caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::r0tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prir >> Prir::r0prin_shft);

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = static_cast<u8>(regs_.mpofr >> Mpofr::ramp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::raplsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabra >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabra >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdra >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdra >> Mpcd::mpd_shft);
            screen.plane_e_start_address = calculatePlaneStartAddress(s, regs_.mpefra >> Mpef::mpe_shft);
            screen.plane_f_start_address = calculatePlaneStartAddress(s, regs_.mpefra >> Mpef::mpf_shft);
            screen.plane_g_start_address = calculatePlaneStartAddress(s, regs_.mpghra >> Mpgh::mpg_shft);
            screen.plane_h_start_address = calculatePlaneStartAddress(s, regs_.mpghra >> Mpgh::mph_shft);
            screen.plane_i_start_address = calculatePlaneStartAddress(s, regs_.mpijra >> Mpij::mpi_shft);
            screen.plane_j_start_address = calculatePlaneStartAddress(s, regs_.mpijra >> Mpij::mpj_shft);
            screen.plane_k_start_address = calculatePlaneStartAddress(s, regs_.mpklra >> Mpkl::mpk_shft);
            screen.plane_l_start_address = calculatePlaneStartAddress(s, regs_.mpklra >> Mpkl::mpl_shft);
            screen.plane_m_start_address = calculatePlaneStartAddress(s, regs_.mpmnra >> Mpmn::mpm_shft);
            screen.plane_n_start_address = calculatePlaneStartAddress(s, regs_.mpmnra >> Mpmn::mpn_shft);
            screen.plane_o_start_address = calculatePlaneStartAddress(s, regs_.mpopra >> Mpop::mpo_shft);
            screen.plane_p_start_address = calculatePlaneStartAddress(s, regs_.mpopra >> Mpop::mpp_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncr >> Pcnxx::pnb_enum, regs_.chctlb >> Chctlb::r0chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncr >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncr >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncr >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncr >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncr >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncr >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctlb >> Chctlb::r0chsz_enum;
            screen.character_color_number
                = getCharacterColorNumberRbg0(regs_.chctlb >> Chctlb::r0chcn_enum, tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(regs_.chctlb >> Chctlb::r0bmen_enum);

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size = getBitmapSize(static_cast<Vdp2Regs::BitmapSize2Bits>(regs_.chctlb >> Chctlb::r0bmsz_enum));
            screen.bitmap_palette_number            = static_cast<u8>(regs_.bmpnb >> Bmpnb::r0bmpx_shft);
            screen.bitmap_special_priority          = static_cast<u8>(regs_.bmpnb >> Bmpnb::r0bmpr_shft);
            screen.bitmap_special_color_calculation = static_cast<u8>(regs_.bmpnb >> Bmpnb::r0bmcc_shft);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Color offset
            screen.color_offset = getColorOffset(VdpLayer::rbg0);
            break;

        case rbg1:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n0caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n0tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prina >> Prina::n0prin_shft);

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = static_cast<u8>(regs_.mpofr >> Mpofr::rbmp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::rbplsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabrb >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabrb >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdrb >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdrb >> Mpcd::mpd_shft);
            screen.plane_e_start_address = calculatePlaneStartAddress(s, regs_.mpefrb >> Mpef::mpe_shft);
            screen.plane_f_start_address = calculatePlaneStartAddress(s, regs_.mpefrb >> Mpef::mpf_shft);
            screen.plane_g_start_address = calculatePlaneStartAddress(s, regs_.mpghrb >> Mpgh::mpg_shft);
            screen.plane_h_start_address = calculatePlaneStartAddress(s, regs_.mpghrb >> Mpgh::mph_shft);
            screen.plane_i_start_address = calculatePlaneStartAddress(s, regs_.mpijrb >> Mpij::mpi_shft);
            screen.plane_j_start_address = calculatePlaneStartAddress(s, regs_.mpijrb >> Mpij::mpj_shft);
            screen.plane_k_start_address = calculatePlaneStartAddress(s, regs_.mpklrb >> Mpkl::mpk_shft);
            screen.plane_l_start_address = calculatePlaneStartAddress(s, regs_.mpklrb >> Mpkl::mpl_shft);
            screen.plane_m_start_address = calculatePlaneStartAddress(s, regs_.mpmnrb >> Mpmn::mpm_shft);
            screen.plane_n_start_address = calculatePlaneStartAddress(s, regs_.mpmnrb >> Mpmn::mpn_shft);
            screen.plane_o_start_address = calculatePlaneStartAddress(s, regs_.mpoprb >> Mpop::mpo_shft);
            screen.plane_p_start_address = calculatePlaneStartAddress(s, regs_.mpoprb >> Mpop::mpp_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn0 >> Pcnxx::pnb_enum, regs_.chctla >> Chctla::n0chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn0 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn0 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn0 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn0 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn0 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn0 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctla >> Chctla::n0chsz_enum;
            screen.character_color_number
                = getCharacterColorNumber3Bits(regs_.chctla >> Chctla::n0chcn_enum, tv_screen_status_.screen_mode_type);
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Color offset
            screen.color_offset = getColorOffset(VdpLayer::rbg1);
            break;
        default: Log::warning(Logger::vdp2, tr("Scroll screen not set !"));
    }

    // Position calculation helpers
    constexpr auto cells_nb_64             = u16{64};
    constexpr auto cells_nb_128            = u16{128};
    screen.character_pattern_screen_offset = [](const Vdp2Regs::CharacterSize sz) {
        return (sz == Vdp2Regs::CharacterSize::one_by_one) ? ScreenOffset{1, 1} : ScreenOffset{2, 2};
    }(screen.character_pattern_size);
    screen.page_screen_offset  = {cells_nb_64, cells_nb_64};
    screen.plane_screen_offset = [&](const Plsz::PlaneSize sz) {
        switch (sz) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: return ScreenOffset{cells_nb_64, cells_nb_64};
            case size_2_by_1: return ScreenOffset{cells_nb_128, cells_nb_64};
            case size_2_by_2: return ScreenOffset{cells_nb_128, cells_nb_128};
            default: Log::warning(Logger::vdp2, tr("Plane screen offset wasn't properly calculated"));
        }
        return ScreenOffset{0, 0};
    }(screen.plane_size);

    // Calculating the total number of cells for current scroll screen. Doesn't take into account identicals
    // map start addresses, meaning the the real number of cells can be lower than the calculated one.

    screen.cells_number = [&screen]() {
        constexpr auto cells_per_page = 64 * 64; // 32 * 32 * 4 or 64 * 64 * 1
        auto           plane_size     = u16{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: plane_size = 1; break;
            case size_2_by_1: plane_size = 2; break;
            case size_2_by_2: plane_size = 4; break;
            default: Log::warning(Logger::vdp2, tr("Unknown plane size for {}!"), screenName(screen.scroll_screen));
        }

        return cells_per_page * plane_size * screen.map_size;
    }();

    // Calculating the total width of the scroll screen, will be used for scrolling calculation.
    screen.total_screen_scroll_width = [&screen]() -> u16 {
        constexpr auto pixels_per_cell      = 8;
        constexpr auto cells_per_page_width = 64;
        auto           plane_width          = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: plane_width = 1; break;
            case size_2_by_1:
            case size_2_by_2: plane_width = 2; break;
            default: Log::warning(Logger::vdp2, tr("Unknown plane size for {}!"), screenName(screen.scroll_screen));
        }
        auto map_width = u8{};
        switch (screen.scroll_screen) {
            using enum ScrollScreen;
            case nbg0:
            case nbg1:
            case nbg2:
            case nbg3: {
                map_width = 2;
                break;
            }
            case rbg0:
            case rbg1: {
                map_width = 4;
                break;
            }
            default: Log::warning(Logger::vdp2, tr("Unknown scroll screen!"));
        }

        return pixels_per_cell * cells_per_page_width * plane_width * map_width;
    }();

    // Calculating the total height of the scroll screen, will be used for scrolling calculation.
    screen.total_screen_scroll_height = [&screen]() -> u16 {
        constexpr auto pixels_per_cell       = 8;
        constexpr auto cells_per_page_height = 64;
        auto           plane_height          = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1:
            case size_2_by_1: {
                plane_height = 1;
                break;
            }
            case size_2_by_2: {
                plane_height = 2;
                break;
            }
            default: Log::warning(Logger::vdp2, tr("Unknown plane height for {}!"), screenName(screen.scroll_screen));
        }
        auto map_height = u8{};
        switch (screen.scroll_screen) {
            using enum ScrollScreen;
            case nbg0:
            case nbg1:
            case nbg2:
            case nbg3: {
                map_height = 2;
                break;
            }
            case rbg0:
            case rbg1: {
                map_height = 4;
                break;
            }
            default: Log::warning(Logger::vdp2, tr("Unknown map height for {}!"), screenName(screen.scroll_screen));
        }

        return pixels_per_cell * cells_per_page_height * plane_height * map_height;
    }();

    screen.scroll_offset_horizontal = [&]() -> u16 {
        constexpr auto plane_width  = 512;
        auto           nb_of_planes = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: nb_of_planes = 1; break;
            case size_2_by_1:
            case size_2_by_2: nb_of_planes = 2; break;
            default: nb_of_planes = 1;
        }

        return screen.screen_scroll_horizontal_integer % (plane_width * nb_of_planes);
    }();

    screen.scroll_offset_vertical = [&]() -> u16 {
        constexpr auto plane_height = 512;
        auto           nb_of_planes = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1:
            case size_2_by_1: nb_of_planes = 1; break;
            case size_2_by_2: nb_of_planes = 2; break;
            default: nb_of_planes = 1;
        }

        return screen.screen_scroll_vertical_integer % (plane_height * nb_of_planes);
    }();

    if (isCacheDirty(s)) { discardCache(s); }
    clearRenderData(s);
    vdp2_parts_[util::toUnderlying(s)].reserve(screen.cells_number);

    saved_bg_[util::toUnderlying(s)] = screen;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::calculatePlaneStartAddress(const ScrollScreen s, const u32 map_addr) -> u32 {
    using Vrsize = Vdp2Regs::Vrsize;
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Pcnxx  = Vdp2Regs::Pcnxx;
    using Plsz   = Vdp2Regs::Plsz;

    constexpr auto multiplier_800   = u32{0x800};
    constexpr auto multiplier_1000  = u32{0x1000};
    constexpr auto multiplier_2000  = u32{0x2000};
    constexpr auto multiplier_4000  = u32{0x4000};
    constexpr auto multiplier_8000  = u32{0x8000};
    constexpr auto multiplier_10000 = u32{0x10000};

    const auto& screen                 = getScreen(s);
    const auto  is_vram_size_4mb       = (regs_.vrsize >> Vrsize::vramsz_enum) == Vrsize::VramSize::size_4_mbits;
    auto        plane_size             = Plsz::PlaneSize{};
    auto        pattern_name_data_size = Pcnxx::PatternNameDataSize{};
    auto        character_size         = Vdp2Regs::CharacterSize{};
    auto        start_address          = u32{screen.map_offset << 6 | map_addr};

    switch (s) {
        using enum ScrollScreen;
        case nbg0:
            plane_size             = regs_.plsz >> Plsz::n0plsz_enum;
            pattern_name_data_size = regs_.pncn0 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctla >> Chctla::n0chsz_enum;
            break;
        case nbg1:
            plane_size             = regs_.plsz >> Plsz::n1plsz_enum;
            pattern_name_data_size = regs_.pncn1 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctla >> Chctla::n1chsz_enum;
            break;
        case nbg2:
            plane_size             = regs_.plsz >> Plsz::n2plsz_enum;
            pattern_name_data_size = regs_.pncn2 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctlb >> Chctlb::n2chsz_enum;
            break;
        case nbg3:
            plane_size             = regs_.plsz >> Plsz::n3plsz_enum;
            pattern_name_data_size = regs_.pncn3 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctlb >> Chctlb::n3chsz_enum;
            break;
        case rbg0:
            plane_size             = regs_.plsz >> Plsz::raplsz_enum;
            pattern_name_data_size = regs_.pncr >> Pcnxx::pnb_enum;
            character_size         = regs_.chctlb >> Chctlb::r0chsz_enum;
            break;
        case rbg1:
            plane_size             = regs_.plsz >> Plsz::rbplsz_enum;
            pattern_name_data_size = regs_.pncn0 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctla >> Chctla::n0chsz_enum;
            break;
        default: break;
    }

    auto mask       = u32{};
    auto multiplier = u32{};
    switch (plane_size) {
        using enum Plsz::PlaneSize;
        case size_1_by_1:
            if (pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) {
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003f};
                    constexpr auto mask_8mb = u16{0x007f};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00ff};
                    constexpr auto mask_8mb = u16{0x01ff};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_800;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001f};
                    constexpr auto mask_8mb = u16{0x003f};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007f};
                    constexpr auto mask_8mb = u16{0x00ff};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_1000;
                }
            }
            return vram_start_address + (start_address & mask) * multiplier;
            break;
        case size_2_by_1:
            if (pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) {
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003e};
                    constexpr auto mask_8mb = u16{0x007e};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00fe};
                    constexpr auto mask_8mb = u16{0x01fe};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_1000;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001e};
                    constexpr auto mask_8mb = u16{0x003e};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_8000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007e};
                    constexpr auto mask_8mb = u16{0x00fe};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                }
            }
            return vram_start_address + ((start_address & mask) >> 1) * multiplier;
            break;
        case size_2_by_2:
            if (pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) {
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003c};
                    constexpr auto mask_8mb = u16{0x007c};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_8000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00fc};
                    constexpr auto mask_8mb = u16{0x01fc};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001c};
                    constexpr auto mask_8mb = u16{0x003c};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_10000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007c};
                    constexpr auto mask_8mb = u16{0x00fc};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                }
            }
            return vram_start_address + ((start_address & mask) >> 2) * multiplier;
            break;
        default: Log::warning(Logger::vdp2, tr("Plane start address wasn't properly calculated")); return 0;
    }
}

auto Vdp2::getScreen(const ScrollScreen s) -> ScrollScreenStatus& { return bg_[util::toUnderlying(s)]; };
auto Vdp2::getScreen(const ScrollScreen s) const -> const ScrollScreenStatus& { return bg_[util::toUnderlying(s)]; };

void Vdp2::readScrollScreenData(const ScrollScreen s) {
    const auto& screen = getScreen(s);
    // if (isCacheDirty(s)) { discardCache(s); }
    // vdp2_parts_[util::toUnderlying(s)].clear();

    if (screen.format == ScrollScreenFormat::cell) {
        std::vector<CellData>().swap(cell_data_to_process_);
        cell_data_to_process_.reserve(screen.cells_number);

        // Using a set to prevent calculating same address data multiple times
        // auto unique_addresses = std::unordered_set<u32>{};
        auto start_addresses = std::vector<std::pair<u32, ScreenOffset>>{};

        const auto addPlane
            = [&](const u32 address, const ScreenOffset& offset) { start_addresses.emplace_back(address, offset); };

        const auto offset_x         = screen.plane_screen_offset.x;
        const auto offset_y         = screen.plane_screen_offset.y;
        const auto rotation_screens = std::array{ScrollScreen::rbg0, ScrollScreen::rbg1};
        if (std::ranges::none_of(rotation_screens, [&s](const ScrollScreen rss) { return rss == s; })) {
            // For NBG
            addPlane(screen.plane_a_start_address, ScreenOffset{0, 0});
            addPlane(screen.plane_b_start_address, ScreenOffset{offset_x, 0});
            addPlane(screen.plane_c_start_address, ScreenOffset{0, offset_y});
            addPlane(screen.plane_d_start_address, ScreenOffset{offset_x, offset_y});
        } else {
            // For RBG
            addPlane(screen.plane_a_start_address, ScreenOffset{0, 0});
            addPlane(screen.plane_b_start_address, ScreenOffset{offset_x, 0});
            addPlane(screen.plane_c_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), 0});
            addPlane(screen.plane_d_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), 0});
            addPlane(screen.plane_e_start_address, ScreenOffset{0, offset_y});
            addPlane(screen.plane_f_start_address, ScreenOffset{offset_x, offset_y});
            addPlane(screen.plane_g_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), offset_y});
            addPlane(screen.plane_h_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), offset_y});
            addPlane(screen.plane_i_start_address, ScreenOffset{0, static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_j_start_address, ScreenOffset{offset_x, static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_k_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_l_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_m_start_address, ScreenOffset{0, static_cast<u16>(offset_y * 3)});
            addPlane(screen.plane_n_start_address, ScreenOffset{offset_x, static_cast<u16>(offset_y * 3)});
            addPlane(screen.plane_o_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), static_cast<u16>(offset_y * 3)});
            addPlane(screen.plane_p_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), static_cast<u16>(offset_y * 3)});
        }

        // Reset planes data. Currently done at every frame, but could be saved between frames in order to speed up display ...
        // needs an update detection (size + color mode + nbg + etc. )
        // for (auto& [address, parts] : address_to_plane_data_) {
        //    std::vector<Vdp2Part>().swap(parts);
        //}
        auto address_to_plane_data = AddressToPlaneData{};

        std::vector<PlaneDetail>().swap(plane_details_[util::toUnderlying(screen.scroll_screen)]);

        auto size = Size{};
        switch (screen.plane_size) {
            using PlaneSize = Vdp2Regs::Plsz::PlaneSize;
            case PlaneSize::size_1_by_1: {
                size.w = 512;
                size.h = 512;
                break;
            }
            case PlaneSize::size_2_by_1: {
                size.w = 1024;
                size.h = 512;
                break;
            }
            case PlaneSize::size_2_by_2: {
                size.w = 1024;
                size.h = 1024;
                break;
            }
            default: {
                Log::warning(Logger::vdp2, tr("Unknown plane size value !"));
            }
        }

        // Each plane similar data is only read once, even if it's used multiple times.
        for (const auto& [addr, offset] : start_addresses) {
            plane_details_[util::toUnderlying(screen.scroll_screen)].emplace_back(addr, offset);

            if (address_to_plane_data.contains(addr)) { continue; }

            current_plane_address_ = addr;
            readPlaneData(screen, addr, offset);
        }
    } else { // ScrollScreenFormat::bitmap
        readBitmapData(screen);
    }
}

void Vdp2::readBitmapData(const ScrollScreenStatus& screen) {
    constexpr auto width_512      = u16{512};
    constexpr auto width_1024     = u16{1024};
    constexpr auto height_256     = u16{256};
    constexpr auto height_512     = u16{512};
    auto           texture_width  = u16{};
    auto           texture_height = u16{};

    switch (screen.bitmap_size) {
        using enum BitmapSize;
        case size_1024_by_256: {
            texture_width  = width_1024;
            texture_height = height_256;
            break;
        }
        case size_1024_by_512: {
            texture_width  = width_1024;
            texture_height = height_512;
            break;
        }
        case size_512_by_256: {
            texture_width  = width_512;
            texture_height = height_256;
            break;
        }
        case size_512_by_512: {
            texture_width  = width_512;
            texture_height = height_512;
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Bitmap size not set !"));
        }
    }
    // const auto      texture_size = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    // texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp2_bitmap,
                                           screen.bitmap_start_address,
                                           toUnderlying(screen.character_color_number),
                                           screen.bitmap_palette_number);

    if (Texture::isTextureLoadingNeeded(key)) {
        if (ram_status_.color_ram_mode == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (screen.character_color_number) {
                using enum ColorCount;
                case palette_16: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 2));
                    read16ColorsBitmapData<u32>(texture_data, screen);

                    break;
                }
                case palette_256: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 4));
                    read256ColorsBitmapData<u32>(texture_data, screen);
                    break;
                }
                case palette_2048: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read2048ColorsBitmapData<u32>(texture_data, screen);
                    break;
                }
                case rgb_32k: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read32KColorsBitmapData(texture_data, screen);
                    break;
                }
                case rgb_16m: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 16));
                    read16MColorsBitmapData(texture_data, screen);

                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        } else {
            // 16 bits access to color RAM
            switch (screen.character_color_number) {
                using enum ColorCount;
                case palette_16: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 2));
                    read16ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case palette_256: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 4));
                    read256ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case palette_2048: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read2048ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case rgb_32k: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read32KColorsBitmapData(texture_data, screen);
                    break;
                }
                case rgb_16m: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 16));
                    read16MColorsBitmapData(texture_data, screen);
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        }
        Texture::storeTexture(Texture(VdpType::vdp2_bitmap,
                                      scrollScreenToLayer(screen.scroll_screen),
                                      screen.bitmap_start_address,
                                      toUnderlying(screen.character_color_number),
                                      screen.bitmap_palette_number,
                                      texture_data,
                                      texture_width,
                                      texture_height));
        modules_.opengl()->addOrUpdateTexture(key, scrollScreenToLayer(screen.scroll_screen));
    }
    saveBitmap(screen, texture_width, texture_height, key);
}

void Vdp2::saveBitmap(const ScrollScreenStatus& screen, const u16 width, const u16 height, const size_t key) {
    // auto pos = ScreenPos{0, 0};

    // auto p = Vdp2Part(key, width, height, screen.priority_number, screen.linked_layer);
    //  auto p = std::make_unique<Vdp2Part>(key, width, height);
    //  p.priority(screen.priority_number);
    //  vdp2_parts_[util::toUnderlying(screen.scroll_screen)].push_back(
    //     std::make_unique<Vdp2Part>(key, width, height, screen.priority_number));
    vdp2_parts_[util::toUnderlying(screen.scroll_screen)]
        .emplace_back(key, width, height, screen.priority_number, screen.color_offset, VdpType::vdp2_bitmap);
}

void Vdp2::readPlaneData(const ScrollScreenStatus& screen, const u32 plane_address, const ScreenOffset& plane_offset) {
    auto page_start_address = plane_address;
    switch (screen.plane_size) {
        using enum Vdp2Regs::Plsz::PlaneSize;
        case size_1_by_1: readPageData(screen, page_start_address, plane_offset); break;
        case size_2_by_1: {
            readPageData(screen, page_start_address, plane_offset);
            page_start_address += screen.page_size;
            const auto page_offset_x = static_cast<u16>(plane_offset.x + screen.page_screen_offset.x);
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, plane_offset.y});
            break;
        }
        case size_2_by_2: {
            readPageData(screen, page_start_address, plane_offset);
            page_start_address += screen.page_size;
            const auto page_offset_x = static_cast<u16>(plane_offset.x + screen.page_screen_offset.x);
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, plane_offset.y});
            page_start_address += screen.page_size;
            const auto page_offset_y = static_cast<u16>(plane_offset.y + screen.page_screen_offset.y);
            readPageData(screen, page_start_address, ScreenOffset{plane_offset.x, page_offset_y});
            page_start_address += screen.page_size;
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, page_offset_y});

            break;
        }
        default: Log::warning(Logger::vdp2, tr("Plane size invalid !"));
    }
}

void Vdp2::readPageData(const ScrollScreenStatus& screen, const u32 page_address, const ScreenOffset& page_offset) {
    // Getting the right function depending on the pattern name data configuration.

    using Vrsize = Vdp2Regs::Vrsize;
    using Pcnxx  = Vdp2Regs::Pcnxx;

    static auto current_pnd_config = PatternNameDataEnum{};

    if (screen.pattern_name_data_size == Pcnxx::PatternNameDataSize::two_words) {
        current_pnd_config = PatternNameDataEnum::two_words;
    } else {
        if (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) {
            if (screen.character_color_number == ColorCount::palette_16) {
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_16_colors_12_bits;
                }
            } else { // Over 16 colors
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_over_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_over_16_colors_12_bits;
                }
            }
        } else { // CharacterSize::two_by_two
            if (screen.character_color_number == ColorCount::palette_16) {
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_16_colors_12_bits;
                }
            } else { // Over 16 colors
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_over_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_over_16_colors_12_bits;
                }
            }
        }
    }

    // Assigning the current configuration function to a function pointer
    // Not using std::function here because of the performance cost.
    using PatterNameDataFunc = PatternNameData (*)(const u32, const ScrollScreenStatus&);
    auto readPatternNameData = PatterNameDataFunc();
    switch (current_pnd_config) {
        using enum PatternNameDataEnum;
        case two_words: readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData2Words); break;
        case one_word_1_cell_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors10Bits);
            break;
        case one_word_1_cell_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors12Bits);
            break;
        case one_word_1_cell_over_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors10Bits);
            break;
        case one_word_1_cell_over_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors12Bits);
            break;
        case one_word_4_cells_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors10Bits);
            break;
        case one_word_4_cells_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors12Bits);
            break;
        case one_word_4_cells_over_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors10Bits);
            break;
        case one_word_4_cells_over_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors12Bits);
            break;
    }

    const auto cp_number   = (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) ? u32{64 * 64} : u32{32 * 32};
    const auto cp_width    = (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) ? u32{1} : u32{2};
    const auto cp_height   = cp_width;
    const auto pnd_size    = (screen.pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) ? 2 : 4;
    auto       pnd_address = page_address;
    auto       cp_offset   = page_offset;

    // Choosing the right precalculated modulo check function
    const auto& modulo_values = (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one)
                                    ? pre_calculated_modulo_64_
                                    : pre_calculated_modulo_32_;

    const auto isEndOfRowReached = [](const std::vector<u32>& modulos, const u32 val) {
        // Value 0 isn't added to the vector.
        return std::ranges::any_of(modulos, [&](const u32 m) { return m == val; });
    };

    const auto character_number_mask
        = (ram_status_.vram_size == Vrsize::VramSize::size_4_mbits) ? vdp2_vram_4mb_mask : vdp2_vram_8mb_mask;

    for (u32 i = 0; i < cp_number; ++i) {
        const auto raw_data
            = (pnd_size == 2) ? modules_.memory()->read<u16>(pnd_address) : modules_.memory()->read<u32>(pnd_address);
        // if (raw_data != 0x0) __debugbreak();
        auto pn_data = readPatternNameData(raw_data, screen);
        pn_data.character_number &= character_number_mask;

        if (isEndOfRowReached(modulo_values, i)) {
            cp_offset.x = page_offset.x;
            cp_offset.y += cp_height;
        }

        readCharacterPattern(screen, pn_data, cp_offset);

        cp_offset.x += cp_width;
        pnd_address += pnd_size;
    }
}

void Vdp2::readCharacterPattern(const ScrollScreenStatus& screen, const PatternNameData& pnd, const ScreenOffset& cp_offset) {
    constexpr auto character_pattern_boundary = u8{0x20};
    // if (pnd.character_number != 0x3000 && pnd.character_number != 0x3345 && pnd.character_number != 0x3741) __debugbreak();
    const auto character_number_address = pnd.character_number * character_pattern_boundary;
    if (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) {
        readCellDispatch(screen, pnd, character_number_address, cp_offset);
    } else { // CharacterSize::two_by_two

        // const auto pnd_offset           = (screen.pattern_name_data_size == PatternNameDataSize::one_word) ? 2 : 4;
        auto current_cell_address = character_number_address;
        auto cells_address        = std::vector<u32>{};

        const auto cell_number = u32{4};
        for (u32 i = 0; i < cell_number; ++i) {
            cells_address.emplace_back(current_cell_address);
            current_cell_address += screen.cell_size;
        }

        const auto readCells = [&](const std::array<u8, cell_number>& cells_pos) {
            auto cell_offset = cp_offset;
            readCellDispatch(screen, pnd, cells_address[cells_pos[0]], cell_offset);
            cell_offset.x++;
            readCellDispatch(screen, pnd, cells_address[cells_pos[1]], cell_offset);
            cell_offset.x = cp_offset.x;
            cell_offset.y++;
            readCellDispatch(screen, pnd, cells_address[cells_pos[2]], cell_offset);
            cell_offset.x++;
            readCellDispatch(screen, pnd, cells_address[cells_pos[3]], cell_offset);
        };

        // Depending on the inversion, cells have to be read in a different order.
        // Flipping itself will be done while rendering.
        if (!pnd.is_horizontally_flipped && !pnd.is_vertically_flipped) {
            // Cells layout
            //  |0|1|
            //  |2|3|
            readCells({0, 1, 2, 3});
            return;
        }
        if (pnd.is_horizontally_flipped && pnd.is_vertically_flipped) {
            // Cells layout
            // |3|2|
            // |1|0|
            readCells({3, 2, 1, 0});
            return;
        }

        if (pnd.is_horizontally_flipped) {
            // Cells layout
            // |1|0|
            // |3|2|
            readCells({1, 0, 3, 2});
            return;
        }
        if (pnd.is_vertically_flipped) {
            // Cells layout
            // |2|3|
            // |0|1|
            readCells({2, 3, 0, 1});
            return;
        }
    }
}

void Vdp2::readCellDispatch(const ScrollScreenStatus& screen,
                            const PatternNameData&    pnd,
                            const u32                 cell_address,
                            const ScreenOffset&       cell_offset) {
    const auto key = Texture::calculateKey(VdpType::vdp2_cell,
                                           cell_address,
                                           toUnderlying(screen.character_color_number),
                                           pnd.palette_number);

    if (Texture::isTextureLoadingNeeded(key)) {
        // if (use_concurrent_read_for_cells) {
        // ThreadPool::pool_.push_task(&Vdp2::readCellMT,
        //                             this,
        //                             std::cref(screen),
        //                             pnd.palette_number,
        //                             cell_address,
        //                             key,
        //                             std::span<const u8>{modules_.memory()->vdp2_vram_},
        //                             std::span<const u8>{modules_.memory()->vdp2_cram_});
        //} else {
        readCell(screen, pnd, cell_address, key);
        //}
    }
    saveCell(screen, pnd, cell_address, cell_offset, key);
}

void Vdp2::readCell(const ScrollScreenStatus& screen, const PatternNameData& pnd, const u32 cell_address, const size_t key) {
    constexpr auto  texture_width  = u16{8};
    constexpr auto  texture_height = u16{8};
    constexpr auto  texture_size   = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);

    auto is_access_32bits = (ram_status_.color_ram_mode == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors);

    switch (screen.character_color_number) {
        using enum ColorCount;
        case palette_16: {
            if (is_access_32bits) {
                read16ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
            } else {
                read16ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
            }
            break;
        }
        case palette_256: {
            if (is_access_32bits) {
                read256ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
            } else {
                read256ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
            }
            break;
        }
        case palette_2048: {
            if (is_access_32bits) {
                read2048ColorsCellData<u32>(texture_data, screen, cell_address);
            } else {
                read2048ColorsCellData<u16>(texture_data, screen, cell_address);
            }
            break;
        }
        case rgb_32k: {
            read32KColorsCellData(texture_data, screen, cell_address);
            break;
        }
        case rgb_16m: {
            read16MColorsCellData(texture_data, screen, cell_address);
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Character color number invalid !"));
        }
    }

    Texture::storeTexture(Texture(VdpType::vdp2_cell,
                                  scrollScreenToLayer(screen.scroll_screen),
                                  cell_address,
                                  static_cast<u8>(toUnderlying(screen.character_color_number)),
                                  pnd.palette_number,
                                  texture_data,
                                  texture_width,
                                  texture_height));
    modules_.opengl()->addOrUpdateTexture(key, scrollScreenToLayer(screen.scroll_screen));
}

void Vdp2::readCellMT(const ScrollScreenStatus& screen,
                      const u16                 palette_number,
                      const u32                 cell_address,
                      const size_t              key,
                      const std::span<const u8> vram,
                      const std::span<const u8> cram) {
    constexpr auto  texture_width  = u16{8};
    constexpr auto  texture_height = u16{8};
    constexpr auto  texture_size   = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);

    auto is_access_32bits = (ram_status_.color_ram_mode == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors);

    switch (screen.character_color_number) {
        using enum ColorCount;
        case palette_16: {
            if (is_access_32bits) {
                read16ColorsCellData<u32>(texture_data, screen, palette_number, cell_address);
            } else {
                read16ColorsCellData<u16>(texture_data, screen, palette_number, cell_address);
            }
            break;
        }
        case palette_256: {
            if (is_access_32bits) {
                read256ColorsCellData<u32>(texture_data, screen, palette_number, cell_address);
            } else {
                if (use_concurrent_read_for_cells) {
                    read256ColorsCellDataMT<u16>(texture_data, screen, palette_number, cell_address, vram, cram);
                } else {
                    read256ColorsCellData<u16>(texture_data, screen, palette_number, cell_address);
                }
            }
            break;
        }
        case palette_2048: {
            if (is_access_32bits) {
                read2048ColorsCellData<u32>(texture_data, screen, cell_address);
            } else {
                read2048ColorsCellData<u16>(texture_data, screen, cell_address);
            }
            break;
        }
        case rgb_32k: {
            read32KColorsCellData(texture_data, screen, cell_address);
            break;
        }
        case rgb_16m: {
            read16MColorsCellData(texture_data, screen, cell_address);
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Character color number invalid !"));
        }
    }
    Texture::storeTexture(Texture(VdpType::vdp2_cell,
                                  scrollScreenToLayer(screen.scroll_screen),
                                  cell_address,
                                  static_cast<u8>(toUnderlying(screen.character_color_number)),
                                  palette_number,
                                  texture_data,
                                  texture_width,
                                  texture_height));
    modules_.opengl()->addOrUpdateTexture(key, scrollScreenToLayer(screen.scroll_screen));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::saveCell(const ScrollScreenStatus& screen,
                    const PatternNameData&    pnd,
                    const u32                 cell_address,
                    const ScreenOffset&       cell_offset,
                    const size_t              key) {
    constexpr auto texture_width  = u16{8};
    constexpr auto texture_height = u16{8};

    auto pos = ScreenPos{static_cast<u16>(cell_offset.x * texture_width), static_cast<u16>(cell_offset.y * texture_height)};

    //: TODO: scroll offset must be handled during display, put in comments here for now
    pos.x -= screen.scroll_offset_horizontal;
    pos.y -= screen.scroll_offset_vertical;

    vdp2_parts_[util::toUnderlying(screen.scroll_screen)]
        .emplace_back(pnd, pos, key, screen.priority_number, screen.color_offset, current_plane_address_);
}

} // namespace saturnin::video
