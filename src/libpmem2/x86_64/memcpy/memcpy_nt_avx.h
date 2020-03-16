// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2017-2020, Intel Corporation */

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>

#include "pmem2_arch.h"
#include "avx.h"
#include "flush.h"
#include "memcpy_memset.h"
#include "memcpy_avx.h"
#include "valgrind_internal.h"

static force_inline void
memmove_movnt8x64b(char *dest, const char *src)
{
	__m256i ymm0 = _mm256_loadu_si256((__m256i *)src + 0);
	__m256i ymm1 = _mm256_loadu_si256((__m256i *)src + 1);
	__m256i ymm2 = _mm256_loadu_si256((__m256i *)src + 2);
	__m256i ymm3 = _mm256_loadu_si256((__m256i *)src + 3);
	__m256i ymm4 = _mm256_loadu_si256((__m256i *)src + 4);
	__m256i ymm5 = _mm256_loadu_si256((__m256i *)src + 5);
	__m256i ymm6 = _mm256_loadu_si256((__m256i *)src + 6);
	__m256i ymm7 = _mm256_loadu_si256((__m256i *)src + 7);
	__m256i ymm8 = _mm256_loadu_si256((__m256i *)src + 8);
	__m256i ymm9 = _mm256_loadu_si256((__m256i *)src + 9);
	__m256i ymm10 = _mm256_loadu_si256((__m256i *)src + 10);
	__m256i ymm11 = _mm256_loadu_si256((__m256i *)src + 11);
	__m256i ymm12 = _mm256_loadu_si256((__m256i *)src + 12);
	__m256i ymm13 = _mm256_loadu_si256((__m256i *)src + 13);
	__m256i ymm14 = _mm256_loadu_si256((__m256i *)src + 14);
	__m256i ymm15 = _mm256_loadu_si256((__m256i *)src + 15);

	_mm256_stream_si256((__m256i *)dest + 0, ymm0);
	_mm256_stream_si256((__m256i *)dest + 1, ymm1);
	_mm256_stream_si256((__m256i *)dest + 2, ymm2);
	_mm256_stream_si256((__m256i *)dest + 3, ymm3);
	_mm256_stream_si256((__m256i *)dest + 4, ymm4);
	_mm256_stream_si256((__m256i *)dest + 5, ymm5);
	_mm256_stream_si256((__m256i *)dest + 6, ymm6);
	_mm256_stream_si256((__m256i *)dest + 7, ymm7);
	_mm256_stream_si256((__m256i *)dest + 8, ymm8);
	_mm256_stream_si256((__m256i *)dest + 9, ymm9);
	_mm256_stream_si256((__m256i *)dest + 10, ymm10);
	_mm256_stream_si256((__m256i *)dest + 11, ymm11);
	_mm256_stream_si256((__m256i *)dest + 12, ymm12);
	_mm256_stream_si256((__m256i *)dest + 13, ymm13);
	_mm256_stream_si256((__m256i *)dest + 14, ymm14);
	_mm256_stream_si256((__m256i *)dest + 15, ymm15);

	VALGRIND_DO_FLUSH(dest, 8 * 64);
}

static force_inline void
memmove_movnt4x64b(char *dest, const char *src)
{
	__m256i ymm0 = _mm256_loadu_si256((__m256i *)src + 0);
	__m256i ymm1 = _mm256_loadu_si256((__m256i *)src + 1);
	__m256i ymm2 = _mm256_loadu_si256((__m256i *)src + 2);
	__m256i ymm3 = _mm256_loadu_si256((__m256i *)src + 3);
	__m256i ymm4 = _mm256_loadu_si256((__m256i *)src + 4);
	__m256i ymm5 = _mm256_loadu_si256((__m256i *)src + 5);
	__m256i ymm6 = _mm256_loadu_si256((__m256i *)src + 6);
	__m256i ymm7 = _mm256_loadu_si256((__m256i *)src + 7);

	_mm256_stream_si256((__m256i *)dest + 0, ymm0);
	_mm256_stream_si256((__m256i *)dest + 1, ymm1);
	_mm256_stream_si256((__m256i *)dest + 2, ymm2);
	_mm256_stream_si256((__m256i *)dest + 3, ymm3);
	_mm256_stream_si256((__m256i *)dest + 4, ymm4);
	_mm256_stream_si256((__m256i *)dest + 5, ymm5);
	_mm256_stream_si256((__m256i *)dest + 6, ymm6);
	_mm256_stream_si256((__m256i *)dest + 7, ymm7);

	VALGRIND_DO_FLUSH(dest, 4 * 64);
}

static force_inline void
memmove_movnt2x64b(char *dest, const char *src)
{
	__m256i ymm0 = _mm256_loadu_si256((__m256i *)src + 0);
	__m256i ymm1 = _mm256_loadu_si256((__m256i *)src + 1);
	__m256i ymm2 = _mm256_loadu_si256((__m256i *)src + 2);
	__m256i ymm3 = _mm256_loadu_si256((__m256i *)src + 3);

	_mm256_stream_si256((__m256i *)dest + 0, ymm0);
	_mm256_stream_si256((__m256i *)dest + 1, ymm1);
	_mm256_stream_si256((__m256i *)dest + 2, ymm2);
	_mm256_stream_si256((__m256i *)dest + 3, ymm3);

	VALGRIND_DO_FLUSH(dest, 2 * 64);
}

static force_inline void
memmove_movnt1x64b(char *dest, const char *src)
{
	__m256i ymm0 = _mm256_loadu_si256((__m256i *)src + 0);
	__m256i ymm1 = _mm256_loadu_si256((__m256i *)src + 1);

	_mm256_stream_si256((__m256i *)dest + 0, ymm0);
	_mm256_stream_si256((__m256i *)dest + 1, ymm1);

	VALGRIND_DO_FLUSH(dest, 64);
}

static force_inline void
memmove_movnt1x32b(char *dest, const char *src)
{
	__m256i ymm0 = _mm256_loadu_si256((__m256i *)src);

	_mm256_stream_si256((__m256i *)dest, ymm0);

	VALGRIND_DO_FLUSH(dest, 32);
}

static force_inline void
memmove_movnt1x16b(char *dest, const char *src)
{
	__m128i xmm0 = _mm_loadu_si128((__m128i *)src);

	_mm_stream_si128((__m128i *)dest, xmm0);

	VALGRIND_DO_FLUSH(dest, 16);
}

static force_inline void
memmove_movnt1x8b(char *dest, const char *src)
{
	_mm_stream_si64((long long *)dest, *(long long *)src);

	VALGRIND_DO_FLUSH(dest, 8);
}

static force_inline void
memmove_movnt1x4b(char *dest, const char *src)
{
	_mm_stream_si32((int *)dest, *(int *)src);

	VALGRIND_DO_FLUSH(dest, 4);
}

static force_inline void
memmove_movnt_avx_fw(char *dest, const char *src, size_t len)
{
	size_t cnt = (uint64_t)dest & 63;
	if (cnt > 0) {
		cnt = 64 - cnt;

		if (cnt > len)
			cnt = len;

		memmove_small_avx(dest, src, cnt);

		dest += cnt;
		src += cnt;
		len -= cnt;
	}

	while (len >= 8 * 64) {
		memmove_movnt8x64b(dest, src);
		dest += 8 * 64;
		src += 8 * 64;
		len -= 8 * 64;
	}

	if (len >= 4 * 64) {
		memmove_movnt4x64b(dest, src);
		dest += 4 * 64;
		src += 4 * 64;
		len -= 4 * 64;
	}

	if (len >= 2 * 64) {
		memmove_movnt2x64b(dest, src);
		dest += 2 * 64;
		src += 2 * 64;
		len -= 2 * 64;
	}

	if (len >= 1 * 64) {
		memmove_movnt1x64b(dest, src);

		dest += 1 * 64;
		src += 1 * 64;
		len -= 1 * 64;
	}

	if (len == 0)
		goto end;

	/* There's no point in using more than 1 nt store for 1 cache line. */
	if (util_is_pow2(len)) {
		if (len == 32)
			memmove_movnt1x32b(dest, src);
		else if (len == 16)
			memmove_movnt1x16b(dest, src);
		else if (len == 8)
			memmove_movnt1x8b(dest, src);
		else if (len == 4)
			memmove_movnt1x4b(dest, src);
		else
			goto nonnt;

		goto end;
	}

nonnt:
	memmove_small_avx(dest, src, len);
end:
	avx_zeroupper();
}

static force_inline void
memmove_movnt_avx_bw(char *dest, const char *src, size_t len)
{
	dest += len;
	src += len;

	size_t cnt = (uint64_t)dest & 63;
	if (cnt > 0) {
		if (cnt > len)
			cnt = len;

		dest -= cnt;
		src -= cnt;
		len -= cnt;

		memmove_small_avx(dest, src, cnt);
	}

	while (len >= 8 * 64) {
		dest -= 8 * 64;
		src -= 8 * 64;
		len -= 8 * 64;
		memmove_movnt8x64b(dest, src);
	}

	if (len >= 4 * 64) {
		dest -= 4 * 64;
		src -= 4 * 64;
		len -= 4 * 64;
		memmove_movnt4x64b(dest, src);
	}

	if (len >= 2 * 64) {
		dest -= 2 * 64;
		src -= 2 * 64;
		len -= 2 * 64;
		memmove_movnt2x64b(dest, src);
	}

	if (len >= 1 * 64) {
		dest -= 1 * 64;
		src -= 1 * 64;
		len -= 1 * 64;
		memmove_movnt1x64b(dest, src);
	}

	if (len == 0)
		goto end;

	/* There's no point in using more than 1 nt store for 1 cache line. */
	if (util_is_pow2(len)) {
		if (len == 32) {
			dest -= 32;
			src -= 32;
			memmove_movnt1x32b(dest, src);
		} else if (len == 16) {
			dest -= 16;
			src -= 16;
			memmove_movnt1x16b(dest, src);
		} else if (len == 8) {
			dest -= 8;
			src -= 8;
			memmove_movnt1x8b(dest, src);
		} else if (len == 4) {
			dest -= 4;
			src -= 4;
			memmove_movnt1x4b(dest, src);
		} else {
			goto nonnt;
		}

		goto end;
	}

nonnt:
	dest -= len;
	src -= len;
	memmove_small_avx(dest, src, len);
end:
	avx_zeroupper();
}

void
EXPORTED_SYMBOL(char *dest, const char *src, size_t len)
{
	LOG(37, "Intrinsic function: memcpy_nt_avx");

	if ((uintptr_t)dest - (uintptr_t)src >= len)
		memmove_movnt_avx_fw(dest, src, len);
	else
		memmove_movnt_avx_bw(dest, src, len);

	maybe_barrier();
}
