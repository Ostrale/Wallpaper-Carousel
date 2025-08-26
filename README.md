# Wallpaper Carousel

Small utility for KDE Plasma that shows a bottom-centered carousel of images and lets you set the wallpaper per-screen.

## Features
- Carousel UI (Qt Quick) placed at the bottom center.
- Keyboard navigation (← / →) and `Enter` to apply wallpaper.
- Scans `~/{XDG_PICTURES_DIR}/Wallpapers` by default (configurable).
- Uses plasmashell DBus `evaluateScript` to set wallpapers per screen.
- `.desktop` provided.

## Build (basic)
Requires Qt6 (Quick, DBus), CMake.

```bash
cmake -Bbuild
make -C build -j
sudo make install
```

## Install app
Copy `packaging/wallpaper-carousel.desktop` to `~/.local/share/applications/`.

## Config
Edit `~/.config/wallpaper-carousel/config.toml` to change `wallpaper_dir` and KDE plugins.
