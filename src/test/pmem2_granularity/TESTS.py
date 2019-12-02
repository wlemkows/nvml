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

import testframework as t
import os


class PMEM2_GRANULARITY(t.BaseTest):
    test_type = t.Short

    IS_EADR = 0

    def run(self, ctx):
        EADR_NOT_AVAILABLE = \
            "linux_eadr_paths/eadr_not_available/sys/bus/nd/devices/"
        EADR_AVAILABLE = \
            "linux_eadr_paths/eadr_available/sys/bus/nd/devices/"

        if (self.IS_EADR == 1):
            ctx.env['BUS_DEVICE_PATH'] = \
                os.path.join(self.cwd, EADR_AVAILABLE)
        else:
            ctx.env['BUS_DEVICE_PATH'] = \
                os.path.join(self.cwd, EADR_NOT_AVAILABLE)

        filepath = ctx.create_holey_file(16 * t.MiB, 'testfile1')
        ctx.exec('pmem2_granularity', self.test_case, filepath)


@t.windows_exclude
class TEST0(PMEM2_GRANULARITY):
    """pass invalid granularity value"""
    test_case = "test_granularity_inval"


@t.windows_exclude
class TEST1(PMEM2_GRANULARITY):
    """granularity test"""
    os.environ['IS_PMEM'] = '0'
    IS_EADR = 1
    test_case = "test_granularity_req_page_avail_page"


@t.windows_exclude
class TEST2(PMEM2_GRANULARITY):
    """granularity test"""
    os.environ['IS_PMEM'] = '0'
    IS_EADR = 0
    test_case = "test_granularity_req_page_avail_cl"
