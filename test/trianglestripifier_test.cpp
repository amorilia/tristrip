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

BOOST_AUTO_TEST_CASE(find_start_face_good_reset_point_test) {
	MeshPtr m(new Mesh());
	MFacePtr f0 = m->add_face(2, 1, 7);
	MFacePtr f1 = m->add_face(0, 1, 2);
	MFacePtr f2 = m->add_face(2, 7, 4);
	MFacePtr f3 = m->add_face(5, 3, 2);
	TriangleStripifier t(m);
	BOOST_CHECK_EQUAL(f0->get_next_face(2, 1), f1);
	BOOST_CHECK_EQUAL(f0->get_next_face(1, 7), MFacePtr());
	BOOST_CHECK_EQUAL(f0->get_next_face(7, 2), f2);
	BOOST_CHECK_EQUAL(f1->get_next_face(0, 1), MFacePtr());
	BOOST_CHECK_EQUAL(f1->get_next_face(1, 2), f0);
	BOOST_CHECK_EQUAL(f1->get_next_face(2, 0), MFacePtr());
	BOOST_CHECK_EQUAL(f2->get_next_face(2, 7), f0);
	BOOST_CHECK_EQUAL(f2->get_next_face(7, 4), MFacePtr());
	BOOST_CHECK_EQUAL(f2->get_next_face(4, 2), MFacePtr());
	BOOST_CHECK_EQUAL(f3->get_next_face(5, 3), MFacePtr());
	BOOST_CHECK_EQUAL(f3->get_next_face(3, 2), MFacePtr());
	BOOST_CHECK_EQUAL(f3->get_next_face(2, 5), MFacePtr());

	// f1 is lexically first triangle with only one neighbour
	t.find_start_face();
	BOOST_CHECK_EQUAL(t.start_face_iter->second, f1);
	f1->strip_id = 0; // fake that it is stripified

	// f0 comes lexically after f1
	t.find_good_reset_point();
	BOOST_CHECK_EQUAL(t.start_face_iter->second, f0);
	// try again: should find the same
	t.find_good_reset_point();
	BOOST_CHECK_EQUAL(t.start_face_iter->second, f0);
	f0->strip_id = 1; // fake that it is stripified

	// f3 comes lexically first after f0
	t.find_good_reset_point();
	BOOST_CHECK_EQUAL(t.start_face_iter->second, f3);
	f3->strip_id = 2; // fake that it is stripified

	// f2 comes lexically last
	t.find_good_reset_point();
	BOOST_CHECK_EQUAL(t.start_face_iter->second, f2);
	f2->strip_id = 3; // fake that it is stripified
}

BOOST_AUTO_TEST_SUITE_END()
