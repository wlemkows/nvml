/*
 * Copyright 2016, Intel Corporation
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
 * rpmem_obc_int.c -- integration test for rpmem_obc and rpmemd_obc modules
 */

#include "unittest.h"
#include "out.h"

#include "librpmem.h"
#include "rpmem.h"
#include "rpmem_proto.h"
#include "rpmem_common.h"
#include "rpmem_util.h"
#include "rpmem_obc.h"
#include "rpmemd_obc.h"
#include "rpmemd_log.h"
#include "base64.h"

#define POOL_SIZE	1024
#define NLANES		32
#define NLANES_RESP	16
#define PROVIDER	RPMEM_PROV_LIBFABRIC_SOCKETS
#define POOL_DESC	"pool_desc"
#define RKEY		0xabababababababab
#define RADDR		0x0101010101010101
#define PORT		1234
#define PERSIST_METHOD	RPMEM_PM_GPSPM
#define RESP_ATTR_INIT {\
	.port = PORT,\
	.rkey = RKEY,\
	.raddr = RADDR,\
	.persist_method = PERSIST_METHOD,\
	.nlanes = NLANES_RESP,\
}
#define REQ_ATTR_INIT {\
	.pool_size = POOL_SIZE,\
	.nlanes = NLANES,\
	.provider = PROVIDER,\
	.pool_desc = POOL_DESC,\
}
#define SIGNATURE	"<RPMEM>"
#define MAJOR		1
#define COMPAT_F	2
#define INCOMPAT_F	3
#define ROCOMPAT_F	4
#define POOLSET_UUID	"POOLSET_UUID0123"
#define UUID		"UUID0123456789AB"
#define NEXT_UUID	"NEXT_UUID0123456"
#define PREV_UUID	"PREV_UUID0123456"
#define USER_FLAGS	"USER_FLAGS012345"
#define POOL_ATTR_INIT {\
	.signature = SIGNATURE,\
	.major = MAJOR,\
	.compat_features = COMPAT_F,\
	.incompat_features = INCOMPAT_F,\
	.ro_compat_features = ROCOMPAT_F,\
	.poolset_uuid = POOLSET_UUID,\
	.uuid = UUID,\
	.next_uuid = NEXT_UUID,\
	.prev_uuid = PREV_UUID,\
	.user_flags = USER_FLAGS,\
}

TEST_CASE_DECLARE(client_create);
TEST_CASE_DECLARE(client_open);
TEST_CASE_DECLARE(server);

/*
 * client_create -- perform create request
 */
int
client_create(const struct test_case *tc, int argc, char *argv[])
{
	if (argc < 1)
		UT_FATAL("usage: %s <addr>[:<port>]", tc->name);

	char *target = argv[0];

	int ret;
	struct rpmem_obc *rpc;
	struct rpmem_req_attr req = REQ_ATTR_INIT;
	struct rpmem_pool_attr pool_attr = POOL_ATTR_INIT;
	struct rpmem_resp_attr ex_res = RESP_ATTR_INIT;
	struct rpmem_resp_attr res;

	rpc = rpmem_obc_init();
	UT_ASSERTne(rpc, NULL);

	ret = rpmem_obc_connect(rpc, target);
	UT_ASSERTeq(ret, 0);

	ret = rpmem_obc_monitor(rpc, 1);
	UT_ASSERTeq(ret, 1);

	ret = rpmem_obc_create(rpc, &req, &res, &pool_attr);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ex_res.port, res.port);
	UT_ASSERTeq(ex_res.rkey, res.rkey);
	UT_ASSERTeq(ex_res.raddr, res.raddr);
	UT_ASSERTeq(ex_res.persist_method, res.persist_method);
	UT_ASSERTeq(ex_res.nlanes, res.nlanes);

	ret = rpmem_obc_monitor(rpc, 1);
	UT_ASSERTeq(ret, 1);

	ret = rpmem_obc_close(rpc);
	UT_ASSERTeq(ret, 0);

	ret = rpmem_obc_disconnect(rpc);
	UT_ASSERTeq(ret, 0);

	rpmem_obc_fini(rpc);

	return 1;
}

/*
 * client_open -- perform open request
 */
int
client_open(const struct test_case *tc, int argc, char *argv[])
{
	if (argc < 1)
		UT_FATAL("usage: %s <addr>[:<port>]", tc->name);

	char *target = argv[0];

	int ret;
	struct rpmem_obc *rpc;
	struct rpmem_req_attr req = REQ_ATTR_INIT;
	struct rpmem_pool_attr ex_pool_attr = POOL_ATTR_INIT;
	struct rpmem_pool_attr pool_attr;
	struct rpmem_resp_attr ex_res = RESP_ATTR_INIT;
	struct rpmem_resp_attr res;

	rpc = rpmem_obc_init();
	UT_ASSERTne(rpc, NULL);

	ret = rpmem_obc_connect(rpc, target);
	UT_ASSERTeq(ret, 0);

	ret = rpmem_obc_monitor(rpc, 1);
	UT_ASSERTeq(ret, 1);

	ret = rpmem_obc_open(rpc, &req, &res, &pool_attr);
	UT_ASSERTeq(ret, 0);
	UT_ASSERTeq(ex_res.port, res.port);
	UT_ASSERTeq(ex_res.rkey, res.rkey);
	UT_ASSERTeq(ex_res.raddr, res.raddr);
	UT_ASSERTeq(ex_res.persist_method, res.persist_method);
	UT_ASSERTeq(ex_res.nlanes, res.nlanes);
	UT_ASSERTeq(memcmp(&ex_pool_attr, &pool_attr,
			sizeof(ex_pool_attr)), 0);

	ret = rpmem_obc_monitor(rpc, 1);
	UT_ASSERTeq(ret, 1);

	ret = rpmem_obc_close(rpc);
	UT_ASSERTeq(ret, 0);

	ret = rpmem_obc_disconnect(rpc);
	UT_ASSERTeq(ret, 0);

	rpmem_obc_fini(rpc);

	return 1;
}

/*
 * req_arg -- request callbacks argument
 */
struct req_arg {
	struct rpmem_resp_attr resp;
	struct rpmem_pool_attr pool_attr;
	int closing;
};

/*
 * req_create -- process create request
 */
static int
req_create(struct rpmemd_obc *obc, void *arg,
	const struct rpmem_req_attr *req,
	const struct rpmem_pool_attr *pool_attr)
{
	struct rpmem_req_attr ex_req = REQ_ATTR_INIT;
	struct rpmem_pool_attr ex_pool_attr = POOL_ATTR_INIT;
	UT_ASSERTne(arg, NULL);
	UT_ASSERTeq(ex_req.provider, req->provider);
	UT_ASSERTeq(ex_req.pool_size, req->pool_size);
	UT_ASSERTeq(ex_req.nlanes, req->nlanes);
	UT_ASSERTeq(strcmp(ex_req.pool_desc, req->pool_desc), 0);
	UT_ASSERTeq(memcmp(&ex_pool_attr, pool_attr, sizeof(ex_pool_attr)), 0);

	struct req_arg *args = arg;

	return rpmemd_obc_create_resp(obc, 0, &args->resp);
}

/*
 * req_open -- process open request
 */
static int
req_open(struct rpmemd_obc *obc, void *arg,
	const struct rpmem_req_attr *req)
{
	struct rpmem_req_attr ex_req = REQ_ATTR_INIT;
	UT_ASSERTne(arg, NULL);
	UT_ASSERTeq(ex_req.provider, req->provider);
	UT_ASSERTeq(ex_req.pool_size, req->pool_size);
	UT_ASSERTeq(ex_req.nlanes, req->nlanes);
	UT_ASSERTeq(strcmp(ex_req.pool_desc, req->pool_desc), 0);

	struct req_arg *args = arg;

	return rpmemd_obc_open_resp(obc, 0,
			&args->resp, &args->pool_attr);
}

/*
 * req_close -- process close request
 */
static int
req_close(struct rpmemd_obc *obc, void *arg)
{
	UT_ASSERTne(arg, NULL);

	struct req_arg *args = arg;
	args->closing = 1;

	return rpmemd_obc_close_resp(obc, 0);
}

/*
 * REQ -- server request callbacks
 */
struct rpmemd_obc_requests REQ = {
	.create = req_create,
	.open = req_open,
	.close = req_close,
};

/*
 * server -- run server and process clients requests
 */
int
server(const struct test_case *tc, int argc, char *argv[])
{
	int ret;

	struct req_arg arg = {
		.resp = RESP_ATTR_INIT,
		.pool_attr = POOL_ATTR_INIT,
		.closing = 0,
	};

	struct rpmemd_obc *obc;

	obc = rpmemd_obc_init(0, 1);
	UT_ASSERTne(obc, NULL);

	ret = rpmemd_obc_status(obc, 0);
	UT_ASSERTeq(ret, 0);


	while (1) {
		ret = rpmemd_obc_process(obc, &REQ, &arg);
		if (arg.closing) {
			break;
		} else {
			UT_ASSERTeq(ret, 0);
		}
	}

	ret = rpmemd_obc_process(obc, &REQ, &arg);
	UT_ASSERTeq(ret, 1);

	rpmemd_obc_fini(obc);

	return 0;
}

/*
 * test_cases -- available test cases
 */
static struct test_case test_cases[] = {
	TEST_CASE(server),
	TEST_CASE(client_create),
	TEST_CASE(client_open),
};

#define NTESTS	(sizeof(test_cases) / sizeof(test_cases[0]))

int
main(int argc, char *argv[])
{
	base64_init();
	START(argc, argv, "rpmem_obc");
	out_init("rpmem_fip",
		"RPMEM_LOG_LEVEL",
		"RPMEM_LOG_FILE", 0, 0);
	rpmemd_log_init("rpmemd", getenv("RPMEMD_LOG_FILE"), 0);
	rpmemd_log_level = rpmemd_log_level_from_str(
			getenv("RPMEMD_LOG_LEVEL"));

	TEST_CASE_PROCESS(argc, argv, test_cases, NTESTS);

	out_fini();
	rpmemd_log_close();
	DONE(NULL);
}
