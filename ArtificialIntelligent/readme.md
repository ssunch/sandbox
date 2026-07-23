# 🧠 Cross-Platform Handwritten Digit Recognition Engine (Pure C)

A lightweight, high-performance neural network engine built **from scratch using pure C (POSIX C standard)** to perform handwritten digit recognition without relying on external machine learning or matrix processing libraries.

---

## 📌 Project Overview
The goal of this project is to understand the fundamental mechanics of Artificial Intelligence and Neural Networks from first principles. By eliminating high-level ML framework abstractions, all core components—including matrix arithmetic, activation functions, backpropagation, and multi-threaded parallel execution—were implemented directly at the system programming level.

---

## ✨ Key Features & Technical Highlights

* **Zero External Dependencies**: Built entirely with standard C libraries (`<stdio.h>`, `<stdlib.h>`, `<math.h>`), providing a zero-dependency architecture for lightweight execution.
* **Low-Level ML Implementation**: Designed custom matrix math pipelines, feedforward inference, and backpropagation learning algorithms from the ground up.
* **Cross-Platform Compatibility**: Features a hardware/OS abstraction layer supporting both **Linux** (`<unistd.h>`) and **Windows** (`<windows.h>`) using preprocessor conditioning (`#ifdef _WIN32`, `__linux__`).
* **Multi-Threaded Optimization**: Accelerated core algorithm throughput and matrix operations using **POSIX Threads (`pthread`)** for multi-core processing.
* **Efficient Resource Management**: Optimized for tight memory footprints and hardware-aware performance through strict C-style dynamic memory allocation and pointer arithmetic.

---

## 🛠️ Architecture & Tech Stack

| Category | Details |
| :--- | :--- |
| **Language** | C (POSIX C Standard) |
| **Concurrency** | POSIX Threads (`pthread`) |
| **OS Abstraction** | Windows (`windows.h`) / Linux (`unistd.h`) via conditional compilation |
| **Dependencies** | Standard C Libraries only (`<math.h>`, `<stdlib.h>`, etc.) |

---

## 🚀 Building and Running

### Prerequisites
* GCC / Clang (Linux/macOS) or MinGW / MSVC (Windows)
* `pthread` support

### Build Instructions

#### Linux / macOS
```bash
gcc -O3 -o digit_recognizer main.c -lpthread -lm
./digit_recognizer
