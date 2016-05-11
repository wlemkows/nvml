---
layout: manual
Content-Style: 'text/css'
generator: 'groff -Thtml, see www.gnu.org'
title: libpmemblk
...

[NAME](#name)<br />
[SYNOPSIS](#synapsis)<br />
[DESCRIPTION](#description)<br />
[MOST COMMONLY USED FUNCTIONS](#most-commonly-used-functions)<br />
[LIBRARY API VERSIONING](#library-api-versioning)<br />
[MANAGING LIBRARY BEHAVIOR](#managing-library-behavior)<br />
[DEBUGGING AND ERROR HANDLING](#debugging-and-error-handling)<br />
[EXAMPLES](#examples)<br />
[BUGS](#bugs)<br />
[ACKNOWLEDGEMENTS](#acknowledgements)<br />
[SEE ALSO](#see-also)<br />


###NAME

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>libpmemblk − persistent memory resident array of blocks</p></td>
</tr>
</tbody>
</table>

###SYNOPSIS

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><pre><code>#include &lt;libpmemblk.h&gt;

cc ... -lpmemblk -lpmem

Most commonly used functions:

PMEMblkpool *pmemblk_open(const char *path, size_t bsize);
PMEMblkpool *pmemblk_create(const char *path, size_t bsize,
    size_t poolsize, mode_t mode);
void pmemblk_close(PMEMblkpool *pbp);
size_t pmemblk_bsize(PMEMblkpool *pbp);
size_t pmemblk_nblock(PMEMblkpool *pbp);
int pmemblk_read(PMEMblkpool *pbp, void *buf, long long blockno);
int pmemblk_write(PMEMblkpool *pbp, const void *buf,
    long long blockno);
int pmemblk_set_zero(PMEMblkpool *pbp, long long blockno);
int pmemblk_set_error(PMEMblkpool *pbp, long long blockno);

Library API versioning:

const char *pmemblk_check_version(
    unsigned major_required,
    unsigned minor_required);

Managing library behavior:

void pmemblk_set_funcs(
    void *(*malloc_func)(size_t size),
    void (*free_func)(void *ptr),
    void *(*realloc_func)(void *ptr, size_t size),
    char *(*strdup_func)(const char *s));
int pmemblk_check(const char *path, size_t bsize);

Error handling:

const char *pmemblk_errormsg(void);</code></pre></td>
</tr>
</tbody>
</table>

[]()

###DESCRIPTION

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>libpmemblk</strong> provides an array of blocks in <em>persistent memory</em> (pmem) such that updates to a single block are atomic. This library is intended for applications using direct access storage (DAX), which is storage that supports load/store access without paging blocks from a block storage device. Some types of <em>non-volatile memory DIMMs</em> (NVDIMMs) provide this type of byte addressable access to storage. A <em>persistent memory aware file system</em> is typically used to expose the direct access to applications. Memory mapping a file from this type of file system results in the load/store, non-paged access to pmem. <strong>libpmemblk</strong> builds on this type of memory mapped file.</p>
<p>This library is for applications that need a potentially large array of blocks, all the same size, where any given block is updated atomically (the update cannot be <em>torn</em> by program interruption such as power failures). This library builds on the low-level pmem support provided by <strong>libpmem</strong>(3), handling the transactional update of the blocks, flushing to persistence, and recovery for the application.</p>
<p><strong>libpmemblk</strong> is one of a collection of persistent memory libraries available, the others are:</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>libpmemobj</strong>(3), a general use persistent memory API, providing memory allocation and transactional operations on variable-sized objects.</p>
<p><strong>libpmemlog</strong>(3), providing a pmem-resident log file.</p>
<p><strong>libpmem</strong>(3), low-level persistent memory support.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>Under normal usage, <strong>libpmemblk</strong> will never print messages or intentionally cause the process to exit. The only exception to this is the debugging information, when enabled, as described under <strong>DEBUGGING AND ERROR HANDLING</strong> below.</p></td>
</tr>
</tbody>
</table>

[]()

### MOST COMMONLY USED FUNCTIONS

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>To use the atomic block arrays supplied by <strong>libpmemblk</strong>, a <em>memory pool</em> is first created. This is done with the <strong>pmemblk_create</strong>() function described in this section. The other functions described in this section then operate on the resulting block memory pool.</p>
<p>Once created, the memory pool is represented by an opaque handle, of type <em>PMEMblkpool *</em>, which is passed to most of the other functions in this section. Internally, <strong>libpmemblk</strong> will use either <strong>pmem_persist</strong>() or <strong>msync</strong>(2) when it needs to flush changes, depending on whether the memory pool appears to be persistent memory or a regular file (see the <strong>pmem_is_pmem</strong>() function in <strong>libpmem</strong>(3) for more information). There is no need for applications to flush changes directly when using the block memory API provided by <strong>libpmemblk</strong>.</p>
<p><strong>PMEMblkpool *pmemblk_open(const char *</strong><em>path</em><strong>, size_t</strong> <em>bsize</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_open</strong>() function opens an existing block memory pool, returning a memory pool handle used with most of the functions in this section. <em>path</em> must be an existing file containing a block memory pool as created by <strong>pmemblk_create</strong>(). The application must have permission to open the file and memory map it with read/write permissions. If the <em>bsize</em> provided is non-zero, <strong>pmemblk_open</strong>() will verify the given block size matches the block size used when the pool was created. Otherwise, <strong>pmemblk_open</strong>() will open the pool without verification of the block size. The <em>bsize</em> can be determined using the <strong>pmemblk_bsize</strong>() function. If an error prevents the pool from being opened, <strong>pmemblk_open</strong>() returns NULL and sets errno appropriately. A block size mismatch with the <em>bsize</em> argument passed in results in errno being set to EINVAL.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>PMEMblkpool *pmemblk_create(const char *</strong><em>path</em><strong>, size_t</strong> <em>bsize</em><strong>,<br />
size_t</strong> <em>poolsize</em><strong>, mode_t</strong> <em>mode</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_create</strong>() function creates a block memory pool with the given total <em>poolsize</em> divided up into as many elements of size <em>bsize</em> as will fit in the pool. Since the transactional nature of a block memory pool requires some space overhead in the memory pool, the resulting number of available blocks is less than <em>poolsize / bsize</em>, and is made available to the caller via the <strong>pmemblk_nblock</strong>() function described below. Given the specifics of the implementation, the number of available blocks for the user cannot be less than 256. This translates to at least 512 internal blocks. <em>path</em> specifies the name of the memory pool file to be created. <em>mode</em> specifies the permissions to use when creating the file as described by <strong>creat</strong>(2). The memory pool file is fully allocated to the size <em>poolsize</em> using <strong>posix_fallocate</strong>(3). The caller may choose to take responsibility for creating the memory pool file by creating it before calling <strong>pmemblk_create</strong>() and then specifying <em>poolsize</em> as zero. In this case <strong>pmemblk_create</strong>() will take the pool size from the size of the existing file and will verify that the file appears to be empty by searching for any non-zero data in the pool header at the beginning of the file. The minimum file size allowed by the library for a block pool is defined in <strong>&lt;libpmemblk.h&gt;</strong> as <strong>PMEMBLK_MIN_POOL</strong>. <em>bsize</em> can be any non-zero value, however <strong>libpmemblk</strong> will silently round up the given size to <strong>PMEMBLK_MIN_BLK</strong>, as defined in <strong>&lt;libpmemblk.h&gt;</strong>.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>Depending on the configuration of the system, the available space of non-volatile memory space may be divided into multiple memory devices. In such case, the maximum size of the pmemblk memory pool could be limited by the capacity of a single memory device. The <strong>libpmemblk</strong> allows building persistent memory resident array spanning multiple memory devices by creation of persistent memory pools consisting of multiple files, where each part of such a <em>pool set</em> may be stored on different pmem-aware filesystem.</p>
<p>Creation of all the parts of the pool set can be done with the <strong>pmemblk_create</strong>() function. However, the recommended method for creating pool sets is to do it by using the <strong>pmempool</strong>(1) utility.</p>
<p>When creating the pool set consisting of multiple files, the <em>path</em> argument passed to <strong>pmemblk_create</strong>() must point to the special <em>set</em> file that defines the pool layout and the location of all the parts of the pool set. The <em>poolsize</em> argument must be 0. The meaning of <em>layout</em> and <em>mode</em> arguments doesn’t change, except that the same <em>mode</em> is used for creation of all the parts of the pool set. If the error prevents any of the pool set files from being created, <strong>pmemblk_create</strong>() returns NULL and sets errno appropriately.</p>
<p>When opening the pool set consisting of multiple files, the <em>path</em> argument passed to <strong>pmemblk_open</strong>() must not point to the pmemblk memory pool file, but to the same <em>set</em> file that was used for the pool set creation. If an error prevents any of the pool set files from being opened, or if the actual size of any file does not match the corresponding part size defined in <em>set</em> file <strong>pmemblk_open</strong>() returns NULL and sets errno appropriately.</p>
<p>The set file is a plain text file, which must start with the line containing a <em>PMEMPOOLSET</em> string, followed by the specification of all the pool parts in the next lines. For each part, the file size and the absolute path must be provided. The minimum file size of each part of the pool set is the same as the minimum size allowed for a block pool consisting of one file. It is defined in <strong>&lt;libpmemblk.h&gt;</strong> as <strong>PMEMBLK_MIN_POOL</strong>. Lines starting with “#” character are ignored.</p>
<p>Here is the example “myblkpool.set” file:</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><pre><code>PMEMPOOLSET
100G /mountpoint0/myfile.part0
200G /mountpoint1/myfile.part1
400G /mountpoint2/myfile.part2</code></pre></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The files in the set may be created by running the following command:</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><pre><code>pmempool create blk &lt;bsize&gt; --from-set=myblkpool.set</code></pre></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>void pmemblk_close(PMEMblkpool *</strong><em>pbp</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_close</strong>() function closes the memory pool indicated by <em>pbp</em> and deletes the memory pool handle. The block memory pool itself lives on in the file that contains it and may be re-opened at a later time using <strong>pmemblk_open</strong>() as described above.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>size_t pmemblk_bsize(PMEMblkpool *</strong><em>pbp</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_bsize</strong>() function returns the block size of the specified block memory pool. It’s the value which was passed as <em>bsize</em> to <strong>pmemblk_create</strong>(). <em>pbp</em> must be a block memory pool handle as returned by <strong>pmemblk_open</strong>() or <strong>pmemblk_create</strong>().</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>size_t pmemblk_nblock(PMEMblkpool *</strong><em>pbp</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_nblock</strong>() function returns the usable space in the block memory pool, expressed as the number of blocks available. <em>pbp</em> must be a block memory pool handle as returned by <strong>pmemblk_open</strong>() or <strong>pmemblk_create</strong>().</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>int pmemblk_read(PMEMblkpool *</strong><em>pbp</em><strong>, void *</strong><em>buf</em><strong>, long long</strong> <em>blockno</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_read</strong>() function reads a block from memory pool <em>pbp</em>, block number <em>blockno</em>, into the buffer <em>buf</em>. On success, zero is returned. On error, -1 is returned and errno is set. Reading a block that has never been written by <strong>pmemblk_write</strong>() will return a block of zeroes.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>int pmemblk_write(PMEMblkpool *</strong><em>pbp</em><strong>, const void *</strong><em>buf</em><strong>,<br />
long long</strong> <em>blockno</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_write</strong>() function writes a block from <em>buf</em> to block number <em>blockno</em> in the memory pool <em>pbp</em>. The write is atomic with respect to other reads and writes. In addition, the write cannot be torn by program failure or system crash; on recovery the block is guaranteed to contain either the old data or the new data, never a mixture of both. On success, zero is returned. On error, -1 is returned and errno is set.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>int pmemblk_set_zero(PMEMblkpool *</strong><em>pbp</em><strong>, long long</strong> <em>blockno</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_set_zero</strong>() function writes zeros to block number <em>blockno</em> in memory pool <em>pbp</em>. Using this function is faster than actually writing a block of zeros since <strong>libpmemblk</strong> uses metadata to indicate the block should read back as zero. On success, zero is returned. On error, -1 is returned and errno is set.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>int pmemblk_set_error(PMEMblkpool *</strong><em>pbp</em><strong>, long long</strong> <em>blockno</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_set_error</strong>() function sets the error state for block number <em>blockno</em> in memory pool <em>pbp</em>. A block in the error state returns errno EIO when read. Writing the block clears the error state and returns the block to normal use. On success, zero is returned. On error, -1 is returned and errno is set.</p></td>
</tr>
</tbody>
</table>

### LIBRARY API VERSIONING

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>This section describes how the library API is versioned, allowing applications to work with an evolving API.</p>
<p><strong>const char *pmemblk_check_version(<br />
unsigned</strong> <em>major_required</em><strong>,<br />
unsigned</strong> <em>minor_required</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_check_version</strong>() function is used to see if the installed <strong>libpmemblk</strong> supports the version of the library API required by an application. The easiest way to do this is for the application to supply the compile-time version information, supplied by defines in <strong>&lt;libpmemblk.h&gt;</strong>, like this:</p>
<pre><code>reason = pmemblk_check_version(PMEMBLK_MAJOR_VERSION,
                            PMEMBLK_MINOR_VERSION);
if (reason != NULL) {
    /*  version check failed, reason string tells you why */
}</code></pre>
<p>Any mismatch in the major version number is considered a failure, but a library with a newer minor version number will pass this check since increasing minor versions imply backwards compatibility.</p>
<p>An application can also check specifically for the existence of an interface by checking for the version where that interface was introduced. These versions are documented in this man page as follows: unless otherwise specified, all interfaces described here are available in version 1.0 of the library. Interfaces added after version 1.0 will contain the text <em>introduced in version x.y</em> in the section of this manual describing the feature.</p>
<p>When the version check performed by <strong>pmemblk_check_version</strong>() is successful, the return value is NULL. Otherwise the return value is a static string describing the reason for failing the version check. The string returned by <strong>pmemblk_check_version</strong>() must not be modified or freed.</p></td>
</tr>
</tbody>
</table>


### MANAGING LIBRARY BEHAVIOR

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The library entry points described in this section are less commonly used than the previous sections.</p>
<p><strong>void pmemblk_set_funcs(<br />
void *(*</strong><em>malloc_func</em><strong>)(size_t</strong> <em>size</em><strong>),<br />
void (*</strong><em>free_func</em><strong>)(void *</strong><em>ptr</em><strong>),<br />
void *(*</strong><em>realloc_func</em><strong>)(void *</strong><em>ptr</em><strong>, size_t</strong> <em>size</em><strong>),<br />
char *(*</strong><em>strdup_func</em><strong>)(const char *</strong><em>s</em><strong>));</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_set_funcs</strong>() function allows an application to override memory allocation calls used internally by <strong>libpmemblk</strong>. Passing in NULL for any of the handlers will cause the <strong>libpmemblk</strong> default function to be used. The library does not make heavy use of the system malloc functions, but it does allocate approximately 4-8 kilobytes for each memory pool in use.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>int pmemblk_check(const char *</strong><em>path</em><strong>, size_t</strong> <em>bsize</em><strong>);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_check</strong>() function performs a consistency check of the file indicated by <em>path</em> and returns 1 if the memory pool is found to be consistent. Any inconsistencies found will cause <strong>pmemblk_check</strong>() to return 0, in which case the use of the file with <strong>libpmemblk</strong> will result in undefined behavior. The debug version of <strong>libpmemblk</strong> will provide additional details on inconsistencies when <strong>PMEMBLK_LOG_LEVEL</strong> is at least 1, as described in the <strong>DEBUGGING AND ERROR HANDLING</strong> section below. When <em>bsize</em> is non-zero <strong>pmemblk_check</strong>() will compare it to the block size of the pool and return 0 when they don’t match. <strong>pmemblk_check</strong>() will return -1 and set errno if it cannot perform the consistency check due to other errors. <strong>pmemblk_check</strong>() opens the given <em>path</em> read-only so it never makes any changes to the file.</p></td>
</tr>
</tbody>
</table>

### DEBUGGING AND ERROR HANDLING

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>Two versions of <strong>libpmemblk</strong> are typically available on a development system. The normal version, accessed when a program is linked using the <strong>-lpmemblk</strong> option, is optimized for performance. That version skips checks that impact performance and never logs any trace information or performs any run-time assertions. If an error is detected during the call to <strong>libpmemblk</strong> function, an application may retrieve an error message describing the reason of failure using the following function:</p>
<p><strong>const char *pmemblk_errormsg(void);</strong></p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The <strong>pmemblk_errormsg</strong>() function returns a pointer to a static buffer containing the last error message logged for current thread. The error message may include description of the corresponding error code (if errno was set), as returned by <strong>strerror</strong>(3). The error message buffer is thread-local; errors encountered in one thread do not affect its value in other threads. The buffer is never cleared by any library function; its content is significant only when the return value of the immediately preceding call to <strong>libpmemblk</strong> function indicated an error, or if errno was set. The application must not modify or free the error message string, but it may be modified by subsequent calls to other library functions.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>A second version of <strong>libpmemblk</strong>, accessed when a program uses the libraries under <strong>/usr/lib/nvml_debug</strong>, contains run-time assertions and trace points. The typical way to access the debug version is to set the environment variable <strong>LD_LIBRARY_PATH</strong> to <strong>/usr/lib/nvml_debug</strong> or <strong>/usr/lib64/nvml_debug</strong> depending on where the debug libraries are installed on the system. The trace points in the debug version of the library are enabled using the environment variable <strong>PMEMBLK_LOG_LEVEL</strong>, which can be set to the following values:</p></td>
</tr>
</tbody>
</table>

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
<td align="left"><p>This is the default level when <strong>PMEMBLK_LOG_LEVEL</strong> is not set. No log messages are emitted at this level.</p></td>
</tr>
<tr class="even">
<td align="left"></td>
<td align="left"><p>1</p></td>
<td align="left"></td>
<td align="left"><p>Additional details on any errors detected are logged (in addition to returning the errno-based errors as usual). The same information may be retrieved using <strong>pmemblk_errormsg</strong>().</p></td>
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
<td align="left"><p>This level enables voluminous and fairly obscure tracing information that is likely only useful to the <strong>libpmemblk</strong> developers.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The environment variable <strong>PMEMBLK_LOG_FILE</strong> specifies a file name where all logging information should be written. If the last character in the name is “-”, the PID of the current process will be appended to the file name when the log file is created. If <strong>PMEMBLK_LOG_FILE</strong> is not set, the logging output goes to stderr.</p>
<p>Setting the environment variable <strong>PMEMBLK_LOG_LEVEL</strong> has no effect on the non-debug version of <strong>libpmemblk</strong>.</p></td>
</tr>
</tbody>
</table>


### EXAMPLES

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>The following example illustrates how the <strong>libpmemblk</strong> API is used.</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><pre><code>#include &lt;stdio.h&gt;
#include &lt;fcntl.h&gt;
#include &lt;errno.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;unistd.h&gt;
#include &lt;string.h&gt;
#include &lt;libpmemblk.h&gt;

/* size of the pmemblk pool -- 1 GB */
#define POOL_SIZE ((size_t)(1 &lt;&lt; 30))

/* size of each element in the pmem pool */
#define ELEMENT_SIZE 1024

int
main(int argc, char *argv[])
{
    const char path[] = &quot;/pmem-fs/myfile&quot;;
    PMEMblkpool *pbp;
    size_t nelements;
    char buf[ELEMENT_SIZE];

   /* create the pmemblk pool or open it if it already exists */
    pbp = pmemblk_create(path, ELEMENT_SIZE, POOL_SIZE, 0666);

   if (pbp == NULL)
        pbp = pmemblk_open(path, ELEMENT_SIZE);

   if (pbp == NULL) {
        perror(path);
        exit(1);
    }

   /* how many elements fit into the file? */
    nelements = pmemblk_nblock(pbp);
    printf(&quot;file holds %zu elements0, nelements);

   /* store a block at index 5 */
    strcpy(buf, &quot;hello, world&quot;);
    if (pmemblk_write(pbp, buf, 5) &lt; 0) {
        perror(&quot;pmemblk_write&quot;);
        exit(1);
    }

   /* read the block at index 10 (reads as zeros initially) */
    if (pmemblk_read(pbp, buf, 10) &lt; 0) {
        perror(&quot;pmemblk_read&quot;);
        exit(1);
    }

   /* zero out the block at index 5 */
    if (pmemblk_set_zero(pbp, 5) &lt; 0) {
        perror(&quot;pmemblk_set_zero&quot;);
        exit(1);
    }

   /* ... */

   pmemblk_close(pbp);
}</code></pre></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>See http://pmem.io/nvml/libpmemblk for more examples using the <strong>libpmemblk</strong> API.</p></td>
</tr>
</tbody>
</table>


### BUGS

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>Unlike <strong>libpmemobj</strong>, data replication is not supported in <strong>libpmemblk</strong>. Thus, it is not allowed to specify replica sections in pool set files.</p></td>
</tr>
</tbody>
</table>

### ACKNOWLEDGEMENTS

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>libpmemblk</strong> builds on the persistent memory programming model recommended by the SNIA NVM Programming Technical Work Group:</p></td>
</tr>
</tbody>
</table>

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p>http://snia.org/nvmp</p></td>
</tr>
</tbody>
</table>


### SEE ALSO

<table>
<colgroup>
<col width="50%" />
<col width="50%" />
</colgroup>
<tbody>
<tr class="odd">
<td align="left"></td>
<td align="left"><p><strong>mmap</strong>(2), <strong>munmap</strong>(2), <strong>msync</strong>(2), <strong>strerror</strong>(3), <strong>libpmemobj</strong>(3), <strong>libpmemlog</strong>(3), <strong>libpmem</strong>(3), <strong>libvmem</strong>(3) and <strong>http://pmem.io</strong>.</p></td>
</tr>
</tbody>
</table>
