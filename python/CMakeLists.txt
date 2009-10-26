# build python bindings
## compile against the tristrip library:
cython_add_module(tristrip)
target_link_libraries(tristrip-cython tristrip)
## self-contained compile:
#cython_add_module(tristrip
#    src/trianglemesh.cpp
#    src/trianglestripifier.cpp
#    src/tristrip.cpp
#)

