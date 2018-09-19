/*
 * Copyright 2018, Intel Corporation
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
 * pmreorder_obj.c -- basic obj pmreorder marker tests
 *
 */

#include <stddef.h>

#include "unittest.h"
#include "util.h"
#include "valgrind_internal.h"

#define TEST_STR_LEN 8
#define TEST_VALUE 5
#define MAX_BUF_LEN 10
/*
 * Layout definition
 */
POBJ_LAYOUT_BEGIN(basic);
POBJ_LAYOUT_ROOT(basic, struct dummy_root);
POBJ_LAYOUT_TOID(basic, struct dummy_node);
POBJ_LAYOUT_TOID(basic, struct dummy_node_c);
POBJ_LAYOUT_END(basic);

struct dummy_node {
	int value;
	char teststr[TEST_STR_LEN];
	POBJ_LIST_ENTRY(struct dummy_node) plist;
	POBJ_LIST_ENTRY(struct dummy_node) plist_m;
};

struct dummy_node_c {
	int value;
	char teststr[TEST_STR_LEN];
	POBJ_LIST_ENTRY(struct dummy_node) plist;
	POBJ_LIST_ENTRY(struct dummy_node) plist_m;
};

struct dummy_root {
	int value;
	PMEMmutex lock;
	TOID(struct dummy_node) node;
	POBJ_LIST_HEAD(dummy_list, struct dummy_node) dummies;
	POBJ_LIST_HEAD(moved_list, struct dummy_node) moved;
};

struct my_root {
	char buf[MAX_BUF_LEN];
};

static int
dummy_node_constructor(PMEMobjpool *pop, void *ptr, void *arg)
{
	struct dummy_node *n = (struct dummy_node *)ptr;
	int *test_val = (int *)arg;
	n->value = *test_val;
	pmemobj_persist(pop, &n->value, sizeof(n->value));

	return 0;
}

static void
test_alloc_api(PMEMobjpool *pop)
{
	TOID(struct dummy_node) node_zeroed;
	TOID(struct dummy_node_c) node_constructed;

	POBJ_ZNEW(pop, &node_zeroed, struct dummy_node);

	UT_ASSERT_rt(OID_INSTANCEOF(node_zeroed.oid, struct dummy_node));

	int *test_val = (int *)MALLOC(sizeof(*test_val));
	*test_val = TEST_VALUE;
	POBJ_NEW(pop, &node_constructed, struct dummy_node_c,
			dummy_node_constructor, test_val);

	FREE(test_val);
}

static void
test_tx(PMEMobjpool *pop)
{
	PMEMoid root = pmemobj_root(pop, sizeof(struct my_root));
	struct my_root *rootp = pmemobj_direct(root);

	char buf[MAX_BUF_LEN] = {0};
	memset(buf, 'a', sizeof(buf));
	buf[MAX_BUF_LEN - 1] = '\0';
	VALGRIND_EMIT_LOG("BEFORE_TRANSACTION");
	TX_BEGIN(pop) {
		pmemobj_tx_add_range(root, 0, sizeof(struct my_root));
		memcpy(rootp->buf, buf, strlen(buf));
	} TX_END
	VALGRIND_EMIT_LOG("AFTER_TRANSACTION");
}

int
main(int argc, char *argv[])
{
	START(argc, argv, "pmreorder_obj");

	util_init();

	if (argc != 2)
		UT_FATAL("usage: %s file-name", argv[0]);

	const char *path = argv[1];

	PMEMobjpool *pop = NULL;
	VALGRIND_EMIT_LOG("WERONIKA");

	VALGRIND_EMIT_LOG("BEFORE PMEMOBJ_OPEN OUT");
	if ((pop = pmemobj_open(path, "intro")) == NULL)
		UT_FATAL("!pmemobj_open: %s", path);
	VALGRIND_EMIT_LOG("AFTER PMEMOBJ_OPEN OUT");

	test_alloc_api(pop);
	test_tx(pop);

	pmemobj_close(pop);

	DONE(NULL);
}
