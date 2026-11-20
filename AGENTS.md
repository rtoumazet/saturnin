## Shell policy
- Never touch files outside this repository.

# Project: Saturnin
- Language: C++20, CMake 4.4+, MSVC v144 (VS 2026)
- Build: `cmake --preset vs2026 -B build && cmake --build build --config Release`
#- Test:  `ctest --test-dir build --preset unit`
#- Lint:  `cmake --build build --target clang-tidy`
- Key modules:
  - saturnin/src/cdrom/      — cdrom, aspi, spti
  - saturnin/src/sh2/        — main cpu
  - saturnin/src/sound/      — sound, scsp
  - saturnin/src/video/      — vdp1, vdp2, opengl, gui, imgui
  - saturnin/src/            — core, config, exceptions, interrupts, locale, scu, smpc, utilities
- Conventions:
  - Use std::unique_ptr, never raw owning pointers
  - No exceptions across module boundaries