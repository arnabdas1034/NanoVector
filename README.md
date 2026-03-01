# NanoVector 🧬

A lightweight, high-performance Vector Database built entirely from scratch in modern C++. Designed specifically to act as a fast, low-level retrieval backend for RAG (Retrieval-Augmented Generation) pipelines.

## 🚀 Core Features

* **Custom Indexing:** Implements a custom KD-Tree from scratch for highly optimized, multi-dimensional nearest-neighbor searches.
* **Memory & Performance:** Strict, low-level control over vector allocations and persistent data storage, avoiding bloated wrapper libraries.
* **Cross-Language Interoperability:** Includes a custom Python bridge (`rag_bridge.py`) to seamlessly connect the high-speed C++ engine with standard Python-based LLM frameworks.
* **Production Build System:** Fully configured using standard CMake.

## 🛠️ Build Instructions

Ensure you have a modern C++ compiler (Clang/GCC) and CMake installed.

```bash
# 1. Clone the repository
git clone [https://github.com/arnabdas1034/NanoVector.git](https://github.com/arnabdas1034/NanoVector.git)
cd NanoVector

# 2. Create a build directory
mkdir build && cd build

# 3. Generate Makefiles and compile
cmake ..
make

# 4. Run the executable
./NanoVector
