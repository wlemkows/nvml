---
layout: manual
Content-Style: 'text/css'
title: _MP(PMEMSET_REMOVE_PART_MAP, 3)
collection: libpmemset
header: PMDK
date: pmemset API version 1.0
...

[comment]: <> (SPDX-License-Identifier: BSD-3-Clause)
[comment]: <> (Copyright 2021, Intel Corporation)

[comment]: <> (pmemset_remove_part_map.3 -- man page for libpmemset pmemset_remove_part_map operation)

[NAME](#name)<br />
[SYNOPSIS](#synopsis)<br />
[DESCRIPTION](#description)<br />
[RETURN VALUE](#return-value)<br />
[ERRORS](#errors)<br />
[SEE ALSO](#see-also)<br />

# NAME #

**pmemset_remove_part_map**() - removes part mapping from the pmemset

# SYNOPSIS #

```c
#include <libpmemset.h>

struct pmemset;
struct pmemset_part_map;
int pmemset_remove_part_map(struct pmemset *set,
		struct pmemset_part_map **part_ptr);
```

# DESCRIPTION #

The **pmemset_remove_part_map**() function removes provided part mapping from the pmemset.
The mapping to be removed can be retrieved using **pmemset_first_part_map**(3),
**pmemset_next_part_map**(3) and **pmemset_part_map_by_address**(3) functions.

When the **pmemset_remove_part_map**() function succeeds it the part mapping
and sets the pointer passed via *part_ptr* variable to *NULL*.

# RETURN VALUE #

The **pmemset_remove_part_map**() function returns 0 on success
or a negative error code on failure.

# ERRORS #

The **pmemset_remove_part_map**() can fail with the following errors:

* **PMEMSET_E_PART_NOT_FOUND** - provided part wasn't found in the pmemset

# SEE ALSO #

**pmemset_first_part_map**(3), **pmemset_next_part_map**(3),
**pmemset_part_map_by_address**(3),
**libpmemset**(7) and **<http://pmem.io>**
