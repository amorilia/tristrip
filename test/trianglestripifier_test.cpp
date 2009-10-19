/*

Copyright (c) 2007-2009, Python File Format Interface
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.

   * Neither the name of the Python File Format Interface
     project nor the names of its contributors may be used to endorse
     or promote products derived from this software without specific
     prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "trianglestripifier.hpp"

BOOST_AUTO_TEST_SUITE(trianglestripifier_test_suite)

BOOST_AUTO_TEST_CASE(find_other_face_test_0) {
	// trivial case
	BOOST_CHECK_THROW(find_other_face(1, 2, MFacePtr()), std::runtime_error);
	// single triangle mesh
	Mesh m;
	MFacePtr f = m.add_face(0, 1, 2);
	BOOST_CHECK_EQUAL(find_other_face(0, 1, f), MFacePtr());
}

BOOST_AUTO_TEST_CASE(find_other_face_test_1) {
	// construct slightly more complicated mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 3);
	MFacePtr f2 = m.add_face(2, 3, 4);
	MFacePtr f3 = m.add_face(5, 3, 2);
	MFacePtr f4 = m.add_face(2, 1, 9); // faces added earlier on get priority when the strip is built, this is to check for that
	// check the function by doing a quick strip traversal
	MFacePtr f = f0;
	BOOST_CHECK_EQUAL(find_other_face(1, 2, f0), f1);
	BOOST_CHECK_EQUAL(find_other_face(2, 3, f1), f2);
	BOOST_CHECK_EQUAL(find_other_face(3, 4, f2), MFacePtr());
}

BOOST_AUTO_TEST_CASE(triangle_strip_build_test) {
	// construct slightly more complicated mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 7);
	MFacePtr f2 = m.add_face(2, 7, 4);
	MFacePtr f3 = m.add_face(5, 3, 2);
	MFacePtr f4 = m.add_face(2, 1, 9);
	TriangleStrip t(f1, f1->get_edge(7, 2));
	t.build();
	std::list<int>::const_iterator i = t.strip.begin();
	BOOST_CHECK_EQUAL(*i, 4); i++;
	BOOST_CHECK_EQUAL(*i, 7); i++;
	BOOST_CHECK_EQUAL(*i, 2); i++;
	BOOST_CHECK_EQUAL(*i, 1); i++;
	BOOST_CHECK_EQUAL(*i, 0); i++;
	std::list<MFacePtr>::const_iterator j = t.faces.begin();
	BOOST_CHECK_EQUAL(*j, f2); j++;
	BOOST_CHECK_EQUAL(*j, f1); j++;
	BOOST_CHECK_EQUAL(*j, f0); j++;
}

BOOST_AUTO_TEST_SUITE_END()
