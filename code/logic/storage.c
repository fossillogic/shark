/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"

typedef struct {
    unsigned int h[8];
    unsigned char block[64];
    unsigned long long bitlen;
    unsigned int idx;
} sha256_ctx;

static const unsigned int K[64] = {
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

#define ROTR(x,n) (((x)>>(n)) | ((x)<<(32-(n))))
#define SHR(x,n)  ((x)>>(n))
#define Ch(x,y,z) (((x)&(y)) ^ (~(x)&(z)))
#define Maj(x,y,z) (((x)&(y)) ^ ((x)&(z)) ^ ((y)&(z)))
#define SIG0(x) (ROTR(x,2) ^ ROTR(x,13) ^ ROTR(x,22))
#define SIG1(x) (ROTR(x,6) ^ ROTR(x,11) ^ ROTR(x,25))
#define Theta0(x) (ROTR(x,7) ^ ROTR(x,18) ^ SHR(x,3))
#define Theta1(x) (ROTR(x,17) ^ ROTR(x,19) ^ SHR(x,10))

static void sha256_init(sha256_ctx *ctx) {
    ctx->bitlen = 0;
    ctx->idx = 0;
    ctx->h[0]=0x6a09e667; ctx->h[1]=0xbb67ae85; ctx->h[2]=0x3c6ef372; ctx->h[3]=0xa54ff53a;
    ctx->h[4]=0x510e527f; ctx->h[5]=0x9b05688c; ctx->h[6]=0x1f83d9ab; ctx->h[7]=0x5be0cd19;
}

static void sha256_transform(sha256_ctx *ctx) {
    unsigned int w[64], a,b,c,d,e,f,g,h,t1,t2;
    int i;

    for (i=0;i<16;i++) {
        w[i] = (ctx->block[i*4]<<24)|(ctx->block[i*4+1]<<16)|(ctx->block[i*4+2]<<8)|ctx->block[i*4+3];
    }
    for (;i<64;i++) w[i] = Theta1(w[i-2]) + w[i-7] + Theta0(w[i-15]) + w[i-16];

    a=ctx->h[0]; b=ctx->h[1]; c=ctx->h[2]; d=ctx->h[3];
    e=ctx->h[4]; f=ctx->h[5]; g=ctx->h[6]; h=ctx->h[7];

    for (i=0;i<64;i++) {
        t1 = h + SIG1(e) + Ch(e,f,g) + K[i] + w[i];
        t2 = SIG0(a) + Maj(a,b,c);
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }

    ctx->h[0]+=a; ctx->h[1]+=b; ctx->h[2]+=c; ctx->h[3]+=d;
    ctx->h[4]+=e; ctx->h[5]+=f; ctx->h[6]+=g; ctx->h[7]+=h;
}

static void sha256_update(sha256_ctx *ctx, const unsigned char *data, size_t len) {
    for (size_t i=0;i<len;i++) {
        ctx->block[ctx->idx++] = data[i];
        ctx->bitlen += 8;
        if (ctx->idx == 64) {
            sha256_transform(ctx);
            ctx->idx = 0;
        }
    }
}

static void sha256_final(sha256_ctx *ctx, unsigned char out[32]) {
    size_t i = ctx->idx;
    ctx->block[i++] = 0x80;
    if (i > 56) {
        while (i<64) ctx->block[i++] = 0;
        sha256_transform(ctx);
        i = 0;
    }
    while (i<56) ctx->block[i++] = 0;
    unsigned long long bits = ctx->bitlen;
    for (int j=7;j>=0;j--) ctx->block[i++] = (bits>>(j*8)) & 0xff;
    sha256_transform(ctx);
    for (int j=0;j<8;j++) {
        out[j*4]   = (ctx->h[j]>>24)&0xff;
        out[j*4+1] = (ctx->h[j]>>16)&0xff;
        out[j*4+2] = (ctx->h[j]>>8)&0xff;
        out[j*4+3] = ctx->h[j]&0xff;
    }
}

// ---------------------------------------------------------------------------
// Utility: compute SHA-256 for a file
// ---------------------------------------------------------------------------
static int sha256_file(const char *path, unsigned char out[32]) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    sha256_ctx ctx;
    sha256_init(&ctx);

    unsigned char buf[4096];
    size_t n;
    while ((n=fread(buf,1,sizeof(buf),f))>0) {
        sha256_update(&ctx, buf, n);
    }
    fclose(f);
    sha256_final(&ctx,out);
    return 0;
}

// Convert hash to hex string
static void hash_to_hex(const unsigned char *h, char out[65]) {
    static const char *hex="0123456789abcdef";
    for (int i=0;i<32;i++) {
        out[i*2]   = hex[h[i]>>4];
        out[i*2+1] = hex[h[i]&0xf];
    }
    out[64]='\0';
}

// Dedup: identical files → hardlink to first instance
static int perform_dedupe(const char *dir) {
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) return -1;

    // Simple hash → path map
    typedef struct { char hash[65], file[1024]; } entry_t;
    entry_t table[2048];
    int table_len = 0;

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        if (e->type != 0) continue; // Only files

        unsigned char h[32];
        if (sha256_file(e->path, h) == 0) {
            char hex[65];
            hash_to_hex(h, hex);

            // check if it exists in table
            for (int i = 0; i < table_len; i++) {
                if (strcmp(hex, table[i].hash) == 0) {
                    // duplicate found – replace with hardlink
                    unlink(e->path);
                    link(table[i].file, e->path);
                    goto next_file;
                }
            }
            strcpy(table[table_len].hash, hex);
            strcpy(table[table_len].file, e->path);
            table_len++;
        }
        next_file: ;
    }
    fossil_io_dir_iter_close(&it);
    return 0;
}

// Catalog: write metadata of all files
static int perform_catalog(const char *dir) {
    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/.catalog.fson", dir);
    FILE *out = fopen(outpath, "w");
    if (!out) return -1;

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) {
        fclose(out);
        return -1;
    }

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        fprintf(out, "file: %s\nsize: %llu\nmtime: %llu\n\n",
            e->path, (unsigned long long)e->size, (unsigned long long)e->modified);
    }

    fossil_io_dir_iter_close(&it);
    fclose(out);
    return 0;
}

// Index: filename + hash database
static int perform_index(const char *dir) {
    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/.index.fson", dir);
    FILE *out = fopen(outpath, "w");
    if (!out) return -1;

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) {
        fclose(out);
        return -1;
    }

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        if (e->type == 0) { // Only files
            unsigned char h[32];
            if (sha256_file(e->path, h) == 0) {
                char hex[65];
                hash_to_hex(h, hex);
                fprintf(out, "file: %s\nhash: %s\n\n", e->path, hex);
            }
        }
    }

    fossil_io_dir_iter_close(&it);
    fclose(out);
    return 0;
}

// Snapshot: timestamped manifest
static int perform_snapshot(const char *dir) {
    time_t now = time(NULL);

    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/.snapshot-%lld.fson", dir, (long long)now);
    FILE *out = fopen(outpath, "w");
    if (!out) return -1;

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) {
        fclose(out);
        return -1;
    }

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        char hex[65] = "";
        if (e->type == 0 && sha256_file(e->path, (unsigned char *)hex) == 0) {
            hash_to_hex((unsigned char *)hex, hex);
        }
        fprintf(out, "file: %s\nsize: %llu\nmtime: %llu\nhash: %s\n\n",
            e->path, (unsigned long long)e->size, (unsigned long long)e->modified, hex);
    }

    fossil_io_dir_iter_close(&it);
    fclose(out);
    return 0;
}

// Prune: delete zero-byte files
static int perform_prune(const char *dir) {
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) return -1;

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        if (e->type == 0 && e->size == 0) {
            unlink(e->path);
        }
    }

    fossil_io_dir_iter_close(&it);
    return 0;
}

// ---------------------------------------------------------------------------
// MAIN STORAGE FUNCTION
// ---------------------------------------------------------------------------
int fossil_shark_storage(const char *path, bool dedupe, bool catalog,
                         bool index, bool snapshot, bool prune)
{
    if (path == NULL) return -1;

    if (dedupe)  perform_dedupe(path);
    if (catalog) perform_catalog(path);
    if (index)   perform_index(path);
    if (snapshot) perform_snapshot(path);
    if (prune)   perform_prune(path);

    return 0;
}
