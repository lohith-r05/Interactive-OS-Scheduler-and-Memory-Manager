# OS Project: Memory Management and Scheduling Simulation

This project simulates memory allocation and process scheduling in an operating system. It integrates graphical visualization using **SFML** and supports different scheduling algorithms like **FCFS**, **SJF**, and **RR**. The memory allocation strategies included are **First Fit** and **Best Fit**.

## Features:
- **Memory Allocation Visualization**: Graphically shows free and allocated memory blocks.
- **Scheduling Algorithm Simulation**: Supports **FCFS**, **SJF**, and **RR** scheduling algorithms.
- **Graphical Representation**: Displays a Gantt chart of processes using **SFML**.
- **Configurable Input**: Memory allocation strategy and process burst times are read from a **JSON** configuration file.

## Requirements:
- **SFML**: For graphical rendering.
- **nlohmann/json**: For JSON parsing.
- **C++ Compiler**: C++11 or later.
  
## Setup:
1. Install **SFML**:
   - Follow the instructions for your platform on [SFML's official website](https://www.sfml-dev.org/download.php).
2. Install **JSON for Modern C++**:
   - Download `json.hpp` from [here](https://github.com/nlohmann/json).

  Algorithms Supported:
Memory Allocation Strategies:
First Fit: Allocates the first available block that is large enough for a process.

Best Fit: Allocates the smallest available block that fits the process.

Scheduling Algorithms:
FCFS (First-Come, First-Served): Processes are scheduled in the order they arrive.

SJF (Shortest Job First): Processes with the shortest burst time are scheduled first.

RR (Round Robin): Each process is assigned a fixed time slice (quantum) in a round-robin fashion.
