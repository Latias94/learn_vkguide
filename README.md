# Learn vkguide

Learn [vkguide](https://vkguide.dev/).

## Build

Install [Vulkan SDK](https://vulkan.lunarg.com/) and [XMake](https://xmake.io/#/guide/installation) first.

### Configure

```shell
# Project configuration:
# `-c`: Clean configuration
# `-a`: Architecture [x86_64|i386|arm64 ...]
# `-m`: Mode [debug|releasedbg|release]
xmake f -c -a x64 -m debug
```

### Build and Run

```shell
xmake b # Build all target
xmake r app # Run the `app` target
```

or

```shell
xmake && xmake r # Build all and run the default target
```

### Works with IDE
```shell
xmake project -k cmakelists # Generate a cmake project for IDEs. e.g. CLion
xmake project -k compile_commands # or if you prefer compile_commands.json
xmake project -k vsxmake -m "debug,release"
# xmake project -k makefile
# xmake project -k xcode
```
