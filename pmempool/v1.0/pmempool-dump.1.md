---
Content-Style: 'text/css'
generator: 'groff -Thtml, see www.gnu.org'
title: 'pmempool-dump'
...

pmempool-dump {#pmempool-dump align="center"}
=============

[NAME](#NAME)\
[SYNOPSIS](#SYNOPSIS)\
[DESCRIPTION](#DESCRIPTION)\
[RANGE](#RANGE)\
[EXAMPLES](#EXAMPLES)\
[SEE ALSO](#SEE%20ALSO)\
[PMEMPOOL](#PMEMPOOL)\

------------------------------------------------------------------------

NAME []()
---------

pmempool-dump − Dump user data from specified pool

SYNOPSIS []()
-------------

**pmempool dump** \[&lt;options&gt;\] &lt;file&gt;

DESCRIPTION []()
----------------

The **pmempool** invoked with **dump** command dumps user data from
specified pool file. The output format may be either binary or
hexadecimal.

By default the output format is hexadecimal.

By default data is dumped to standard output. It is possible to dump
data to other file by specifying **-o** option. In this case data will
be appended to this file.

Using **-r** option you can specify number of blocks/bytes/data chunks
using special text format. See **RANGE** section for details.

**Available options:\
-b, --binary**

Dump data in binary format.

**-r, --range** &lt;range&gt;

Range of pool file to dump. This may be number of blocks for **blk**
pool type or either number of bytes or number of data chunks for **log**
pool type.

**-c, --chunk** &lt;size&gt;

Size of chunk for **log** pool type. See **pmemlog\_walk**() in
**libpmemlog(3)** for details.

**-o, --output** &lt;file&gt;

Name of output file.

**-h, --help**

Display help message and exit.

RANGE []()
----------

Using **-r, --range** option it is possible to dump only a range of user
data. This section describes valid format of *&lt;range&gt;* string.

You can specify multiple ranges separated by commas.

**&lt;first&gt;-&lt;last&gt;**

All blocks/bytes/data chunks from **&lt;first&gt;** to **&lt;last&gt;**
will be dumped.

**-&lt;last&gt;**

All blocks/bytes/data chunks up to **&lt;last&gt;** will be dumped.

**&lt;first&gt;-**

All blocks/bytes/data chunks starting from **&lt;first&gt;** will be
dumped.

**&lt;number&gt;**

Only **&lt;number&gt;** block/byte/data chunk will be dumped.

EXAMPLES []()
-------------

pmempool dump pool.bin

\# Dump user data from pool.bin file to standard output

pmempool dump -o output.bin -r1,10-100 pool\_blk.bin

\# Dump block number 1 and blocks from 10 to 100 from pool\_blk.bin
containing pmem blk pool to output.bin file

pmempool dump -r 1K-2K pool.bin

\# Dump data form 1K to 2K from pool.bin file.

SEE ALSO []()
-------------

**libpmemblk(3) libpmemlog(3) pmempool(1)**

PMEMPOOL []()
-------------

Part of the **pmempool(1)** suite.

------------------------------------------------------------------------
