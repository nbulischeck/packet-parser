# Packet Parser

Fast polymorphic packet parser in C - utilizes AF_PACKET's TPACKET_V3 with CPU fanout.

## Dependencies

### Build System

Meson

* Ubuntu: `sudo apt install meson`
* Arch: `sudo pacman -S meson`

### Compilation

GCC

* Ubuntu: `sudo apt install gcc`
* Arch: `sudo pacman -S gcc`

## Configuring

Debug output is controlled with a single `#ifdef` in `main.h`.

## Building

1. `meson build`
2. `ninja -C build`

## Running

To run: `sudo ./build/ids lo cpu`

When you're finished with the session, press ctrl+c and it will print packet statistics if debug mode is enabled.
