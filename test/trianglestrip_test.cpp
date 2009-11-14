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

BOOST_AUTO_TEST_SUITE(triangle_strip_test_suite)

BOOST_AUTO_TEST_CASE(triangle_strip_build_test_0) {
	// simple test
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	int vertices[] = {0, 1, 2};
	BOOST_FOREACH(int pv0, vertices) {
		TriangleStrip t(pv0, pv0); // using vertex index as strip id and experiment id
		t.build(pv0, f0);
		BOOST_CHECK_EQUAL(t.reversed, false);
		std::list<int>::const_iterator i = t.vertices.begin();
		BOOST_CHECK_EQUAL(*i++, pv0);
		BOOST_CHECK_EQUAL(*i++, f0->get_next_vertex(pv0));
		BOOST_CHECK_EQUAL(*i++, f0->get_next_vertex(f0->get_next_vertex(pv0)));
		BOOST_CHECK(i == t.vertices.end());
		std::list<MFacePtr>::const_iterator j = t.faces.begin();
		BOOST_CHECK_EQUAL(*j++, f0);
		BOOST_CHECK(j == t.faces.end());
		// not reversed, so...
		BOOST_CHECK(t.get_strip() == t.vertices);
	};
}

BOOST_AUTO_TEST_CASE(triangle_strip_build_test_1) {
	// another simple test
	// also tests get_strip on strips of length 4
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 3);
	{
		TriangleStrip t(1, 1);
		t.build(0, f0);
		BOOST_CHECK_EQUAL(t.reversed, false);
		std::list<int>::const_iterator i = t.vertices.begin();
		BOOST_CHECK_EQUAL(*i++, 0);
		BOOST_CHECK_EQUAL(*i++, 1);
		BOOST_CHECK_EQUAL(*i++, 2);
		BOOST_CHECK_EQUAL(*i++, 3);
		BOOST_CHECK(i == t.vertices.end());
		std::list<MFacePtr>::const_iterator j = t.faces.begin();
		BOOST_CHECK_EQUAL(*j++, f0);
		BOOST_CHECK_EQUAL(*j++, f1);
		BOOST_CHECK(j == t.faces.end());
		// not reversed, so...
		BOOST_CHECK(t.get_strip() == t.vertices);
	}
	{
		TriangleStrip t(2, 2);
		t.build(1, f0);
		BOOST_CHECK_EQUAL(t.reversed, true);
		std::list<int>::const_iterator i = t.vertices.begin();
		BOOST_CHECK_EQUAL(*i++, 3);
		BOOST_CHECK_EQUAL(*i++, 1);
		BOOST_CHECK_EQUAL(*i++, 2);
		BOOST_CHECK_EQUAL(*i++, 0);
		BOOST_CHECK(i == t.vertices.end());
		std::list<MFacePtr>::const_iterator j = t.faces.begin();
		BOOST_CHECK_EQUAL(*j++, f1);
		BOOST_CHECK_EQUAL(*j++, f0);
		BOOST_CHECK(j == t.faces.end());
		std::list<int> strip = t.get_strip();
		i = strip.begin();
		BOOST_CHECK_EQUAL(*i++, 3);
		BOOST_CHECK_EQUAL(*i++, 2);
		BOOST_CHECK_EQUAL(*i++, 1);
		BOOST_CHECK_EQUAL(*i++, 0);
		BOOST_CHECK(i == strip.end());
	}
	{
		TriangleStrip t(3, 3);
		t.build(2, f1);
		BOOST_CHECK_EQUAL(t.reversed, true);
		std::list<int>::const_iterator i = t.vertices.begin();
		BOOST_CHECK_EQUAL(*i++, 0);
		BOOST_CHECK_EQUAL(*i++, 2);
		BOOST_CHECK_EQUAL(*i++, 1);
		BOOST_CHECK_EQUAL(*i++, 3);
		BOOST_CHECK(i == t.vertices.end());
		std::list<MFacePtr>::const_iterator j = t.faces.begin();
		BOOST_CHECK_EQUAL(*j++, f0);
		BOOST_CHECK_EQUAL(*j++, f1);
		BOOST_CHECK(j == t.faces.end());
		std::list<int> strip = t.get_strip();
		i = strip.begin();
		BOOST_CHECK_EQUAL(*i++, 0);
		BOOST_CHECK_EQUAL(*i++, 1);
		BOOST_CHECK_EQUAL(*i++, 2);
		BOOST_CHECK_EQUAL(*i++, 3);
		BOOST_CHECK(i == strip.end());
	}
	{
		TriangleStrip t(4, 4);
		t.build(3, f1);
		BOOST_CHECK_EQUAL(t.reversed, false);
		std::list<int>::const_iterator i = t.vertices.begin();
		BOOST_CHECK_EQUAL(*i++, 3);
		BOOST_CHECK_EQUAL(*i++, 2);
		BOOST_CHECK_EQUAL(*i++, 1);
		BOOST_CHECK_EQUAL(*i++, 0);
		BOOST_CHECK(i == t.vertices.end());
		std::list<MFacePtr>::const_iterator j = t.faces.begin();
		BOOST_CHECK_EQUAL(*j++, f1);
		BOOST_CHECK_EQUAL(*j++, f0);
		BOOST_CHECK(j == t.faces.end());
		// not reversed, so...
		BOOST_CHECK(t.get_strip() == t.vertices);
	}
}

BOOST_AUTO_TEST_CASE(triangle_strip_build_test_2) {
	// checks that extra vertex is appended to fix winding
	// 1---3---5
	//  \ / \ / \
	//   2---4---6
	Mesh m;
	MFacePtr f0 = m.add_face(1, 3, 2);
	MFacePtr f1 = m.add_face(2, 3, 4);
	MFacePtr f2 = m.add_face(4, 3, 5);
	MFacePtr f3 = m.add_face(4, 5, 6);
	TriangleStrip t(1, 1);
	t.build(2, f1);
	BOOST_CHECK_EQUAL(t.reversed, true);
	std::list<int>::const_iterator i = t.vertices.begin();
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 3);
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 5);
	BOOST_CHECK_EQUAL(*i++, 6);
	BOOST_CHECK(i == t.vertices.end());
	std::list<int> strip = t.get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 1); // extra vertex
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 3);
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 5);
	BOOST_CHECK_EQUAL(*i++, 6);
	BOOST_CHECK(i == strip.end());
}

BOOST_AUTO_TEST_CASE(triangle_strip_build_test_3) {
	// checks that strip is reversed to fix winding
	Mesh m;
	MFacePtr f0 = m.add_face(1, 3, 2);
	MFacePtr f1 = m.add_face(2, 3, 4);
	MFacePtr f2 = m.add_face(3, 5, 4);
	TriangleStrip t(1, 1);
	t.build(2, f1);
	BOOST_CHECK_EQUAL(t.reversed, true);
	std::list<int>::const_iterator i = t.vertices.begin();
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 3);
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 5);
	BOOST_CHECK(i == t.vertices.end());
	std::list<int> strip = t.get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 5); // reversed order
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 3);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK(i == strip.end());
}

BOOST_AUTO_TEST_CASE(triangle_strip_build_test_4) {
	// construct slightly more complicated mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 7);
	MFacePtr f2 = m.add_face(2, 7, 4);
	MFacePtr f3 = m.add_face(5, 3, 2);
	MFacePtr f4 = m.add_face(2, 1, 9);
	MFacePtr f5 = m.add_face(4, 7, 10);
	MFacePtr f6 = m.add_face(4, 10, 11);
	MFacePtr f7 = m.add_face(11, 10, 12);
	MFacePtr f8 = m.add_face(1, 0, 13);
	TriangleStrip t(1, 1);
	t.build(7, f1);
	BOOST_CHECK_EQUAL(t.reversed, false);
	std::list<int>::const_iterator i = t.vertices.begin();
	BOOST_CHECK_EQUAL(*i++, 12);
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK_EQUAL(*i++, 10);
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 7);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK_EQUAL(*i++, 0);
	BOOST_CHECK_EQUAL(*i++, 13);
	BOOST_CHECK(i == t.vertices.end());
	std::list<MFacePtr>::const_iterator j = t.faces.begin();
	BOOST_CHECK_EQUAL(*j++, f7);
	BOOST_CHECK_EQUAL(*j++, f6);
	BOOST_CHECK_EQUAL(*j++, f5);
	BOOST_CHECK_EQUAL(*j++, f2);
	BOOST_CHECK_EQUAL(*j++, f1);
	BOOST_CHECK_EQUAL(*j++, f0);
	BOOST_CHECK_EQUAL(*j++, f8);
	BOOST_CHECK(j == t.faces.end());
	// not reversed, so...
	BOOST_CHECK(t.get_strip() == t.vertices);
}

BOOST_AUTO_TEST_SUITE_END()
