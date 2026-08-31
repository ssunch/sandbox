# Native X11 Image Viewer

A lightweight image viewer for Linux implemented in **C using Xlib/X11**, without relying on higher-level GUI frameworks such as Qt or GTK.

This project was designed and implemented as a personal systems-programming exercise to work directly with native Linux windowing APIs, image buffers, event handling, and image decoding.

## Features

* Native Linux GUI implemented directly with **Xlib/X11**
* JPEG image decoding using **libjpeg**
* BMP image loading and rendering
* Direct RGB image-buffer handling
* Window creation and native X11 event processing
* Image rendering through low-level X11 APIs
* Custom image scaling
* Fit-to-window behavior while preserving image usability
* Handling of window resize and redraw events

## Technical Focus

The project intentionally uses low-level APIs rather than a GUI framework.

The implementation includes direct handling of:

* X11 window creation and configuration
* X11 event loops
* Window expose and resize events
* Native image buffers
* JPEG scanline decoding
* BMP pixel data
* RGB pixel conversion and manipulation
* Image resizing and scaling
* Rendering decoded image data to an X11 window

The goal was to understand and implement the complete path from an encoded image file to pixels rendered in a native Linux window.

## Architecture

The application is structured around several responsibilities:

### Window and Event Handling

Xlib is used directly to create the application window and process native X11 events.

The viewer manages redraw and resize events and updates image rendering according to the current window dimensions.

### Image Decoding

JPEG files are decoded using **libjpeg**, with decoded scanlines converted into an internal RGB representation.

BMP files are parsed and converted into the same rendering path where applicable.

### Image Processing

Image dimensions and pixel buffers are handled directly in C.

Custom scaling logic is used to resize images for display rather than relying on a GUI toolkit's image-processing utilities.

### Rendering

Decoded and scaled image data is transferred to the X11 display using native graphics APIs.

## Technologies

* C
* Linux
* Xlib / X11
* libjpeg
* Native image-buffer manipulation
* Low-level graphics programming

## Motivation

I originally created this project to explore Linux graphics programming below the level of common desktop GUI frameworks.

Rather than using a ready-made image widget, the application handles window management, image decoding, pixel-buffer processing, scaling, and rendering directly.

The project reflects my general interest in understanding software across abstraction layers and implementing functionality from lower-level building blocks.

## Build

Requirements:

* Linux with X11 development libraries
* Xlib development headers
* libjpeg development library
* C compiler

Build instructions depend on the local environment and project build configuration.

## Usage

Run the viewer with a supported image file and display it in the native X11 window.

Supported formats include:

* JPEG
* BMP

## Project Scope

This is a personal technical project rather than a production image-viewing application.

The emphasis is on **native Linux programming, X11 interaction, image decoding, buffer manipulation, and graphics fundamentals**, rather than providing a full-featured desktop image viewer.

## Build

This project uses low-level X11/Xlib APIs for window creation, event handling, and image rendering.

JPEG decoding is handled by the system-provided `libjpeg` implementation, while BMP parsing, image buffer management, scaling, and rendering are implemented directly in this project.

### Dependencies

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential cmake libx11-dev libjpeg-dev
```

Required dependencies:

* **CMake** — build system generation
* **Xlib (`libx11-dev`)** — X11 window creation, event handling, and rendering
* **libjpeg (`libjpeg-dev`)** — JPEG decoding
* **build-essential** — GCC and basic build tools

The project uses the system-provided `jpeglib.h` together with the corresponding `libjpeg` library.

A local copy of `jpeglib.h` is intentionally not maintained in the repository because mixing a bundled header with a different system-installed libjpeg implementation can cause header/library version mismatches.

---

### Build with CMake

Configure the project:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

The executable will be generated at:

```text
build/imgViewer
```

Run:

```bash
./build/imgViewer
```

---

### Debug Build

For a debug build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

This enables the compiler debug configuration managed by CMake.

For a release build:

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

---

## CMake Configuration

The project uses a minimal CMake configuration.

```cmake
cmake_minimum_required(VERSION 3.16)

project(imageViewer
    VERSION 1.0
    LANGUAGES C
)

find_package(X11 REQUIRED)
find_package(JPEG REQUIRED)

add_executable(imgViewer
    viewer.c
    bmpprocess.c
    jpgprocess.c
    util.c
)

target_include_directories(imgViewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(imgViewer PRIVATE
    X11::X11
    JPEG::JPEG
)
```

CMake is used mainly to:

* locate the installed X11 development environment
* locate the installed libjpeg development environment
* manage compiler and linker options
* keep build artifacts outside the source tree
* avoid hard-coded system include or library paths

The project intentionally keeps the CMake configuration simple and does not download or bundle external libraries automatically.

---

## External Dependencies

The project keeps external dependencies minimal:

```text
Xlib
  └─ Native X11 window system access

libjpeg
  └─ JPEG codec / decompression
```

The following parts are implemented directly in the project:

* BMP file parsing
* RGB image buffer management
* Image scaling
* X11 window creation
* X11 event handling
* Pixel conversion
* X11 image rendering

The goal of this project is not to eliminate every system library.

Instead, the project avoids higher-level GUI and image-processing frameworks such as Qt, GTK, or OpenCV in order to work directly with Xlib and raw image data.

---

## Source Layout

```text
imageViewer/
├── CMakeLists.txt
├── README.md
├── viewer.c
├── bmpprocess.c
├── jpgprocess.c
├── util.c
├── include/
│   └── ...
└── build/
```

The `build/` directory contains generated CMake files and compiled objects and should not be committed to Git.

Recommended `.gitignore` entries:

```gitignore
/build/
/build-*/
```

---

## Notes

JPEG headers should be included as a system dependency:

```c
#include <jpeglib.h>
```

rather than using a locally copied version:

```c
#include "jpeglib.h"
```

This ensures that the compiler uses the header corresponding to the installed `libjpeg` implementation.
