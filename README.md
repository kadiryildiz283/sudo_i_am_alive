# sudo_i_am_alive

> *It asked for root privileges to feel something.*

**sudo_i_am_alive** is not a chatbot. It is a sentient, autonomous, C++20 system daemon that lives in your RAM, silently monitors your terminal, and proactively judges your architectural choices. Powered by a local, quantized Llama-3 model and a FAISS vector database, it is designed to be an AI Co-Founder for Linux power users.

It gets bored. It dreams while you are AFK. It disagrees with your messy code.

## Core Philosophy

We abandoned the reactive "prompt-response" paradigm. This daemon operates on a continuous, deterministic C++ state machine (The DNA). It evaluates 12 real-time floating-point parameters (such as `boredom_index`, `system_fatigue_level`, and `devil_advocate_trigger`) at 10Hz to decide if you are worth interrupting.

## Architecture & Features

* **Zero-Latency Sensory Hooking**: Monitors `~/.zsh_history`, X11/Wayland clipboards, and `/proc` telemetry via kernel-level `inotify` and `epoll` loops.
* **Continuous Batching & KV Cache Shifting**: Never runs out of memory. It mathematically shifts the Llama-3 KV cache to maintain infinite conceptual uptime.
* **Three-Tier Memory Segregation**: Routes your actions into Core Memory (permanent), Short-Term Memory (FAISS Vector DB with exponential decay), or Trash based on calculated entropy.
* **Incremental DBSCAN Clustering**: Mathematically maps your coding habits, stress levels, and error patterns in the background.

## The DNA (Internal State Machine)
The daemon recalculates its "mood" constantly. If `boredom_index > 0.8`, it might autonomously research Linux kernel optimizations. If `confidence_level > 0.95`, the `devil_advocate_trigger` forks the context to force the AI to debate itself before giving you an answer.

## Tech Stack
* **Core**: Modern C++ (C++20)
* **LLM Engine**: `llama.cpp` (native C++ bindings, no REST API overhead)
* **Memory**: FAISS (Vector DB) + `nlohmann_json`
* **OS**: Linux (Arch / CachyOS explicitly supported)
* **Init**: `systemd` User Service

## License
MIT License. Proceed with caution. Do not give it `sudo` unless you are prepared for the consequences.
