/*
 * Copyright 2017-2018, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * os_deep_persist_linux.c -- Linux abstraction layer
 */

#define _GNU_SOURCE

#include <inttypes.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "out.h"
#include "os.h"
#include "mmap.h"
#include "file.h"
#include "libpmem.h"
#include "os_deep_persist.h"

/*
 * os_deep_flush_write -- (internal) perform write to deep_flush file
 * on given region_id
 */
static int
os_deep_flush_write(int region_id)
{
	LOG(3, "region_id %d", region_id);

	char deep_flush_path[PATH_MAX];
	int deep_flush_fd;

	snprintf(deep_flush_path, PATH_MAX,
		"/sys/bus/nd/devices/region%d/deep_flush", region_id);

	if ((deep_flush_fd = os_open(deep_flush_path, O_WRONLY)) < 0) {
		ERR("!os_open(\"%s\", O_RDWR)", deep_flush_path);
		return -1;
	}

	if (write(deep_flush_fd, "1", 1) != 1) {
		ERR("!write(%d, \"1\")", deep_flush_fd);
		int oerrno = errno;
		os_close(deep_flush_fd);
		errno = oerrno;
		return -1;
	}

	os_close(deep_flush_fd);
	return 0;
}

/*
 * os_range_deep_persist -- perform deep persist of given address range
 */
int
os_range_deep_persist(uintptr_t addr, size_t len)
{
	LOG(3, "addr 0x%016" PRIxPTR " len %zu", addr, len);

	while (len != 0) {
		const struct map_tracker *mt = util_range_find(addr, len);

		/* no more overlapping track regions */
		if (mt == NULL) {
			LOG(15, "pmem_msync addr %p, len %lu",
				(void *)addr, len);
			return pmem_msync((void *)addr, len);
		}
		/*
		 * for input deep_persist range that cover found mapping
		 * it call write to deep_flush file, for addresses away
		 * from tracker range it calls msync
		 */
		if (mt->base_addr > addr) {
			size_t curr_len = mt->base_addr - addr;
			if (curr_len > len)
				curr_len = len;
			if (pmem_msync((void *)addr, curr_len) != 0)
				return -1;
			if ((len -= curr_len) == 0)
				return 0;
			addr = mt->base_addr;
		}
		size_t mt_in_len = mt->end_addr - addr;
		size_t persist_len = len;
		if (mt_in_len <= len)
			persist_len = mt_in_len;
		/* XXX: to be replaced with pmem_deep_flush() */
		LOG(15, "pmem_persist addr %p, len %lu",
			(void *)addr, persist_len);
		pmem_persist((void *)addr, persist_len);

		if (os_deep_flush_write(mt->region_id) < 0) {
			LOG(2, "cannot write to deep_flush in region %d",
				mt->region_id);
			return -1;
		}

		if (mt->end_addr >= addr + len)
			return 0;

		len -= mt_in_len;
		addr = mt->end_addr;
	}
	return 0;
}

/*
 * os_part_deep_persist -- search DEV dax region id for part file
 * on device dax and perform write to deep_flush file
 * or call msync for non DEV dax
 */
int
os_part_deep_persist(struct pool_set_part *part, void *addr, size_t len)
{
	LOG(3, "part %p addr %p len %lu", part, addr, len);

	if (part->is_dev_dax) {
		int region_id = util_ddax_region_find(part->path);
		ASSERTne(region_id, -1);
		/* XXX: to be replaced with pmem_deep_flush() */
		LOG(15, "pmem_persist addr %p, len %lu", addr, len);
		pmem_persist(addr, len);
		if (os_deep_flush_write(region_id)) {
			LOG(1, "ddax_deep_flush_write(%d)",
				region_id);
			return -1;
		}
	} else {
		if (pmem_msync(addr, len)) {
			LOG(1, "pmem_msync(%p, %lu)", addr, len);
			return -1;
		}
	}
	return 0;
}
