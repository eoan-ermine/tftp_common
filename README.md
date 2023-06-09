# tftp_common

A simple header-only Trivial File Transfer Protocol packets parsing and serializing library

## Supported platforms

|               |                          |               |               |
|---------------|--------------------------|---------------|---------------|
| **Platform name** | Windows | Linux | MacOS |
| **Minimum required version** | 98 | --- | --- |

## Dependencies

| Package name            | Minimum required version | Ubuntu 22.04                         |
|-------------------------|--------------------------|--------------------------------------|
| C++                     | C++17                    | sudo apt-get install build-essential |

## Quick start

1. Download and install CMake. Version 3.12.0 is the minimum required.
2. Open a shell. Your development tools must be reachable from this shell through the PATH environment variable.
3. Create a build directory, go to this directory:
```shell
mkdir build
cd build
```
4. Execute this command in the shell replacing `path/to/tftp_common/source/root` with the path to the root of your tftp_common source tree:
```shell
cmake path/to/tftp_common/source/root
```
5. After CMake has finished running, proceed to use IDE project files, or start the build from the build directory:
```shell
cmake --build .
```
The --build option tells cmake to invoke the underlying build tool (make, ninja, xcodebuild, msbuild, etc.)

The underlying build tool can be invoked directly, of course, but the --build option is portable.

6. After tftp_common has finished building, install it from the build directory:
```shell
cmake --build . --target install
```

The --target option with install parameter in addition to the --build option tells cmake to build the install target.
