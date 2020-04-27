// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2017-2020, Intel Corporation */

/*
 * ndctl_region_namespace.h -- internal definitions for libpmem2
 *                             common ndctl functions
 */

#ifndef PMDK_NDCTL_REGION_NAMESPACE_H
#define PMDK_NDCTL_REGION_NAMESPACE_H 1

#include "os.h"
#include "region_namespace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FOREACH_BUS_REGION_NAMESPACE(ctx, bus, region, ndns)	\
	ndctl_bus_foreach(ctx, bus)				\
	ndctl_region_foreach(bus, region)			\
	ndctl_namespace_foreach(region, ndns)

int ndctl_region_namespace(struct ndctl_ctx *ctx, const os_stat_t *st,
			struct ndctl_region **pregion,
			struct ndctl_namespace **pndns);

#ifdef __cplusplus
}
#endif

#endif /* PMDK_NDCTL_REGION_NAMESPACE_H */
