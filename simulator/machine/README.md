# Machine Simulator

Behavioral C++ simulator for the ASMC CPU.

The simulator uses the shared CPU and ISA definitions from the main project.

It is independent from the Digital Logic Simulator implementation located
in `simulator/computer`.

## Structure

```text
machine/
├── include/
│   └── machine/
│       └── Machine.hpp
├── src/
│   ├── Machine.cpp
│   └── main.cpp
└── README.md
