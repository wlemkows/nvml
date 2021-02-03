---
layout: manual
Content-Style: 'text/css'
title: _MP(PMEMSET_PART_MAP, 3)
collection: libpmemset
header: PMDK
date: pmemset API version 1.0
...

[comment]: <> (SPDX-License-Identifier: BSD-3-Clause)
[comment]: <> (Copyright 2020, Intel Corporation)

[comment]: <> (pmemset_part_map.3 -- man page for libpmemset pmemset_part_map operation)

[NAME](#name)<br />
[SYNOPSIS](#synopsis)<br />
[DESCRIPTION](#description)<br />
[RETURN VALUE](#return-value)<br />
[ERRORS](#errors)<br />
[SEE ALSO](#see-also)<br />

# NAME #

**pmemset_part_map**() - creates a part mapping

# SYNOPSIS #

```c
#include <libpmemset.h>

struct pmemset_extras;
struct pmemset_part;
struct pmemset_part_descriptor;
struct pmemset_source;
int pmemset_part_map(struct pmemset_part **part, struct pmemset_extras *extra,
		struct pmemset_part_descriptor *desc)
```

# DESCRIPTION #

The **pmemset_part_map**() function creates new part mapping in the virtual address space
of the calling process and adds structure describing this mapping to the pmemset. It requires
an address of a pointer to initialized part provided in the *part* parameter. A part can be created using
**pmemset_part_new**(3) function. The mapping can later be retrieved using **pmemset_first_part_map**(3),
**pmemset_next_part_map**(3) and **pmemset_part_map_by_address**(3) functions.

Optionally **pmemset_part_map**() function can take a part descriptor object passed via *desc* parameter.
If an optional descriptor was provided then address and size of the part mapping are stored in the
descriptor when this function succeeds.

Before mapping any part, pmemset can have its contiguous part coalescing feature enabled or disabled.
With this feature enabled, **pmemset_part_map**() function tries to map each new part at the virtual
memory region that is situated right after the previous mapped part memory range. There's no guarantee
that the desired memory region is unoccupied, so in case it's occupied an error will be returned.
Mapping parts contiguously allows modifying the virtual address space of multiple parts with one operation
using for example **memset**(3). When this feature is disabled, each new part is mapped at the
memory region is arbitrarily chosen by the operating system.

When the **pmemset_part_map**() function succeeds it consumes the part thereby deleting it and
the variable pointed by *part* is set to NULL.

# RETURN VALUE #

The **pmemset_part_map**() function returns 0 on success
or a negative error code on failure.

# ERRORS #

The **pmemset_part_map**() can fail with the following errors:

* **PMEMSET_E_CANNOT_ALLOCATE_INTERNAL_STRUCTURE** - an internal structure
needed by the function cannot be allocated.

* **PMEMSET_E_INVALID_OFFSET_VALUE** - the offset value assigned to the part
is invalid. Offset value is bigger than INT64_MAX.

* **PMEMSET_E_GRANULARITY_NOT_SUPPORTED** - the granularity stored in the
provided part *part* is invalid. The concept of granularity is explained
in **libpmem2**(7) manpage.

* **PMEMSET_E_INVALID_PMEM2_MAP** - the pmem2 mapping that pmemset mapping relies on
cannot be created. The error code of **libpmem2**(7) error is printed in the logs and
can be checked for further information.

* **PMEMSET_E_LENGTH_UNALIGNED** - the length of the part to be mapped is no aligned
to the allocation granularity.

* **PMEMSET_E_CANNOT_COALESCE_PARTS** - new part couldn't be coalesced with previously
mapped part in the pmemset. The memory range after the ending address of previous
mapped part is occupied.

* **-ENOMEM** in case of insufficient memory to allocate an instance
of *struct pmemset_part_map*.

# SEE ALSO #

**pmemset_part_new**(3), **pmemset_first_part_map**(3),
**pmemset_next_part_map**(3), **pmemset_part_map_by_address**(3),
**libpmemset**(7), **libpmem2**(7),
and **<http://pmem.io>**
