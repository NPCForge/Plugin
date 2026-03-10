# NPCForge — UE5 Plugin

**AI-powered NPC system for Unreal Engine 5**

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.4-black?logo=unrealengine)](https://www.unrealengine.com/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?logo=cplusplus)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)
[![Website](https://img.shields.io/badge/Website-npcforge.fr-orange)](https://npcforge.fr)

[🌐 Website](https://npcforge.fr) · [🗂 NPCForge Organization](https://github.com/NPCForge) · [📋 Report a Bug](https://github.com/NPCForge/NPCForge/issues)

---

## 📖 Overview

**NPCForge** is an open-source Unreal Engine 5 plugin that brings NPCs to life with **infinite, context-aware dialogue** and **persistent memory** — powered by a centralized AI backend.

Inspired by research projects like [Voyager](https://voyager.minedojo.org/) and [Stanford Smallville](https://hai.stanford.edu/news/computational-agents-exhibit-believable-humanlike-behavior), NPCForge was built to make high-quality, believable AI characters accessible to **indie developers and small studios**, without the budget of AAA productions.

This repository contains the **Unreal Engine 5 plugin and demo environment**. The backend API (Go) lives in a separate repository within the [NPCForge organization](https://github.com/NPCForge).

---

## ✨ Features

- 🧩 **Drop-in C++ component** — Attach `UAIComponent` to any Actor to turn it into an AI-driven NPC
- 🔌 **WebSocket communication** — Real-time, bidirectional link to the NPCForge backend
- 🧠 **Asynchronous AI "brains"** — Each NPC operates independently; no global bottleneck
- 👁️ **Environment scanning** — NPCs perceive their surroundings and nearby entities at runtime
- ✂️ **Cognitive culling** — UE5 controls which NPCs are actively "thinking" to optimize API costs
- 🗳️ **Event-driven actions** — Blueprint-assignable delegates for messages, votes, and custom behaviors
- 📦 **Blueprint-ready** — Fully exposed to Blueprints for setup without writing C++

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────┐
│           Unreal Engine 5 (Client)           │
│                                             │
│  Actor                                      │
│  └── UAIComponent                           │
│       ├── ScanEnvironment()                 │
│       ├── MakeDecision()                    │
│       └── UWebSocketHandler                 │
│            └── WebSocket ──────────────────►│
└─────────────────────────────────────────────┘
                                              │
                    ┌─────────────────────────▼──────────┐
                    │     NPCForge Backend (Go API)        │
                    │                                       │
                    │  ┌──────────┐   ┌────────────────┐  │
                    │  │Goroutine │   │  Goroutine (N) │  │  ◄── One per NPC
                    │  │  Brain   │...│  Brain         │  │
                    │  └────┬─────┘   └───────┬────────┘  │
                    │       └────────┬─────────┘           │
                    │           ┌────▼─────┐               │
                    │           │  OpenAI  │               │
                    │           │   API    │               │
                    │           └──────────┘               │
                    │       PostgreSQL (Memory)             │
                    └──────────────────────────────────────┘
```

The plugin connects to the Go backend over a **WebSocket**. Each NPC spawns a dedicated **goroutine** server-side — its own asynchronous "brain" — that independently queries the LLM, preventing the queue congestion that plagued our early single-tunnel design.

---

## 🚀 Getting Started

### Prerequisites

| Requirement | Version |
|---|---|
| Unreal Engine | 5.4+ |
| Visual Studio | 2022 (with C++ game dev workload) |
| NPCForge Backend | Running locally or remotely |

### Installation

1. **Clone this repository** into your UE5 project folder:
   ```bash
   git clone https://github.com/NPCForge/NPCForge.git
   ```

2. **Open the project** by double-clicking `NPCForgeEnvironment.uproject`.
   Visual Studio solution files will be generated automatically.

3. **Start the NPCForge backend** (see the [API repository](https://github.com/NPCForge/API_AI)) and ensure it is reachable at `ws://127.0.0.1:3000/ws` (default).

4. **Compile and run** the project from the Unreal Editor.

---

## 🧩 Plugin Integration

### Adding an NPC to your Actor

1. Open your Actor Blueprint (or C++ class).
2. Add the **`NPCForge`** component (`UAIComponent`) from the component panel.
3. Fill in the exposed properties in the Details panel:

| Property | Description |
|---|---|
| `UniqueName` | The NPC's display name, used to identify it on the backend |
| `PersonalityPrompt` | System prompt defining the NPC's personality and role |

### Binding to Events (Blueprint)

`UAIComponent` exposes two Blueprint-assignable delegates:

| Delegate | Parameters | Description |
|---|---|---|
| `OnSendMessage` | `Message`, `Reasoning` | Fired when the NPC sends a message |
| `OnVote` | `Voter`, `VoteTarget` | Fired when the NPC casts a vote |

Bind these in Blueprint to drive animations, UI, or any game logic.

---

## 📡 WebSocket Protocol

Communication uses JSON messages routed by an `action` field. The handler (`UWebSocketHandler`) manages the full lifecycle:

| Action (Client → Server) | Description |
|---|---|
| `Register` | Authenticate and start a new game session |
| `Connect` | Reconnect to an existing session |
| `CreateEntity` | Register a new NPC with its name, prompt, and role |
| `GetEntities` | Fetch all existing entities for the current session |
| `EntityDied` | Notify the backend that an NPC is no longer active |
| `Disconnect` | End the session gracefully |

---

## 🐺 Demo: Werewolf Game

The included demo environment showcases a fully autonomous **Werewolf** game: AI players receive secret roles, discuss, argue, and vote — with no human input. This stress-tests the plugin's ability to coordinate multiple concurrent NPC conversations.

> **Game prompt used:** *"You are playing a turn-based social deduction game: Werewolf. Identify the werewolves while protecting the innocent. Analyze the discussion and deduce who could be a werewolf at each turn."*

---

## 👥 Contributors

| Name | GitHub | Role |
|---|---|---|
| Grainbox | [@Grainbox](https://github.com/Grainbox) | Project Lead & Plugin Dev |
| CogalTek | [@CogalTek](https://github.com/CogalTek) | Backend (Go API) |
| HugoFtn | [@Hugoftn](https://github.com/Hugoftn) | Demo Game Dev |
| TristanEpitech | [@Tristanepitech](https://github.com/Tristanepitech) | Backend Dev |

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

Built with ❤️ by the NPCForge team · [npcforge.fr](https://npcforge.fr)
