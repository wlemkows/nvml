---
layout: manual
Content-Style: 'text/css'
title: libpmemobj
...

# libpmemobj

[NAME](#name)<br />
[SYNOPSIS](#synopsis)<br />
[DESCRIPTION](#description)<br />
[MOST COMMONLY USED FUNCTIONS](#most-commonly-used-functions)<br />
[LOW-LEVEL MEMORY MANIPULATION](#low-level-memory-manipulation)<br />
[POOL SETS AND REPLICAS](#pool-sets-and-replicas)<br />
[LOCKING](#locking)<br />
[PERSISTENT OBJECTS](#persistent-objects)<br />
[TYPE-SAFETY](#type-safety)<br />
[LAYOUT DECLARATION](#layout-declaration)<br />
[OBJECT CONTAINERS](#object-containers)<br />
[ROOT OBJECT MANAGEMENT](#root-object-management)<br />
[NON-TRANSACTIONAL ATOMIC ALLOCATIONS](#non-transactional-atomic-allocations)<br />
[NON-TRANSACTIONAL PERSISTENT ATOMIC LISTS](#non-transactional-persistent-allocations)<br />
[TYPE-SAFE NON-TRANSACTIONAL PERSISTENT ATOMIC LISTS](#type-safe-non-transactional-persistent-atomic-lists)<br />
[TRANSACTIONAL OBJECT MANIPULATION](#transactional-object-manipulation)<br />
[CAVEATS](#caveats)<br />
[LIBRARY API VERSIONING](#library-api-versioning)<br />
[MANAGING LIBRARY BEHAVIOR](#managing-library-versioning)<br />
[DEBUGGING AND ERROR HANDLING](#debugging-and-error-handling)<br />
[EXAMPLES](#examples)<br />
[ACKNOWLEDGEMENTS](#acknowledgements)<br />
[SEE ALSO](#see-also)<br />


### NAME

**libpmemobj** − persistent memory transactional object store


### SYNOPSIS

```
#include <libpmemobj.h>
```

cc -std=gnu99 ... -lpmemobj -lpmem

**Most commonly used functions:**

: PMEMobjpool \*pmemobj_open(const char \*path, const char \*layout);

  PMEMobjpool \*pmemobj_create(const char \*path, const char \*layout, size_t poolsize, mode_t mode);

  void pmemobj_close(PMEMobjpool \*pop);

**Low-level memory manipulation:**

: void \*pmemobj_memcpy_persist(PMEMobjpool \*pop, void \*dest, const void \*src, size_t len);

  void \*pmemobj_memset_persist(PMEMobjpool \*pop, void \*dest, int c, size_t len);

  void pmemobj_persist(PMEMobjpool \*pop, const void \*addr, size_t len);

  void pmemobj_flush(PMEMobjpool \*pop, const void \*addr, size_t len);

  void pmemobj_drain(PMEMobjpool \*pop);

**Locking:**

: void pmemobj_mutex_zero(PMEMobjpool \*pop, PMEMmutex \*mutexp);

  int pmemobj_mutex_lock(PMEMobjpool \*pop, PMEMmutex \*mutexp);

  int pmemobj_mutex_timedlock(PMEMobjpool \*pop, PMEMmutex \*restrict mutexp, const struct timespec \*restrict abs_timeout);

  int pmemobj_mutex_trylock(PMEMobjpool \*pop, PMEMmutex \*mutexp);

  int pmemobj_mutex_unlock(PMEMobjpool \*pop, PMEMmutex \*mutexp);

  void pmemobj_rwlock_zero(PMEMobjpool \*pop, PMEMrwlock \*rwlockp);

  int pmemobj_rwlock_rdlock(PMEMobjpool \*pop, PMEMrwlock \*rwlockp);

  int pmemobj_rwlock_wrlock(PMEMobjpool \*pop, PMEMrwlock \*rwlockp);

  int pmemobj_rwlock_timedrdlock(PMEMobjpool \*pop, PMEMrwlock \*restrict rwlockp, const struct timespec \*restrict abs_timeout);

  int pmemobj_rwlock_timedwrlock(PMEMobjpool \*pop, PMEMrwlock \*restrict rwlockp, const struct timespec \*restrict abs_timeout);

  int pmemobj_rwlock_tryrdlock(PMEMobjpool \*pop, PMEMrwlock \*rwlockp);

  int pmemobj_rwlock_trywrlock(PMEMobjpool \*pop, PMEMrwlock \*rwlockp);

  int pmemobj_rwlock_unlock(PMEMobjpool \*pop, PMEMrwlock \*rwlockp);

  void pmemobj_cond_zero(PMEMobjpool \*pop, PMEMcond \*condp);

  int pmemobj_cond_broadcast(PMEMobjpool \*pop, PMEMcond \*condp);

  int pmemobj_cond_signal(PMEMobjpool \*pop, PMEMcond \*condp);

  int pmemobj_cond_timedwait(PMEMobjpool \*pop, PMEMcond \*restrict condp, PMEMmutex \*restrict mutexp, const struct timespec \*restrict abs_timeout);

  int pmemobj_cond_wait(PMEMobjpool \*pop, PMEMcond \*condp, PMEMmutex \*restrict mutexp);

**Persistent object identifier:**

: OID_IS_NULL(PMEMoid oid)
  OID_EQUALS(PMEMoid lhs, PMEMoid rhs)

**Type-safety:**

: TOID(TYPE)
  TOID_DECLARE(TYPE, uint64_t type_num)
  TOID_DECLARE_ROOT(ROOT_TYPE)

  TOID_TYPE_NUM(TYPE)
  TOID_TYPE_NUM_OF(TOID oid)
  TOID_VALID(TOID oid)
  OID_INSTANCEOF(PMEMoid oid, TYPE)

  TOID_ASSIGN(TOID oid, VALUE)

  TOID_IS_NULL(TOID oid)
  TOID_EQUALS(TOID lhs, TOID rhs)
  DIRECT_RW(TOID oid)
  DIRECT_RO(TOID oid)
  D_RW(TOID oid)
  D_RO(TOID oid)

**Layout declaration:**

  POBJ_LAYOUT_BEGIN(layout)
  POBJ_LAYOUT_TOID(layout, TYPE)
  POBJ_LAYOUT_ROOT(layout, ROOT_TYPE)
  POBJ_LAYOUT_END(layout)
  POBJ_LAYOUT_NAME(layout)
  POBJ_LAYOUT_TYPES_NUM(layout)

**Non-transactional atomic allocations:**

typedef int (\*pmemobj_constr)(PMEMobjpool \*pop, void \*ptr, void \*arg);

int pmemobj_alloc(PMEMobjpool \*pop, PMEMoid \*oidp, size_t size, uint64_t type_num, pmemobj_constr constructor, void \*arg);
int pmemobj_zalloc(PMEMobjpool \*pop, PMEMoid \*oidp, size_t size, uint64_t type_num);
int pmemobj_realloc(PMEMobjpool \*pop, PMEMoid \*oidp, size_t size, uint64_t type_num);
int pmemobj_zrealloc(PMEMobjpool \*pop, PMEMoid \*oidp, size_t size, uint64_t type_num);
int pmemobj_strdup(PMEMobjpool \*pop, PMEMoid \*oidp, const char \*s, uint64_t type_num);
void pmemobj_free(PMEMoid \*oidp);
size_t pmemobj_alloc_usable_size(PMEMoid oid);
PMEMobjpool \*pmemobj_pool_by_oid(PMEMoid oid);
PMEMobjpool \*pmemobj_pool_by_ptr(const void \*addr);
void \*pmemobj_direct(PMEMoid oid);
uint64_t pmemobj_type_num(PMEMoid oid);

POBJ_NEW(PMEMobjpool \*pop, TOID \*oidp, TYPE, pmemobj_constr constructor, void \*arg)
POBJ_ALLOC(PMEMobjpool \*pop, TOID \*oidp, TYPE, size_t size, pmemobj_constr constructor, void \*arg)
POBJ_ZNEW(PMEMobjpool \*pop, TOID \*oidp, TYPE)
POBJ_ZALLOC(PMEMobjpool \*pop, TOID \*oidp, TYPE, size_t size)
POBJ_REALLOC(PMEMobjpool \*pop, TOID \*oidp, TYPE, size_t size)
POBJ_ZREALLOC(PMEMobjpool \*pop, TOID \*oidp, TYPE, size_t size)
POBJ_FREE(TOID \*oidp)

**Root object management:**

PMEMoid pmemobj_root(PMEMobjpool \*pop, size_t size);
PMEMoid pmemobj_root_construct(PMEMobjpool \*pop, size_t size, pmemobj_constr constructor, void \*arg);
size_t pmemobj_root_size(PMEMobjpool \*pop);

POBJ_ROOT(PMEMobjpool \*pop, TYPE)

**Object containers:**

PMEMoid pmemobj_first(PMEMobjpool \*pop);
PMEMoid pmemobj_next(PMEMoid oid);

POBJ_FIRST_TYPE_NUM(PMEMobjpool \*pop, uint64_t type_num)
POBJ_FIRST(PMEMobjpool \*pop, TYPE)
POBJ_NEXT_TYPE_NUM(PMEMoid oid)
POBJ_NEXT(TOID oid)

POBJ_FOREACH(PMEMobjpool \*pop, PMEMoid varoid)
POBJ_FOREACH_SAFE(PMEMobjpool \*pop, PMEMoid varoid, PMEMoid nvaroid)
POBJ_FOREACH_TYPE(PMEMobjpool \*pop, TOID var)
POBJ_FOREACH_SAFE_TYPE(PMEMobjpool \*pop, TOID var, TOID nvar)

**Non-transactional persistent atomic circular doubly-linked list:**

int pmemobj_list_insert(PMEMobjpool \*pop, size_t pe_offset, void \*head, PMEMoid dest, int before, PMEMoid oid);
PMEMoid pmemobj_list_insert_new(PMEMobjpool \*pop, size_t pe_offset, void \*head, PMEMoid dest, int before, size_t size, uint64_t type_num, pmemobj_constr constructor, void \*arg);
int pmemobj_list_remove(PMEMobjpool \*pop, size_t pe_offset, void \*head, PMEMoid oid, int free);
int pmemobj_list_move(PMEMobjpool \*pop,
    size_t pe_old_offset, void \*head_old,
    size_t pe_new_offset, void \*head_new,
    PMEMoid dest, int before, PMEMoid oid);

POBJ_LIST_ENTRY(TYPE)
POBJ_LIST_HEAD(HEADNAME, TYPE)

POBJ_LIST_FIRST(POBJ_LIST_HEAD \*head)
POBJ_LIST_LAST(POBJ_LIST_HEAD \*head, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_EMPTY(POBJ_LIST_HEAD \*head)
POBJ_LIST_NEXT(TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_PREV(TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_DEST_HEAD
POBJ_LIST_DEST_TAIL

POBJ_LIST_FOREACH(TOID var, POBJ_LIST_HEAD \*head, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_FOREACH_REVERSE(TOID var, POBJ_LIST_HEAD \*head, POBJ_LIST_ENTRY FIELD)

POBJ_LIST_INSERT_HEAD(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_INSERT_TAIL(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_INSERT_AFTER(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID listelm, TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_INSERT_BEFORE(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID listelm, TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_INSERT_NEW_HEAD(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    POBJ_LIST_ENTRY FIELD, size_t size,
    pmemobj_constr constructor, void \*arg)
POBJ_LIST_INSERT_NEW_TAIL(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    POBJ_LIST_ENTRY FIELD, size_t size,
    void (\*constructor)(PMEMobjpool \*pop, void \*ptr, void \*arg),
    void \*arg)
POBJ_LIST_INSERT_NEW_AFTER(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID listelm, POBJ_LIST_ENTRY FIELD, size_t size,
    pmemobj_constr constructor, void \*arg)
POBJ_LIST_INSERT_NEW_BEFORE(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID listelm, POBJ_LIST_ENTRY FIELD, size_t size,
    pmemobj_constr constructor, void \*arg)
POBJ_LIST_REMOVE(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_REMOVE_FREE(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    TOID elm, POBJ_LIST_ENTRY FIELD)
POBJ_LIST_MOVE_ELEMENT_HEAD(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    POBJ_LIST_HEAD \*head_new, TOID elm, POBJ_LIST_ENTRY FIELD,
    POBJ_LIST_ENTRY field_new)
POBJ_LIST_MOVE_ELEMENT_TAIL(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    POBJ_LIST_HEAD \*head_new, TOID elm, POBJ_LIST_ENTRY FIELD,
    POBJ_LIST_ENTRY field_new)
POBJ_LIST_MOVE_ELEMENT_AFTER(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    POBJ_LIST_HEAD \*head_new, TOID listelm, TOID elm,
    POBJ_LIST_ENTRY FIELD, POBJ_LIST_ENTRY field_new)
POBJ_LIST_MOVE_ELEMENT_BEFORE(PMEMobjpool \*pop, POBJ_LIST_HEAD \*head,
    POBJ_LIST_HEAD \*head_new, TOID listelm, TOID elm,
    POBJ_LIST_ENTRY FIELD, POBJ_LIST_ENTRY field_new)

**Transactional object manipulation:**

enum tx_stage pmemobj_tx_stage(void);

int pmemobj_tx_begin(PMEMobjpool \*pop, jmp_buf \*env, enum tx_lock, ...);
int pmemobj_tx_lock(enum tx_lock lock_type, void \*lockp);
void pmemobj_tx_abort(int errnum);
void pmemobj_tx_commit(void);
int pmemobj_tx_end(void);
int pmemobj_tx_errno(void);
void pmemobj_tx_process(void);

int pmemobj_tx_add_range(PMEMoid oid, uint64_t off, size_t size);
int pmemobj_tx_add_range_direct(const void \*ptr, size_t size);
PMEMoid pmemobj_tx_alloc(size_t size, uint64_t type_num);
PMEMoid pmemobj_tx_zalloc(size_t size, uint64_t type_num);
PMEMoid pmemobj_tx_realloc(PMEMoid oid, size_t size, uint64_t type_num);
PMEMoid pmemobj_tx_zrealloc(PMEMoid oid, size_t size, uint64_t type_num);
PMEMoid pmemobj_tx_strdup(const char \*s, uint64_t type_num);
int pmemobj_tx_free(PMEMoid oid);

TX_BEGIN_LOCK(PMEMobjpool \*pop, ...)
TX_BEGIN(PMEMobjpool \*pop)
TX_ONABORT
TX_ONCOMMIT
TX_FINALLY
TX_END

TX_ADD(TOID o)
TX_ADD_FIELD(TOID o, FIELD)
TX_ADD_DIRECT(TYPE \*p)
TX_ADD_FIELD_DIRECT(TYPE \*p, FIELD)

TX_NEW(TYPE)
TX_ALLOC(TYPE, size_t size)
TX_ZNEW(TYPE)
TX_ZALLOC(TYPE, size_t size)
TX_REALLOC(TOID o, size_t size)
TX_ZREALLOC(TOID o, size_t size)
TX_STRDUP(const char \*s, uint64_t type_num)
TX_FREE(TOID o)

TX_SET(TOID o, FIELD, VALUE)
TX_SET_DIRECT(TYPE \*p, FIELD, VALUE)
TX_MEMCPY(void \*dest, const void \*src, size_t num)
TX_MEMSET(void \*dest, int c, size_t num)

**Library API versioning:**

const char \*pmemobj_check_version(
    unsigned major_required,
    unsigned minor_required);

Managing library behavior:

void pmemobj_set_funcs(
    void \*(\*malloc_func)(size_t size),
    void (\*free_func)(void \*ptr),
    void \*(\*realloc_func)(void \*ptr, size_t size),
    char \*(\*strdup_func)(const char \*s));
int pmemobj_check(const char \*path, const char \*layout);

**Error handling:**

const char \*pmemobj_errormsg(void);


### DESCRIPTION

<strong>libpmemobj</strong> provides a transactional object store in <em>persistent memory</em> (pmem). This library is intended for applications using direct access storage (DAX), which is storage that supports load/store access without paging blocks from a block storage device. Some types of <em>non-volatile memory DIMMs</em> (NVDIMMs) provide this type of byte addressable access to storage. A <em>persistent memory aware file system</em> is typically used to expose the direct access to applications. Memory mapping a file from this type of file system results in the load/store, non-paged access to pmem. <strong>libpmemobj</strong> builds on this type of memory mapped file.</p>
<p>This library is for applications that need a transactions and persistent memory management. The <strong>libpmemobj</strong> requires a <strong>-std=gnu99</strong> compilation flag to build properly. This library builds on the low-level pmem support provided by <strong>libpmem</strong>, handling the transactional updates, flushing changes to persistence, and recovery for the application.</p>
<p><strong>libpmemobj</strong> is one of a collection of persistent memory libraries available, the others are:</p></td>


<strong>libpmemblk</strong>(3), providing pmem-resident arrays of fixed-sized blocks with atomic updates.</p>
<p><strong>libpmemlog</strong>(3), providing a pmem-resident log file.</p>
<p><strong>libpmem</strong>(3), low-level persistent memory support.</p></td>


Under normal usage, <strong>libpmemobj</strong> will never print messages or intentionally cause the process to exit. The only exception to this is the debugging information, when enabled, as described under <strong>DEBUGGING AND ERROR HANDLING</strong> below.</p></td>


[]()

MOST COMMONLY USED FUNCTIONS
----------------------------

To use the pmem-resident transactional object store provided by <strong>libpmemobj</strong>, a <em>memory pool</em> is first created. This is done with the <strong>pmemobj_create</strong>() function described in this section. The other functions described in this section then operate on the resulting memory pool.</p>
<p>Once created, the memory pool is represented by an opaque handle, of type <em>PMEMobjpool *</em>, which is passed to most of the other functions in this section. Internally, <strong>libpmemobj</strong> will use either <strong>pmem_persist</strong>() or <strong>msync</strong>(2) when it needs to flush changes, depending on whether the memory pool appears to be persistent memory or a regular file (see the <strong>pmem_is_pmem</strong>() function in <strong>libpmem</strong>(3) for more information). There is no need for applications to flush changes directly when using the obj memory API provided by <strong>libpmemobj</strong>.</p>
<p><strong>PMEMobjpool *pmemobj_open(const char *</strong><em>path</em><strong>, const char *</strong><em>layout</em><strong>);</strong></p></td>


The <strong>pmemobj_open</strong>() function opens an existing object store memory pool, returning a memory pool handle used with most of the functions in this section. <em>path</em> must be an existing file containing a pmemobj memory pool as created by <strong>pmemobj_create</strong>(). If <em>layout</em> is non-NULL, it is compared to the layout name provided to <strong>pmemobj_create</strong>() when the pool was first created. This can be used to verify the layout of the pool matches what was expected. The application must have permission to open the file and memory map it with read/write permissions. If an error prevents the pool from being opened, or if the given <em>layout</em> does not match the pool’s layout, <strong>pmemobj_open</strong>() returns NULL and sets errno appropriately.</p></td>


<strong>PMEMobjpool *pmemobj_create(const char *</strong><em>path</em><strong>, const char *</strong><em>layout</em><strong>,<br />
size_t</strong> <em>poolsize</em><strong>, mode_t</strong> <em>mode</em><strong>);</strong></p></td>


The <strong>pmemobj_create</strong>() function creates a transactional object store with the given total <em>poolsize</em>. <em>path</em> specifies the name of the memory pool file to be created. <em>layout</em> specifies the application’s layout type in the form of a string. The layout name is not interpreted by <strong>libpmemobj</strong>, but may be used as a check when <strong>pmemobj_open</strong>() is called. The layout name, including the null termination, cannot be longer than <strong>PMEMOBJ_MAX_LAYOUT</strong> as defined in <strong><libpmemobj.h></strong>. It is allowed to pass NULL as <em>layout</em>, which is equivalent for using an empty string as a layout name. <em>mode</em> specifies the permissions to use when creating the file as described by <strong>creat</strong>(2). The memory pool file is fully allocated to the size <em>poolsize</em> using <strong>posix_fallocate</strong>(3). The caller may choose to take responsibility for creating the memory pool file by creating it before calling <strong>pmemobj_create</strong>() and then specifying <em>poolsize</em> as zero. In this case <strong>pmemobj_create</strong>() will take the pool size from the size of the existing file and will verify that the file appears to be empty by searching for any non-zero data in the pool header at the beginning of the file. The minimum file size allowed by the library for a transactional object store is defined in <strong><libpmemobj.h></strong> as <strong>PMEMOBJ_MIN_POOL</strong>.</p></td>


<strong>void pmemobj_close(PMEMobjpool *</strong><em>pop</em><strong>);</strong></p></td>


The <strong>pmemobj_close</strong>() function closes the memory pool indicated by <em>pop</em> and deletes the memory pool handle. The object store itself lives on in the file that contains it and may be re-opened at a later time using <strong>pmemobj_open</strong>() as described above.</p></td>


[]()

LOW-LEVEL MEMORY MANIPULATION
-----------------------------

The <strong>libpmemobj</strong> specific low-level memory manipulation functions leverage the knowledge of the additional configuration options available for <strong>libpmemobj</strong> pools, such as replication. They also take advantage of the type of storage behind the pool and use appropriate flush/drain functions. It is advised to use these functions in conjunction with <strong>libpmemobj</strong> objects, instead of using low-level memory manipulations functions from <strong>libpmem</strong>.</p>
<p><strong>void pmemobj_persist(PMEMobjpool *</strong><em>pop</em><strong>, const void *</strong><em>addr</em><strong>, size_t</strong> <em>len</em><strong>);</strong></p></td>


Forces any changes in the range [<em>addr</em>, <em>addr</em>+<em>len</em>) to be stored durably in persistent memory. Internally this may call either <strong>pmem_msync</strong>() or <strong>pmem_persist</strong>(). There are no alignment restrictions on the range described by <em>addr</em> and <em>len</em>, but <strong>pmemobj_persist</strong>() may expand the range as necessary to meet platform alignment requirements.</p>
<p>WARNING: Like <strong>msync</strong>(2), there is nothing atomic or transactional about this call. Any unwritten stores in the given range will be written, but some stores may have already been written by virtue of normal cache eviction/replacement policies. Correctly written code must not depend on stores waiting until <strong>pmemobj_persist</strong>() is called to become persistent – they can become persistent at any time before <strong>pmemobj_persist</strong>() is called.</p></td>


<strong>void pmemobj_flush(PMEMobjpool *</strong><em>pop</em><strong>, const void *</strong><em>addr</em><strong>, size_t</strong> <em>len</em><strong>);<br />
void pmemobj_drain(PMEMobjpool *</strong><em>pop</em><strong>);</strong></p></td>


These functions provide partial versions of the <strong>pmemobj_persist</strong>() function described above. <strong>pmemobj_persist</strong>() can be thought of as this:</p>
<pre><code>void
pmemobj_persist(PMEMobjpool *pop, const void *addr, size_t len)
{
    /* flush the processor caches */
    pmemobj_flush(pop, addr, len);

   /* wait for any pmem stores to drain from HW buffers */
    pmemobj_drain(pop);
}</code></pre>
<p>These functions allow advanced programs to create their own variations of <strong>pmemobj_persist</strong>(). For example, a program that needs to flush several discontiguous ranges can call <strong>pmemobj_flush</strong>() for each range and then follow up by calling <strong>pmemobj_drain</strong>() once. For more information on partial flushing operations see the <strong>libpmem</strong> manpage.</p></td>


<strong>void *pmemobj_memcpy_persist(PMEMobjpool *</strong><em>pop</em><strong>, void *</strong><em>dest</em><strong>,<br />
const void *</strong><em>src</em><strong>, size_t</strong> <em>len</em><strong>);<br />
void *pmemobj_memset_persist(PMEMobjpool *</strong><em>pop</em><strong>, void *</strong><em>dest</em><strong>,<br />
int</strong> <em>c</em><strong>, size_t</strong> <em>len</em><strong>);</strong></p></td>


The <strong>pmemobj_memcpy_persist</strong>(), and <strong>pmemobj_memset_persist</strong>(), functions provide the same memory copying as their namesakes <strong>memcpy</strong>(3), and <strong>memset</strong>(3), and ensure that the result has been flushed to persistence before returning. For example, the following code is functionally equivalent to <strong>pmemobj_memcpy_persist</strong>():</p>
<pre><code>void *
pmemobj_memcpy_persist(PMEMobjpool *pop, void *dest,
    const void *src, size_t len)
{
    void *retval = memcpy(pop, dest, src, len);

   pmemobj_persist(pop, dest, len);

   return retval;
}</code></pre></td>


[]()

POOL SETS AND REPLICAS
----------------------

Depending on the configuration of the system, the available space of non-volatile memory space may be divided into multiple memory devices. In such case, the maximum size of the transactional object store could be limited by the capacity of a single memory device. The <strong>libpmemobj</strong> allows building transactional object stores spanning multiple memory devices by creation of persistent memory pools consisting of multiple files, where each part of such a <em>pool set</em> may be stored on different pmem-aware filesystem.</p>
<p>To improve reliability and eliminate the single point of failure, all the changes of the data stored in the persistent memory pool could be also automatically written to local pool replicas, thereby providing a backup for a persistent memory pool by producing a <em>mirrored pool set</em>. In practice, the pool replicas may be considered binary copies of the “master” pool set.</p>
<p>Creation of all the parts of the pool set and the associated replica sets can be done with the <strong>pmemobj_create</strong>() function or by using the <strong>pmempool</strong>(1) utility.</p>
<p>When creating the pool set consisting of multiple files, or when creating the replicated pool set, the <em>path</em> argument passed to <strong>pmemobj_create</strong>() must point to the special <em>set</em> file that defines the pool layout and the location of all the parts of the pool set. The <em>poolsize</em> argument must be 0. The meaning of <em>layout</em> and <em>mode</em> arguments doesn’t change, except that the same <em>mode</em> is used for creation of all the parts of the pool set and replicas. If the error prevents any of the pool set files from being created, <strong>pmemobj_create</strong>() returns NULL and sets errno appropriately.</p>
<p>When opening the pool set consisting of multiple files, or when opening the replicated pool set, the <em>path</em> argument passed to <strong>pmemobj_open</strong>() must not point to the pmemobj memory pool file, but to the same <em>set</em> file that was used for the pool set creation. If an error prevents any of the pool set files from being opened, or if the actual size of any file does not match the corresponding part size defined in <em>set</em> file <strong>pmemobj_open</strong>() returns NULL and sets errno appropriately.</p>
<p>The set file is a plain text file, which must start with the line containing a <em>PMEMPOOLSET</em> string, followed by the specification of all the pool parts in the next lines. For each part, the file size and the absolute path must be provided. The size has to be compliant with the format specified in IEC 80000-13, IEEE 1541 or the Metric Interchange Format. Standards accept SI units with obligatory B - kB, MB, GB … (multiplier by 1000) and IEC units with optional “iB” - KiB, MiB, GiB, …, K, M, G, … - (multiplier by 1024).</p>
<p>The minimum file size of each part of the pool set is the same as the minimum size allowed for a transactional object store consisting of one file. It is defined in <strong><libpmemobj.h></strong> as <strong>PMEMOBJ_MIN_POOL</strong>. Sections defining the replica sets are optional. There could be multiple replica sections and each must start with the line containing a <em>REPLICA</em> string. Lines starting with “#” character are ignored.</p>
<p>Here is the example “myobjpool.set” file:</p></td>


<code>PMEMPOOLSET
100G /mountpoint0/myfile.part0
200G /mountpoint1/myfile.part1
400G /mountpoint2/myfile.part2
REPLICA
500G /mountpoint3/mymirror.part0
200G /mountpoint4/mymirror.part1</code></pre></td>


The files in the set may be created by running the following command:</p></td>


<code>pmempool create --layout=&quot;mylayout&quot; obj myobjpool.set</code></pre></td>


[]()

LOCKING
-------

<strong>libpmemobj</strong> provides several types of synchronization primitives, designed so as to use them with persistent memory. The locks are not dynamically allocated, but embedded in pmem-resident objects. For performance reasons, they are also padded up to 64 bytes (cache line size).</p>
<p>Pmem-aware locks implementation is based on the standard POSIX Thread Library, as described in <strong>pthread_mutex</strong>(3), <strong>pthread_rwlock</strong>(3) and <strong>pthread_cond</strong>(3). They provide semantics similar to standard <strong>pthread</strong> locks, except that they are considered initialized by zeroing them. So allocating the locks with <strong>pmemobj_zalloc</strong>() or <strong>pmemobj_tx_zalloc</strong>() does not require another initialization step.</p>
<p>The fundamental property of pmem-aware locks is their automatic reinitialization every time the persistent object store pool is open. This way, all the pmem-aware locks may be considered initialized (unlocked) right after opening the pool, regardless of their state at the time the pool was closed for the last time.</p>
<p>Pmem-aware mutexes, read/write locks and condition variables must be declared with one of the <em>PMEMmutex</em>, <em>PMEMrwlock</em>, or <em>PMEMcond</em> type respectively.</p>
<p><strong>void pmemobj_mutex_zero(PMEMobjpool *</strong><em>pop</em><strong>, PMEMmutex *</strong><em>mutexp</em><strong>);</strong></p></td>


The <strong>pmemobj_mutex_zero</strong>() function explicitly initializes pmem-aware mutex pointed by <em>mutexp</em> by zeroing it. Initialization is not necessary if the object containing the mutex has been allocated using one of <strong>pmemobj_zalloc</strong>() or <strong>pmemobj_tx_zalloc</strong>() functions.</p></td>


<strong>int pmemobj_mutex_lock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMmutex *</strong><em>mutexp</em><strong>);</strong></p></td>


The <strong>pmemobj_mutex_lock</strong>() function locks pmem-aware mutex pointed by <em>mutexp</em>. If the mutex is already locked, the calling thread will block until the mutex becomes available. If this is the first use of the mutex since opening of the pool <em>pop</em>, the mutex is automatically reinitialized and then locked.</p></td>


<strong>int pmemobj_mutex_timedlock(PMEMobjpool *</strong><em>pop</em><strong>,<br />
PMEMmutex *restrict</strong> <em>mutexp</em><strong>,<br />
const struct timespec *restrict</strong> <em>abs_timeout</em><strong>);</strong></p></td>


The <strong>pmemobj_mutex_timedlock</strong>() performs the same action as <strong>pmemobj_mutex_lock</strong>(), but will not wait beyond <em>abs_timeout</em> to obtain the lock before returning.</p></td>


<strong>int pmemobj_mutex_trylock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMmutex *</strong><em>mutexp</em><strong>);</strong></p></td>


The <strong>pmemobj_mutex_trylock</strong>() function locks pmem-aware mutex pointed by <em>mutexp</em>. If the mutex is already locked, <strong>pthread_mutex_trylock</strong>() will not block waiting for the mutex, but will return an error condition. If this is the first use of the mutex since opening of the pool <em>pop</em> the mutex is automatically reinitialized and then locked.</p></td>


<strong>int pmemobj_mutex_unlock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMmutex *</strong><em>mutexp</em><strong>);</strong></p></td>


The <strong>pmemobj_mutex_unlock</strong>() function unlocks an acquired pmem-aware mutex pointed by <em>mutexp</em>. Undefined behavior follows if a thread tries to unlock a mutex that has not been locked by it, or if a thread tries to release a mutex that is already unlocked or not initialized.</p></td>


<strong>void pmemobj_rwlock_zero(PMEMobjpool *</strong><em>pop</em><strong>, PMEMrwlock *</strong><em>rwlockp</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_zero</strong>() function is used to explicitly initialize pmem-aware read/write lock pointed by <em>rwlockp</em> by zeroing it. Initialization is not necessary if the object containing the lock has been allocated using one of <strong>pmemobj_zalloc</strong>() or <strong>pmemobj_tx_zalloc</strong>() functions.</p></td>


<strong>int pmemobj_rwlock_rdlock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMrwlock *</strong><em>rwlockp</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_rdlock</strong>() function acquires a read lock on <em>rwlockp</em> provided that lock is not presently held for writing and no writer threads are presently blocked on the lock. If the read lock cannot be immediately acquired, the calling thread blocks until it can acquire the lock. If this is the first use of the lock since opening of the pool <em>pop</em>, the lock is automatically reinitialized and then acquired.</p></td>


<strong>int pmemobj_rwlock_timedrdlock(PMEMobjpool *</strong><em>pop</em><strong>,<br />
PMEMrwlock *restrict</strong> <em>rwlockp</em><strong>,<br />
const struct timespec *restrict</strong> <em>abs_timeout</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_timedrdlock</strong>() performs the same action, but will not wait beyond <em>abs_timeout</em> to obtain the lock before returning.</p></td>


A thread may hold multiple concurrent read locks. If so, <strong>pmemobj_rwlock_unlock</strong>() must be called once for each lock obtained.</p>
<p>The results of acquiring a read lock while the calling thread holds a write lock are undefined.</p>
<p><strong>int pmemobj_rwlock_wrlock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMrwlock *</strong><em>rwlockp</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_wrlock</strong>() function blocks until a write lock can be acquired against lock pointed by <em>rwlockp</em>. If this is the first use of the lock since opening of the pool <em>pop</em>, the lock is automatically reinitialized and then acquired.</p></td>


<strong>int pmemobj_rwlock_timedwrlock(PMEMobjpool *</strong><em>pop</em><strong>,<br />
PMEMrwlock *restrict</strong> <em>rwlockp</em><strong>,<br />
const struct timespec *restrict</strong> <em>abs_timeout</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_timedwrlock</strong>() performs the same action, but will not wait beyond <em>abs_timeout</em> to obtain the lock before returning.</p></td>


<strong>int pmemobj_rwlock_tryrdlock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMrwlock *</strong><em>rwlockp</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_tryrdlock</strong>() function performs the same action as <strong>pmemobj_rwlock_rdlock</strong>(), but does not block if the lock cannot be immediately obtained.</p>
<p>The results are undefined if the calling thread already holds the lock at the time the call is made.</p></td>


<strong>int pmemobj_rwlock_trywrlock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMrwlock *</strong><em>rwlockp</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_trywrlock</strong>() function performs the same action as <strong>pmemobj_rwlock_wrlock</strong>(), but does not block if the lock cannot be immediately obtained.</p>
<p>The results are undefined if the calling thread already holds the lock at the time the call is made.</p></td>


<strong>int pmemobj_rwlock_unlock(PMEMobjpool *</strong><em>pop</em><strong>, PMEMrwlock *</strong><em>rwlockp</em><strong>);</strong></p></td>


The <strong>pmemobj_rwlock_unlock</strong>() function is used to release the read/write lock previously obtained by <strong>pmemobj_rwlock_rdlock</strong>(), <strong>pmemobj_rwlock_wrlock</strong>(), <strong>pthread_rwlock_tryrdlock</strong>(), or <strong>pmemobj_rwlock_trywrlock</strong>().</p></td>


<strong>void pmemobj_cond_zero(PMEMobjpool *</strong><em>pop</em><strong>, PMEMcond *</strong><em>condp</em><strong>);</strong></p></td>


The <strong>pmemobj_cond_zero</strong>() function explicitly initializes pmem-aware condition variable by zeroing it. Initialization is not necessary if the object containing the condition variable has been allocated using one of <strong>pmemobj_zalloc</strong>() or <strong>pmemobj_tx_zalloc</strong>() functions.</p></td>


<strong>int pmemobj_cond_broadcast(PMEMobjpool *</strong><em>pop</em><strong>, PMEMcond *</strong><em>condp</em><strong>);</strong></p>
<p><strong>int pmemobj_cond_signal(PMEMobjpool *</strong><em>pop</em><strong>, PMEMcond *</strong><em>condp</em><strong>);</strong></p></td>


The difference between <strong>pmemobj_cond_broadcast</strong>() and <strong>pmemobj_cond_signal</strong>() is that the former unblocks all threads waiting for the condition variable, whereas the latter blocks only one waiting thread. If no threads are waiting on <em>cond</em>, neither function has any effect. If more than one thread is blocked on a condition variable, the used scheduling policy determines the order in which threads are unblocked. The same mutex used for waiting must be held while calling either function. Although neither function strictly enforces this requirement, undefined behavior may follow if the mutex is not held.</p></td>


<strong>int pmemobj_cond_timedwait(PMEMobjpool *</strong><em>pop</em><strong>, PMEMcond *restrict</strong> <em>condp</em><strong>,<br />
PMEMmutex *restrict</strong> <em>mutexp</em><strong>,<br />
const struct timespec *restrict</strong> <em>abs_timeout</em><strong>);</strong></p>
<p><strong>int pmemobj_cond_wait(PMEMobjpool *</strong><em>pop</em><strong>, PMEMcond *</strong><em>condp</em><strong>,<br />
PMEMmutex *restrict</strong> <em>mutexp</em><strong>);</strong></p></td>


The <strong>pmemobj_cond_timedwait</strong>() and <strong>pmemobj_cond_wait</strong>() functions shall block on a condition variable. They shall be called with mutex locked by the calling thread or undefined behavior results. These functions atomically release mutex pointed by <em>mutexp</em> and cause the calling thread to block on the condition variable <em>cond</em>; atomically here means “atomically with respect to access by another thread to the mutex and then the condition variable”. That is, if another thread is able to acquire the mutex after the about-to-block thread has released it, then a subsequent call to <strong>pmemobj_cond_broadcast</strong>() or <strong>pmemobj_cond_signal</strong>() in that thread shall behave as if it were issued after the about-to-block thread has blocked. Upon successful return, the mutex shall have been locked and shall be owned by the calling thread.</p></td>


[]()

PERSISTENT OBJECTS
------------------

Each object stored in persistent memory pool is represented by an object handle of type <em>PMEMoid</em>. In practice, such a handle is a unique Object IDentifier (OID) of a global scope, which means that two objects from different pools may not have the same OID. The special <em>OID_NULL</em> macro defines a NULL-like handle that does not represent any object. The size of a single object is limited by a <em>PMEMOBJ_MAX_ALLOC_SIZE</em>. Thus an allocation with requested size greater than this value will fail.</p>
<p>An OID cannot be considered as a direct pointer to an object. Each time the program attempts to read or write object data, it must obtain the current memory address of the object by converting its OID into the pointer.</p>
<p>In contrast to the memory address, the OID value for given object does not change during the life of an object (except for realloc operation), and remains valid after closing and reopening the pool. For this reason, if an object contains a reference to another persistent object - necessary to build some kind of a linked data structure - it shall never use memory address of an object, but its OID.</p>
<p><strong>void *pmemobj_direct(PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_direct</strong>() function returns a pointer to an object represented by <em>oid</em>. If OID_NULL is passed as an argument, function returns NULL.</p></td>


<strong>uint64_t pmemobj_type_num(PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_type_num</strong>() function returns a type number of the object represented by <em>oid</em>.</p></td>


<strong>PMEMobjpool *pmemobj_pool_by_oid(PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_pool_by_oid</strong>() function returns a handle to the pool which contains the object represented by <em>oid</em>. If the pool is not open or OID_NULL is passed as an argument, function returns NULL.</p></td>


<strong>PMEMobjpool *pmemobj_pool_by_ptr(const void *</strong><em>addr</em><strong>);</strong></p></td>


The <strong>pmemobj_pool_by_ptr</strong>() function returns a handle to the pool which contains the address. If the address does not belong to any open pool, function returns NULL.</p></td>


At the time of allocation (or reallocation), each object may be assigned a number representing its type. Such a <em>type number</em> may be used to arrange the persistent objects based on their actual user-defined structure type, thus facilitating implementation of a simple run-time type safety mechanism. It also allows to iterate through all the objects of given type stored in the persistent memory pool. See <strong>OBJECT CONTAINERS</strong> section for more details.</p>
<p>The <strong>OID_IS_NULL</strong> macro checks if given <em>PMEMoid</em> represents a NULL object.</p>
<p>The <strong>OID_EQUALS</strong> macro compares two <em>PMEMoid</em> objects.</p></td>


[]()

TYPE-SAFETY
-----------

Operating on untyped object handles, as well as on direct untyped object pointers (void *) may be confusing and error prone. To facilitate implementation of type safety mechanism, <strong>libpmemobj</strong> defines a set of macros that provide a static type enforcement, catching potential errors at compile time. For example, a compile-time error is generated when an attempt is made to assign a handle to an object of one type to the object handle variable of another type of object.</p>
<p><strong>TOID_DECLARE(</strong><em>TYPE</em><strong>, uint64_t</strong> <em>type_num)</em></p></td>


The <strong>TOID_DECLARE</strong> macro declares a typed OID of user-defined type specified by argument <em>TYPE</em>, and with type number specified by argument <em>type_num</em>.</p></td>


<strong>TOID_DECLARE_ROOT(</strong><em>ROOT_TYPE</em><strong>)</strong></p></td>


The <strong>TOID_DECLARE_ROOT</strong> macro declares a typed OID of user-defined type specified by argument <em>ROOT_TYPE</em>, and with type number for root object <strong>POBJ_ROOT_TYPE_NUM</strong>.</p></td>


<strong>TOID(</strong><em>TYPE</em><strong>)</strong></p></td>


The <strong>TOID</strong> macro declares a handle to an object of type specified by argument <em>TYPE</em>, where <em>TYPE</em> is the name of a user-defined structure. The typed OID must be declared first using the <strong>TOID_DECLARE</strong>, <strong>TOID_DECLARE_ROOT</strong>, <strong>POBJ_LAYOUT_TOID</strong> or <strong>POBJ_LAYOUT_ROOT</strong> macros.</p></td>


<strong>TOID_TYPE_NUM(</strong><em>TYPE</em><strong>)</strong></p></td>


The <strong>TOID_TYPE_NUM</strong> macro returns a type number of the type specified by argument <em>TYPE</em>.</p></td>


<strong>TOID_TYPE_NUM_OF(TOID</strong> <em>oid</em><strong>)</strong></p></td>


The <strong>TOID_TYPE_NUM_OF</strong> macro returns a type number of the object specified by argument <em>oid</em>. The type number is read from the typed OID.</p></td>


<strong>TOID_VALID(TOID</strong> <em>oid</em><strong>)</strong></p></td>


The <strong>TOID_VALID</strong> macro validates whether the type number stored in object’s metadata is equal to the type number read from typed OID.</p></td>


<strong>OID_INSTANCEOF(PMEMoid</strong> <em>oid</em><strong>,</strong> <em>TYPE</em><strong>)</strong></p></td>


The <strong>OID_INSTANCEOF</strong> macro checks whether the <em>oid</em> is of the type specified by argument <em>TYPE</em>.</p></td>


<strong>TOID_ASSIGN(TOID</strong> <em>o</em><strong>,</strong> <em>VALUE</em><strong>)</strong></p></td>


The <strong>TOID_ASSIGN</strong> macro assigns an object handle specified by <em>VALUE</em> to the variable <em>o</em>.</p></td>


<strong>TOID_IS_NULL(TOID</strong> <em>o</em><strong>)</strong></p></td>


The <strong>TOID_IS_NULL</strong> macro evaluates to true if the object handle represented by argument <em>o</em> has OID_NULL value.</p></td>


<strong>TOID_EQUALS(TOID</strong> <em>lhs</em><strong>, TOID</strong> <em>rhs</em><strong>)</strong></p></td>


The <strong>TOID_EQUALS</strong> macro evaluates to true if both <em>lhs</em> and <em>rhs</em> object handles are referencing the same persistent object.</p></td>


<strong>DIRECT_RW(TOID</strong> <em>oid</em><strong>)</strong></p>
<p><strong>D_RW(TOID</strong> <em>oid</em><strong>)</strong></p></td>


The <strong>DIRECT_RW</strong>() macro and its shortened form <strong>D_RW</strong>() return a typed write pointer (TYPE *) to an object represented by <em>oid</em>. If <em>oid</em> holds OID_NULL value, the macro evaluates to NULL.</p></td>


<strong>DIRECT_RO(TOID</strong> <em>oid</em><strong>)</strong></p>
<p><strong>D_RO(TOID</strong> <em>oid</em><strong>)</strong></p></td>


The <strong>DIRECT_RO</strong>() macro and its shortened form <strong>D_RO</strong>() return a typed read-only (const) pointer (TYPE *) to an object represented by <em>oid</em>. If <em>oid</em> holds OID_NULL value, the macro evaluates to NULL.</p></td>


[]()

LAYOUT DECLARATION
------------------

The <em>libpmemobj</em> defines a set of macros for convenient declaration of pool’s layout. The declaration of layout consist of declaration of number of used types. The declared types will be assigned consecutive type numbers. Declared types may be used in conjunction with type safety macros. Once created the layout declaration shall not be changed unless the new types are added at the end of the existing layout declaration. Modifying any of existing declaration may lead to changes in type numbers of declared types which in consequence may cause data corruption.</p>
<p><strong>POBJ_LAYOUT_BEGIN(</strong><em>LAYOUT</em><strong>)</strong></p></td>


The <strong>POBJ_LAYOUT_BEGIN</strong> macro indicates a begin of declaration of layout. The <em>LAYOUT</em> argument is a name of layout. This argument must be passed to all macros related to the declaration of layout.</p></td>


<strong>POBJ_LAYOUT_TOID(</strong><em>LAYOUT</em><strong>,</strong> <em>TYPE</em><strong>)</strong></p></td>


The <strong>POBJ_LAYOUT_TOID</strong> macro declares a typed OID for type passed as <em>TYPE</em> argument inside the declaration of layout. All types declared using this macro are assigned with consecutive type numbers. This macro must be used between the <strong>POBJ_LAYOUT_BEGIN</strong> and <strong>POBJ_LAYOUT_END</strong> macros, with the same name passed as <em>LAYOUT</em> argument.</p></td>


<strong>POBJ_LAYOUT_ROOT(</strong><em>LAYOUT</em><strong>,</strong> <em>ROOT_TYPE</em><strong>)</strong></p></td>


The <strong>POBJ_LAYOUT_ROOT</strong> macro declares a typed OID for type passed as <em>ROOT_TYPE</em> argument inside the declaration of layout. The typed OID will be assigned with type number for root object <strong>POBJ_ROOT_TYPE_NUM</strong>.</p></td>


<strong>POBJ_LAYOUT_END(</strong><em>LAYOUT</em><strong>)</strong></p></td>


The <strong>POBJ_LAYOUT_END</strong> macro ends the declaration of layout.</p></td>


<strong>POBJ_LAYOUT_NAME(</strong><em>LAYOUT</em><strong>)</strong></p></td>


The <strong>POBJ_LAYOUT_NAME</strong> macro returns the name of layout as a NULL-terminated string.</p></td>


<strong>POBJ_LAYOUT_TYPES_NUM(</strong><em>LAYOUT</em><strong>)</strong></p></td>


The <strong>POBJ_LAYOUT_TYPES_NUM</strong> macro returns number of types declared using the <strong>POBJ_LAYOUT_TOID</strong> macro within the layout declaration.</p></td>


This is an example of layout declaration:</p></td>


<code>POBJ_LAYOUT_BEGIN(mylayout);
POBJ_LAYOUT_ROOT(mylayout, struct root);
POBJ_LAYOUT_TOID(mylayout, struct node);
POBJ_LAYOUT_TOID(mylayout, struct foo);
POBJ_LAYOUT_END(mylayout);

struct root {</code></pre></td>


<table>
<colgroup>
<col width="33%" />
<col width="33%" />
<col width="33%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>TOID(struct node) node;</p></td>
<td align="left"></td>


};</p>
<p>struct node {</p></td>


TOID(struct node) next;</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>TOID(struct foo) foo;</p></td>


};</p></td>


The name of layout and the number of declared types can be retrieved using the following code:</p></td>


<code>const char *layout_name = POBJ_LAYOUT_NAME(mylayout);
int num_of_types = POBJ_LAYOUT_TYPES_NUM(mylayout);</code></pre></td>


[]()

OBJECT CONTAINERS
-----------------

All the objects in the persistent memory pool are assigned a <em>type number</em> and are accessible by it.</p>
<p>The <em>libpmemobj</em> provides a mechanism allowing to iterate through the internal object collection, either looking for a specific object, or performing a specific operation on each object of given type. Software should not make any assumptions about the order of the objects in the internal object containers.</p>
<p><strong>PMEMoid pmemobj_first(PMEMobjpool *</strong><em>pop</em><strong>);</strong></p></td>


The <strong>pmemobj_first</strong>() function returns the first object from the pool. If the pool is empty, OID_NULL is returned.</p></td>


<strong>POBJ_FIRST(PMEMobjpool *</strong><em>pop</em><strong>,</strong> <em>TYPE</em><strong>)</strong></p></td>


The <strong>POBJ_FIRST</strong> macro returns the first object from the pool of the type specified by <em>TYPE</em>.</p></td>


<strong>POBJ_FIRST_TYPE_NUM(PMEMobjpool *</strong><em>pop</em><strong>, uint64_t</strong> <em>type_num</em><strong>)</strong></p></td>


The <strong>POBJ_FIRST_TYPE_NUM</strong> macro returns the first object from the pool of the type specified by <em>type_num</em>.</p></td>


<strong>PMEMoid pmemobj_next(PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_next</strong>() function returns the next object from the pool. If an object referenced by <em>oid</em> is the last object in the collection, or if the OID_NULL is passed as an argument, function returns OID_NULL.</p></td>


<strong>POBJ_NEXT(TOID</strong> <em>oid</em><strong>)</strong></p></td>


The <strong>POBJ_NEXT</strong> macro returns the next object of the same type as the object referenced by <em>oid</em>.</p></td>


<strong>POBJ_NEXT_TYPE_NUM(PMEMoid</strong> <em>oid</em><strong>)</strong></p></td>


The <strong>POBJ_NEXT_TYPE_NUM</strong> macro returns the next object of the same type as the object referenced by <em>oid</em>.</p></td>


The following four macros provide more convenient way to iterate through the internal collections, performing a specific operation on each object.</p>
<p><strong>POBJ_FOREACH(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid</strong> <em>varoid</em><strong>)</strong></p></td>


The <strong>POBJ_FOREACH</strong>() macro allows to perform a specific operation on each allocated object stored in the persistent memory pool pointed by <em>pop</em>. It traverses the internal collection of all the objects, assigning a handle to each element in turn to <em>varoid</em> variable.</p></td>


<strong>POBJ_FOREACH_TYPE(PMEMobjpool *</strong><em>pop</em><strong>, TOID</strong> <em>var</em><strong>)</strong></p></td>


<strong>POBJ_FOREACH_TYPE</strong>() macro allows to perform a specific operation on each allocated object of the same type as object passed as <em>var</em> argument, stored in the persistent memory pool pointed by <em>pop</em>. It traverses the internal collection of all the objects of the specified type, assigning a handle to each element in turn to <em>var</em> variable.</p></td>


<strong>POBJ_FOREACH_SAFE(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid</strong> <em>varoid</em><strong>,<br />
PMEMoid</strong> <em>nvaroid</em><strong>)</strong></p>
<p><strong>POBJ_FOREACH_SAFE_TYPE(PMEMobjpool *</strong><em>pop</em><strong>, TOID</strong> <em>var</em><strong>, TOID</strong> <em>nvar</em><strong>)</strong></p></td>


The macros <strong>POBJ_FOREACH_SAFE</strong>() and <strong>POBJ_FOREACH_SAFE_TYPE</strong>() work in a similar fashion as <strong>POBJ_FOREACH</strong>() and <strong>POBJ_FOREACH_TYPE</strong>() except that prior to performing the operation on the object, they preserve a handle to the next object in the collection by assigning it to <em>nvaroid</em> or <em>nvar</em> variable. This allows safe deletion of selected objects while iterating through the collection.</p></td>


[]()

ROOT OBJECT MANAGEMENT
----------------------

The root object of persistent memory pool is an entry point for all other persistent objects allocated using the <em>libpmemobj</em> API. In other words, every single object stored in persistent memory pool should have the root object at the end of its reference path. It may be assumed that for each persistent memory pool the root object always exists, and there is exactly one root object in each pool.</p>
<p><strong>PMEMoid pmemobj_root(PMEMobjpool *</strong><em>pop</em><strong>, size_t</strong> <em>size</em><strong>);</strong></p></td>


The <strong>pmemobj_root</strong>() function returns a handle to the root object associated with the persistent memory pool pointed by <em>pop</em>. If this is the first call to <strong>pmemobj_root</strong>() and the root object does not exists yet, it is implicitly allocated in a thread-safe manner, so if the function is called by more than one thread simultaneously (with identical <em>size</em> value), the same root object handle is returned in all the threads.</p>
<p>The size of the root object is guaranteed to be not less than the requested <em>size</em>. If the requested size is larger than the current size, the root object is automatically resized. In such case, the old data is preserved and the extra space is zeroed. The <strong>pmemobj_root</strong>() function shall not fail, except for the case if the requested object size is larger than the maximum allocation size supported for given pool, or if there is not enough free space in the pool to satisfy the reallocation of the root object. In such case, OID_NULL is returned.</p></td>


<strong>PMEMoid pmemobj_root_construct(PMEMobjpool *</strong><em>pop</em><strong>, size_t</strong> <em>size</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em><strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The <strong>pmemobj_root_construct</strong>() performs the same actions as the <strong>pmemobj_root</strong>() function, but instead of zeroing the newly allocated object a <em>constructor</em> function is called. The constructor is also called on reallocations. If the constructor returns non-zero value the allocation is canceled, the <strong>OID_NULL</strong> value is returned from the caller and errno is set to <strong>ECANCELED .</strong> The <strong>pmemobj_root_size</strong>() can be used in the constructor to check whether it’s the first call to the constructor.</p></td>


<strong>POBJ_ROOT(PMEMobjpool *</strong><em>pop</em><strong>,</strong> <em>TYPE</em><strong>)</strong></p></td>


The <strong>POBJ_ROOT</strong> macro works the same way as the <strong>pmemobj_root</strong>() function except it returns a typed OID of type <em>TYPE</em> instead of <strong>PMEMoid</strong>.</p></td>


<strong>size_t pmemobj_root_size(PMEMobjpool *</strong><em>pop</em><strong>);</strong></p></td>


The <strong>pmemobj_root_size</strong>() function returns the current size of the root object associated with the persistent memory pool pointed by <em>pop</em>. The returned size is the largest value requested by any of the earlier <strong>pmemobj_root</strong>() calls. 0 is returned if the root object has not been allocated yet.</p></td>


[]()

NON-TRANSACTIONAL ATOMIC ALLOCATIONS
------------------------------------

Functions described in this section provide the mechanism to allocate, resize and free objects from the persistent memory pool in a thread-safe and fail-safe manner. All the routines are atomic with respect to other threads and any power-fail interruptions. If any of those operations is torn by program failure or system crash; on recovery they are guaranteed to be entirely completed or discarded, leaving the persistent memory heap and internal object containers in a consistent state.</p>
<p>All these functions can be used outside transactions. Note that operations performed using non-transactional API are considered durable after completion, even if executed within the open transaction. Such non-transactional changes will not be rolled-back if the transaction is aborted or interrupted.</p>
<p>The allocations are always aligned to the cache-line boundary.</p>
<p><strong>typedef int (*</strong><em>pmemobj_constr</em><strong>)(PMEMobjpool *</strong><em>pop</em><strong>,<br />
void *</strong><em>ptr</em><strong>, void *</strong><em>arg</em><strong>);</strong></p></td>


The <strong>pmemobj_constr</strong> type represents a constructor for atomic allocation from persistent memory heap associated with memory pool <em>pop</em>. The <em>ptr</em> is a pointer to allocating memory area and the <em>arg</em> is an user-defined argument passed to an appropriate function.</p></td>


<strong>int pmemobj_alloc(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid *</strong><em>oidp</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>, pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>);</strong></p></td>


The <strong>pmemobj_alloc</strong> function allocates a new object from the persistent memory heap associated with memory pool <em>pop</em>. The <strong>PMEMoid</strong> of allocated object is stored in <em>oidp</em>. If NULL is passed as <em>oidp</em>, then the newly allocated object may be accessed only by iterating objects in the object container associated with given <em>type_num</em>, as described in <strong>OBJECT CONTAINERS</strong> section. If the <em>oidp</em> points to memory location from the <strong>pmemobj</strong> heap the <em>oidp</em> is modified atomically. Before returning, it calls the <strong>constructor</strong> function passing the pool handle <em>pop</em>, the pointer to the newly allocated object in <em>ptr</em> along with the <em>arg</em> argument. It is guaranteed that allocated object is either properly initialized, or if the allocation is interrupted before the constructor completes, the memory space reserved for the object is reclaimed. If the constructor returns non-zero value the allocation is canceled, the -1 value is returned from the caller and errno is set to <strong>ECANCELED .</strong> The <em>size</em> can be any non-zero value, however due to internal padding and object metadata, the actual size of the allocation will differ from the requested one by at least 64 bytes. For this reason, making the allocations of a size less than 64 bytes is extremely inefficient and discouraged. If <em>size</em> equals 0, then <strong>pmemobj_alloc</strong>() returns non-zero value, sets the errno and leaves the <em>oidp</em> untouched. The allocated object is added to the internal container associated with given <em>type_num</em>.</p></td>


<strong>int pmemobj_zalloc(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid *</strong><em>oidp</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_zalloc</strong>() function allocates a new zeroed object from the the persistent memory heap associated with memory pool <em>pop</em>. The <strong>PMEMoid</strong> of allocated object is stored in <em>oidp</em>. If NULL is passed as <em>oidp</em>, then the newly allocated object may be accessed only by iterating objects in the object container associated with given <em>type_num</em>, as described in <strong>OBJECT CONTAINERS</strong> section. If the <em>oidp</em> points to memory location from the <strong>pmemobj</strong> heap the <em>oidp</em> is modified atomically. The <em>size</em> can be any non-zero value, however due to internal padding and object metadata, the actual size of the allocation will differ from the requested one by at least 64 bytes. For this reason, making the allocations of a size less than 64 bytes is extremely inefficient and discouraged. If <em>size</em> equals 0, then <strong>pmemobj_zalloc</strong>() returns non-zero value, sets the errno and leaves the <em>oidp</em> untouched. The allocated object is added to the internal container associated with given <em>type_num</em>.</p></td>


<strong>void pmemobj_free(PMEMoid *</strong><em>oidp</em><strong>);</strong></p></td>


The <strong>pmemobj_free</strong>() function provides the same semantics as <strong>free</strong>(3), but instead of the process heap supplied by the system, it operates on the persistent memory heap. It frees the memory space represented by <em>oidp</em>, which must have been returned by a previous call to <strong>pmemobj_alloc</strong>(), <strong>pmemobj_zalloc</strong>(), <strong>pmemobj_realloc</strong>(), or <strong>pmemobj_zrealloc</strong>(). If <em>oidp</em> is NULL or if it points to the root object’s OID, behavior of the function is undefined. If it points to <em>OID_NULL</em>, no operation is performed. It sets the <em>oidp</em> to <em>OID_NULL</em> value after freeing the memory. If the <em>oidp</em> points to memory location from the <strong>pmemobj</strong> heap the <em>oidp</em> is changed atomically.</p></td>


<strong>int pmemobj_realloc(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid *</strong><em>oidp</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_realloc</strong>() function provide similar semantics to <strong>realloc</strong>(3), but operates on the persistent memory heap associated with memory pool <em>pop</em>. It changes the size of the object represented by <em>oidp</em>, to <em>size</em> bytes. The resized object is also added or moved to the internal container associated with given <em>type_num</em>. The contents will be unchanged in the range from the start of the region up to the minimum of the old and new sizes. If the new size is larger than the old size, the added memory will <em>not</em> be initialized. If <em>oidp</em> is NULL or if it points to the root object’s OID, behavior of the function is undefined. If it points to <em>OID_NULL</em>, then the call is equivalent to <strong>pmemobj_alloc(</strong><em>pop</em><strong>,</strong> <em>size</em><strong>,</strong> <em>type_num</em><strong>).</strong> If <em>size</em> is equal to zero, and <em>oidp</em> is not <em>OID_NULL</em>, then the call is equivalent to <strong>pmemobj_free(</strong><em>oid</em><strong>).</strong> Unless <em>oidp</em> is <em>OID_NULL</em>, it must have been returned by an earlier call to <strong>pmemobj_alloc</strong>(), <strong>pmemobj_zalloc</strong>(), <strong>pmemobj_realloc</strong>(), or <strong>pmemobj_zrealloc</strong>(). Note that the object handle value may change in result of reallocation. If the object was moved, a memory space represented by <em>oid</em> is reclaimed. If <em>oidp</em> points to memory location from the <strong>pmemobj</strong> heap the <em>oidp</em> is changed atomically. If <strong>pmemobj_realloc</strong>() is unable to satisfy the allocation request, a non-zero value is returned and errno is set appropriately.</p></td>


<strong>int pmemobj_zrealloc(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid *</strong><em>oidp</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_zrealloc</strong>() function provide similar semantics to <strong>realloc</strong>(3), but operates on the persistent memory heap associated with memory pool <em>pop</em>. It changes the size of the object represented by <em>oid</em>, to <em>size</em> bytes. The resized object is also added or moved to the internal container associated with given <em>type_num</em>. The contents will be unchanged in the range from the start of the region up to the minimum of the old and new sizes. If the new size is larger than the old size, the added memory will be zeroed. If <em>oidp</em> is NULL or if it points to the root object’s OID, behavior of the function is undefined. If it points to <em>OID_NULL</em>, then the call is equivalent to <strong>pmemobj_zalloc(</strong><em>pop</em><strong>,</strong> <em>size</em><strong>,</strong> <em>type_num</em><strong>).</strong> If <em>size</em> is equal to zero, and <em>oidp</em> doesn’t point to <em>OID_NULL</em>, then the call is equivalent to <strong>pmemobj_free(</strong><em>pop</em><strong>,</strong> <em>oid</em><strong>).</strong> Unless <em>oidp</em> points to <em>OID_NULL</em>, it must have been returned by an earlier call to <strong>pmemobj_alloc</strong>(), <strong>pmemobj_zalloc</strong>(), <strong>pmemobj_realloc</strong>(), or <strong>pmemobj_zrealloc</strong>(). Note that the object handle value may change in result of reallocation. If the object was moved, a memory space represented by <em>oidp</em> is reclaimed. If <em>oidp</em> points to memory location from the <strong>pmemobj</strong> heap the <em>oidp</em> is changed atomically. If <strong>pmemobj_zrealloc</strong>() is unable to satisfy the allocation request, OID_NULL is returned and errno is set appropriately.</p></td>


<strong>int pmemobj_strdup(PMEMobjpool *</strong><em>pop</em><strong>, PMEMoid *</strong><em>oidp</em><strong>, const char *</strong><em>s</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_strdup</strong>() function provides the same semantics as <strong>strdup</strong>(3), but operates on the persistent memory heap associated with memory pool <em>pop</em>. It stores a handle to a new object in <em>oidp</em> which is a duplicate of the string <em>s</em>. If NULL is passed as <em>oidp</em>, then the newly allocated object may be accessed only by iterating objects in the object container associated with given <em>type_num</em>, as described in <strong>OBJECT CONTAINERS</strong> section. If the <em>oidp</em> points to memory location from the <strong>pmemobj</strong> heap the <em>oidp</em> is changed atomically. The allocated string object is also added to the internal container associated with given <em>type_num</em>. Memory for the new string is obtained with <strong>pmemobj_alloc</strong>(), on the given memory pool, and can be freed with <strong>pmemobj_free</strong>() on the same memory pool. If <strong>pmemobj_strdup</strong>() is unable to satisfy the allocation request, OID_NULL is returned and errno is set appropriately.</p></td>


<strong>size_t pmemobj_alloc_usable_size(PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_alloc_usable_size</strong>() function provides the same semantics as <strong>malloc_usable_size</strong>(3), but instead of the process heap supplied by the system, it operates on the persistent memory heap. It returns the number of usable bytes in the object represented by <em>oid</em>, a handle to an object allocated by <strong>pmemobj_alloc</strong>() or a related function. If <em>oid</em> is OID_NULL, 0 is returned.</p></td>


<strong>POBJ_NEW(PMEMobjpool *</strong><em>pop</em><strong>, TOID *</strong><em>oidp</em><strong>,</strong> <em>TYPE</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The <strong>POBJ_NEW</strong> macro is a wrapper around the <strong>pmemobj_alloc</strong>() function which takes the type name <strong>TYPE</strong> and passes the size and type number to the <strong>pmemobj_alloc</strong>() function from the typed OID. Instead of taking a pointer to <strong>PMEMoid</strong> it takes a pointer to typed OID of <strong>TYPE</strong>.</p></td>


<strong>POBJ_ALLOC(PMEMobjpool *</strong><em>pop</em><strong>, TOID *</strong><em>oidp</em><strong>,</strong> <em>TYPE</em><strong>, size_t</strong> <em>size</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The <strong>POBJ_ALLOC</strong> macro is a wrapper around the <strong>pmemobj_alloc</strong>() function which takes the type name <strong>TYPE</strong>, the size of allocation <em>size</em> and passes the type number to the <strong>pmemobj_alloc</strong>() function from the typed OID. Instead of taking a pointer to <strong>PMEMoid</strong> it takes a pointer to typed OID of <strong>TYPE</strong>.</p></td>


<strong>POBJ_ZNEW(PMEMobjpool *</strong><em>pop</em><strong>, TOID *</strong><em>oidp</em><strong>,</strong> <em>TYPE</em><strong>)</strong></p></td>


The <strong>POBJ_ZNEW</strong> macro is a wrapper around the <strong>pmemobj_zalloc</strong>() function which takes the type name <strong>TYPE</strong> and passes the size and type number to the <strong>pmemobj_zalloc</strong>() function from the typed OID. Instead of taking a pointer to <strong>PMEMoid</strong> it takes a pointer to typed OID of <strong>TYPE</strong>.</p></td>


<strong>POBJ_ZALLOC(PMEMobjpool *</strong><em>pop</em><strong>, TOID *</strong><em>oidp</em><strong>,</strong> <em>TYPE</em><strong>, size_t</strong> <em>size</em><strong>)</strong></p></td>


The <strong>POBJ_ZALLOC</strong> macro is a wrapper around the <strong>pmemobj_zalloc</strong>() function which takes the type name <strong>TYPE</strong>, the size of allocation <em>size</em> and passes the type number to the <strong>pmemobj_zalloc</strong>() function from the typed OID. Instead of taking a pointer to <strong>PMEMoid</strong> it takes a pointer to typed OID of <strong>TYPE</strong>.</p></td>


<strong>POBJ_REALLOC(PMEMobjpool *</strong><em>pop</em><strong>, TOID *</strong><em>oidp</em><strong>,</strong> <em>TYPE</em><strong>, size_t</strong> <em>size</em><strong>)</strong></p></td>


The <strong>POBJ_REALLOC</strong> macro is a wrapper around the <strong>pmemobj_realloc</strong>() function which takes the type name <strong>TYPE</strong> and passes the type number to the <strong>pmemobj_realloc</strong>() function from the typed OID. Instead of taking a pointer to <strong>PMEMoid</strong> it takes a pointer to typed OID of <strong>TYPE</strong>.</p></td>


<strong>POBJ_ZREALLOC(PMEMobjpool *</strong><em>pop</em><strong>, TOID *</strong><em>oidp</em><strong>,</strong> <em>TYPE</em><strong>, size_t</strong> <em>size</em><strong>)</strong></p></td>


The <strong>POBJ_ZREALLOC</strong> macro is a wrapper around the <strong>pmemobj_zrealloc</strong>() function which takes the type name <strong>TYPE</strong> and passes the type number to the <strong>pmemobj_zrealloc</strong>() function from the typed OID. Instead of taking a pointer to <strong>PMEMoid</strong> it takes a pointer to typed OID of <strong>TYPE</strong>.</p></td>


<strong>POBJ_FREE(TOID *</strong><em>oidp</em><strong>)</strong></p></td>


The <strong>POBJ_FREE</strong> macro is a wrapper around the <strong>pmemobj_free</strong>() function which takes pointer to typed OID <em>oidp</em> as an argument instead of <strong>PMEMoid</strong>.</p></td>


[]()

NON-TRANSACTIONAL PERSISTENT ATOMIC LISTS
-----------------------------------------

Besides the internal objects collections described in section <strong>OBJECT CONTAINERS</strong> the <strong>libpmemobj</strong> provides a mechanism to organize persistent objects in the user-defined persistent atomic circular doubly linked lists. All the routines and macros operating on the persistent lists provide atomicity with respect to any power-fail interruptions. If any of those operations is torn by program failure or system crash; on recovery they are guaranteed to be entirely completed or discarded, leaving the lists, persistent memory heap and internal object containers in a consistent state.</p>
<p>The persistent atomic circular doubly linked lists support the following functionality:</p></td>


<table>
<colgroup>
<col width="25%" />
<col width="25%" />
<col width="25%" />
<col width="25%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Insertion of an object at the head of the list, or at the end of the list.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Insertion of an object before or after any element in the list.</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Atomic allocation and insertion of a new object at the head of the list, or at the end of the list.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Atomic allocation and insertion of a new object before or after any element in the list.</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Atomic moving of an element from one list to the specific location on another list.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Removal of any object in the list.</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Atomic removal and freeing of any object in the list.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>*</p></td>
<td align="left"></td>
<td align="left"><p>Forward or backward traversal through the list.</p></td>


A list is headed by a <em>list_head</em> structure containing a single object handle of the first element on the list. The elements are doubly linked so that an arbitrary element can be removed without a need to traverse the list. New elements can be added to the list before or after an existing element, at the head of the list, or at the end of the list. A list may be traversed in either direction.</p>
<p>The user-defined structure of each element must contain a field of type <strong>list_entry</strong> holding the object handles to the previous and next element on the list. Both the <strong>list_head</strong> and the <strong>list_entry</strong> structures are declared in <strong><libpmemobj.h></strong>.</p>
<p>The functions below are intended to be used outside transactions - transactional variants are described in section <strong>TRANSACTIONAL OBJECT MANIPULATION</strong>. Note that operations performed using this non-transactional API are independent from their transactional counterparts. If any non-transactional allocations or list manipulations are performed within an open transaction, the changes will not be rolled-back if such a transaction is aborted or interrupted.</p>
<p><strong>int pmemobj_list_insert(PMEMobjpool *</strong><em>pop</em><strong>, size_t</strong> <em>pe_offset</em><strong>,<br />
void *</strong><em>head</em><strong>, PMEMoid</strong> <em>dest</em><strong>, int</strong> <em>before</em><strong>, PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_list_insert</strong> function inserts an element represented by object handle <em>oid</em> into the list referenced by <em>head</em>. Depending on the value of flag <em>before</em>, the object is added before or after the element <em>dest</em>. If <em>dest</em> value is OID_NULL, the object is inserted at the head or at the end of the list, depending on the <em>before</em> flag value. If value is 1 the object is inserted at the head, if value is 0 the object is inserted at the end of the list. The relevant values are available through <strong>POBJ_LIST_DEST_HEAD</strong> and <strong>POBJ_LIST_DEST_TAIL</strong> defines respectively. The argument <em>pe_offset</em> declares an offset of the structure that connects the elements in the list. All the handles <em>head</em>, <em>dest</em> and <em>oid</em> must point to the objects allocated from the same memory pool <em>pop</em>. The <em>head</em> and <em>oid</em> cannot be OID_NULL. On success, zero is returned. On error, -1 is returned and errno is set.</p></td>


<strong>PMEMoid pmemobj_list_insert_new(PMEMobjpool *</strong><em>pop</em><strong>, size_t</strong> <em>pe_offset</em><strong>,<br />
void *</strong><em>head</em><strong>, PMEMoid</strong> <em>dest</em><strong>, int</strong> <em>before</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>, pmemobj_constr</strong> <em>constructor</em><strong>, void *</strong><em>arg</em><strong>);</strong></p></td>


The <strong>pmemobj_list_insert_new</strong> function atomically allocates a new object of given <em>size</em> and type <em>type_num</em> and inserts it into the list referenced by <em>head</em>. Depending on the value of <em>before</em> flag, the newly allocated object is added before or after the element <em>dest</em>. If <em>dest</em> value is OID_NULL, the object is inserted at the head or at the end of the list, depending on the <em>before</em> flag value. If value is 1 the object is inserted at the head, if value is 0 the object is inserted at the end of the list. The relevant values are available through <strong>POBJ_LIST_DEST_HEAD</strong> and <strong>POBJ_LIST_DEST_TAIL</strong> defines respectively. The argument <em>pe_offset</em> declares an offset of the structure that connects the elements in the list. All the handles <em>head</em>, <em>dest</em> must point to the objects allocated from the same memory pool <em>pop</em>. Before returning, it calls the <strong>constructor</strong> function passing the pool handle <em>pop</em>, the pointer to the newly allocated object in <em>ptr</em> along with the <em>arg</em> argument. It is guaranteed that allocated object is either properly initialized or, if the allocation is interrupted before the constructor completes, the memory space reserved for the object is reclaimed. If the constructor returns non-zero value the allocation is canceled, the -1 value is returned from the caller and errno is set to <strong>ECANCELED .</strong> The <em>head</em> cannot be OID_NULL. The allocated object is also added to the internal container associated with given <em>type_num</em>. as described in section <strong>OBJECT CONTAINERS</strong>. On success, it returns a handle to the newly allocated object. On error, OID_NULL is returned and errno is set.</p></td>


<strong>int pmemobj_list_remove(PMEMobjpool *</strong><em>pop</em><strong>, size_t</strong> <em>pe_offset</em><strong>,<br />
void *</strong><em>head</em><strong>, PMEMoid</strong> <em>oid</em><strong>, int</strong> <em>free</em><strong>);</strong></p></td>


The <strong>pmemobj_list_remove</strong> function removes the object referenced by <em>oid</em> from the list pointed by <em>head</em>. If <em>free</em> flag is set, it also removes the object from the internal object container and frees the associated memory space. The argument <em>pe_offset</em> declares an offset of the structure that connects the elements in the list. Both <em>head</em> and <em>oid</em> must point to the objects allocated from the same memory pool <em>pop</em> and cannot be OID_NULL. On success, zero is returned. On error, -1 is returned and errno is set.</p></td>


<strong>int pmemobj_list_move(PMEMobjpool *</strong><em>pop</em><strong>,<br />
size_t</strong> <em>pe_old_offset</em><strong>, void *</strong><em>head_old</em><strong>,<br />
size_t</strong> <em>pe_new_offset</em><strong>, void *</strong><em>head_new</em><strong>,<br />
PMEMoid</strong> <em>dest</em><strong>, int</strong> <em>before</em><strong>, PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_list_move</strong> function moves the object represented by <em>oid</em> from the list pointed by <em>head_old</em> to the list pointed by <em>head_new</em>. Depending on the value of flag <em>before</em>, the newly allocated object is added before or after the element <em>dest</em>. If <em>dest</em> value is OID_NULL, the object is inserted at the head or at the end of the second list, depending on the <em>before</em> flag value. If value is 1 the object is inserted at the head, if value is 0 the object is inserted at the end of the list. The relevant values are available through <strong>POBJ_LIST_DEST_HEAD</strong> and <strong>POBJ_LIST_DEST_TAIL</strong> defines respectively. The arguments <em>pe_old_offset</em> and <em>pe_new_offset</em> declare the offsets of the structures that connects the elements in the old and new lists respectively. All the handles <em>head_old</em>, <em>head_new</em>, <em>dest</em> and <em>oid</em> must point to the objects allocated from the same memory pool <em>pop</em>. <em>head_old</em>, <em>head_new</em> and <em>oid</em> cannot be OID_NULL. On success, zero is returned. On error, -1 is returned and errno is set.</p></td>


[]()

TYPE-SAFE NON-TRANSACTIONAL PERSISTENT ATOMIC LISTS
---------------------------------------------------

The following macros define and operate on a type-safe persistent atomic circular doubly linked list data structure that consist of a set of persistent objects of a well-known type. Unlike the functions described in the previous section, these macros provide type enforcement by requiring declaration of type of the objects stored in given list, and not allowing mixing objects of different types in a single list.</p>
<p>The functionality and semantics of those macros is similar to circular queues defined in <strong>queue</strong>(3).</p>
<p>The majority of the macros must specify the handle to the memory pool <em>pop</em> and the name of the <em>field</em> in the user-defined structure, which must be of type <em>POBJ_LIST_ENTRY</em> and is used to connect the elements in the list.</p>
<p>A list is headed by a structure defined by the <em>POBJ_LIST_HEAD</em> macro. This structure contains an object handle of the first element on the list. The elements are doubly linked so that an arbitrary element can be removed without a need to traverse the list. New elements can be added to the list before or after an existing element, at the head of the list, or at the end of the list. A list may be traversed in either direction. A <strong>POBJ_LIST_HEAD</strong> structure is declared as follows:</p></td>


<code>#define POBJ_LIST_HEAD(HEADNAME, TYPE)
struct HEADNAME {
    TOID(TYPE) pe_first;
    PMEMmutex lock;
};</code></pre></td>


In the macro definitions, <em>TYPE</em> is the name of a user-defined structure, that must contain a field of type <em>POBJ_LIST_ENTRY</em>. The argument <em>HEADNAME</em> is the name of a user-defined structure that must be declared using the macro <em>POBJ_LIST_HEAD</em>. See the examples below for further explanation of how these macros are used.</p>
<p>The macro <strong>POBJ_LIST_ENTRY</strong> declares a structure that connects the elements in the list.</p></td>


<code>#define POBJ_LIST_ENTRY(TYPE)
struct {
    TOID(TYPE) pe_next;
    TOID(TYPE) pe_prev;
};</code></pre></td>


<strong>POBJ_LIST_FIRST(POBJ_LIST_HEAD *</strong><em>head</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_FIRST</strong> returns the first element on the list referenced by <em>head</em>. If the list is empty OID_NULL is returned.</p></td>


<strong>POBJ_LIST_LAST(POBJ_LIST_HEAD *</strong><em>head</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_LAST</strong> returns the last element on the list referenced by <em>head</em>. If the list is empty OID_NULL is returned.</p></td>


<strong>POBJ_LIST_EMPTY(POBJ_LIST_HEAD *</strong><em>head</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_EMPTY</strong> evaluates to 1 if the list referenced by <em>head</em> is empty. Otherwise, 0 is returned.</p></td>


<strong>POBJ_LIST_NEXT(TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_NEXT</strong> returns the element next to the element <em>elm</em>.</p></td>


<strong>POBJ_LIST_PREV(TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_PREV</strong> returns the element preceding the element <em>elm</em>.</p></td>


<strong>POBJ_LIST_FOREACH(TOID</strong> <em>var</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_FOREACH</strong> traverses the list referenced by <em>head</em> assigning a handle to each element in turn to <em>var</em> variable.</p></td>


<strong>POBJ_LIST_FOREACH_REVERSE(TOID</strong> <em>var</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_FOREACH_REVERSE</strong> traverses the list referenced by <em>head</em> in reverse order, assigning a handle to each element in turn to <em>var</em> variable. The <em>field</em> argument is the name of the field of type <em>POBJ_LIST_ENTRY</em> in the element structure.</p></td>


<strong>POBJ_LIST_INSERT_HEAD(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_HEAD</strong> inserts the element <em>elm</em> at the head of the list referenced by <em>head</em>.</p></td>


<strong>POBJ_LIST_INSERT_TAIL(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_TAIL</strong> inserts the element <em>elm</em> at the end of the list referenced by <em>head</em>.</p></td>


<strong>POBJ_LIST_INSERT_AFTER(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>listelm</em><strong>, TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_AFTER</strong> inserts the element <em>elm</em> into the list referenced by <em>head</em> after the element <em>listelm</em>. If <em>listelm</em> value is TOID_NULL, the object is inserted at the end of the list.</p></td>


<strong>POBJ_LIST_INSERT_BEFORE(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>listelm</em><strong>, TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_BEFORE</strong> inserts the element <em>elm</em> into the list referenced by <em>head</em> before the element <em>listelm</em>. If <em>listelm</em> value is TOID_NULL, the object is inserted at the head of the list.</p></td>


<strong>POBJ_LIST_INSERT_NEW_HEAD(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>, size_t</strong> <em>size</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_NEW_HEAD</strong> atomically allocates a new object of size <em>size</em> and inserts it at the head of the list referenced by <em>head</em>. The newly allocated object is also added to the internal object container associated with a type number which is retrieved from the typed OID of the first element on list.</p></td>


<strong>POBJ_LIST_INSERT_NEW_TAIL(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>, size_t</strong> <em>size</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_NEW_TAIL</strong> atomically allocates a new object of size <em>size</em> and inserts it at the tail of the list referenced by <em>head</em>. The newly allocated object is also added to the internal object container associated with with a type number which is retrieved from the typed OID of the first element on list.</p></td>


<strong>POBJ_LIST_INSERT_NEW_AFTER(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>listelm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>, size_t</strong> <em>size</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_NEW_AFTER</strong> atomically allocates a new object of size <em>size</em> and inserts it into the list referenced by <em>head</em> after the element <em>listelm</em>. If <em>listelm</em> value is TOID_NULL, the object is inserted at the end of the list. The newly allocated object is also added to the internal object container associated with with a type number which is retrieved from the typed OID of the first element on list.</p></td>


<strong>POBJ_LIST_INSERT_NEW_BEFORE(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>listelm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>, size_t</strong> <em>size</em><strong>,<br />
pmemobj_constr</strong> <em>constructor</em> <strong>, void *</strong><em>arg</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_INSERT_NEW_BEFORE</strong> atomically allocates a new object of size <em>size</em> and inserts it into the list referenced by <em>head</em> before the element <em>listelm</em>. If <em>listelm</em> value is TOID_NULL, the object is inserted at the head of the list. The newly allocated object is also added to the internal object container associated with with a type number which is retrieved from the typed OID of the first element on list.</p></td>


<strong>POBJ_LIST_REMOVE(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_REMOVE</strong> removes the element <em>elm</em> from the list referenced by <em>head</em>.</p></td>


<strong>POBJ_LIST_REMOVE_FREE(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_REMOVE_FREE</strong> removes the element <em>elm</em> from the list referenced by <em>head</em> and frees the memory space represented by this element.</p></td>


<strong>POBJ_LIST_MOVE_ELEMENT_HEAD(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_HEAD *</strong><em>head_new</em><strong>, TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>field_new</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_MOVE_ELEMENT_HEAD</strong> moves the element <em>elm</em> from the list referenced by <em>head</em> to the head of the list <em>head_new</em>. The <em>field</em> and <em>field_new</em> arguments are the names of the fields of type <em>POBJ_LIST_ENTRY</em> in the element structure that are used to connect the elements in both lists.</p></td>


<strong>POBJ_LIST_MOVE_ELEMENT_TAIL(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_HEAD *</strong><em>head_new</em><strong>, TOID</strong> <em>elm</em><strong>, POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>field_new</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_MOVE_ELEMENT_TAIL</strong> moves the element <em>elm</em> from the list referenced by <em>head</em> to the end of the list <em>head_new</em>. The <em>field</em> and <em>field_new</em> arguments are the names of the fields of type <em>POBJ_LIST_ENTRY</em> in the element structure that are used to connect the elements in both lists.</p></td>


<strong>POBJ_LIST_MOVE_ELEMENT_AFTER(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_HEAD *</strong><em>head_new</em><strong>, TOID</strong> <em>listelm</em><strong>, TOID</strong> <em>elm</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>, POBJ_LIST_ENTRY</strong> <em>field_new</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_MOVE_ELEMENT_AFTER</strong> atomically removes the element <em>elm</em> from the list referenced by <em>head</em> and inserts it into the list referenced by <em>head_new</em> after the element <em>listelm</em>. If <em>listelm</em> value is TOID_NULL, the object is inserted at the end of the list. The <em>field</em> and <em>field_new</em> arguments are the names of the fields of type <em>POBJ_LIST_ENTRY</em> in the element structure that are used to connect the elements in both lists.</p></td>


<strong>POBJ_LIST_MOVE_ELEMENT_BEFORE(PMEMobjpool *</strong><em>pop</em><strong>, POBJ_LIST_HEAD *</strong><em>head</em><strong>,<br />
POBJ_LIST_HEAD *</strong><em>head_new</em><strong>, TOID</strong> <em>listelm</em><strong>, TOID</strong> <em>elm</em><strong>,<br />
POBJ_LIST_ENTRY</strong> <em>FIELD</em><strong>, POBJ_LIST_ENTRY</strong> <em>field_new</em><strong>)</strong></p></td>


The macro <strong>POBJ_LIST_MOVE_ELEMENT_BEFORE</strong> atomically removes the element <em>elm</em> from the list referenced by <em>head</em> and inserts it into the list referenced by <em>head_new</em> before the element <em>listelm</em>. If <em>listelm</em> value is TOID_NULL, the object is inserted at the head of the list. The <em>field</em> and <em>field_new</em> arguments are the names of the fields of type <em>POBJ_LIST_ENTRY</em> in the element structure that are used to connect the elements in both lists.</p></td>


[]()

TRANSACTIONAL OBJECT MANIPULATION
---------------------------------

The functions described in sections <strong>NON-TRANSACTIONAL ATOMIC ALLOCATIONS</strong> and <strong>NON-TRANSACTIONAL PERSISTENT ATOMIC LISTS</strong> only guarantee the atomicity in scope of a single operation on an object. In case of more complex changes, involving multiple operations on an object, or allocation and modification of multiple objects; data consistency and fail-safety may be provided only by using <em>atomic transactions</em>.</p>
<p>A transaction is defined as series of operations on persistent memory objects that either all occur, or nothing occurs. In particular, if the execution of a transaction is interrupted by a power failure or a system crash, it is guaranteed that after system restart, all the changes made as a part of the uncompleted transaction will be rolled-back, restoring the consistent state of the memory pool from the moment when the transaction was started.</p>
<p>Note that transactions do not provide the atomicity with respect to other threads. All the modifications performed within the transactions are immediately visible to other threads, and this is the responsibility of the program to implement a proper thread synchronization mechanism.</p>
<p>Each transaction is visible only for the thread that started it. No other threads can add operations, commit or abort the transaction initiated by another thread. There may be multiple open transactions on given memory pool at the same time, but only one transaction per thread.</p>
<p>Nested transactions are supported but flattened. Committing the nested transaction does not commit the outer transaction, however errors in the nested transaction are propagated up to the outer-most level, resulting in the interruption of the entire transaction.</p>
<p>Please see the <strong>CAVEATS</strong> section for known limitations of the transactional API.</p>
<p><strong>enum tx_stage pmemobj_tx_stage(void);</strong></p></td>


The <strong>pmemobj_tx_stage</strong>() function returns the stage of the current transaction stage for a thread. Stages are changed only by the <em>pmemobj_tx_*</em> functions. The transaction stages are defined as follows:</p>
<p><strong>TX_STAGE_NONE</strong> - no open transaction in this thread</p>
<p><strong>TX_STAGE_WORK</strong> - transaction in progress</p>
<p><strong>TX_STAGE_ONCOMMIT</strong> - successfully committed</p>
<p><strong>TX_STAGE_ONABORT</strong> - starting the transaction failed or transaction aborted</p>
<p><strong>TX_STAGE_FINALLY</strong> - ready for clean up</p></td>


<strong>int pmemobj_tx_begin(PMEMobjpool *</strong><em>pop</em><strong>, jmp_buf *</strong><em>env</em><strong>,</strong> <em>…</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_begin</strong>() function starts a new transaction in the current thread. If called within an open transaction, it starts a nested transaction. The caller may use <em>env</em> argument to provide a pointer to the information of a calling environment to be restored in case of transaction abort. This information must be filled by a caller, using <strong>setjmp</strong>(3) macro.</p>
<p>Optionally, a list of pmem-resident locks may be provided as the last arguments. Each lock is specified by a pair of lock type ( <em>TX_LOCK_MUTEX</em> or <em>TX_LOCK_RWLOCK</em>) and the pointer to the lock of type <em>PMEMmutex</em> or <em>PMEMrwlock</em> respectively. The list must be terminated with <em>TX_LOCK_NONE</em>. In case of rwlocks, a write lock is acquired. It is guaranteed that <strong>pmemobj_tx_begin</strong>() will grab all the locks prior to successful completion and they will be held by the current thread until the transaction is finished. Locks are taken in the order from left to right. To avoid deadlocks, user must take care about the proper order of locks.</p>
<p>New transaction may be started only if the current stage is <em>TX_STAGE_NONE</em> or <em>TX_STAGE_WORK</em>. If successful, transaction stage changes to <em>TX_STAGE_WORK</em> and function returns zero. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em> and an error number is returned.</p></td>


<strong>int pmemobj_tx_lock(enum tx_lock</strong> <em>lock_type</em><strong>, void *</strong><em>lockp</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_lock</strong>() function grabs a lock pointed by <em>lockp</em> and adds it to the current transaction. The lock type is specified by <em>lock_type</em> ( <em>TX_LOCK_MUTEX</em> or <em>TX_LOCK_RWLOCK</em> ) and the pointer to the <em>lockp</em> of <em>PMEMmutex</em> or <em>PMEMrwlock</em> type. If successful, <em>lockp</em> is added to transaction, locked and function returns zero. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em> and an error number is returned. In case of <em>PMEMrwlock lock_type</em> function acquires a write lock. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>void pmemobj_tx_abort(int</strong> <em>errnum</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_abort</strong>() aborts the current transaction and causes transition to <em>TX_STAGE_ONABORT</em>. This function must be called during <em>TX_STAGE_WORK</em>. If the passed <em>errnum</em> is equal to zero, it shall be set to <em>ECANCELED</em>.</p></td>


<strong>void pmemobj_tx_commit(void);</strong></p></td>


The <strong>pmemobj_tx_commit</strong>() function commits the current open transaction and causes transition to <em>TX_STAGE_ONCOMMIT</em> stage. If called in context of the outer-most transaction, all the changes may be considered as durably written upon successful completion. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>int pmemobj_tx_end(void);</strong></p></td>


The <strong>pmemobj_tx_end</strong>() function performs a clean up of a current transaction. If called in context of the outer-most transaction, it releases all the locks acquired by <strong>pmemobj_tx_begin</strong>() for outer and nested transactions. Then it causes the transition to <em>TX_STAGE_NONE</em>. In case of the nested transaction, it returns to the context of the outer transaction with <em>TX_STAGE_WORK</em> stage without releasing any locks. Must always be called for each <strong>pmemobj_tx_begin</strong>(), even if starting the transaction failed. This function must <em>not</em> be called during <em>TX_STAGE_WORK</em>. If transaction was successful, returns 0. Otherwise returns error code set by <strong>pmemobj_tx_abort</strong>(). Note that <strong>pmemobj_tx_abort</strong>() can be called internally by the library.</p></td>


<strong>int pmemobj_tx_errno(void);</strong></p></td>


The <strong>pmemobj_tx_errno</strong>() function returns the error code of the last transaction.</p></td>


<strong>void pmemobj_tx_process(void);</strong></p></td>


The <strong>pmemobj_tx_process</strong>() function performs the actions associated with current stage of the transaction, and makes the transition to the next stage. It must be called in transaction. Current stage must always be obtained by a call to <strong>pmemobj_tx_stage</strong>().</p></td>


<strong>int pmemobj_tx_add_range(PMEMoid</strong> <em>oid</em><strong>, uint64_t</strong> <em>off</em><strong>, size_t</strong> <em>size</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_add_range</strong>() takes a “snapshot” of the memory block of given <em>size</em>, located at given offset <em>off</em> in the object specified by <em>oid</em> and saves it to the undo log. The application is then free to directly modify the object in that memory range. In case of a failure or abort, all the changes within this range will be rolled-back. The supplied block of memory has to be within the pool registered in the transaction. If successful, returns zero. Otherwise, state changes to <em>TX_STAGE_ONABORT</em> and an error number is returned. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>int pmemobj_tx_add_range_direct(const void *</strong><em>ptr</em><strong>, size_t</strong> <em>size</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_add_range_direct</strong>() behaves the same as <strong>pmemobj_tx_add_range</strong>() with the exception that it operates on virtual memory addresses and not persistent memory objects. It takes a “snapshot” of a persistent memory block of given <em>size</em>, located at the given address <em>ptr</em> in the virtual memory space and saves it to the undo log. The application is then free to directly modify the object in that memory range. In case of a failure or abort, all the changes within this range will be rolled-back. The supplied block of memory has to be within the pool registered in the transaction. If successful, returns zero. Otherwise, state changes to <em>TX_STAGE_ONABORT</em> and an error number is returned. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>PMEMoid pmemobj_tx_alloc(size_t</strong> <em>size</em><strong>, uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_alloc</strong>() transactionally allocates a new object of given <em>size</em> and <em>type_num</em>. In contrast to the non-transactional allocations, the objects are added to the internal object containers of given <em>type_num</em> only after the transaction is committed, making the objects visible to the <strong>POBJ_FOREACH_*</strong> macros. If successful, returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately. If <em>size</em> equals 0, OID_NULL is returned and errno is set appropriately. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>PMEMoid pmemobj_tx_zalloc(size_t</strong> <em>size</em><strong>, uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The pmemobj_tx_zalloc () function transactionally allocates new zeroed object of given <em>size</em> and <em>type_num</em>. If successful, returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately. If <em>size</em> equals 0, OID_NULL is returned and errno is set appropriately. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>PMEMoid pmemobj_tx_realloc(PMEMoid</strong> <em>oid</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_realloc</strong>() function transactionally resizes an existing object to the given <em>size</em> and changes its type to <em>type_num</em>. If <em>oid</em> is OID_NULL, then the call is equivalent to <strong>pmemobj_tx_alloc(</strong><em>pop</em><strong>,</strong> <em>size</em><strong>,</strong> <em>type_num</em><strong>).</strong> If <em>size</em> is equal to zero and <em>oid</em> is not OID_NULL, then the call is equivalent to <strong>pmemobj_tx_free(</strong><em>oid</em><strong>).</strong> If the new size is larger than the old size, the added memory will <em>not</em> be initialized. If successful, returns returns a handle to the resized object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately. Note that the object handle value may change in result of reallocation. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>PMEMoid pmemobj_tx_zrealloc(PMEMoid</strong> <em>oid</em><strong>, size_t</strong> <em>size</em><strong>,<br />
uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_zrealloc</strong>() function transactionally resizes an existing object to the given <em>size</em> and changes its type to <em>type_num</em>. If the new size is larger than the old size, the extended new space is zeroed. If successful, returns returns a handle to the resized object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately. Note that the object handle value may change in result of reallocation. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>PMEMoid pmemobj_tx_strdup(const char *</strong><em>s</em><strong>, uint64_t</strong> <em>type_num</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_strdup</strong>() function transactionally allocates a new object containing a duplicate of the string <em>s</em> and assigns it a type <em>type_num</em>. If successful, returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


<strong>int pmemobj_tx_free(PMEMoid</strong> <em>oid</em><strong>);</strong></p></td>


The <strong>pmemobj_tx_free</strong>() function transactionally frees an existing object referenced by <em>oid</em>. If successful, returns zero. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em> and an error number is returned. This function must be called during <em>TX_STAGE_WORK</em>.</p></td>


In addition to the above API, the <strong>libpmemobj</strong> offers a more intuitive method of building transactions using a set of macros described below. When using macros, the complete transaction flow looks like this:</p></td>


<code>TX_BEGIN(Pop) {</code></pre></td>


/* the actual transaction code goes here… */</p></td>


} TX_ONCOMMIT {</p></td>


/*</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>* optional - executed only if the above block</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>* successfully completes</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>*/</p></td>


} TX_ONABORT {</p></td>


/*</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>* optional - executed only if starting the transaction fails,</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>* or if transaction is aborted by an error or a call to</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>* pmemobj_tx_abort()</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>*/</p></td>


} TX_FINALLY {</p></td>


/*</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>* optional - if exists, it is executed after</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>* TX_ONCOMMIT or TX_ONABORT block</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>*/</p></td>


} TX_END /* mandatory */</p></td>


<strong>TX_BEGIN_LOCK(PMEMobjpool *</strong><em>pop</em><strong>,</strong> <em>…</em><strong>)</strong></p>
<p><strong>TX_BEGIN(PMEMobjpool *</strong><em>pop</em><strong>)</strong></p></td>


The <strong>TX_BEGIN_LOCK</strong>() and <strong>TX_BEGIN</strong>() macros start a new transaction in the same way as <strong>pmemobj_tx_begin</strong>(), except that instead of the environment buffer provided by a caller, they set up the local <em>jmp_buf</em> buffer and use it to catch the transaction abort. The <strong>TX_BEGIN</strong>() macro may be used in case when there is no need to grab any locks prior to starting a transaction (like for a single-threaded program). Each of those macros shall be followed by a block of code with all the operations that are to be performed atomically.</p></td>


<strong>TX_ONABORT</strong></p></td>


The <strong>TX_ONABORT</strong> macro starts a block of code that will be executed only if starting the transaction fails due to an error in <strong>pmemobj_tx_begin</strong>(), or if the transaction is aborted. This block is optional, but in practice it should not be omitted. If it’s desirable to crash the application when transaction aborts and there’s no <strong>TX_ONABORT</strong> section, application can define <strong>POBJ_TX_CRASH_ON_NO_ONABORT</strong> macro before inclusion of <strong><libpmemobj.h></strong>. It provides default <strong>TX_ONABORT</strong> section which just calls <strong>abort</strong>(3).</p></td>


<strong>TX_ONCOMMIT</strong></p></td>


The <strong>TX_ONCOMMIT</strong> macro starts a block of code that will be executed only if the transaction is successfully committed, which means that the execution of code in <strong>TX_BEGIN</strong> block has not been interrupted by an error or by a call to <strong>pmemobj_tx_abort</strong>(). This block is optional.</p></td>


<strong>TX_FINALLY</strong></p></td>


The <strong>TX_FINALLY</strong> macro starts a block of code that will be executed regardless of whether the transaction is committed or aborted. This block is optional.</p></td>


<strong>TX_END</strong></p></td>


The <strong>TX_END</strong> macro cleans up and closes the transaction started by <strong>TX_BEGIN</strong>() or <strong>TX_BEGIN_LOCK</strong>() macro. It is mandatory to terminate each transaction with this macro. If the transaction was aborted, errno is set appropriately.</p></td>


Similarly to the macros controlling the transaction flow, the <strong>libpmemobj</strong> defines a set of macros that simplify the transactional operations on persistent objects. Note that those macros operate on typed object handles, thus eliminating the need to specify the size of the object, or the size and offset of the field in the user-defined structure that is to be modified.</p>
<p><strong>TX_ADD_FIELD(TOID</strong> <em>o</em><strong>,</strong> <em>FIELD</em><strong>)</strong></p></td>


The <strong>TX_ADD_FIELD</strong>() macro saves in the undo log the current value of given <em>FIELD</em> of the object referenced by a handle <em>o</em>. The application is then free to directly modify the specified <em>FIELD</em>. In case of a failure or abort, the saved value will be restored.</p></td>


<strong>TX_ADD(TOID</strong> <em>o</em><strong>)</strong></p></td>


The <strong>TX_ADD</strong>() macro takes a “snapshot” of the entire object referenced by object handle <em>o</em> and saves it in the undo log. The object size is determined from its <em>TYPE</em>. The application is then free to directly modify the object. In case of a failure or abort, all the changes within the object will be rolled-back.</p></td>


<strong>TX_ADD_FIELD_DIRECT(TYPE *</strong><em>p</em><strong>,</strong> <em>FIELD</em><strong>)</strong></p></td>


The <strong>TX_ADD_FIELD_DIRECT</strong>() macro saves in the undo log the current value of given <em>FIELD</em> of the object referenced by (direct) pointer <em>p</em>. The application is then free to directly modify the specified <em>FIELD</em>. In case of a failure or abort, the saved value will be restored.</p></td>


<strong>TX_ADD_DIRECT(TYPE *</strong><em>p</em><strong>)</strong></p></td>


The <strong>TX_ADD_DIRECT</strong>() macro takes a “snapshot” of the entire object referenced by (direct) pointer <em>p</em> and saves it in the undo log. The object size is determined from its <em>TYPE</em>. The application is then free to directly modify the object. In case of a failure or abort, all the changes within the object will be rolled-back.</p></td>


<strong>TX_SET(TOID</strong> <em>o</em><strong>,</strong> <em>FIELD</em><strong>,</strong> <em>VALUE</em><strong>)</strong></p></td>


The <strong>TX_SET</strong> macro saves in the undo log the current value of given <em>FIELD</em> of the object referenced by a handle <em>o</em>, and then set its new <em>VALUE</em>. In case of a failure or abort, the saved value will be restored.</p></td>


<strong>TX_SET_DIRECT(TYPE *</strong><em>p</em><strong>,</strong> <em>FIELD</em><strong>,</strong> <em>VALUE</em><strong>)</strong></p></td>


The <strong>TX_SET_DIRECT</strong> macro saves in the undo log the current value of given <em>FIELD</em> of the object referenced by (direct) pointer <em>p</em>, and then set its new <em>VALUE</em>. In case of a failure or abort, the saved value will be restored.</p></td>


<strong>TX_MEMCPY(void *</strong><em>dest</em><strong>, const void *</strong><em>src</em><strong>, size_t</strong> <em>num</em><strong>)</strong></p></td>


The <strong>TX_MEMCPY</strong> macro saves in the undo log the current content of <em>dest</em> buffer and then overwrites the first <em>num</em> bytes of its memory area with the data copied from the buffer pointed by <em>src</em>. In case of a failure or abort, the saved value will be restored.</p></td>


<strong>TX_MEMSET(void *</strong><em>dest</em><strong>, int</strong> <em>c</em><strong>, size_t</strong> <em>num</em><strong>)</strong></p></td>


The <strong>TX_MEMSET</strong> macro saves in the undo log the current content of <em>dest</em> buffer and then fills the first <em>num</em> bytes of its memory area with the constant byte <em>c</em>. In case of a failure or abort, the saved value will be restored.</p></td>


<strong>TX_NEW(</strong><em>TYPE</em><strong>)</strong></p></td>


The <strong>TX_NEW</strong>() macro transactionally allocates a new object of given <em>TYPE</em> and assigns it a type number read from the typed OID. The allocation size is determined from the size of the user-defined structure <em>TYPE</em>. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_ALLOC(</strong><em>TYPE</em><strong>, size_t</strong> <em>size</em><strong>)</strong></p></td>


The <strong>TX_ALLOC</strong>() macro transactionally allocates a new object of given <em>TYPE</em> and assigns it a type number read from the typed OID. The allocation size is passed by <em>size</em> parameter. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_ZNEW(</strong><em>TYPE</em><strong>)</strong></p></td>


The <strong>TX_ZNEW</strong>() macro transactionally allocates a new zeroed object of given <em>TYPE</em> and assigns it a type number read from the typed OID. The allocation size is determined from the size of the user-defined structure <em>TYPE</em>. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_ZALLOC(</strong><em>TYPE</em><strong>)</strong></p></td>


The <strong>TX_ZALLOC</strong>() macro transactionally allocates a new zeroed object of given <em>TYPE</em> and assigns it a type number read from the typed OID. The allocation size is passed by <em>size</em> argument. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_REALLOC(TOID</strong> <em>o</em><strong>, size_t</strong> <em>size</em><strong>)</strong></p></td>


The <strong>TX_REALLOC</strong>() macro transactionally resizes an existing object referenced by a handle <em>o</em> to the given <em>size</em>. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the reallocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_ZREALLOC(TOID</strong> <em>o</em><strong>, size_t</strong> <em>size</em><strong>)</strong></p></td>


The <strong>TX_ZREALLOC</strong>() macro transactionally resizes an existing object referenced by a handle <em>o</em> to the given <em>size</em>. If the new size is larger than the old size, the extended new space is zeroed. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the reallocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_STRDUP(const char *</strong><em>s</em><strong>, uint64_t</strong> <em>type_num</em><strong>)</strong></p></td>


The <strong>TX_STRDUP</strong>() macro transactionally allocates a new object containing a duplicate of the string <em>s</em> and assigns it a type <em>type_num</em>. If successful and called during <em>TX_STAGE_WORK</em> it returns a handle to the newly allocated object. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em>, OID_NULL is returned, and errno is set appropriately.</p></td>


<strong>TX_FREE(TOID</strong> <em>o</em><strong>)</strong></p></td>


The <strong>TX_FREE</strong>() transactionally frees the memory space represented by an object handle <em>o</em>. If <em>o</em> is OID_NULL, no operation is performed. If successful and called during <em>TX_STAGE_WORK</em> it returns zero. Otherwise, stage changes to <em>TX_STAGE_ONABORT</em> and an error number is returned.</p></td>


[]()

CAVEATS
-------

The transaction flow control is governed by the <strong>setjmp</strong>(3)/<strong>longjmp</strong>(3) macros and they are used in both the macro and function flavors of the API. The transaction will longjmp on transaction abort. This has one major drawback which is described in the ISO C standard subsection 7.13.2.1. It says that <strong>the values of objects of automatic storage duration that are local to the function containing the setjmp invocation that do not have volatile-qualified type and have been changed between the setjmp invocation and longjmp call are indeterminate.</strong></p>
<p>The following example illustrates the issue described above.</p></td>


<code>int *bad_example_1 = NULL;
int *bad_example_2 = NULL;
int *bad_example_3 = NULL;
volatile int *good_example = NULL;

TX_BEGIN(Pop) {
        bad_example_1 = malloc(...);
        bad_example_2 = malloc(...);
        bad_example_3 = malloc(...);
        good_example = malloc(...);
        ...
        pmemobj_tx_abort(EINVAL); /* manual or library abort called here */
} TX_ONCOMMIT {
        /*
         * This section is longjmp-safe
         */
} TX_ONABORT {
        /*
         * This section is not longjmp-safe
         */

        free(bad_example_1); /* undefined behavior */
         free(good_example); /* OK */
} TX_FINALLY {
        /*
         * This section is not longjmp-safe on transaction abort only
         */

        free(bad_example_2); /* undefined behavior */
} TX_END

free(bad_example_3); /* undefined behavior */</code></pre></td>


Objects which are not volatile-qualified, are of automatic storage duration and have been changed between the invocations of <strong>setjmp</strong>(3) and <strong>longjmp</strong>(3) (that also means within the work section of the transaction after TX_BEGIN) should not be used after a transaction abort or should be used with utmost care. This also includes code after the <strong>TX_END</strong> macro.</p></td>


[]()

LIBRARY API VERSIONING
----------------------

This section describes how the library API is versioned, allowing applications to work with an evolving API.</p>
<p><strong>const char *pmemobj_check_version(<br />
unsigned</strong> <em>major_required</em><strong>,<br />
unsigned</strong> <em>minor_required</em><strong>);</strong></p></td>


The <strong>pmemobj_check_version</strong>() function is used to see if the installed <strong>libpmemobj</strong> supports the version of the library API required by an application. The easiest way to do this is for the application to supply the compile-time version information, supplied by defines in <strong><libpmemobj.h></strong>, like this:</p>
<pre><code>reason = pmemobj_check_version(PMEMOBJ_MAJOR_VERSION,
                            PMEMOBJ_MINOR_VERSION);
if (reason != NULL) {
    /*  version check failed, reason string tells you why */
}</code></pre>
<p>Any mismatch in the major version number is considered a failure, but a library with a newer minor version number will pass this check since increasing minor versions imply backwards compatibility.</p>
<p>An application can also check specifically for the existence of an interface by checking for the version where that interface was introduced. These versions are documented in this man page as follows: unless otherwise specified, all interfaces described here are available in version 1.0 of the library. Interfaces added after version 1.0 will contain the text <em>introduced in version x.y</em> in the section of this manual describing the feature.</p>
<p>When the version check performed by <strong>pmemobj_check_version</strong>() is successful, the return value is NULL. Otherwise the return value is a static string describing the reason for failing the version check. The string returned by <strong>pmemobj_check_version</strong>() must not be modified or freed.</p></td>


[]()

MANAGING LIBRARY BEHAVIOR
-------------------------

The library entry points described in this section are less commonly used than the previous sections.</p>
<p><strong>void pmemobj_set_funcs(<br />
void *(*</strong><em>malloc_func</em><strong>)(size_t</strong> <em>size</em><strong>),<br />
void (*</strong><em>free_func</em><strong>)(void *</strong><em>ptr</em><strong>),<br />
void *(*</strong><em>realloc_func</em><strong>)(void *</strong><em>ptr</em><strong>, size_t</strong> <em>size</em><strong>),<br />
char *(*</strong><em>strdup_func</em><strong>)(const char *</strong><em>s</em><strong>));</strong></p></td>


The <strong>pmemobj_set_funcs</strong>() function allows an application to override memory allocation calls used internally by <strong>libpmemobj</strong>. Passing in NULL for any of the handlers will cause the <strong>libpmemobj</strong> default function to be used. The library does not make heavy use of the system malloc functions, but it does allocate approximately 4-8 kilobytes for each memory pool in use.</p></td>


<strong>int pmemobj_check(const char *</strong><em>path</em><strong>, const char *</strong><em>layout</em><strong>);</strong></p></td>


The <strong>pmemobj_check</strong>() function performs a consistency check of the file indicated by <em>path</em> and returns 1 if the memory pool is found to be consistent. Any inconsistencies found will cause <strong>pmemobj_check</strong>() to return 0, in which case the use of the file with <strong>libpmemobj</strong> will result in undefined behavior. The debug version of <strong>libpmemobj</strong> will provide additional details on inconsistencies when <strong>PMEMOBJ_LOG_LEVEL</strong> is at least 1, as described in the <strong>DEBUGGING AND ERROR HANDLING</strong> section below. <strong>pmemobj_check</strong>() will return -1 and set errno if it cannot perform the consistency check due to other errors. <strong>pmemobj_check</strong>() opens the given <em>path</em> read-only so it never makes any changes to the file.</p></td>


[]()

DEBUGGING AND ERROR HANDLING
----------------------------

Two versions of <strong>libpmemobj</strong> are typically available on a development system. The normal version, accessed when a program is linked using the <strong>-lpmemobj</strong> option, is optimized for performance. That version skips checks that impact performance and never logs any trace information or performs any run-time assertions. If an error is detected during the call to <strong>libpmemobj</strong> function, an application may retrieve an error message describing the reason of failure using the following function:</p>
<p><strong>const char *pmemobj_errormsg(void);</strong></p></td>


The <strong>pmemobj_errormsg</strong>() function returns a pointer to a static buffer containing the last error message logged for current thread. The error message may include description of the corresponding error code (if errno was set), as returned by <strong>strerror</strong>(3). The error message buffer is thread-local; errors encountered in one thread do not affect its value in other threads. The buffer is never cleared by any library function; its content is significant only when the return value of the immediately preceding call to <strong>libpmemobj</strong> function indicated an error, or if errno was set. The application must not modify or free the error message string, but it may be modified by subsequent calls to other library functions.</p></td>


A second version of <strong>libpmemobj</strong>, accessed when a program uses the libraries under <strong>/usr/lib/nvml_debug</strong>, contains run-time assertions and trace points. The typical way to access the debug version is to set the environment variable <strong>LD_LIBRARY_PATH</strong> to <strong>/usr/lib/nvml_debug</strong> or <strong>/usr/lib64/nvml_debug</strong> depending on where the debug libraries are installed on the system. The trace points in the debug version of the library are enabled using the environment variable <strong>PMEMOBJ_LOG_LEVEL</strong>, which can be set to the following values:</p></td>


<table>
<colgroup>
<col width="25%" />
<col width="25%" />
<col width="25%" />
<col width="25%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>0</p></td>
<td align="left"></td>
<td align="left"><p>This is the default level when <strong>PMEMOBJ_LOG_LEVEL</strong> is not set. No log messages are emitted at this level.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>1</p></td>
<td align="left"></td>
<td align="left"><p>Additional details on any errors detected are logged (in addition to returning the errno-based errors as usual). The same information may be retrieved using <strong>pmemobj_errormsg</strong>().</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>2</p></td>
<td align="left"></td>
<td align="left"><p>A trace of basic operations is logged.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>3</p></td>
<td align="left"></td>
<td align="left"><p>This level enables a very verbose amount of function call tracing in the library.</p></td>
</tr>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>4</p></td>
<td align="left"></td>
<td align="left"><p>This level enables voluminous and fairly obscure tracing information that is likely only useful to the <strong>libpmemobj</strong> developers.</p></td>


The environment variable <strong>PMEMOBJ_LOG_FILE</strong> specifies a file name where all logging information should be written. If the last character in the name is “-”, the PID of the current process will be appended to the file name when the log file is created. If <strong>PMEMOBJ_LOG_FILE</strong> is not set, the logging output goes to stderr.</p>
<p>Setting the environment variable <strong>PMEMOBJ_LOG_LEVEL</strong> has no effect on the non-debug version of <strong>libpmemobj</strong>.</p></td>


[]()

EXAMPLES
--------

See http://pmem.io/nvml/libpmemobj for examples using the <strong>libpmemobj</strong> API.</p></td>


[]()

ACKNOWLEDGEMENTS
----------------

<strong>libpmemobj</strong> builds on the persistent memory programming model recommended by the SNIA NVM Programming Technical Work Group:</p></td>


http://snia.org/nvmp</p></td>


[]()

SEE ALSO
--------

<strong>mmap</strong>(2), <strong>munmap</strong>(2), <strong>msync</strong>(2), <strong>pthread_mutex</strong>(3), <strong>pthread_rwlock</strong>(3), <strong>pthread_cond</strong>(3), <strong>strerror</strong>(3), <strong>libpmemblk</strong>(3), <strong>libpmemlog</strong>(3), <strong>libpmem</strong>(3), <strong>libvmem</strong>(3) and <strong>http://pmem.io</strong>.</p></td>


------------------------------------------------------------------------
