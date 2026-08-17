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
