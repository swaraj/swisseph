#!/bin/bash

# SRCS="swedate.c sweph.c swephlib.c swecl.c swejpl.c swehouse.c swemmoon.c \
#       swehel.c swemplan.c swephgen4.c swevents.c ffi_shim.c"

SRCS="swedate.c sweph.c swephlib.c swecl.c swejpl.c swehouse.c swemmoon.c swehel.c swemplan.c swevents.c ffi_shim.c"

echo ${WASI_SYSROOT}

#-OReleaseSmall
zig cc -target wasm32-wasi \
  -Wl,--no-entry \
  -Wl,--export-memory \
  -Wl,--export=swe_set_ephe_path_ffi \
  -Wl,--export=swe_set_jpl_file_ffi \
  -Wl,--export=swe_close_ffi \
  -Wl,--export=swe_calc_ut_ffi \
  -Wl,--export=swe_julday_ffi \
  -Wl,--export=swe_sidtime_ffi \
  -Wl,--export=swe_set_sid_mode_ffi \
  -Wl,--export=swe_set_topo_ffi \
  -Wl,--export=swe_get_ayanamsa_ffi \
  -Wl,--export=swe_get_ayanamsa_name_ffi \
  -Wl,--export=swe_calc_ffi \
  -Wl,--export=swe_houses_ffi \
  -Wl,--export=swe_houses_ex_ffi \
  -o swisseph.wasm \
  $SRCS -lc -lm