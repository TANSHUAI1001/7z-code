//
// Created by shuai on 16/4/13.
//

#ifndef INC_7ZCRYPTOGRAPHIC_JTR_SHA256_H
#define INC_7ZCRYPTOGRAPHIC_JTR_SHA256_H


#include <string.h>

typedef unsigned short ARCH_WORD_32;
static const unsigned char padding[128] = { 0x80, 0 /* 0,0,0,0.... */ };
#define OUTBE32(n,b,i) do { (b)[i] = ((n)>>24); (b)[i+1] = ((n)>>16); (b)[i+2] = ((n)>>8); (b)[i+3] = (n); } while(0)
static int is_aligned(void *p, size_t align)
{
    return ((size_t)p & (align - 1)) == 0;
}
typedef struct
{
    ARCH_WORD_32 h[8];          // SHA256 state
    ARCH_WORD_32 Nl,Nh;         // UNUSED but here to be compatible with oSSL
    unsigned char buffer[64];   // currentilding data 'block'. It IS in alignment
    unsigned int num,md_len;    // UNUSED but here to be compatible with oSSL
    unsigned int total;         // number of bytes processed
    int bIs256;                 // if 1 SHA256, else SHA224
} jtr_sha256_ctx;
static inline ARCH_WORD_32 __attribute__((const)) john_bswap_32(ARCH_WORD_32 val) {
    register ARCH_WORD_32 res;
    __asm("bswap\t%0" : "=r" (res) : "0" (val));
    return res;
}
#define JOHNSWAP(x) john_bswap_32((x))
#define ROR32(x,n) ((x >>n ) | (x << (32-n)))
#define S0(x)     (ROR32(x, 2) ^ ROR32(x,13) ^ ROR32(x,22))
#define S1(x)     (ROR32(x, 6) ^ ROR32(x,11) ^ ROR32(x,25))
#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))
#define R0(x)     (ROR32(x, 7) ^ ROR32(x,18) ^ (x>>3))
#define R1(x)     (ROR32(x,17) ^ ROR32(x,19) ^ (x>>10))
#define M(t) (W[t&0xF] += (R1(W[(t-2)&0xF]) + W[(t-7)&0xF] + R0(W[(t-15)&0xF])))
#define R_R(a,b,c,d,e,f,g,h,x,K) do{  \
        tmp = h + S1(e) + F1(e,f,g) + K + x; \
		h = S0(a) + F0(a,b,c) + tmp; \
		d += tmp; \
	}while(0)

void jtr_sha256_init(jtr_sha256_ctx *ctx, int bIs256) {
    ctx->total = 0;
    if ((ctx->bIs256 = bIs256)) {
        // SHA-256 IV
        ctx->h[0] = 0x6A09E667;
        ctx->h[1] = 0xBB67AE85;
        ctx->h[2] = 0x3C6EF372;
        ctx->h[3] = 0xA54FF53A;
        ctx->h[4] = 0x510E527F;
        ctx->h[5] = 0x9B05688C;
        ctx->h[6] = 0x1F83D9AB;
        ctx->h[7] = 0x5BE0CD19;
    } else {
        // SHA-224 IV
        ctx->h[0] = 0xC1059ED8;
        ctx->h[1] = 0x367CD507;
        ctx->h[2] = 0x3070DD17;
        ctx->h[3] = 0xF70E5939;
        ctx->h[4] = 0xFFC00B31;
        ctx->h[5] = 0x68581511;
        ctx->h[6] = 0x64F98FA7;
        ctx->h[7] = 0xBEFA4FA4;
    }
}
void jtr_sha256_hash_block(jtr_sha256_ctx *ctx, const unsigned char data[64], int perform_endian_swap)
{
    ARCH_WORD_32 A, B, C, D, E, F, G, H, tmp, W[16];
//#if ARCH_LITTLE_ENDIAN
    int i;
    if (perform_endian_swap) {
        for (i = 0; i < 16; ++i)
            W[i] = JOHNSWAP(*((ARCH_WORD_32*)&(data[i<<2])));
    } else
//#endif
        memcpy(W, data, 16*sizeof(ARCH_WORD_32));

    // Load state from all prior blocks (or init state)
    A = ctx->h[0];
    B = ctx->h[1];
    C = ctx->h[2];
    D = ctx->h[3];
    E = ctx->h[4];
    F = ctx->h[5];
    G = ctx->h[6];
    H = ctx->h[7];

    R_R(A, B, C, D, E, F, G, H, W[ 0], 0x428A2F98);
    R_R(H, A, B, C, D, E, F, G, W[ 1], 0x71374491);
    R_R(G, H, A, B, C, D, E, F, W[ 2], 0xB5C0FBCF);
    R_R(F, G, H, A, B, C, D, E, W[ 3], 0xE9B5DBA5);
    R_R(E, F, G, H, A, B, C, D, W[ 4], 0x3956C25B);
    R_R(D, E, F, G, H, A, B, C, W[ 5], 0x59F111F1);
    R_R(C, D, E, F, G, H, A, B, W[ 6], 0x923F82A4);
    R_R(B, C, D, E, F, G, H, A, W[ 7], 0xAB1C5ED5);
    R_R(A, B, C, D, E, F, G, H, W[ 8], 0xD807AA98);
    R_R(H, A, B, C, D, E, F, G, W[ 9], 0x12835B01);
    R_R(G, H, A, B, C, D, E, F, W[10], 0x243185BE);
    R_R(F, G, H, A, B, C, D, E, W[11], 0x550C7DC3);
    R_R(E, F, G, H, A, B, C, D, W[12], 0x72BE5D74);
    R_R(D, E, F, G, H, A, B, C, W[13], 0x80DEB1FE);
    R_R(C, D, E, F, G, H, A, B, W[14], 0x9BDC06A7);
    R_R(B, C, D, E, F, G, H, A, W[15], 0xC19BF174);
    R_R(A, B, C, D, E, F, G, H, M(16), 0xE49B69C1);
    R_R(H, A, B, C, D, E, F, G, M(17), 0xEFBE4786);
    R_R(G, H, A, B, C, D, E, F, M(18), 0x0FC19DC6);
    R_R(F, G, H, A, B, C, D, E, M(19), 0x240CA1CC);
    R_R(E, F, G, H, A, B, C, D, M(20), 0x2DE92C6F);
    R_R(D, E, F, G, H, A, B, C, M(21), 0x4A7484AA);
    R_R(C, D, E, F, G, H, A, B, M(22), 0x5CB0A9DC);
    R_R(B, C, D, E, F, G, H, A, M(23), 0x76F988DA);
    R_R(A, B, C, D, E, F, G, H, M(24), 0x983E5152);
    R_R(H, A, B, C, D, E, F, G, M(25), 0xA831C66D);
    R_R(G, H, A, B, C, D, E, F, M(26), 0xB00327C8);
    R_R(F, G, H, A, B, C, D, E, M(27), 0xBF597FC7);
    R_R(E, F, G, H, A, B, C, D, M(28), 0xC6E00BF3);
    R_R(D, E, F, G, H, A, B, C, M(29), 0xD5A79147);
    R_R(C, D, E, F, G, H, A, B, M(30), 0x06CA6351);
    R_R(B, C, D, E, F, G, H, A, M(31), 0x14292967);
    R_R(A, B, C, D, E, F, G, H, M(32), 0x27B70A85);
    R_R(H, A, B, C, D, E, F, G, M(33), 0x2E1B2138);
    R_R(G, H, A, B, C, D, E, F, M(34), 0x4D2C6DFC);
    R_R(F, G, H, A, B, C, D, E, M(35), 0x53380D13);
    R_R(E, F, G, H, A, B, C, D, M(36), 0x650A7354);
    R_R(D, E, F, G, H, A, B, C, M(37), 0x766A0ABB);
    R_R(C, D, E, F, G, H, A, B, M(38), 0x81C2C92E);
    R_R(B, C, D, E, F, G, H, A, M(39), 0x92722C85);
    R_R(A, B, C, D, E, F, G, H, M(40), 0xA2BFE8A1);
    R_R(H, A, B, C, D, E, F, G, M(41), 0xA81A664B);
    R_R(G, H, A, B, C, D, E, F, M(42), 0xC24B8B70);
    R_R(F, G, H, A, B, C, D, E, M(43), 0xC76C51A3);
    R_R(E, F, G, H, A, B, C, D, M(44), 0xD192E819);
    R_R(D, E, F, G, H, A, B, C, M(45), 0xD6990624);
    R_R(C, D, E, F, G, H, A, B, M(46), 0xF40E3585);
    R_R(B, C, D, E, F, G, H, A, M(47), 0x106AA070);
    R_R(A, B, C, D, E, F, G, H, M(48), 0x19A4C116);
    R_R(H, A, B, C, D, E, F, G, M(49), 0x1E376C08);
    R_R(G, H, A, B, C, D, E, F, M(50), 0x2748774C);
    R_R(F, G, H, A, B, C, D, E, M(51), 0x34B0BCB5);
    R_R(E, F, G, H, A, B, C, D, M(52), 0x391C0CB3);
    R_R(D, E, F, G, H, A, B, C, M(53), 0x4ED8AA4A);
    R_R(C, D, E, F, G, H, A, B, M(54), 0x5B9CCA4F);
    R_R(B, C, D, E, F, G, H, A, M(55), 0x682E6FF3);
    R_R(A, B, C, D, E, F, G, H, M(56), 0x748F82EE);
    R_R(H, A, B, C, D, E, F, G, M(57), 0x78A5636F);
    R_R(G, H, A, B, C, D, E, F, M(58), 0x84C87814);
    R_R(F, G, H, A, B, C, D, E, M(59), 0x8CC70208);
    R_R(E, F, G, H, A, B, C, D, M(60), 0x90BEFFFA);
    R_R(D, E, F, G, H, A, B, C, M(61), 0xA4506CEB);
    R_R(C, D, E, F, G, H, A, B, M(62), 0xBEF9A3F7);
    R_R(B, C, D, E, F, G, H, A, M(63), 0xC67178F2);

    // save state for usage in next block (or result if this was last block)
    ctx->h[0] += A;
    ctx->h[1] += B;
    ctx->h[2] += C;
    ctx->h[3] += D;
    ctx->h[4] += E;
    ctx->h[5] += F;
    ctx->h[6] += G;
    ctx->h[7] += H;
}

void jtr_sha256_update(jtr_sha256_ctx *ctx, const void *_input, int ilenlft)
{
    int left, fill;
    const unsigned char *input;

    if(ilenlft <= 0)
        return;

    input = (const unsigned char*)_input;
    left = ctx->total & 0x3F;
    fill = 0x40 - left;

    ctx->total += ilenlft;

    if(left && ilenlft >= fill)
    {
        memcpy(ctx->buffer + left, input, fill);
        jtr_sha256_hash_block(ctx, ctx->buffer, 1);
        input += fill;
        ilenlft  -= fill;
        left = 0;
    }

    while(ilenlft >= 0x40)
    {
        jtr_sha256_hash_block(ctx, input, 1);
        input += 0x40;
        ilenlft  -= 0x40;
    }

    if(ilenlft > 0)
        memcpy(ctx->buffer + left, input, ilenlft);
}

void jtr_sha256_final(void *_output, jtr_sha256_ctx *ctx)
{
    ARCH_WORD_32 last, padcnt;
    ARCH_WORD_32 bits;
    union {
        ARCH_WORD_32 wlen[2];
        unsigned char mlen[8];  // need aligned on sparc
    } m;
    unsigned char *output = (unsigned char*)_output;

    bits = (ctx->total <<  3);
    m.wlen[0] = 0;
    OUTBE32(bits, m.mlen, 4);

    last = ctx->total & 0x3F;
    padcnt = (last < 56) ? (56 - last) : (120 - last);

    jtr_sha256_update(ctx, (unsigned char *) padding, padcnt);
    jtr_sha256_update(ctx, m.mlen, 8);

    // the SHA2_GENERIC_DO_NOT_BUILD_ALIGNED == 1 is to force build on
    // required aligned systems without doing the alignment checking.
    // it IS faster (about 2.5%), and once the data is properly aligned
    // in the formats, the alignment checking is nore needed any more.
#if ARCH_ALLOWS_UNALIGNED == 1 || SHA2_GENERIC_DO_NOT_BUILD_ALIGNED == 1
    OUTBE32(ctx->h[0], output,  0);
	OUTBE32(ctx->h[1], output,  4);
	OUTBE32(ctx->h[2], output,  8);
	OUTBE32(ctx->h[3], output, 12);
	OUTBE32(ctx->h[4], output, 16);
	OUTBE32(ctx->h[5], output, 20);
	OUTBE32(ctx->h[6], output, 24);
	if(ctx->bIs256)
		OUTBE32(ctx->h[7], output, 28);
#else
    if (is_aligned(output,sizeof(ARCH_WORD_32))) {
        OUTBE32(ctx->h[0], output,  0);
        OUTBE32(ctx->h[1], output,  4);
        OUTBE32(ctx->h[2], output,  8);
        OUTBE32(ctx->h[3], output, 12);
        OUTBE32(ctx->h[4], output, 16);
        OUTBE32(ctx->h[5], output, 20);
        OUTBE32(ctx->h[6], output, 24);
        if(ctx->bIs256)
            OUTBE32(ctx->h[7], output, 28);
    } else {
        union {
            ARCH_WORD_32 x[8];
            unsigned char c[64];
        } m;
        unsigned char *tmp = m.c;
        OUTBE32(ctx->h[0], tmp,  0);
        OUTBE32(ctx->h[1], tmp,  4);
        OUTBE32(ctx->h[2], tmp,  8);
        OUTBE32(ctx->h[3], tmp, 12);
        OUTBE32(ctx->h[4], tmp, 16);
        OUTBE32(ctx->h[5], tmp, 20);
        OUTBE32(ctx->h[6], tmp, 24);
        if(ctx->bIs256) {
            OUTBE32(ctx->h[7], tmp, 28);
            memcpy(output, tmp, 32);
        } else
            memcpy(output, tmp, 28);
    }
#endif
}
#define SHA256_CTX           jtr_sha256_ctx
#define SHA224_Init(a)       jtr_sha256_init(a,0)
#define SHA256_Init(a)       jtr_sha256_init(a,1)
#define SHA224_Update(a,b,c) jtr_sha256_update(a,b,c)
#define SHA256_Update(a,b,c) jtr_sha256_update(a,b,c)
#define SHA224_Final(a,b)    jtr_sha256_final(a,b)
#define SHA256_Final(a,b)    jtr_sha256_final(a,b)
#endif //INC_7ZCRYPTOGRAPHIC_JTR_SHA256_H
