//
// Created by shuai on 16/4/13.
//

#ifndef INC_7ZCRYPTOGRAPHIC_FORMATS_H
#define INC_7ZCRYPTOGRAPHIC_FORMATS_H
#define FMT_TUNABLE_COSTS	3
#define FMT_MAIN_VERSION 12
#define PASSWORD_HASH_SIZES 7
#define BIG_ENOUGH (8192*32)

struct fmt_tests {
    char *ciphertext, *plaintext;
    char *fields[10];
};
struct fmt_params {
    char *label;
    char *format_name;
    char *algorithm_name;
    char *benchmark_comment;
    int benchmark_length;
    int plaintext_length;
    int binary_size;
    int binary_align;
    int salt_size;
    int salt_align;
    int min_keys_per_crypt;
    int max_keys_per_crypt;
    unsigned int flags;

#if FMT_MAIN_VERSION > 11
	char *tunable_cost_name[FMT_TUNABLE_COSTS];
#endif
    struct fmt_tests *tests;
};
struct fmt_methods {
    void (*init)(struct fmt_main *self);

    void (*done)(void);

    void (*reset)(struct db_main *db);

    char *(*prepare)(char *fields[10], struct fmt_main *self);

    int (*valid)(char *ciphertext, struct fmt_main *self);

    char *(*split)(char *ciphertext, int index, struct fmt_main *self);

    void *(*binary)(char *ciphertext);

    void *(*salt)(char *ciphertext);

#if FMT_MAIN_VERSION > 11
    unsigned int (*tunable_cost_value[FMT_TUNABLE_COSTS])(void *salt);
#endif

    char *(*source)(char *source, void *binary);

    int (*binary_hash[PASSWORD_HASH_SIZES])(void *binary);

    int (*salt_hash)(void *salt);

    void (*set_salt)(void *salt);

    void (*set_key)(char *key, int index);

    char *(*get_key)(int index);

    void (*clear_keys)(void);

    int (*crypt_all)(int *count, struct db_salt *salt);

    int (*get_hash[PASSWORD_HASH_SIZES])(int index);

    int (*cmp_all)(void *binary, int count);

    int (*cmp_one)(void *binary, int index);

    int (*cmp_exact)(char *source, int index);
};
struct fmt_private {
	int initialized;
	void *data;
};
struct fmt_main {
	struct fmt_params params;
	struct fmt_methods methods;
	struct fmt_private privates;
	struct fmt_main *next;
};
static struct custom_salt {
	int NumCyclesPower;
	int SaltSize;
	int ivSize;
	int type;
	unsigned char data[BIG_ENOUGH];
	unsigned char iv[16];
	unsigned char salt[16];
	unsigned int crc;
	int length;     /* used in decryption */
	int unpacksize; /* used in CRC calculation */
} *cur_salt;

typedef unsigned short UTF16;
typedef unsigned char UTF8;
typedef uint UTF32;
#define UTF_8 17
const char opt_trailingBytesUTF8[64] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};
const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                   0x03C82080UL, 0xFA082080UL, 0x82082080UL };
#define UNI_MAX_BMP (UTF32)0x0000FFFF
static const int halfShift  = 10; /* used for shifting by 10 bits */

static const UTF32 halfBase = 0x0010000UL;
static const UTF32 halfMask = 0x3FFUL;
#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF

inline int utf8_to_utf16(UTF16 *target, unsigned int len, const UTF8 *source,
                         unsigned int sourceLen)
{
    const UTF16 *targetStart = target;
    const UTF16 *targetEnd = target + len;
    const UTF8 *sourceStart = source;
    const UTF8 *sourceEnd = source + sourceLen;
    UTF32 ch;
    unsigned int extraBytesToRead;

    while (source < sourceEnd) {
        if (*source < 0xC0) {
//#if ARCH_LITTLE_ENDIAN
            *target++ = (UTF16)*source++;
//#else
//            UTF8 val = *source++;
//            SSVAL(target, 0, val);
//            ++target;
//#endif
            if (*source == 0)
                break;
            if (target >= targetEnd) {
                *target = 0;
                return -1 * (source - sourceStart);
            }
            continue;
        }
        ch = *source;
        // The original code in ConvertUTF.c has a much larger (slower)
        // lookup table including zeros. This point must not be reached
        // with *source < 0xC0
        extraBytesToRead =
                opt_trailingBytesUTF8[ch & 0x3f];
        if (source + extraBytesToRead >= sourceEnd) {
            *target = 0;
            return -1 * (source - sourceStart);
        }
        switch (extraBytesToRead) {
            case 3:
                ch <<= 6;
                ch += *++source;
            case 2:
                ch <<= 6;
                ch += *++source;
            case 1:
                ch <<= 6;
                ch += *++source;
                ++source;
                break;
            default:
                *target = 0;
                return -1 * (source - sourceStart);
        }
        ch -= offsetsFromUTF8[extraBytesToRead];
#if 0 /* This only supports UCS-2 */
        #if ARCH_LITTLE_ENDIAN
		*target++ = (UTF16)ch;
#else
		SSVAL(target, 0, ch);
		++target;
#endif
#else /* This supports full UTF-16 with surrogate pairs */
        if (ch <= UNI_MAX_BMP) {  /* Target is a character <= 0xFFFF */
//#if ARCH_LITTLE_ENDIAN
            *target++ = (UTF16)ch;
//#else
//            SSVAL(target, 0, ch);
//            ++target;
//#endif
        } else {  /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                source -= (extraBytesToRead+1); /* Back up source pointer! */
                *target = 0;
                return -1 * (source - sourceStart);
            }
            ch -= halfBase;
//#if ARCH_LITTLE_ENDIAN
            *target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
//#else
//            SSVAL(target, 0, (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START));
//            ++target;
//            SSVAL(target, 0, (UTF16)((ch & halfMask) + UNI_SUR_LOW_START));
//            ++target;
//#endif
        }
#endif
        if (*source == 0)
            break;
        if (target >= targetEnd) {
            *target = 0;
            return -1 * (source - sourceStart);
        }
    }
    *target = 0;		// Null-terminate
    return (target - targetStart);
}
int enc_to_utf16(UTF16 *dst, unsigned int maxdstlen, const UTF8 *src,
                 unsigned int srclen)
{
//    if (pers_opts.target_enc != UTF_8) {
//        int i, trunclen = (int)srclen;
//        if (trunclen > maxdstlen)
//            trunclen = maxdstlen;
//
//        for (i = 0; i < trunclen; i++) {
////#if ARCH_LITTLE_ENDIAN
//            *dst++ = CP_to_Unicode[*src++];
////#else
////            UTF16 val = CP_to_Unicode[*src++];
////            SSVAL(dst, 0, val);
////            ++dst;
////#endif
//        }
//        *dst = 0;
//        if (i < srclen)
//            return -i;
//        else
//            return i;
//    } else {		// Convert from UTF-8
        return utf8_to_utf16(dst, maxdstlen, src, srclen);
//    }
}
inline unsigned int strlen16(const UTF16 *str)
{
    unsigned int len = 0;
    while (*str++ != 0)
        len++;
    return len;
}
typedef unsigned int CRC32_t;

#define POLY 0xEDB88320
#define ALL1 0xFFFFFFFF
#define PLAINTEXT_LENGTH 125
static CRC32_t table[256];
static int bInit=0;

void CRC32_Init(CRC32_t *value)
{
    unsigned int index, bit;
    CRC32_t entry;

    *value = ALL1;

    if (bInit) return;
    bInit = 1;
    for (index = 0; index < 0x100; index++) {
        entry = index;

        for (bit = 0; bit < 8; bit++)
            if (entry & 1) {
                entry >>= 1;
                entry ^= POLY;
            } else
                entry >>= 1;

        table[index] = entry;
    }
}

void CRC32_Update(CRC32_t *value, void *data, unsigned int size)
{
    unsigned char *ptr;
    unsigned int count;
    CRC32_t result;

    result = *value;
    ptr = (unsigned char*)data;
    count = size;

    if (count)
        do {
            result = (result >> 8) ^ table[(result ^ *ptr++) & 0xFF];
        } while (--count);

    *value = result;
}

void CRC32_Final(unsigned char *out, CRC32_t value)
{
    value = ~value;
    out[0] = value;
    out[1] = value >> 8;
    out[2] = value >> 16;
    out[3] = value >> 24;
}

#endif //INC_7ZCRYPTOGRAPHIC_FORMATS_H
