# C++ HTTP Server

> **Work in Progress:** This project is currently under active development and is not finished yet. Features, structure, and APIs are subject to change.

A simple HTTP/1.1 server built from scratch in modern C++ as a personal learning project to understand socket programming, networking, and how the HTTP protocol works under the hood.

## Current Status

- [x] Basic TCP socket setup and connection handling
- [x] Basic HTTP request parsing (`GET`)
- [x] Serving static HTML/CSS files
- [ ] Multithreading / concurrent connections
- [ ] Full error handling and status codes (`404`, `500`, etc.)
- [ ] Code cleanup and refactoring

## Build & Run

```bash
# Clone the repository
git clone https://github.com/ampired/HTTP-Server-in-Cpp
cd HTTP-Server-in-Cpp

# Compile (example using g++)
g++ -std=c++20 src/*.cpp -o server

# Run
./server
