#ifndef MEDIAEXTRACT_H__
#define MEDIAEXTRACT_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#	define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#	include <endian.h>

#elif defined(__APPLE__)

#	include <libkern/OSByteOrder.h>

#	define htobe16 OSSwapHostToBigInt16
#	define htole16 OSSwapHostToLittleInt16
#	define be16toh OSSwapBigToHostInt16
#	define le16toh OSSwapLittleToHostInt16
 
#	define htobe32 OSSwapHostToBigInt32
#	define htole32 OSSwapHostToLittleInt32
#	define be32toh OSSwapBigToHostInt32
#	define le32toh OSSwapLittleToHostInt32
 
#	define htobe64 OSSwapHostToBigInt64
#	define htole64 OSSwapHostToLittleInt64
#	define be64toh OSSwapBigToHostInt64
#	define le64toh OSSwapLittleToHostInt64

#elif defined(__OpenBSD__)

#	include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#	include <sys/endian.h>

#	define be16toh betoh16
#	define le16toh letoh16

#	define be32toh betoh32
#	define le32toh letoh32

#	define be64toh betoh64
#	define le64toh letoh64

#elif defined(__WINDOWS__)

#	include <winsock2.h>
#	include <sys/param.h>

#	if BYTE_ORDER == LITTLE_ENDIAN

#		define htobe16 htons
#		define htole16(x) (x)
#		define be16toh ntohs
#		define le16toh(x) (x)
 
#		define htobe32 htonl
#		define htole32(x) (x)
#		define be32toh ntohl
#		define le32toh(x) (x)
 
#		define htobe64 htonll
#		define htole64(x) (x)
#		define be64toh ntohll
#		define le64toh(x) (x)

#	elif BYTE_ORDER == BIG_ENDIAN

		/* that would be xbox 360 */
#		define htobe16(x) (x)
#		define htole16(x) __builtin_bswap16(x)
#		define be16toh(x) (x)
#		define le16toh(x) __builtin_bswap16(x)
 
#		define htobe32(x) (x)
#		define htole32(x) __builtin_bswap32(x)
#		define be32toh(x) (x)
#		define le32toh(x) __builtin_bswap32(x)
 
#		define htobe64(x) (x)
#		define htole64(x) __builtin_bswap64(x)
#		define be64toh(x) (x)
#		define le64toh(x) __builtin_bswap64(x)

#	else

#		error "byte order not supported"

#	endif

#else

#	error "platform not supported"

#endif

#ifdef __WINDOWS__

#	define PATH_SEP '\\'

#else

#	define PATH_SEP '/'

#endif

#define MAGIC(STR) (*(const uint32_t*)(STR))

struct file_info {
	size_t length;
	const char *ext;
};

struct extract_options {
	const char *filepath;
	const char *outdir;
	size_t   minsize;
	size_t   maxsize;
	uint64_t offset;
	size_t   length;
	int formats;
	int quiet;
	int simulate;
};

int probalby_mod_text(const uint8_t *str, size_t length);
int extract(const struct extract_options *options,
            size_t *numfilesptr, size_t *sumsizeptr);
int do_extract(const uint8_t *filedata, size_t filesize,
               const struct extract_options *options,
               size_t *numfilesptr, size_t *sumsizeptr);
int write_data(const char *filename, const uint8_t *data, size_t length);

#endif /* MEDIAEXTRACT_H__ */