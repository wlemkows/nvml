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
 * config.c -- pmem2_config implementation
 */

#include <unistd.h>
#include "alloc.h"
#include "config.h"
#include "libpmem2.h"
#include "out.h"
#include "pmem2.h"
#include "pmem2_utils.h"

/*
 * config_init -- (internal) initialize cfg structure.
 */
void
config_init(struct pmem2_config *cfg)
{
#ifdef _WIN32
	cfg->handle = INVALID_HANDLE_VALUE;
#else
	cfg->fd = INVALID_FD;
#endif
	cfg->offset = 0;
	cfg->length = 0;
	cfg->alignment = 0;
	cfg->requested_max_granularity =
		(enum pmem2_granularity)PMEM2_GRANULARITY_INVALID;
}

/*
 * pmem2_config_new -- allocates and initialize cfg structure.
 */
int
pmem2_config_new(struct pmem2_config **cfg)
{
	int ret;
	*cfg = pmem2_malloc(sizeof(**cfg), &ret);

	if (ret)
		return ret;

	ASSERTne(cfg, NULL);

	config_init(*cfg);
	return 0;
}

/*
 * pmem2_config_delete -- dealocate cfg structure.
 */
int
pmem2_config_delete(struct pmem2_config **cfg)
{
	Free(*cfg);
	*cfg = NULL;
	return 0;
}

/*
 * pmem2_config_set_required_store_granularity -- set granularity
 * requested by user in the pmem2_config structure
 */
int
pmem2_config_set_required_store_granularity(struct pmem2_config *cfg,
		enum pmem2_granularity g)
{
	switch (g) {
		case PMEM2_GRANULARITY_BYTE:
		case PMEM2_GRANULARITY_CACHE_LINE:
		case PMEM2_GRANULARITY_PAGE:
			break;
		default:
			ERR("unknown granularity value %d", g);
			return PMEM2_E_INVALID_ARG;
	}

	cfg->requested_max_granularity = g;

	return 0;
}
