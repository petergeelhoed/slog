# slog - Suspended Logging Library

A lightweight C library for suspended logging. Messages are buffered and flushed on demand, providing efficient batch logging capabilities.

## Features

- **Suspended Logging**: Buffer log messages for later flushing
- **Printf-style Formatting**: Familiar format string syntax
- **Lightweight**: Minimal overhead and dependencies
- **C-based**: High performance and portability

## Building

### Prerequisites

- CMake 3.21+ (required for workflow presets)
- C compiler (gcc, clang, or compatible)

## CMake Workflow Presets

This project uses **CMake workflow presets** to simplify common build tasks.

Available workflow presets:

| Preset | Description |
|------|------------|
| `debug` | Configure + build + test (Debug, gcc) |
| `release` | Configure + build + test (Release, gcc) |
| `release-doxygen` | Configure + generate Doxygen documentation |
| `release-all` | Build + generate documentation |
| `release-install` | Configure + build + install (sudo)|


> ⚠️ Only the install workflow should be run with sudo.
> All configure, build, test, and documentation workflows should be run as a normal user.


### Running a Workflow

```bash
cmake --workflow --preset release
```




## Code Quality

This project supports clang-based tooling.

### Formatting check
```bash
cmake --workflow --preset lint
```

### Auto-format
```
cmake --build build/debug --target clang-format
```


## Usage

### API

#### `void slog(const char* fmt, ...)`
Queue a log message with printf-style formatting.

**Parameters:**
- `fmt`: Format string (printf compatible)
- `...`: Variable arguments matching the format string

#### `void flushlog(void)`
Flush all buffered log messages to output.

### Examples

See the `example/` directory for a complete working example.

```
#include "slog.h"

int main() {
    const double golden = 1.6180;
    const double euler = 2.7183;
    slog("pi %f\n", golden);
    puts("mid");
    slog("e  %f\n", euler);
    flushlog();
}
```
