/*
 * Copyright 2019, Intel Corporation
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
 * obj_ulog_size.c -- unit tests for pmemobj_action API and
 * redo, undo logs
 */
#include <sys/param.h>
#include <string.h>
#include <stddef.h>

#include "unittest.h"

/* needed for defines */
#include "tx.h"

/*
 * lane.h -- needed for LANE_REDO_EXTERNAL_SIZE and LANE_UNDO_SIZE
 */
#include "lane.h"

#define LAYOUT_NAME "obj_ulog_size"
#define TEST_VALUE_1 1
#define TEST_VALUE_2 2

struct object {
	size_t value;
};

TOID_DECLARE(struct object, 0);
#define MIN_ALLOC 64
#define MAX_ALLOC (1024 * 1024)
#define HALF_OF_DEFAULT_UNDO_SIZE (LANE_UNDO_SIZE / 2) /* 1024 bytes */
#define LOG_BUFFER 0
#define LOG_BUFFER_NUM 6
#define RANGE (LOG_BUFFER + LOG_BUFFER_NUM)
#define RANGE_NUM 6
#define MIN_NOIDS (RANGE + RANGE_NUM)

/*
 * REDO_OVERFLOW -- size for trigger out of memory
 * during redo log extension
 */
#define REDO_OVERFLOW ((size_t)((LANE_REDO_EXTERNAL_SIZE\
		/ TX_INTENT_LOG_ENTRY_OVERHEAD) + 1))

/*
 * free_pool -- frees the pool from all allocated objects
 * and releases oids dynamic array
 */
static void
free_pool(PMEMoid *oids, size_t noids)
{
	for (int i = 0; i < noids; i++) {
		pmemobj_free(&oids[i]);
		UT_ASSERT(OID_IS_NULL(oids[i]));
	}

	FREE(oids);
}

/*
 * fill_pool -- fills provided pmemobj pool with as many allocations
 * as possible. Returns array of PMEMoids allocated from the
 * provided pool. The number of valid allocation stored in the
 * returned array is stored in the noids output argument.
 */

static PMEMoid *
fill_pool(PMEMobjpool *pop, size_t *noids)
{
	size_t oids_size = 2048; /* let's start with something big enough */
	PMEMoid *oids = (PMEMoid *)MALLOC(oids_size * sizeof(PMEMoid));

	*noids = 0;
	int ret;
	/* alloc as much space as possible */
	for (size_t size = MAX_ALLOC; size >= MIN_ALLOC; size /= 2) {
		ret = 0;

		while (ret == 0) {
			ret = pmemobj_alloc(pop, &oids[*noids], size,
				0, NULL, NULL);
			if (!ret)
				(*noids)++;
			if (*noids == oids_size) {
				oids_size *= 2;
				oids = (PMEMoid *)REALLOC(oids, oids_size);
			}
		}
	}
	return oids;
}

/*
 * do_tx_max_alloc_tx_publish_abort -- fills the pool and then tries
 * to overfill redo log - transaction abort expected
 */
static void
do_tx_max_alloc_tx_publish_abort(PMEMobjpool *pop)
{
	UT_OUT("do_tx_max_alloc_tx_publish_abort");
	PMEMoid *allocated = NULL;
	PMEMoid reservations[REDO_OVERFLOW];
	size_t nallocated = 0;
	struct pobj_action act[REDO_OVERFLOW];

	for (int i = 0; i < REDO_OVERFLOW; i++) {
		/* size is 64 - it can be any size */
		reservations[i] = pmemobj_reserve(pop, &act[i], 64, 0);
		UT_ASSERT(!OID_IS_NULL(reservations[i]));
	}

	allocated = fill_pool(pop, &nallocated);
	UT_ASSERT(nallocated >= MIN_NOIDS);

	/* it should abort - cannot extend redo log */
	TX_BEGIN(pop) {
		pmemobj_tx_publish(act, REDO_OVERFLOW);
	} TX_ONABORT {
		UT_OUT("!Cannot publish");
	} TX_ONCOMMIT {
		UT_FATAL("Can publish - can overfill redo log");
	} TX_END

	free_pool(allocated, nallocated);
	pmemobj_cancel(pop, act, REDO_OVERFLOW);
}

/*
 * do_ctl_overhead_test -- ctls read overhead and alignment
 * for buffer in ulog
 */
static void
do_ctl_overhead_test(PMEMobjpool *pop)
{
	int ret;
	unsigned ro_value = 0;

	ret = pmemobj_ctl_get(pop, "tx.snapshot.log.entry_alignment",
			&ro_value);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ro_value, TX_SNAPSHOT_LOG_ENTRY_ALIGNMENT);

	ro_value = 0;
	ret = pmemobj_ctl_get(pop, "tx.intent.log.buffer_alignment", &ro_value);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ro_value, TX_INTENT_LOG_BUFFER_ALIGNMENT);

	ro_value = 0;
	ret = pmemobj_ctl_get(pop, "tx.snapshot.log.entry_overhead", &ro_value);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ro_value, TX_SNAPSHOT_LOG_ENTRY_OVERHEAD);

	ro_value = 0;
	ret = pmemobj_ctl_get(pop, "tx.intent.log.entry_overhead", &ro_value);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ro_value, TX_INTENT_LOG_ENTRY_OVERHEAD);

	ro_value = 0;
	ret = pmemobj_ctl_get(pop, "tx.snapshot.log.buffer_overhead",
			&ro_value);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ro_value, TX_SNAPSHOT_LOG_BUFFER_OVERHEAD);

	ro_value = 0;
	ret = pmemobj_ctl_get(pop, "tx.intent.log.buffer_overhead", &ro_value);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ro_value, TX_INTENT_LOG_BUFFER_OVERHEAD);
}

/*
 * do_tx_max_alloc_no_user_alloc_snap -- fills the pool and tries to do
 * snapshot which is bigger than ulog size
 */
static void
do_tx_max_alloc_no_user_alloc_snap(PMEMobjpool *pop)
{
	UT_OUT("do_tx_max_alloc_no_user_alloc_snap");
	size_t noids = 0;
	PMEMoid *oids = fill_pool(pop, &noids);
	UT_ASSERT(noids >= MIN_NOIDS);

	/* querying snapshot size */
	size_t range_size =
		pmemobj_alloc_usable_size(oids[LOG_BUFFER]);
	UT_ASSERT(range_size > LANE_UNDO_SIZE);

	TX_BEGIN(pop) {
		/* it should abort - cannot allocate memory */
		pmemobj_tx_add_range(oids[LOG_BUFFER], 0, range_size);
	} TX_ONABORT {
		UT_OUT("!Cannot add snapshot");
	} TX_ONCOMMIT {
		UT_FATAL("Can add snapshot with size bigger than ulog");
	} TX_END

	free_pool(oids, noids);
}

/*
 * do_tx_max_alloc_user_alloc_snap -- fills the pool, appends allocated
 * buffer and tries to do snapshot which is bigger than ulog size
 */
static void
do_tx_max_alloc_user_alloc_snap(PMEMobjpool *pop)
{
	UT_OUT("do_tx_max_alloc_user_alloc_snap");
	size_t noids = 0;
	PMEMoid *oids = fill_pool(pop, &noids);
	UT_ASSERT(noids >= MIN_NOIDS);

	/* querying snapshot size */
	size_t buff_size =
		pmemobj_alloc_usable_size(oids[LOG_BUFFER]);
	void *buff_addr = pmemobj_direct(oids[LOG_BUFFER]);
	size_t range_size =
		pmemobj_alloc_usable_size(oids[RANGE]);
	UT_ASSERT(range_size > LANE_UNDO_SIZE);

	TX_BEGIN(pop) {
		pmemobj_tx_log_append_buffer(
			TX_LOG_TYPE_SNAPSHOT, buff_addr, buff_size);
		pmemobj_tx_add_range(oids[RANGE], 0, range_size);
	} TX_ONABORT {
		UT_FATAL("!Cannot add snapshot with size bigger than ulog\
			while buffer is allocated");
	} TX_ONCOMMIT {
		UT_OUT("Can add snapshot");
	} TX_END

	free_pool(oids, noids);
}

/*
 * do_tx_max_alloc_user_alloc_nested -- example of buffer appending
 * allocated by user in nested transaction
 */
static void
do_tx_max_alloc_user_alloc_nested(PMEMobjpool *pop)
{
	UT_OUT("do_tx_max_alloc_user_alloc_nested");
	size_t noids = 0;
	PMEMoid *oids = fill_pool(pop, &noids);
	UT_ASSERT(noids >= MIN_NOIDS);

	size_t buff_size =
		pmemobj_alloc_usable_size(oids[LOG_BUFFER]);
	void *buff_addr = pmemobj_direct(oids[LOG_BUFFER]);
	size_t range_size =
		pmemobj_alloc_usable_size(oids[RANGE]);

	TOID(struct object) obj0;
	TOID(struct object) obj1;
	TOID_ASSIGN(obj0, oids[RANGE]);
	TOID_ASSIGN(obj1, oids[RANGE + 1]);

	TX_BEGIN(pop) {
		D_RW(obj0)->value = TEST_VALUE_1;
		TX_BEGIN(pop) {
			pmemobj_tx_log_append_buffer(
				TX_LOG_TYPE_SNAPSHOT, buff_addr, buff_size);
			pmemobj_tx_add_range(oids[RANGE],
				0, range_size);
			D_RW(obj1)->value = TEST_VALUE_2;
		} TX_ONABORT {
			UT_FATAL("!Cannot add snapshot of buffer allocated by\
				user in nested transaction");
		} TX_ONCOMMIT {
			UT_OUT("Can add snapshot");
		} TX_END
	} TX_END

	UT_ASSERTeq(D_RO(obj1)->value, TEST_VALUE_2);
	free_pool(oids, noids);
}

/*
 * do_tx_max_alloc_user_alloc_snap_multi -- appending of many buffers
 * in one transaction
 */
static void
do_tx_max_alloc_user_alloc_snap_multi(PMEMobjpool *pop)
{
	UT_OUT("do_tx_max_alloc_user_alloc_snap_multi");
	size_t noids = 0;
	PMEMoid *oids = fill_pool(pop, &noids);
	UT_ASSERT(noids >= MIN_NOIDS);

	const unsigned long snap_size = 3;
	size_t buff_sizes[snap_size];
	void *buff_addrs[snap_size];
	size_t range_sizes[snap_size];

	for (unsigned long i = 0; i < snap_size; i++) {
		buff_sizes[i] = pmemobj_alloc_usable_size(oids[LOG_BUFFER + i]);
		buff_addrs[i] = pmemobj_direct(oids[LOG_BUFFER + i]);
		range_sizes[i] = pmemobj_alloc_usable_size(oids[RANGE + i]);
	}

	errno = 0;
	TX_BEGIN(pop) {
		pmemobj_tx_log_append_buffer(
			TX_LOG_TYPE_SNAPSHOT, buff_addrs[0], buff_sizes[0]);
		pmemobj_tx_log_append_buffer(
			TX_LOG_TYPE_SNAPSHOT, buff_addrs[1], buff_sizes[1]);
		pmemobj_tx_log_append_buffer(
			TX_LOG_TYPE_SNAPSHOT, buff_addrs[2], buff_sizes[2]);

		pmemobj_tx_add_range(oids[RANGE + 0], 0, range_sizes[0]);
		pmemobj_tx_add_range(oids[RANGE + 1], 0, range_sizes[1]);
		pmemobj_tx_add_range(oids[RANGE + 2], 0, range_sizes[2]);
	} TX_ONABORT {
		UT_FATAL("!Cannot add snapshot of appended buffers in\
			one transaction");
	} TX_ONCOMMIT {
		UT_OUT("Can add snapshot");
	} TX_END

	free_pool(oids, noids);
}

/*
 * do_tx_do_not_auto_reserve_snapshot -- blocking of automatic expansion
 * of ulog. When auto expansion of ulog is off, snapshot with size
 * of default undo log is going to fail, because of buffer overhead.
 */
static void
do_tx_do_not_auto_reserve_snapshot(PMEMobjpool *pop)
{
	UT_OUT("do_tx_do_not_auto_reserve_snapshot");
	PMEMoid oid0, oid1;

	int ret = pmemobj_alloc(pop, &oid0, HALF_OF_DEFAULT_UNDO_SIZE,
		0, NULL, NULL);
	UT_ASSERTeq(ret, 0);
	ret = pmemobj_alloc(pop, &oid1, HALF_OF_DEFAULT_UNDO_SIZE,
		0, NULL, NULL);
	UT_ASSERTeq(ret, 0);

	TX_BEGIN(pop) {
		pmemobj_tx_log_auto_alloc(TX_LOG_TYPE_SNAPSHOT, 0);
		pmemobj_tx_add_range(oid0, 0, HALF_OF_DEFAULT_UNDO_SIZE);
		/* it should abort - cannot extend ulog (first entry is full) */
		pmemobj_tx_add_range(oid1, 0, HALF_OF_DEFAULT_UNDO_SIZE);
	} TX_ONABORT {
		UT_OUT("!Cannot reserve more");
	} TX_ONCOMMIT {
		UT_FATAL("Can add snapshot with size of default undo\
			log while auto expansion of ulog is off");
	} TX_END

	pmemobj_free(&oid0);
	pmemobj_free(&oid1);
}

/*
 * do_tx_max_alloc_wrong_pop_addr -- allocates two pools and tries to
 * do transaction with the first pool and address from the second
 * pool. Abort expected - cannot allocate from different pool.
 */
static void
do_tx_max_alloc_wrong_pop_addr(PMEMobjpool *pop, PMEMobjpool *pop2)
{
	UT_OUT("do_tx_max_alloc_wrong_pop_addr");
	size_t noids = 0;
	PMEMoid *oids = fill_pool(pop, &noids);
	UT_ASSERT(noids >= MIN_NOIDS);
	PMEMoid oid2;

	int ret = pmemobj_alloc(pop2, &oid2, MAX_ALLOC, 0, NULL, NULL);
	UT_ASSERTeq(ret, 0);

	/* pools are allocated now, let's try to get address from wrong pool */
	size_t buff2_size = pmemobj_alloc_usable_size(oid2);
	void *buff2_addr = pmemobj_direct(oid2);

	/* abort expected - cannot allocate from different pool */
	TX_BEGIN(pop) {
		pmemobj_tx_log_append_buffer(
			TX_LOG_TYPE_SNAPSHOT, buff2_addr, buff2_size);
	} TX_ONABORT {
		UT_OUT("!Allocation from different pool");
	} TX_ONCOMMIT {
		UT_FATAL("Can append log buffer from a different memory pool");
	} TX_END

	free_pool(oids, noids);
	pmemobj_free(&oid2);
}

int
main(int argc, char *argv[])
{
	START(argc, argv, "obj_ulog_size");

	if (argc != 3)
		UT_FATAL("usage: %s [file] [file1]", argv[0]);

	PMEMobjpool *pop = pmemobj_create(argv[1], LAYOUT_NAME, 0,
		S_IWUSR | S_IRUSR);

	if (pop == NULL)
		UT_FATAL("!pmemobj_create");

	PMEMobjpool *pop2 = pmemobj_create(argv[2], LAYOUT_NAME, 0,
		S_IWUSR | S_IRUSR);

	if (pop2 == NULL)
		UT_FATAL("!pmemobj_create");

	do_ctl_overhead_test(pop);
	do_tx_max_alloc_no_user_alloc_snap(pop);
	do_tx_max_alloc_user_alloc_snap(pop);
	do_tx_max_alloc_user_alloc_nested(pop);
	do_tx_max_alloc_user_alloc_snap_multi(pop);
	do_tx_do_not_auto_reserve_snapshot(pop);
	do_tx_max_alloc_wrong_pop_addr(pop, pop2);
	do_tx_max_alloc_tx_publish_abort(pop);

	pmemobj_close(pop);
	pmemobj_close(pop2);

	DONE(NULL);
}
