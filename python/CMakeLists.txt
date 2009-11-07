# build python bindings
## compile against the tristrip library:
#cython_add_module(pytristrip)
#target_link_libraries(pytristrip tristrip)
## self-contained compile:
cython_add_module(pytristrip
    ../src/trianglemesh.cpp
    ../src/trianglestripifier.cpp
    ../src/tristrip.cpp
)
target_link_libraries(pytristrip ${PYTHON_LIBRARY})