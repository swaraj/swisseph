# How to Build

## CheckList
* make sure you are in this project root directory
* make sure you zig 0.13 installed
* make sure you have `export ZIG_HOME=/Users/swyadav/Tools/zig-macos-aarch64-0.13.0` set in ~/.zprofile
* make sure you have `export PATH=$PATH:$ZIG_HOME` set in ~/.zprofile
* make sure you have WABT installed as well, to install do `brew install wabt`, it will provide `wasm-objdump`, `wasm2wat` etc.
* make sure you have `export WABT_HOME=/opt/homebrew/Cellar/wabt/1.0.37` set in ~/.zprofile
* make sure you have `export PATH=$PATH:$WABT_HOME/bin` set in ~/.zprofile

## Clear the cache, first
* Do `rm -rf ~/.cache`
* Do `rm swisseph.wasm`
* Do `rm objdump.txt`

## To build 
* `./wasm_build.sh`
* It should build the swisseph.wasm fle in project root.

## validate
* Do `wasm-objdump -x swisseph.wasm > objdump.txt` and check the Imports and Exports of the module.
* Do `wasm2wat swisseph.wasm| grep export` to quickly check if module exports contains all functions and memory.

## Sample Output
For `wasm2wat swisseph.wasm| grep export` sample output should look like
```
  (export "memory" (memory 0))
  (export "_start" (func $_start))
  (export "swe_calc_ut_ffi" (func $swe_calc_ut_ffi))
  (export "swe_set_ephe_path_ffi" (func $swe_set_ephe_path_ffi))
  (export "swe_set_jpl_file_ffi" (func $swe_set_jpl_file_ffi))
  (export "swe_close_ffi" (func $swe_close_ffi))
  (export "swe_julday_ffi" (func $swe_julday_ffi))
  (export "swe_sidtime_ffi" (func $swe_sidtime_ffi))
  (export "swe_set_sid_mode_ffi" (func $swe_set_sid_mode_ffi))
  (export "swe_get_ayanamsa_ffi" (func $swe_get_ayanamsa_ffi))
  (export "swe_get_ayanamsa_name_ffi" (func $swe_get_ayanamsa_name_ffi))
  (export "swe_calc_ffi" (func $swe_calc_ffi))
  (export "swe_houses_ffi" (func $swe_houses_ffi))
```