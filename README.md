fcitx5-vi
---
**NOTICE**: This is 100% claude code vibe.

A lightweight Vietnamese input method addon for [Fcitx5](https://github.com/fcitx/fcitx5). Supports both **Telex** and **VNI** input methods.

Works on Wayland compositors (including COSMIC DE on Pop!_OS) where built-in IME support may be limited.

## Features

- **Telex** input method: `aa`→â, `aw`→ă, `ee`→ê, `oo`→ô, `ow`→ơ, `uw`→ư, `dd`→đ
- **VNI** input method: number keys for tones and diacritics
- Proper Vietnamese tone placement rules
- Toggle behavior (pressing the same key twice undoes the diacritic/tone)
- Preedit display with underline formatting

### Telex Reference

| Input | Result | Description |
|-------|--------|-------------|
| `aa` | â | a circumflex |
| `ee` | ê | e circumflex |
| `oo` | ô | o circumflex |
| `aw` | ă | a breve |
| `ow` | ơ | o horn |
| `uw` | ư | u horn |
| `dd` | đ | d stroke |
| `s` | sắc | acute accent (á) |
| `f` | huyền | grave accent (à) |
| `r` | hỏi | hook above (ả) |
| `x` | ngã | tilde (ã) |
| `j` | nặng | dot below (ạ) |
| `z` | — | remove tone |

### VNI Reference

| Key | Effect |
|-----|--------|
| `1` | sắc (acute) |
| `2` | huyền (grave) |
| `3` | hỏi (hook above) |
| `4` | ngã (tilde) |
| `5` | nặng (dot below) |
| `6` | circumflex (â, ê, ô) |
| `7` | horn (ơ, ư) |
| `8` | breve (ă) |
| `9` | stroke (đ) |
| `0` | remove tone |

## Dependencies

| Dependency | Minimum Version | Package (Ubuntu/Pop!_OS) |
|------------|-----------------|--------------------------|
| CMake | 3.16 | `cmake` |
| ECM (Extra CMake Modules) | 1.0.0 | `extra-cmake-modules` |
| Fcitx5Core | 5.1.0 | `libfcitx5core-dev` |
| C++17 compiler | — | `g++` |

```bash
sudo apt install cmake extra-cmake-modules libfcitx5core-dev g++
```

## Build & Install

```bash
git clone https://github.com/user/fcitx5-vi.git
cd fcitx5-vi
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
sudo make install
```

This installs:
- `vi.so` → `/usr/lib/x86_64-linux-gnu/fcitx5/`
- `vi.conf` → `/usr/share/fcitx5/addon/`
- `vi-telex.conf` → `/usr/share/fcitx5/inputmethod/`
- `vi-vni.conf` → `/usr/share/fcitx5/inputmethod/`

After installing, add "Vietnamese Telex" or "Vietnamese VNI" in `fcitx5-configtool`, then use **Ctrl+Space** to switch input methods.

## Verify Installation

```bash
# Check the addon is loaded
fcitx5-diagnose | grep "Vietnamese"

# Should show: Vietnamese 0.1.0
```

## COSMIC DE Setup

If you're using COSMIC DE on Pop!_OS, fcitx5 requires extra configuration since COSMIC doesn't natively support IME frameworks. See [docs/cosmic_de_fcitx5_setup.md](docs/cosmic_de_fcitx5_setup.md) for a complete setup guide.

## License

GPL-2.0-or-later
