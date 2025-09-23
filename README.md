# Async HTTP Server

A high-performance **asynchronous HTTP server** in C++ with routing, TLS, and thread pool concurrency.
Support extremely low Latency, high-throughput, fast ciphers + keep-alive

Load test with hey - HTTP load generator at 90k RPS with Median latency (~0.9 ms)

## Features

- **Asynchronous I/O** – Efficiently handles multiple connections concurrently.  
- **HTTP Parsing** – Supports full HTTP request parsing and response generation.  
- **Routing** – Map URLs to handler functions easily.  
- **TLS via OpenSSL** – Secure HTTPS connections.  
- **Thread Pool Concurrency** – Handles multiple requests using a configurable pool of threads.  
- **Graceful Shutdown** – Cleanly shuts down, finishing ongoing requests and freeing resources.

## Requirements

- C++11 or later  
- OpenSSL  
- CMake (optional)
