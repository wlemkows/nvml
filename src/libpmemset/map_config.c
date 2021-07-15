// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2021, Intel Corporation */

/*
 * map_config.c -- implementation of common map_config API
 */

#include "alloc.h"
#include "libpmemset.h"
#include "map_config.h"
#include "pmemset.h"
#include "pmemset_utils.h"
#include "source.h"

struct pmemset_map_config {
	struct pmemset *set;
	size_t offset;
	size_t length;
	struct pmemset_file *file;
};

int pmemset_map_config_new(struct pmemset_map_config **map_cfg,
		struct pmemset *set)
{
	LOG(3, "map_cfg %p set %p", map_cfg, set);
	PMEMSET_ERR_CLR();

	int ret;
	struct pmemset_map_config *mapcfg;
	*map_cfg = NULL;

	mapcfg = pmemset_malloc(sizeof(*mapcfg), &ret);
	if (ret)
		return ret;

	ASSERTne(mapcfg, NULL);

	mapcfg->set = set;
	mapcfg->offset = 0;
	mapcfg->length = 0;
	*map_cfg = mapcfg;

	return ret;
}

/*
 * pmemset_map_config_set_source -- sets source in the map configuraton struct
 */
int
pmemset_map_config_set_source(struct pmemset_map_config *map_cfg,
	struct pmemset_source *src)
{
	LOG(3, "map_cfg %p src %p", map_cfg, src);
	PMEMSET_ERR_CLR();

	if (!src) {
		ERR("invalid pmemset source %p", src);
		return PMEMSET_E_INVALID_SOURCE_TYPE;
	}

	map_cfg->file = pmemset_source_get_set_file(src);

	return 0;
}

/*
 * pmemset_map_config_set_offset -- sets offset in the map configuraton struct
 */
void
pmemset_map_config_set_offset(struct pmemset_map_config *map_cfg,
	size_t offset)
{
	LOG(3, "map_cfg %p offset %zu", map_cfg, offset);

	map_cfg->offset = offset;
}

/*
 * pmemset_map_config_set_length -- sets length in the map configuraton struct
 */
void
pmemset_map_config_set_length(struct pmemset_map_config *map_cfg,
	size_t length)
{
	LOG(3, "map_cfg %p length %zu", map_cfg, length);

	map_cfg->length = length;
}

/*
 * pmemset_map_config_delete -- deletes pmemset map config
 */
int
pmemset_map_config_delete(struct pmemset_map_config **map_cfg)
{
	LOG(3, "map_cfg %p", map_cfg);
	PMEMSET_ERR_CLR();

	Free(*map_cfg);
	*map_cfg = NULL;

	return 0;
}

/*
 * pmemset_map_config_get_set -- return set assigned to the map config
 */
struct pmemset *
pmemset_map_config_get_set(struct pmemset_map_config *map_cfg)
{
	return map_cfg->set;
}

/*
 * pmemset_map_config_get_length -- return length assigned to the map config
 */
size_t
pmemset_map_config_get_length(struct pmemset_map_config *map_cfg)
{
	return map_cfg->length;
}

/*
 * pmemset_map_config_get_offset -- return offset assigned to the map config
 */
size_t
pmemset_map_config_get_offset(struct pmemset_map_config *map_cfg)
{
	return map_cfg->offset;
}

/*
 * pmemset_map_config_get_offset -- return offset assigned to the map config
 */
struct pmemset_file *
pmemset_map_config_get_file(struct pmemset_map_config *map_cfg)
{
	return map_cfg->file;
}
