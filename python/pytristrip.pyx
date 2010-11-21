"""A python wrapper for the tristrip library."""

# ***** BEGIN LICENSE BLOCK *****
#
# Copyright (c) 2007-2009, Python File Format Interface
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
#    * Neither the name of the Python File Format Interface
#      project nor the names of its contributors may be used to endorse
#      or promote products derived from this software without specific
#      prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# ***** END LICENSE BLOCK *****

cdef extern from "tristrip.hpp":

    cdef struct list_int "std::list<int>":
         void (* push_back)(int elem)
         void (* clear)()
         void (* pop_front)()
         int (* front)()
         int (* empty)()

    cdef struct deque_int "std::deque<int>":
         void (* push_back)(int elem)
         void (* clear)()
         void (* pop_front)()
         int (* front)()
         int (* empty)()

    cdef struct list_list_int "std::list<std::list<int > >":
         void (* push_back)(list_int elem)
         void (* pop_front)()
         list_int (* front)()
         int (* empty)()

    cdef struct list_deque_int "std::list<std::deque<int > >":
         void (* push_back)(deque_int elem)
         void (* pop_front)()
         deque_int (* front)()
         int (* empty)()

    list_deque_int c_stripify "stripify" (list_list_int triangles)

def stripify(triangles):
    """Stripify list of triangles."""

    cdef list_int c_triangle
    cdef list_list_int c_triangles
    cdef deque_int c_strip
    cdef list_deque_int c_strips
    # convert triangles to list of list of ints
    for triangle in triangles:
        c_triangle.clear()
        for vertex in triangle:
            c_triangle.push_back(vertex)
        c_triangles.push_back(c_triangle)
    # stripify
    c_strips = c_stripify(c_triangles)
    # convert result back to python list
    strips = []
    while (not c_strips.empty()):
        c_strip = c_strips.front()
        c_strips.pop_front()
        strip = []
        while (not c_strip.empty()):
            strip.append(c_strip.front())
            c_strip.pop_front()
        strips.append(strip)
    return strips

