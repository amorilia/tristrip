cdef extern from "tristrip.hpp":

    cdef struct list_int "std::list<int>":
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

    list_list_int c_stripify "stripify" (list_list_int triangles)

def stripify(triangles):
    cdef list_int c_triangle
    cdef list_list_int c_triangles
    cdef list_int c_strip
    cdef list_list_int c_strips
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

