#!/usr/bin/python
#
# Copyright 2018, Intel Corporation
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

import argparse
import statemachine
import opscontext
import consistencycheckwrap
import loggingfacility

def main():
    # TODO unicode support
    # TODO parameterize reorder engine type
    parser = argparse.ArgumentParser(description="Store reordering tool")
    parser.add_argument("-l", "--logfile", required=True, help="the pmemcheck log file to process")
    parser.add_argument("-c", "--checker", required=True, choices=consistencycheckwrap.checkers,
                        help="choose consistency checker type")
    parser.add_argument("-p", "--path", required=True, help="path to the consistency checker")
    parser.add_argument("-n", "--name", help="consistency check function for the 'lib' checker")
    parser.add_argument("-t", "--output_type", choices=loggingfacility.loggers, default="print",
                        help='choose logger type default="print"')
    parser.add_argument("-o", "--output", help="set the logger output")
    parser.add_argument("-e", "--output_level", choices=loggingfacility.log_levels, help="set the output log level")
    parser.add_argument("args", nargs=argparse.REMAINDER, help="remaining args passed to the checker")
    args = parser.parse_args()

    logger = loggingfacility.get_logger(args.output_type, args.output, args.output_level)
    checker = consistencycheckwrap.get_checker(args.checker, args.path, args.name, args.args)

    # create the script context
    context = opscontext.OpsContext(args.logfile, checker, logger)

    # init and run the state machine
    a = statemachine.StateMachine(statemachine.InitState(context))
    a.run_all(context.extract_operations())

if __name__ == "__main__":
    main()
