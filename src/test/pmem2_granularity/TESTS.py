#!../env.py
#
# Copyright 2019, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


from enum import Enum
import testframework as t
import os


class Granularity(Enum):
    BYTE = 1
    CACHE_LINE = 2
    PAGE = 3


class PMEM2_GRANULARITY(t.BaseTest):
    test_type = t.Short
    available_granularity = None

    def run(self, ctx):
        filepath = ctx.create_holey_file(16 * t.MiB, 'testfile1')

        EADR_NOT_AVAILABLE = \
            "linux_eadr_paths/eadr_not_available/sys/bus/nd/devices/"
        EADR_AVAILABLE = \
            "linux_eadr_paths/eadr_available/sys/bus/nd/devices/"

        if self.available_granularity == Granularity.BYTE:
            ctx.env['IS_EADR'] = '1'
            ctx.env['IS_PMEM'] = '1'
            ctx.env['BUS_DEVICE_PATH'] = \
                os.path.join(self.cwd, EADR_AVAILABLE)
        elif self.available_granularity == Granularity.CACHE_LINE:
            ctx.env['IS_EADR'] = '0'
            ctx.env['IS_PMEM'] = '1'
            ctx.env['BUS_DEVICE_PATH'] = \
                os.path.join(self.cwd, EADR_NOT_AVAILABLE)
        elif self.available_granularity == Granularity.PAGE:
            ctx.env['IS_EADR'] = '0'
            ctx.env['IS_PMEM'] = '0'
            ctx.env['BUS_DEVICE_PATH'] = \
                os.path.join(self.cwd, EADR_NOT_AVAILABLE)

        ctx.exec('pmem2_granularity', self.test_case, filepath)


class TEST0(PMEM2_GRANULARITY):
    """pass byte granularity, available byte granularity"""
    test_case = "test_granularity_req_byte_avail_byte"
    available_granularity = Granularity.BYTE


class TEST1(PMEM2_GRANULARITY):
    """pass byte granularity, available cache line granularity"""
    test_case = "test_granularity_req_byte_avail_cl"
    available_granularity = Granularity.CACHE_LINE


class TEST2(PMEM2_GRANULARITY):
    """pass byte granularity, available page granularity"""
    test_case = "test_granularity_req_byte_avail_page"
    available_granularity = Granularity.PAGE


class TEST3(PMEM2_GRANULARITY):
    """pass cache line granularity, available byte granularity"""
    test_case = "test_granularity_req_cl_avail_byte"
    available_granularity = Granularity.BYTE


class TEST4(PMEM2_GRANULARITY):
    """pass cache line granularity, available cache line granularity"""
    test_case = "test_granularity_req_cl_avail_cl"
    available_granularity = Granularity.CACHE_LINE


class TEST5(PMEM2_GRANULARITY):
    """pass cache line granularity, available page granularity"""
    test_case = "test_granularity_req_cl_avail_page"
    available_granularity = Granularity.PAGE


class TEST6(PMEM2_GRANULARITY):
    """pass page granularity, available byte granularity"""
    test_case = "test_granularity_req_page_avail_byte"
    available_granularity = Granularity.BYTE


class TEST7(PMEM2_GRANULARITY):
    """pass page granularity, available cache line granularity"""
    test_case = "test_granularity_req_page_avail_cl"
    available_granularity = Granularity.CACHE_LINE


class TEST8(PMEM2_GRANULARITY):
    """pass page granularity, available page granularity"""
    test_case = "test_granularity_req_page_avail_page"
    available_granularity = Granularity.PAGE
