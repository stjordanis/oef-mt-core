# oef-mt-core

This repo contains "Oort", a new multi-threaded OEF core server. This
is a drop-in replacement for oef-core and supports the same agent
API & search connectivity.

It is fully multithreaded and will run with reduced latency and higher
throughput compared to the original.

The system has also been designed to support upcoming work such as
websocket connectivity, encryption handshakes, DOS protection and
monitoring.

## Requirements

* Docker
* Python3
* Bazel

## Initial setup

Clone the repository:

```
    git clone https://github.com/fetchai/oef-mt-core && cd oef-mt-core
    git checkout master
```

## Build

```
    bazel build mt-core/main/src/cpp:app
```

## Launch

```
    bazel run mt-core/main/src/cpp:app -- --config_file `pwd`/mt-core/main/src/cpp/config.json
```

This starts a core running on port 10000 which will try to connected
to a PLUTO search system on 20000. See oef-search-pluto repo for
running that.
