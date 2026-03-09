# Setting Up Fcitx5 on COSMIC DE (Pop!_OS)

COSMIC DE does not integrate with input method frameworks like fcitx5 or ibus through its Settings panel ([pop-os/cosmic-settings#1684](https://github.com/pop-os/cosmic-settings/issues/1684)). It only supports XKB keyboard layouts natively.

This guide shows how to get fcitx5 working independently on COSMIC DE for Vietnamese (or any IME-based) input.

## 1. Install Fcitx5 and an input method

Install fcitx5 and a Vietnamese input method addon:

```bash
# Core fcitx5
sudo apt install fcitx5 fcitx5-config-qt

# Pick one (or more) Vietnamese input method addons:
sudo apt install fcitx5-unikey    # Unikey engine (Telex, VNI, VIQR)
sudo apt install fcitx5-bamboo    # Bamboo engine (Telex, VNI)
```

Or build and install `fcitx5-vi` from this repo (see the main [README](../README.md)).

## 2. Set Fcitx5 as the default input method framework

```bash
im-config -n fcitx5
```

This writes `run_im fcitx5` to `~/.xinputrc`.

### Remove Pop!_OS ibus override (if present)

Pop!_OS ships `/etc/profile.d/pop-im-ibus.sh` which forces ibus. Remove or disable it:

```bash
sudo mv /etc/profile.d/pop-im-ibus.sh /etc/profile.d/pop-im-ibus.sh.bak
```

### Check ~/.bashrc for ibus overrides

Pop!_OS or previous configurations may have added ibus exports to your shell config:

```bash
grep -n "ibus" ~/.bashrc
```

If you see lines like `export GTK_IM_MODULE="ibus"`, change them to `fcitx`:

```bash
export GTK_IM_MODULE="fcitx"
export QT_IM_MODULE="fcitx"
export XMODIFIERS="@im=fcitx"
```

## 3. Set environment variables

COSMIC DE is a systemd-based Wayland compositor. Shell profile.d scripts may not be sourced for graphical apps. Set the variables in **both** locations:

### System-wide (`/etc/environment`)

```bash
# Add these lines to /etc/environment
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
SDL_IM_MODULE=fcitx
```

### Systemd user environment (`~/.config/environment.d/`)

This ensures graphical apps launched by COSMIC get the correct variables:

```bash
mkdir -p ~/.config/environment.d
cat > ~/.config/environment.d/fcitx5.conf << 'EOF'
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
SDL_IM_MODULE=fcitx
EOF
```

## 4. Autostart Fcitx5

COSMIC DE does not auto-launch fcitx5. Add it to autostart:

```bash
mkdir -p ~/.config/autostart
cp /usr/share/applications/org.fcitx.Fcitx5.desktop ~/.config/autostart/
```

## 5. Log out and log back in

The environment variables only take effect in a new session. **You must log out and log back in** for apps to connect to fcitx5.

## 6. Add your input method

Use `fcitx5-configtool` (not COSMIC Settings) to manage input methods:

```bash
fcitx5-configtool
```

Search for your desired input method (e.g. "Unikey", "Bamboo", "Vietnamese Telex") and add it to your list.

Alternatively, edit the profile manually (stop fcitx5 first since it overwrites the profile on exit):

```bash
pkill fcitx5
```

Edit `~/.config/fcitx5/profile` and add an entry, e.g.:

```ini
[Groups/0/Items/2]
# Name
Name=unikey
# Layout
Layout=
```

Then start fcitx5:

```bash
fcitx5 -r -d
```

## 7. Switch input methods

Use **Ctrl+Space** (default) to cycle between input methods. The fcitx5 tray icon in the system tray also lets you switch.

## Verify

```bash
# Environment variables should all say "fcitx"
echo $GTK_IM_MODULE $QT_IM_MODULE $XMODIFIERS

# Fcitx5 should be running
pgrep -a fcitx5

# Check addon status
fcitx5-diagnose | grep -A3 "Addon List"
```

## Troubleshooting

### Input method not activating in apps

Check which IM module apps are using:

```bash
echo $GTK_IM_MODULE  # should print: fcitx
echo $QT_IM_MODULE   # should print: fcitx
echo $XMODIFIERS     # should print: @im=fcitx
```

If any show `ibus`, something is overriding the config. Common culprits:
- `~/.bashrc` with hardcoded `ibus` exports
- `/etc/profile.d/pop-im-ibus.sh` still active
- `~/.xinputrc` set to `run_im ibus` (fix with `im-config -n fcitx5`)

### Ctrl+Space doesn't switch input methods

1. Make sure fcitx5 is running: `pgrep fcitx5`
2. Check that an app has focus — `fcitx5-remote` should return `1` (active) or `2` (inactive but connected), not `0`
3. COSMIC may grab the shortcut. Try a different keybinding in `fcitx5-configtool` under Global Options > Trigger Input Method

### COSMIC Settings doesn't show input methods

This is expected. COSMIC Settings only manages XKB keyboard layouts, not fcitx5/ibus input methods. Use `fcitx5-configtool` instead. See [cosmic-settings#1684](https://github.com/pop-os/cosmic-settings/issues/1684) for the upstream feature request.

### Fcitx5 not autostarting after login

Verify the autostart entry exists:

```bash
ls ~/.config/autostart/org.fcitx.Fcitx5.desktop
```

If missing, recreate it (see step 4).
