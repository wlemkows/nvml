---
layout: manual
Content-Style: 'text/css'
title: _MP(PMEMSET_CONFIG_SET_REQUIRE_STORE_GRANULARITY, 3)
collection: libpmemset
header: PMDK
date: pmemset API version 1.0
...

[comment]: <> (SPDX-License-Identifier: BSD-3-Clause)
[comment]: <> (Copyright 2020, Intel Corporation)

[comment]: <> (pmemset_config_set_require_store_granularity.3 -- man page for pmemset_config_set_require_store_granularity)

[NAME](#name)<br />
[SYNOPSIS](#synopsis)<br />
[DESCRIPTION](#description)<br />
[RETURN VALUE](#return-value)<br />
[ERRORS](#errors)<br />
[SEE ALSO](#see-also)<br />

# NAME #

**pmemset_config_set_require_store_granularity**() - set granularity required for pmemset structure.

# SYNOPSIS #

```c
#include <libpmemset.h>

int pmemset_config_set_require_store_granularity(struct pmemset_config *cfg,
	enum pmem2_granularity g);
```

# DESCRIPTION #

The **pmemset_config_set_require_store_granularity**() sets granularity value *g* required for entire pmemset object.
It means that each part has to have the same granularity defined in advanced using **pmemset_config_set_require_store_granularity**().

Regardless of the pmemset source type, the **libpmemset(7)** library uses **libpmem2(7)** API to map parts.

For this reason, the granularity concept is also valid for **libpmemset(7)** funtions, and
granularity *g* must be one of the following values:

 * **PMEM2_GRANULARITY_BYTE**

 * **PMEM2_GRANULARITY_CACHE_LINE**

 * **PMEM2_GRANULARITY_PAGE**

For more information please read **pmem2_config_set_require_store_granularity(3)** man page
and section **GRANULARITY** in the **libpmem2(7)** man.

# RETURN VALUE

The **pmemset_config_set_require_store_granularity**() function returns 0 on success or a negative error code on failure.

# ERRORS #

The **pmemset_config_set_require_store_granularity**() can fail with the following error:

* **PMEMSET_E_GRANULARITY_NOT_SUPPORTED** - granularity *g* is not a valid value.

# SEE ALSO #

**pmem2_config_set_require_store_graularity**(3), **libpmem2**(7),
**libpmemset**(7) and **<http://pmem.io>**
