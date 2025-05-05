# Simple Traffic Balancer

This is a lightweight C++ traffic balancer for routing HTTP requests to local service instances. Built for a student project.

## Features

- Supports GET, POST, PUT, PATCH, DELETE
- Reads service config from `config.json`
- Implements basic load balancing (e.g. round robin or random)
- Ignores unreachable service instances temporarily

## Build

No CMake. Just use g++ or Visual Studio.

