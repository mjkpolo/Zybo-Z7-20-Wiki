## Introduction

[Advanced eXtensible Interface (AXI)](https://en.wikipedia.org/wiki/Advanced_eXtensible_Interface) is a communication bus developed by ARM. It is used in the Xilinx FPGAs. For this project, we will be using AXI4, although I believe AXI4-Lite is acceptable for communication with the [Mutex IP](https://docs.xilinx.com/v/u/en-US/pg117-mutex)

[There are three types of AXI4 interfaces:](https://docs.xilinx.com/v/u/en-US/ug761_axi_reference_guide)
- AXI4—For high-performance memory-mapped requirements.
- AXI4-Lite—For simple, low-throughput memory-mapped communication (for example,
to and from control and status registers).
- AXI4-Stream—For high-speed streaming data.

## Board Design

