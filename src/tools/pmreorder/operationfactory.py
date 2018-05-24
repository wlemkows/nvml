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

from memoryoperations import *


class OperationFactory:
    """
    An abstract memory operation factory.

    This object factory puts special constraints on names of classes.
    It creates objects based on log in string format, as such the c
    lasses have to start with a capital letter and the rest of the
    name has to be in lowercase. For example::

        STORE -> Store
        FULL_REORDER -> Full_reorder

    The object to be created has to have and internal **Factory** class
    with a :func:`create` method taking a string parameter. For example see
    :class:`memoryoperations.Store`.

    :cvar __factories: The registered object factories.
    :type __factories: dict
    """
    __factories = {}

    @staticmethod
    def add_factory(id_, operation_factory):
        """
        Explicitly register an object factory.

        This method should be used when the factory cannot be inferred
        from the name of the object to be created.

        :param id_: The id under which this factory is to be registered
            in the dictionary.
        :type id_: str
        :param operation_factory: The operation factory to be registered.
        :return: None
        """
        OperationFactory.__factories[id_] = operation_factory

    @staticmethod
    def create_operation(string_operation):
        """
        Creates the object based on the pre-formatted string.

        The string needs to be of specific format. Each specific value
        in the string has to be separated with a `;`. The first field
        has to be the name of the operation, the rest are operation
        specific values.

        :param string_operation: The string describing the operation.
        :return: The specific object instantiated based on the string.
        """
        id_ = string_operation.split(";")[0]
        if id_ not in OperationFactory.__factories:
            OperationFactory.__factories[id_] = eval(id_.lower().capitalize() + '.Factory()')
        return OperationFactory.__factories[id_].create(string_operation)
