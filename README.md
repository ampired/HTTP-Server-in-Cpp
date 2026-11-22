# C++ HTTP Server

> **Work in Progress:** This project is currently under active development and is not finished yet. Features, structure, and APIs are subject to change.

A simple HTTP/1.1 server built from scratch in modern C++ as a personal learning project to understand socket programming, networking, and how the HTTP protocol works under the hood.

## Current Status

- [x] Basic TCP socket setup and connection handling
- [x] Basic HTTP request parsing (`GET`)
- [x] Serving static HTML/CSS files
- [x] Multithreading / concurrent connections (via `boost::asio::thread_pool`)
- [ ] Full error handling and status codes (`404`, `500`, etc.)
- [ ] Code cleanup and refactoring

## Requirements

- **Compiler:** `g++` (or any C++17/C++20 compatible compiler)
- **Build tool:** `make`
- **Libraries:**
  - [Boost.Asio](https://github.com/boostorg/asio) (`boost::asio` for the thread pool)
  - `pthread`

On Debian/Ubuntu-based systems, you can install the dependencies with:

```bash
sudo apt update && sudo apt install build-essential libboost-all-dev
```

## Build & Run

```bash
# Clone the repository
git clone https://github.com/ampired/HTTP-Server-in-Cpp
cd HTTP-Server-in-Cpp

# Compile (using make)
cd src
make

# Run
./server
```