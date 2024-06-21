# listvolumes

This is a small utlity which just prints list of volumes, their associated drive letters, filesystem name, filesystem sizes.

Windows only.

I put this utility into my custom Windows PE image so i can easily see current volumes.

## Building

1. Install [MSYS2](https://www.msys2.org) ;

2. Install `mingw-w64-x86_64-toolchain` package with `pacman -S mingw-w64-x86_64-toolchain` ;

3. `cd` into `src/` ;

4. Run `mingw32-make` ;

5. Resulting exes will be located in `src/bin/` ;

## Usage

Utility accepts not arguments. Just run it.
