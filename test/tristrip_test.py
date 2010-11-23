"""Tests for pytristrip.

>>> import pytristrip
>>> triangles = [(0,1,4),(1,2,4),(2,3,4),(3,0,4)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips)
>>> triangles = [(0, 1, 2), (3, 4, 5), (6, 7, 8), (9, 10, 11), (12, 13, 14), (15, 16, 17), (18, 19, 20), (21, 22, 23)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips)
>>> triangles = [(0, 1, 2), (0, 1, 2)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips)
>>> triangles = [(0, 1, 2), (2, 1, 0)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips)
>>> triangles = [(0, 1, 2), (2, 1, 0), (1, 2, 3)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips) # NvTriStrip gives wrong result
>>> triangles = [(0, 1, 2), (0, 1, 3)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips) # NvTriStrip gives wrong result
>>> triangles = [(1, 5, 2), (5, 2, 6), (5, 9, 6), (9, 6, 10), (9, 13, 10), (13, 10, 14), (0, 4, 1), (4, 1, 5), (4, 8, 5), (8, 5, 9), (8, 12, 9), (12, 9, 13), (2, 6, 3), (6, 3, 7), (6, 10, 7), (10, 7, 11), (10, 14, 11), (14, 11, 15)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips) # NvTriStrip gives wrong result
>>> triangles = [(1, 2, 3), (4, 5, 6), (6, 5, 7), (8, 5, 9), (4, 10, 9), (8, 3, 11), (8, 10, 3), (12, 13, 6), (14, 2, 15), (16, 13, 15), (16, 2, 3), (3, 2, 1)]
>>> strips = pytristrip.stripify(triangles)
>>> _check_strips(triangles, strips) # NvTriStrip gives wrong result
"""

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

# helper functions for the test

def _triangulate(strips):
    """A generator for iterating over the faces in a set of
    strips. Degenerate triangles in strips are discarded.

    >>> _triangulate([[1, 0, 1, 2, 3, 4, 5, 6]])
    [(0, 2, 1), (1, 2, 3), (2, 4, 3), (3, 4, 5), (4, 6, 5)]
    """

    triangles = []

    for strip in strips:
        if len(strip) < 3: continue # skip empty strips
        i = strip.__iter__()
        j = False
        t1, t2 = i.next(), i.next()
        for k in xrange(2, len(strip)):
            j = not j
            t0, t1, t2 = t1, t2, i.next()
            if t0 == t1 or t1 == t2 or t2 == t0: continue
            triangles.append((t0, t1, t2) if j else (t0, t2, t1))

    return triangles

def _check_strips(triangles, strips):
    """Check that triangles and strips describe the same geometry."""
    strips_triangles = _triangulate(strips)
    for t0,t1,t2 in triangles:
        if t0 == t1 or t1 == t2 or t2 == t0: continue
        if (t0,t1,t2) not in strips_triangles \
           and (t1,t2,t0) not in strips_triangles \
           and (t2,t0,t1) not in strips_triangles:
            raise ValueError('triangle %s in triangles but not in strips\ntriangles = %s\nstrips = %s'%((t0,t1,t2),triangles,strips))
    for t0,t1,t2 in strips_triangles:
        if t0 == t1 or t1 == t2 or t2 == t0: continue
        if (t0,t1,t2) not in triangles \
           and (t1,t2,t0) not in triangles \
           and (t2,t0,t1) not in triangles:
            raise ValueError('triangle %s in strips but not in triangles\ntriangles = %s\nstrips = %s'%((t0,t1,t2),triangles,strips))

if __name__=='__main__':
    import doctest
    doctest.testmod()
