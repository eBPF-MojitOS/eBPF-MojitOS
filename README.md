# Project Overview

This repository contains various experiments and plugins related to eBPF technology for MOJITOS. Below is an overview of the directory structure:

## Directory Structure

```
├── LICENSE
├── README
├── experiment                 # Directory for eBPF experiments
│   ├── Packet_counter
│   ├── XDP
│   ├── cpu_monitor
│   └── ...
├── plugins                    # Directory for eBPF plugins
│   ├── Packet_counter
│   └── ...
├── Hooks                      # Directory with hooks for git
    └── pre-commit             # Add the files to .git/hooks folder (on your local folder)
├── quick_ebpf                 # Directory for quick eBPF experiments
└── setup.sh                   # Setup script
```

## Description

- **experiment**: This directory contains various experiments.

- **plugins**: Plugins developed for Mojitos.

- **quick_ebpf**: Simple programs to experiment eBPF capabilities.

- **setup.sh**: eBPF Setup script (debian distro).

## Usage

For detailed usage, please refer to the **[online documentation](https://ebpf-mojitos.github.io/eBPF-Docs/)**.
