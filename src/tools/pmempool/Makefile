# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2014-2020, Intel Corporation
#
# Makefile -- top Makefile for pmempool
#
SCP_TO_REMOTE_NODES = y

TOP = ../../..

include $(TOP)/src/common.inc

vpath %.c ../../libpmemblk/
vpath %.c ../../libpmempool/
vpath %.c ../../libpmemobj/
vpath %.c ../../librpmem/
vpath %.c ../../rpmem_common/

TARGET = pmempool

#LIBPMEM=y
#LIBPMEMBLK=y
#LIBPMEMOBJ=internal-debug
#LIBPMEMLOG=y
#LIBPMEMPOOL=y
TOOLS_COMMON=y

OBJS += pmempool.o\
       create.o info.o info_blk.o info_log.o info_obj.o\
       dump.o check.o rm.o convert.o synchronize.o transform.o\
       rpmem_ssh.o rpmem_cmd.o rpmem_util.o rpmem_common.o feature.o\
       btt.o blk.o libpmemblk.o \

OBJS += $(TOP)/src/debug/libpmempool/check.o \
	$(TOP)/src/debug/libpmempool/check_backup.o \
	$(TOP)/src/debug/libpmempool/check_bad_blocks.o \
	$(TOP)/src/debug/libpmempool/check_blk.o \
	$(TOP)/src/debug/libpmempool/check_btt_info.o \
	$(TOP)/src/debug/libpmempool/check_btt_map_flog.o \
	$(TOP)/src/debug/libpmempool/check_log.o \
	$(TOP)/src/debug/libpmempool/check_pool_hdr.o \
	$(TOP)/src/debug/libpmempool/check_sds.o \
	$(TOP)/src/debug/libpmempool/check_util.o \
	$(TOP)/src/debug/libpmempool/check_write.o \
	$(TOP)/src/debug/libpmempool/pool.o \
	$(TOP)/src/debug/libpmempool/replica.o \
	$(TOP)/src/debug/libpmempool/rm.o \
	$(TOP)/src/debug/libpmempool/sync.o \
	$(TOP)/src/debug/libpmempool/transform.o \
	$(TOP)/src/debug/libpmempool/feature.o \
	libpmempool.o
$(info ************  PMEMPOOL MAKEFILE **********************************)

OBJS += $(OBJS_LIBPMEMOBJ_DEBUG)

OBJS += $(TOP)/src/debug/libpmemlog/libpmemlog.o\
	$(TOP)/src/debug/libpmemlog/log.o

OBJS +=	$(TOP)/src/debug/libpmem/libpmem.o\
	$(TOP)/src/debug/libpmem/memops_generic.o\
	$(TOP)/src/debug/libpmem/pmem.o\
	$(TOP)/src/debug/libpmem/pmem_posix.o

include $(TOP)/src/libpmem2/$(ARCH)/sources.inc
OBJS_MEM = $(LIBPMEM2_ARCH_SOURCE:.c=.o)
OBJS += $(addprefix $(TOP)/src/debug/libpmem/, ${OBJS_MEM})

INCS += -I$(TOP)/src/libpmem
INCS += -I$(TOP)/src/libpmem2
INCS += -I$(TOP)/src/libpmem2/$(ARCH)

#alloc_class.o critnib.o memblock.o heap.o recycler.o\
 #      bucket.o container_ravl.o container_seglists.o memops.o obj.o\
      # list.o pmalloc.o lane.o palloc.o\
      # $(TOP)/src/debug/libpmemobj/sync.o\
      # tx.o \
      # ctl_debug.o

#LIBPMEMOBJ_PRIV=memblock_from_offset alloc_class_by_id\
	memblock_rebuild_state alloc_class_by_run\
	heap_run_foreach_object alloc_class_collection_new\
	alloc_class_collection_delete

#LIBPMEMBLK_PRIV=btt_init btt_write btt_fini btt_info_convert2h\
	btt_info_convert2le btt_flog_convert2h btt_flog_convert2le

INCS += -I$(TOP)/src/common
INCS += -I$(TOP)/src/rpmem_common
INCS += -I$(TOP)/src/libpmem2
INCS += -I$(TOP)/src/librpmem
#INCS += -I$(TOP)/src/libpmemlog
#INCS += -I$(TOP)/src/libpmemblk
INCS += -I$(TOP)/src/libpmemobj
#INCS += -I$(TOP)/src/libpmempool

CFLAGS += -DPMEMOBJ_DIRECT_NON_INLINE -DUSE_RPMEM

MANPAGES = $(TOP)/doc/pmempool.1\
           $(TOP)/doc/pmempool-info.1\
	   $(TOP)/doc/pmempool-create.1\
	   $(TOP)/doc/pmempool-check.1\
	   $(TOP)/doc/pmempool-dump.1\
	   $(TOP)/doc/pmempool-rm.1\
	   $(TOP)/doc/pmempool-convert.1\
	   $(TOP)/doc/pmempool-sync.1\
	   $(TOP)/doc/pmempool-transform.1

BASH_COMP_FILES = bash_completion/pmempool

include ../Makefile.inc

.PHONY: test check
