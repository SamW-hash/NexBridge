# NexBridge
NexBridge is a lightweight Linux-first telemetry and automation bridge for Celestron NexStar telescopes.

The project separates low-level telescope communication from higher-level APIs and dashboard tooling through a modular architecture built with C++ and Go. A native C++ driver layer handles serial communication, protocol parsing, and telescope state management, while a Go backend exposes real-time telemetry, automation endpoints, and developer-friendly APIs for dashboards, scripting, and observatory tooling.

NexBridge is designed for developers, tinkerers, and observatory automation enthusiasts who want programmable access to telescope data without relying on heavyweight proprietary control suites.

Features and goals include:

* Real-time telescope telemetry streaming
* REST and WebSocket APIs
* Linux-native and headless operation
* Telescope session logging and playback
* Target queue and automation support
* Stellarium and external software integration
* Lightweight dashboard and observability tools
* Modular architecture for future mount support

Tech Stack:

* C++: telescope driver and protocol layer
* Go: backend API and telemetry services
* Web frontend: live dashboard and control interface
