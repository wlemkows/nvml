# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2014-2016, Intel Corporation
#
# Makefile -- Makefile for pmemspoil tool
#

TOP=../../../..

SCP_TO_REMOTE_NODES = y

vpath %.c ../../../libpmemblk/

TARGET = pmemspoil

OBJS = spoil.o btt.o

LIBPMEM=internal-debug
TOOLS_COMMON=y
#LIBPMEMBLK_PRIV=btt_info_convert2h
INCS += -I$(TOP)/src/libpmemblk

include $(TOP)/src/tools/Makefile.inc
