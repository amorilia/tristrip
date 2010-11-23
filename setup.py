"""Setup script for pytristrip."""

classifiers = """\
Development Status :: 4 - Beta
License :: OSI Approved :: BSD License
Intended Audience :: Developers
Intended Audience :: End Users/Desktop
Topic :: Multimedia :: Graphics :: 3D Modeling
Programming Language :: C++
Programming Language :: Cython
Programming Language :: Python
Programming Language :: Python :: 2
Programming Language :: Python :: 3
Operating System :: OS Independent"""

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

readme = open("README.rst").read()

setup(
    name="tristrip",
    version="0.2.0",
    ext_modules=[
        Extension(
            "tristrip",
            ["tristrip.pyx",
             "src/trianglemesh.cpp",
             "src/trianglestripifier.cpp",
             "src/tristrip.cpp"],
            language="c++",
            include_dirs=["include"],
            depends=[
                 "include/trianglemesh.hpp",
                 "include/trianglestripifier.hpp",
                 "include/tristrip.hpp"],
            )
        ],
    author="Amorilia",
    author_email="amorilia@users.sourceforge.net",
    license="BSD",
    keywords="stripify",
    platforms="any",
    description=readme[0],
    classifiers=filter(None, classifiers.split("\n")),
    long_description="\n".join(readme[2:]),
    url="http://github.com/amorilia/tristrip",
    download_url="http://github.com/amorilia/tristrip/downloads",
    cmdclass={'build_ext': build_ext},
)
