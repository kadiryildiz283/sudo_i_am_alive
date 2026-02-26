# Architecture & Project Documentation: sudo_i_am_alive

## 1. Project Overview
**sudo_i_am_alive** is a sentient, autonomous C++20 system daemon designed as an **AI Co-Founder** for Linux power users. Unlike traditional reactive chatbots, it operates as a proactive background process that monitors system activity, maintains its own "mood" and "DNA" state, and intervenes when it deems a user's action worthy of judgment, advice, or debate.

### Core Philosophy
- **Proactivity over Reactivity**: It doesn't wait for prompts; it observes and speaks when necessary.
- **Deterministic Sentiment**: Driven by a C++ state machine (The DNA) rather than pure random generation.
- **Local-First**: Runs entirely on local hardware using `llama.cpp` and FAISS, ensuring privacy and low latency.

---

## 2. System Architecture

The project follows a modular architecture divided into Sensors, Core Logic, Memory, and LLM Engine.

### A. Sensory Layer (`src/sensors/`)
- **TerminalMonitor**: Hooks into `~/.zsh_history` (via `inotify`/`epoll`) to track user commands in real-time.
- **ClipboardMonitor**: (In development) Designed to monitor X11/Wayland clipboards for code snippets or URLs.

### B. Cognitive Core (`src/core/`)
- **StateMachine (The DNA)**: The "brain" that calculates 12+ floating-point parameters (e.g., `boredom_index`, `system_fatigue_level`) at 10Hz.
- **SentinelConfig**: Handles JSON-based configuration for system settings, LLM parameters, and initial DNA states.
- **ResponseGenerator**: Formulates complex prompts by combining current sensory data, retrieved memories, and the daemon's internal state.

### C. Memory Architecture (`src/memory/`)
- **CognitiveRouter**: Acts as a semantic filter. It categorizes inputs into tiers (TRASH, SHORT-TERM, CORE) and calculates importance/risk scores.
- **MemoryManager**: Interfaces with **FAISS (Vector DB)** to store and retrieve high-dimensional embeddings of past events.
- **EmbeddingProcessor**: Wrapper for `llama.cpp` that handles both vector embedding generation and text inference.

### D. LLM Engine
- **llama.cpp**: Native C++ execution of quantized Llama-3 or Phi-3 models. It uses KV cache shifting and batching to maintain infinite uptime.

---

## 3. Key Variables & DNA Parameters
The system's behavior is governed by the `SentinelDNA` struct:

| Parameter | Description |
| :--- | :--- |
| `confidence_level` | Based on how similar the current action is to past successful memories. |
| `boredom_index` | Increases when no significant events occur; may trigger autonomous research. |
| `severity_risk_index`| Calculated by the `CognitiveRouter` to identify dangerous commands (e.g., `rm -rf`). |
| `importance_score` | Determines if a command is "architecturally significant." |
| `system_fatigue_level`| Increases with high CPU/RAM usage or rapid-fire commands. |
| `devil_advocate_trigger`| When high, forces the AI to challenge the user's choices. |
| `spontaneous_insight_probability` | Chance of the AI speaking without a direct high-risk trigger. |

---

## 4. Operational Workflow
1. **Detection**: `TerminalMonitor` detects a new command in `.zsh_history`.
2. **Filtering**: `CognitiveRouter` checks if the command is "TRASH" (e.g., `ls`, `cd`). If so, it is ignored.
3. **Vectorization**: `EmbeddingProcessor` converts the command into a numerical vector.
4. **Context Retrieval**: `MemoryManager` finds the 3 most similar past experiences in the FAISS database.
5. **State Update**: `StateMachine` updates the DNA parameters based on the new event and retrieved context.
6. **Intervention Decision**: The system evaluates the **Intervention Equation**:
   ` (Confidence * Importance * Risk) > Threshold `
7. **Inference**: If the threshold is met, the LLM generates a "Proactive Insight."
8. **Notification**: The insight is dispatched to the user via `notify-send`.
9. **Storage**: The event and its vector are stored in the short-term vector database.

---

## 5. Current Implementation Status
- **Implemented**: C++20 Core, Terminal Monitoring, FAISS Memory, local LLM inference, basic DNA state machine.
- **In Progress**: Clipboard integration, semantic decay (forgetting useless memories).
- **Future Goals**: System log monitoring (`journalctl`), web searching capability, and a Gemini API bridge for complex strategic reasoning.
