// ffi_shim.c
// FFI-friendly wrappers around Swiss Ephemeris C API for use from Java via Wasmtime.
// - Pointers are passed as wasm linear-memory offsets (i32).
// - Strings returned by SE are copied into guest memory buffers you provide.
// - Arrays are copied into guest memory (caller supplies out_ptr offsets).

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "swephexp.h"   // main API
#include "swedate.h"   // swe_julday()
#include "swephlib.h"

// ---- helpers ---------------------------------------------------------------

static inline void write_bytes(int dst_ptr, const void *src, size_t n) {
    if (dst_ptr && n) {
        memcpy((void*)(uintptr_t)dst_ptr, src, n);
    }
}

static inline int write_cstr(int dst_ptr, const char *s, int max_len) {
    if (!dst_ptr || max_len <= 0) return 0;
    if (!s) {
        ((char*)(uintptr_t)dst_ptr)[0] = '\0';
        return 0;
    }
    size_t len = strlen(s);
    if ((size_t)max_len <= 1) { ((char*)(uintptr_t)dst_ptr)[0] = '\0'; return 0; }
    if (len >= (size_t)max_len) len = (size_t)max_len - 1;
    memcpy((void*)(uintptr_t)dst_ptr, s, len);
    ((char*)(uintptr_t)dst_ptr)[len] = '\0';
    return (int)len;
}

// ---- existing basics (from earlier messages) -------------------------------

__attribute__((used))
int swe_calc_ut_ffi(double jd_ut, int ipl, int iflag, int out_ptr) {
    double xx[6];
    int32 rv = swe_calc_ut(jd_ut, (int32)ipl, (int32)iflag, xx, NULL);
    write_bytes(out_ptr, xx, sizeof(xx));
    return (int)rv;
}

__attribute__((used))
void swe_set_ephe_path_ffi(int ptr) {
    swe_set_ephe_path((const char*)(uintptr_t)ptr);
}

__attribute__((used))
void swe_set_jpl_file_ffi(int ptr) {
    swe_set_jpl_file((const char*)(uintptr_t)ptr);
}

__attribute__((used))
void swe_close_ffi(void) {
    swe_close();
}

// ---- NEW: requested wrappers -----------------------------------------------

// swe_julday from swedate.h
// double swe_julday(int year, int month, int day, double hour, int gregflag);
__attribute__((used))
double swe_julday_ffi(int year, int month, int day, double hour, int gregflag) {
    double xx = swe_julday(year, month, day, hour, (int32)gregflag);
    return xx;
}

// swe_sidtime from swephexp.h
// double swe_sidtime(double tjd_ut);
__attribute__((used))
double swe_sidtime_ffi(double tjd_ut) {
    return swe_sidtime(tjd_ut);
}

// swe_set_sid_mode from swephexp.h
// void swe_set_sid_mode(int32 sid_mode, double t0, double ayan_t0);
__attribute__((used))
void swe_set_sid_mode_ffi(int sid_mode, double t0, double ayan_t0) {
    swe_set_sid_mode((int32)sid_mode, t0, ayan_t0);
}

// swe_set_topo from swephexp.h
// set geographic position of observer
// void swe_set_topo(double geolon, double geolat, double geoalt)
__attribute__((used))
void swe_set_topo_ffi(double geolon, double geolat, double geoalt) {
    swe_set_topo(geolon, geolat, geoalt);
}

// swe_get_ayanamsa from swephexp.h
// double swe_get_ayanamsa(double tjd_et);
__attribute__((used))
double swe_get_ayanamsa_ffi(double tjd_et) {
    return swe_get_ayanamsa(tjd_et);
}

// swe_get_ayanamsa_name from swephexp.h
// const char* swe_get_ayanamsa_name(int32 sid_mode);
__attribute__((used))
int swe_get_ayanamsa_name_ffi(int sid_mode, int out_ptr, int out_max_len) {
    const char *name = swe_get_ayanamsa_name((int32)sid_mode);
    return write_cstr(out_ptr, name, out_max_len);
}

// swe_calc from swephexp.h
// int32 swe_calc(double tjd_et, int32 ipl, int32 iflag, double *xx, char *serr);
__attribute__((used))
int swe_calc_ffi(double tjd_et, int ipl, int iflag, int out_ptr, int serr_ptr, int serr_max_len) {
    double xx[6];
    char   serr_local[512];  // local buffer for the error text
    char  *serr = serr_ptr ? serr_local : NULL;

    int32 rv = swe_calc(tjd_et, (int32)ipl, (int32)iflag, xx, serr);
    write_bytes(out_ptr, xx, sizeof(xx));

    if (serr_ptr && serr) {
        // copy error text (if any) into wasm memory, always NUL-terminated
        write_cstr(serr_ptr, serr, serr_max_len > 0 ? serr_max_len : (int)sizeof(serr_local));
    }
    return (int)rv;
}

// swe_houses from swephexp.h
// int swe_houses(
//          double tjd_ut, double geolat, double geolon, int hsys, double *cusps,
//          double *ascmc);
//
// Notes:
//  - cusps[1..12] are used; we copy the whole 13-slot array (0..12) for convenience.
//  - ascmc[10] is fully used and copied.
__attribute__((used))
int swe_houses_ffi(double tjd_ut, double geolat, double geolon, int hsys, 
                   int cusps_ptr, int ascmc_ptr) {
    double cusps[13] = {0};
    double ascmc[10] = {0};

    int rv = swe_houses(tjd_ut, geolat, geolon, (int32)hsys, cusps, ascmc);

    write_bytes(cusps_ptr, cusps, sizeof(cusps));   // 13 * 8 = 104 bytes
    write_bytes(ascmc_ptr, ascmc, sizeof(ascmc));   // 10 * 8 = 80 bytes
    return rv;
}

// swe_houses_ex from swephexp.h
// int swe_houses_ex(
//         double tjd_ut, int32 iflag, double geolat, double geolon, int hsys, 
// 	       double *cusps, double *ascmc);
//
// Notes:
//  - cusps[1..12] are used; we copy the whole 13-slot array (0..12) for convenience.
//  - ascmc[10] is fully used and copied.
__attribute__((used))
int swe_houses_ex_ffi(double tjd_ut, int iflag, double geolat, double geolon, int hsys,
                   int cusps_ptr, int ascmc_ptr) {
    double cusps[13] = {0};
    double ascmc[10] = {0};

    int rv = swe_houses_ex(tjd_ut, (int32)iflag, geolat, geolon, (int32)hsys, cusps, ascmc);

    write_bytes(cusps_ptr, cusps, sizeof(cusps));   // 13 * 8 = 104 bytes
    write_bytes(ascmc_ptr, ascmc, sizeof(ascmc));   // 10 * 8 = 80 bytes
    return rv;
}

//double swi_epsiln(double J, int32 iflag) 
__attribute__((used))
double swi_epsiln_ffi(double jd, int iflag) {
    double xx = swi_epsiln(jd, (int32)iflag);
    return xx;
}
