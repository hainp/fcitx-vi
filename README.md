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

## Setting Up Fcitx5

### 1. Set Fcitx5 as the default input method framework

```bash
im-config -n fcitx5
```

This writes `run_im fcitx5` to `~/.xinputrc`. If your system has an ibus override script (e.g. `/etc/profile.d/pop-im-ibus.sh` on Pop!_OS), rename or remove it:

```bash
sudo mv /etc/profile.d/pop-im-ibus.sh /etc/profile.d/pop-im-ibus.sh.bak
```

### 2. Set environment variables

Add to `/etc/environment` (system-wide) or `~/.profile` (per-user):

```
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
SDL_IM_MODULE=fcitx
```

### 3. Autostart Fcitx5

For COSMIC DE (or any desktop that doesn't auto-launch fcitx5):

```bash
cp /usr/share/applications/org.fcitx.Fcitx5.desktop ~/.config/autostart/
```

### 4. Log out and log back in

The environment variables only take effect in a new session.

### 5. Add Vietnamese input method

**Option A — Using fcitx5-configtool (GUI):**

```bash
fcitx5-configtool
```

Search for "Vietnamese Telex" or "Vietnamese VNI" and add it to your input method list.

**Option B — Edit the profile manually:**

Stop fcitx5 first (it overwrites the profile on exit):

```bash
pkill fcitx5
```

Edit `~/.config/fcitx5/profile` and add an entry:

```ini
[Groups/0/Items/2]
# Name
Name=vi-telex
# Layout
Layout=
```

Then start fcitx5:

```bash
fcitx5 -r -d
```

### 6. Switch input methods

Use **Ctrl+Space** (default keybinding) to cycle between input methods.

## Verify Installation

```bash
# Check the addon is loaded
fcitx5-diagnose | grep "Vietnamese"

# Should show: Vietnamese 0.1.0
```

## Troubleshooting

**Addon not loading:**
- Run `fcitx5-diagnose` and check the "Addon List" section
- Verify files exist: `ls /usr/share/fcitx5/addon/vi.conf /usr/share/fcitx5/inputmethod/vi-telex.conf /usr/lib/x86_64-linux-gnu/fcitx5/vi.so`

**Input method not working in apps:**
- Check env vars: `echo $GTK_IM_MODULE` should print `fcitx`
- If it prints `ibus`, you need to log out and back in (see steps 1–4)

**COSMIC DE settings doesn't show input methods:**
- This is expected. COSMIC doesn't integrate with fcitx5. Use `fcitx5-configtool` instead.

## License

GPL-2.0-or-later
