// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2020-2021, Intel Corporation */

/*
 * pmemset.c -- implementation of common pmemset API
 */

#include <stdbool.h>
#include "pmemset.h"
#include "libpmemset.h"
#include "libpmem2.h"
#include "part.h"
#include "pmemset_utils.h"
#include "ravl_interval.h"
#include "ravl.h"
#include "alloc.h"
#include "config.h"

/*
 * pmemset
 */
struct pmemset {
	struct pmemset_config *set_config;
	struct ravl_interval *part_map_tree;
	bool effective_granularity_valid;
	enum pmem2_granularity effective_granularity;
	pmem2_persist_fn persist_fn;
	pmem2_flush_fn flush_fn;
	pmem2_drain_fn drain_fn;
	struct pmemset_part_map *previous_pmap;
	enum pmemset_coalescing part_coalescing;
};

static char *granularity_name[3] = {
	"PMEM2_GRANULARITY_BYTE",
	"PMEM2_GRANULARITY_CACHE_LINE",
	"PMEM2_GRANULARITY_PAGE"
};

/*
 * pmemset_header
 */
struct pmemset_header {
	char stub;
};

/*
 * pmemset_mapping_min
 */
static size_t
pmemset_mapping_min(void *addr)
{
	if (addr == NULL)
		return 0;

	struct pmemset_part_map *pmap = (struct pmemset_part_map *)addr;
	return (size_t)pmap->desc.addr;
}

/*
 * pmemset_mapping_max
 */
static size_t
pmemset_mapping_max(void *addr)
{
	if (addr == NULL)
		return SIZE_MAX;

	struct pmemset_part_map *pmap = (struct pmemset_part_map *)addr;
	void *pmap_addr = pmap->desc.addr;
	size_t pmap_size = pmap->desc.size;
	return (size_t)pmap_addr + pmap_size;
}

/*
 * pmemset_new_init -- initialize set structure.
 */
static int
pmemset_new_init(struct pmemset *set, struct pmemset_config *config)
{
	ASSERTne(config, NULL);

	int ret;

	/* duplicate config */
	set->set_config = NULL;
	ret = pmemset_config_duplicate(&set->set_config, config);
	if (ret)
		return ret;

	/* initialize RAVL */
	set->part_map_tree = ravl_interval_new(pmemset_mapping_min,
						pmemset_mapping_max);

	if (set->part_map_tree == NULL) {
		ERR("ravl tree initialization failed");
		return PMEMSET_E_ERRNO;
	}

	set->effective_granularity_valid = false;
	set->previous_pmap = NULL;
	set->part_coalescing = PMEMSET_COALESCING_NONE;

	set->persist_fn = NULL;
	set->flush_fn = NULL;
	set->drain_fn = NULL;

	return 0;
}

/*
 * pmemset_new -- allocates and initialize pmemset structure.
 */
int
pmemset_new(struct pmemset **set, struct pmemset_config *cfg)
{
	PMEMSET_ERR_CLR();

	if (pmemset_get_config_granularity_valid(cfg) == false) {
		ERR(
			"please define the max granularity requested for the mapping");

		return PMEMSET_E_GRANULARITY_NOT_SET;
	}

	int ret = 0;

	/* allocate set structure */
	*set = pmemset_malloc(sizeof(**set), &ret);

	if (ret)
		return ret;

	ASSERTne(set, NULL);

	/* initialize set */
	ret = pmemset_new_init(*set, cfg);

	if (ret) {
		Free(*set);
		*set = NULL;
	}

	return ret;
}

/*
 * pmemset_unmap_stored_part_maps_callback -- unmaps and deletes part mappings
 *                                            stored in the ravl tree
 */
static void
pmemset_unmap_stored_part_maps_callback(void *data, void *arg)
{
	struct pmemset_part_map **pmap = (struct pmemset_part_map **)data;
	pmemset_part_map_delete(pmap);
}

/*
 * pmemset_delete -- de-allocate set structure
 */
int
pmemset_delete(struct pmemset **set)
{
	LOG(3, "pmemset %p", set);
	PMEMSET_ERR_CLR();

	if (*set == NULL)
		return 0;

	/* delete RAVL tree with part_map nodes */
	ravl_interval_delete_cb((*set)->part_map_tree,
			pmemset_unmap_stored_part_maps_callback, NULL);

	/* delete cfg */
	pmemset_config_delete(&(*set)->set_config);

	Free(*set);

	*set = NULL;

	return 0;
}

/*
 * pmemset_insert_part_map -- insert part mapping into the ravl interval tree
 */
static int
pmemset_insert_part_map(struct pmemset *set, struct pmemset_part_map *map)
{
	int ret = ravl_interval_insert(set->part_map_tree, map);
	if (ret == 0) {
		return 0;
	} else if (ret == -EEXIST) {
		ERR("part already exists");
		return PMEMSET_E_PART_EXISTS;
	} else {
		return PMEMSET_E_ERRNO;
	}
}

/*
 * pmemset_unregister_part_map -- unregister part mapping from the ravl
 *                                interval tree
 */
static int
pmemset_unregister_part_map(struct pmemset *set, struct pmemset_part_map *map)
{
	int ret = 0;

	struct ravl_interval *tree = set->part_map_tree;
	struct ravl_interval_node *node = ravl_interval_find_equal(tree, map);

	if (!(node && !ravl_interval_remove(tree, node))) {
		ERR("cannot find part mapping %p in the set %p", map, set);
		ret = PMEMSET_E_PART_NOT_FOUND;
	}

	return ret;
}

/*
 * pmemset_set_store_granularity -- set effective_graunlarity
 * in the pmemset structure
 */
static void
pmemset_set_store_granularity(struct pmemset *set, enum pmem2_granularity g)
{
	LOG(3, "set %p g %d", set, g);
	set->effective_granularity = g;
}

/*
 * pmemset_get_store_granularity -- get effective_graunlarity
 * from pmemset
 */
int
pmemset_get_store_granularity(struct pmemset *set, enum pmem2_granularity *g)
{
	LOG(3, "%p", set);

	if (set->effective_granularity_valid == false) {
		ERR(
			"effective granularity value for pmemset is not set, no part is mapped");
		return PMEMSET_E_NO_PART_MAPPED;
	}

	*g = set->effective_granularity;

	return 0;
}

/*
 * pmemset_set_persisting_fn -- sets persist, flush and
 * drain functions for pmemset
 */
static void
pmemset_set_persisting_fn(struct pmemset *set, struct pmemset_part_map *pmap)
{
	if (!pmap)
		return;

	struct pmem2_map *p2m;
	struct pmem2_vm_reservation *pmem2_reserv = pmap->pmem2_reserv;
	size_t pmem2_reserv_size = pmem2_vm_reservation_get_size(pmem2_reserv);
	pmem2_vm_reservation_map_find(pmem2_reserv, 0, pmem2_reserv_size, &p2m);

	/* should be set only once per pmemset */
	if (!set->persist_fn)
		set->persist_fn = pmem2_get_persist_fn(p2m);
	if (!set->flush_fn)
		set->flush_fn = pmem2_get_flush_fn(p2m);
	if (!set->drain_fn)
		set->drain_fn = pmem2_get_drain_fn(p2m);
}

/*
 * pmemset_part_map -- create new part mapping and add it to the set
 */
int
pmemset_part_map(struct pmemset_part **part, struct pmemset_extras *extra,
		struct pmemset_part_descriptor *desc)
{
	LOG(3, "part %p extra %p desc %p", part, extra, desc);
	PMEMSET_ERR_CLR();

	struct pmemset_part *p = *part;
	struct pmemset_part_map *part_map;
	struct pmemset *set = pmemset_part_get_pmemset(p);
	struct pmemset_config *set_config = pmemset_get_pmemset_config(set);
	enum pmem2_granularity mapping_gran;
	enum pmem2_granularity config_gran =
		pmemset_get_config_granularity(set_config);

	int ret = pmemset_part_map_new(&part_map, p, config_gran, &mapping_gran,
			set->previous_pmap, set->part_coalescing);
	if (ret)
		return ret;

	/*
	 * effective granularity is only set once and
	 * must have the same value for each mapping
	 */

	bool effective_gran_valid = set->effective_granularity_valid;

	if (effective_gran_valid == false) {
		pmemset_set_store_granularity(set, mapping_gran);
		set->effective_granularity_valid = true;
	} else {
		enum pmem2_granularity set_effective_gran;
		ret = pmemset_get_store_granularity(set, &set_effective_gran);
		ASSERTeq(ret, 0);

		if (set_effective_gran != mapping_gran) {
			ERR(
				"the part granularity is %s, all parts in the set must have the same granularity %s",
				granularity_name[mapping_gran],
				granularity_name[set_effective_gran]);
			ret = PMEMSET_E_GRANULARITY_MISMATCH;
			goto err_delete_pmap;
		}
	}

	pmemset_set_persisting_fn(set, part_map);

	if (part_map) {
		ret = pmemset_insert_part_map(set, part_map);
		if (ret)
			goto err_delete_pmap;
		set->previous_pmap = part_map;
	}

	if (desc)
		*desc = part_map ? part_map->desc :
				set->previous_pmap->desc;

	pmemset_part_delete(part);

	return 0;

err_delete_pmap:
	if (part_map)
		pmemset_part_map_delete(&part_map);
	else
		pmemset_part_map_delete(&set->previous_pmap);
	return ret;
}

#ifndef _WIN32
/*
 * pmemset_header_init -- not supported
 */
int
pmemset_header_init(struct pmemset_header *header, const char *layout,
		int major, int minor)
{
	return PMEMSET_E_NOSUPP;
}
#else
/*
 * pmemset_header_initU -- not supported
 */
int
pmemset_header_initU(struct pmemset_header *header, const char *layout,
		int major, int minor)
{
	return PMEMSET_E_NOSUPP;
}

/*
 * pmemset_header_initW -- not supported
 */
int
pmemset_header_initW(struct pmemset_header *header, const wchar_t *layout,
		int major, int minor)
{
	return PMEMSET_E_NOSUPP;
}
#endif

/*
 * pmemset_remove_part_map -- unmaps the part and removes it from the set
 */
int
pmemset_remove_part_map(struct pmemset *set, struct pmemset_part_map **part_ptr)
{
	LOG(3, "set %p part map %p", set, part_ptr);
	PMEMSET_ERR_CLR();

	struct pmemset_part_map *pmap = *part_ptr;
	struct pmem2_vm_reservation *rsv = pmap->pmem2_reserv;
	size_t rsv_size = pmem2_vm_reservation_get_size(rsv);

	int ret = pmemset_unregister_part_map(set, pmap);
	if (ret)
		return ret;

	struct pmem2_map *map;
	pmem2_vm_reservation_map_find(rsv, 0, rsv_size, &map);
	while (map) {
		ret = pmem2_map_delete(&map);
		if (ret)
			return ret;

		pmem2_vm_reservation_map_find(rsv, 0, rsv_size, &map);
	}

	ret = pmem2_vm_reservation_delete(&rsv);
	if (ret)
		return ret;

	/*
	 * if the part mapping to be removed is the same as the one being stored
	 * in the pmemset to map parts contiguously, then update it
	 */
	if (set->previous_pmap == pmap) {
		struct ravl_interval_node *node;
		node = ravl_interval_find_closest_prior(set->part_map_tree,
				pmap);
		if (!node)
			node = ravl_interval_find_closest_later(
				set->part_map_tree, pmap);

		set->previous_pmap = (node) ? ravl_interval_data(node) : NULL;
	}

	Free(pmap);
	*part_ptr = NULL;

	return 0;
}

/*
 * pmemset_remove_range -- not supported
 */
int
pmemset_remove_range(struct pmemset *set, void *addr, size_t len)
{
	return PMEMSET_E_NOSUPP;
}

/*
 * pmemset_persist -- persists stores from provided range
 */
int
pmemset_persist(struct pmemset *set, const void *ptr, size_t size)
{
	LOG(15, "ptr %p size %zu", ptr, size);

	/*
	 * someday, for debug purposes, we can validate
	 * if ptr and size belongs to the set
	 */
	set->persist_fn(ptr, size);
	return 0;
}

/*
 * pmemset_flush -- flushes stores from passed range
 */
int
pmemset_flush(struct pmemset *set, const void *ptr, size_t size)
{
	LOG(15, "ptr %p size %zu", ptr, size);

	/*
	 * someday, for debug purposes, we can validate
	 * if ptr and size belongs to the set
	 */
	set->flush_fn(ptr, size);
	return 0;
}

/*
 * pmemset_drain -- drain stores
 */
int
pmemset_drain(struct pmemset *set)
{
	LOG(15, "set %p", set);

	set->drain_fn();
	return 0;
}

/*
 * pmemset_memmove -- not supported
 */
int
pmemset_memmove(struct pmemset *set, void *pmemdest, const void *src,
		size_t len, unsigned flags)
{
	return PMEMSET_E_NOSUPP;
}

/*
 * pmemset_memcpy -- not supported
 */
int
pmemset_memcpy(struct pmemset *set, void *pmemdest, const void *src, size_t len,
		unsigned flags)
{
	return PMEMSET_E_NOSUPP;
}

/*
 * pmemset_memset -- not supported
 */
int
pmemset_memset(struct pmemset *set, void *pmemdest, int c, size_t len,
		unsigned flags)
{
	return PMEMSET_E_NOSUPP;
}

/*
 * deep_flush_pmem2_maps_from_rsv -- perform pmem2 deep flush
 * for each pmem2_map from reservation from range.
 * This function sets *end* param to true if in the reservation
 * is last pmem2 map from the provided pmemset_deep_flush range
 * or the reservation end arrder is gt than range and addr.
 */
static int
deep_flush_pmem2_maps_from_rsv(struct pmem2_vm_reservation *rsv,
		char *range_ptr, char *range_end, bool *end)
{
	int ret = 0;
	struct pmem2_map *map;
	size_t rsv_len = pmem2_vm_reservation_get_size(rsv);
	char *rsv_addr = pmem2_vm_reservation_get_address(rsv);
	size_t off = 0;
	char *map_addr;
	char *map_end;
	size_t map_size;
	char *flush_addr;
	size_t flush_size;
	size_t len = rsv_len;
	*end = false;

	while (*end == false && ret == 0) {
		ret = pmem2_vm_reservation_map_find(rsv, off, len, &map);
		if (ret == PMEM2_E_MAPPING_NOT_FOUND) {
			ret = 0;
			if (range_end <= rsv_addr + rsv_len)
				*end = true;
			break;
		}

		map_size = pmem2_map_get_size(map);
		map_addr = pmem2_map_get_address(map);
		map_end = map_addr + map_size;

		flush_addr = map_addr;
		flush_size = map_size;

		if (range_end <= map_addr) {
			*end = true;
			break;
		}
		if (range_ptr >= map_end)
			goto next;

		if (range_ptr >= map_addr && range_ptr < map_end)
			flush_addr = range_ptr;

		if (range_end <= map_end) {
			flush_size = (size_t)(range_end - flush_addr);
			*end = true;
		} else {
			flush_size = (size_t)(map_end - flush_addr);
		}

		ret = pmem2_deep_flush(map, flush_addr, flush_size);
		if (ret) {
			ERR("cannot perform deep flush on the reservation");
			ret = PMEMSET_E_DEEP_FLUSH_FAIL;
		}
	next:
		off = (size_t)(map_end - rsv_addr);
		len = rsv_len - off;
	}

	return ret;
}

/*
 * pmemset_deep_flush -- perform deep flush operation
 */
int
pmemset_deep_flush(struct pmemset *set, void *ptr, size_t size)
{
	LOG(3, "set %p ptr %p size %zu", set, ptr, size);
	PMEMSET_ERR_CLR();

	struct pmemset_part_map *pmap = NULL;
	struct pmemset_part_map *next_pmap = NULL;

	int ret = pmemset_part_map_by_address(set, &pmap, ptr);
	if (ret == PMEMSET_E_CANNOT_FIND_PART_MAP) {
		struct pmemset_part_map cur;
		cur.desc.addr = ptr;
		cur.desc.size = 1;

		pmemset_next_part_map(set, &cur, &next_pmap);

		if (!next_pmap)
			return 0;
		pmap = next_pmap;
	}

	struct pmem2_vm_reservation *rsv = pmap->pmem2_reserv;
	void *rsv_addr = pmem2_vm_reservation_get_address(rsv);
	char *range_end = (char *)ptr + size;

	do {
		bool end;
		ret = deep_flush_pmem2_maps_from_rsv(rsv, (char *)ptr,
				range_end, &end);
		if (ret || end)
			break;

		pmemset_next_part_map(set, pmap, &next_pmap);
		if (next_pmap == NULL)
			break;
		rsv = next_pmap->pmem2_reserv;
		rsv_addr = pmem2_vm_reservation_get_address(rsv);

	} while ((char *)rsv_addr < range_end);

	return ret;
}

/*
 * pmemset_get_pmemset_config -- get pmemset config
 */
struct pmemset_config *
pmemset_get_pmemset_config(struct pmemset *set)
{
	LOG(3, "%p", set);
	return set->set_config;
}

/*
 * pmemset_part_map_access -- gains access to the part mapping
 */
static void
pmemset_part_map_access(struct pmemset_part_map *pmap)
{
	pmap->refcount += 1;
}

/*
 * pmemset_part_map_access_drop -- drops the access to the part mapping
 */
static void
pmemset_part_map_access_drop(struct pmemset_part_map *pmap)
{
	pmap->refcount -= 1;
	ASSERT(pmap->refcount >= 0);
}

/*
 * pmemset_first_part_map -- retrieve first part map from the set
 */
void
pmemset_first_part_map(struct pmemset *set, struct pmemset_part_map **pmap)
{
	LOG(3, "set %p pmap %p", set, pmap);
	PMEMSET_ERR_CLR();

	*pmap = NULL;

	struct ravl_interval_node *first = ravl_interval_find_first(
			set->part_map_tree);

	if (first) {
		*pmap = ravl_interval_data(first);
		pmemset_part_map_access(*pmap);
	}

}

/*
 * pmemset_next_part_map -- retrieve successor part map in the set
 */
void
pmemset_next_part_map(struct pmemset *set, struct pmemset_part_map *cur,
		struct pmemset_part_map **next)
{
	LOG(3, "set %p cur %p next %p", set, cur, next);
	PMEMSET_ERR_CLR();

	*next = NULL;

	struct ravl_interval_node *found = ravl_interval_find_next(
			set->part_map_tree, cur);

	if (found) {
		*next = ravl_interval_data(found);
		pmemset_part_map_access(*next);
	}
}

/*
 * pmemset_part_map_by_address -- returns part map by passed address
 */
int
pmemset_part_map_by_address(struct pmemset *set, struct pmemset_part_map **pmap,
		void *addr)
{
	LOG(3, "set %p pmap %p addr %p", set, pmap, addr);
	PMEMSET_ERR_CLR();

	*pmap = NULL;

	struct pmemset_part_map ppm;
	ppm.desc.addr = addr;
	ppm.desc.size = 1;

	struct ravl_interval_node *node;
	node = ravl_interval_find(set->part_map_tree, &ppm);

	if (!node) {
		ERR("cannot find part_map at addr %p in the set %p", addr, set);
		return PMEMSET_E_CANNOT_FIND_PART_MAP;
	}

	*pmap = (struct pmemset_part_map *)ravl_interval_data(node);
	pmemset_part_map_access(*pmap);

	return 0;
}

/*
 * pmemset_map_descriptor -- create and return a part map descriptor
 */
struct pmemset_part_descriptor
pmemset_descriptor_part_map(struct pmemset_part_map *pmap)
{
	return pmap->desc;
}

/*
 * pmemset_part_map_drop -- drops the reference to the part map through provided
 *                          pointer. Doesn't delete part map.
 */
void
pmemset_part_map_drop(struct pmemset_part_map **pmap)
{
	LOG(3, "pmap %p", pmap);

	pmemset_part_map_access_drop(*pmap);
	*pmap = NULL;
}

/*
 * pmemset_set_contiguous_part_coalescing -- sets the part coalescing feature
 *                                           in the provided set on or off
 */
int
pmemset_set_contiguous_part_coalescing(struct pmemset *set,
		enum pmemset_coalescing value)
{
	LOG(3, "set %p coalescing %d", set, value);

	switch (value) {
		case PMEMSET_COALESCING_NONE:
		case PMEMSET_COALESCING_OPPORTUNISTIC:
		case PMEMSET_COALESCING_FULL:
			break;
		default:
			ERR("invalid coalescing value %d", value);
			return PMEMSET_E_INVALID_COALESCING_VALUE;
	}
	set->part_coalescing = value;

	return 0;
}
