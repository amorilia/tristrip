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

BOOST_AUTO_TEST_SUITE(triangle_stripifier_test_suite)

BOOST_AUTO_TEST_CASE(find_start_face_good_reset_point_test)
{
	MeshPtr m(new Mesh());
	MFacePtr f0 = m->add_face(2, 1, 7);
	MFacePtr f1 = m->add_face(0, 1, 2);
	MFacePtr f2 = m->add_face(2, 7, 4);
	MFacePtr f3 = m->add_face(5, 3, 2);
	TriangleStripifier t(m);

	BOOST_CHECK_EQUAL(t.find_good_reset_point(), true);
	BOOST_CHECK_EQUAL(*t.start_face_iter, f0);
	// try again: should find the same
	BOOST_CHECK_EQUAL(t.find_good_reset_point(), true);
	BOOST_CHECK_EQUAL(*t.start_face_iter, f0);
	f0->strip_id = 1; // fake that it is stripified

	BOOST_CHECK_EQUAL(t.find_good_reset_point(), true);
	BOOST_CHECK_EQUAL(*t.start_face_iter, f1);
	f1->strip_id = 2; // fake that it is stripified

	BOOST_CHECK_EQUAL(t.find_good_reset_point(), true);
	BOOST_CHECK_EQUAL(*t.start_face_iter, f2);
	f2->strip_id = 3; // fake that it is stripified

	BOOST_CHECK_EQUAL(t.find_good_reset_point(), true);
	BOOST_CHECK_EQUAL(*t.start_face_iter, f3);
	f3->strip_id = 4; // fake that it is stripified

	BOOST_CHECK_EQUAL(t.find_good_reset_point(), false);
}

BOOST_AUTO_TEST_CASE(experiment_build)
{
	MeshPtr m(new Mesh());

	// first strip

	m->add_face(2, 1, 7); // in strip
	MFacePtr s1_face = m->add_face(0, 1, 2); // in strip
	m->add_face(2, 7, 4); // in strip
	m->add_face(4, 7, 11); // in strip
	m->add_face(5, 3, 2);
	m->add_face(1, 0, 8); // in strip
	m->add_face(0, 8, 9); // bad orientation!
	m->add_face(8, 0, 10); // in strip
	m->add_face(10, 11, 8); // in strip

	// parallel strip
	MFacePtr s2_face = m->add_face(0, 2, 21); // in strip
	m->add_face(21, 2, 22); // in strip
	m->add_face(2, 4, 22); // in strip
	m->add_face(21, 24, 0); // in strip
	m->add_face(9, 0, 24); // in strip

	// parallel strip, further down
	MFacePtr s3_face = m->add_face(8, 11, 31); // in strip
	m->add_face(8, 31, 32); // in strip
	m->add_face(31, 11, 33); // in strip

	// build experiment
	ExperimentPtr exp(new Experiment(0, s1_face));
	exp->build();

	std::list<TriangleStripPtr>::const_iterator t = exp->strips.begin();

	std::deque<int> strip = (*t)->get_strip();
	std::deque<int>::const_iterator i = strip.begin();

	// ... extra check, could omit this
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 7);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK_EQUAL(*i++, 0);
	BOOST_CHECK_EQUAL(*i++, 8);
	BOOST_CHECK_EQUAL(*i++, 10);
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK(i == strip.end());

	// check parallel strip
	t++;
	strip = (*t)->get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 22);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 21);
	BOOST_CHECK_EQUAL(*i++, 0);
	BOOST_CHECK_EQUAL(*i++, 24);
	BOOST_CHECK_EQUAL(*i++, 9);
	BOOST_CHECK(i == strip.end());

	/*
	// another parallel strip - not in experiment with current algorithm
	t++;
	strip = (*t)->get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 32);
	BOOST_CHECK_EQUAL(*i++, 8);
	BOOST_CHECK_EQUAL(*i++, 31);
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK_EQUAL(*i++, 33);
	BOOST_CHECK(i == strip.end());
	*/

	// no more strips
	t++;
	BOOST_CHECK(t == exp->strips.end());
}

BOOST_AUTO_TEST_CASE(triangle_stripifier_find_all_strips_0)
{
	// stripify on empty mesh should not fail
	MeshPtr m(new Mesh());
	TriangleStripifier ts(m);
	std::list<TriangleStripPtr> all_strips = ts.find_all_strips();
	BOOST_CHECK_EQUAL(all_strips.size(), 0);
}

BOOST_AUTO_TEST_CASE(triangle_stripifier_find_all_strips_1)
{
	MeshPtr m(new Mesh());

	// first strip

	m->add_face(2, 1, 7); // in strip
	MFacePtr s1_face = m->add_face(0, 1, 2); // in strip
	m->add_face(2, 7, 4); // in strip
	m->add_face(4, 7, 11); // in strip
	m->add_face(5, 3, 2);
	m->add_face(1, 0, 8); // in strip
	m->add_face(0, 8, 9); // bad orientation!
	m->add_face(8, 0, 10); // in strip
	m->add_face(10, 11, 8); // in strip

	// parallel strip
	MFacePtr s2_face = m->add_face(0, 2, 21); // in strip
	m->add_face(21, 2, 22); // in strip
	m->add_face(2, 4, 22); // in strip
	m->add_face(21, 24, 0); // in strip
	m->add_face(9, 0, 24); // in strip

	// parallel strip, further down
	MFacePtr s3_face = m->add_face(8, 11, 31); // in strip
	m->add_face(8, 31, 32); // in strip
	m->add_face(31, 11, 33); // in strip

	// find strips
	TriangleStripifier ts(m);
	std::list<TriangleStripPtr> all_strips = ts.find_all_strips();
	std::list<TriangleStripPtr>::const_iterator t = all_strips.begin();

	std::deque<int> strip = (*t)->get_strip();
	std::deque<int>::const_iterator i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 7);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 1);
	BOOST_CHECK_EQUAL(*i++, 0);
	BOOST_CHECK_EQUAL(*i++, 8);
	BOOST_CHECK_EQUAL(*i++, 10);
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK(i == strip.end());

	t++;
	strip = (*t)->get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 4);
	BOOST_CHECK_EQUAL(*i++, 22);
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 21);
	BOOST_CHECK_EQUAL(*i++, 0);
	BOOST_CHECK_EQUAL(*i++, 24);
	BOOST_CHECK_EQUAL(*i++, 9);
	BOOST_CHECK(i == strip.end());

	t++;
	strip = (*t)->get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 32);
	BOOST_CHECK_EQUAL(*i++, 8);
	BOOST_CHECK_EQUAL(*i++, 31);
	BOOST_CHECK_EQUAL(*i++, 11);
	BOOST_CHECK_EQUAL(*i++, 33);
	BOOST_CHECK(i == strip.end());

	t++;
	strip = (*t)->get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 2);
	BOOST_CHECK_EQUAL(*i++, 5);
	BOOST_CHECK_EQUAL(*i++, 3);
	BOOST_CHECK(i == strip.end());

	t++;
	strip = (*t)->get_strip();
	i = strip.begin();
	BOOST_CHECK_EQUAL(*i++, 0);
	BOOST_CHECK_EQUAL(*i++, 8);
	BOOST_CHECK_EQUAL(*i++, 9);
	BOOST_CHECK(i == strip.end());

	t++;
	BOOST_CHECK(t == all_strips.end());
}

BOOST_AUTO_TEST_SUITE_END()
