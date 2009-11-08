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

#include "trianglemesh.hpp"

BOOST_AUTO_TEST_SUITE(trianglemesh_test_suite)

BOOST_AUTO_TEST_CASE(declare_test) {
	// check that declarations work as expected
	BOOST_CHECK_NO_THROW(Edge edge(0, 1));
	BOOST_CHECK_NO_THROW(Face face(0, 1, 2));
	BOOST_CHECK_NO_THROW(Mesh mesh);

	// degenerate edges and faces
	BOOST_CHECK_THROW(Edge edge(20, 20), std::runtime_error);
	BOOST_CHECK_THROW(Face face(30, 0, 30), std::runtime_error);
	BOOST_CHECK_THROW(Face face(0, 40, 40), std::runtime_error);
	BOOST_CHECK_THROW(Face face(50, 50, 0), std::runtime_error);
	BOOST_CHECK_THROW(Face face(7, 7, 7), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(edge_index_test_0) {
	Edge edge(10, 20);
	BOOST_CHECK_EQUAL(edge.ev0, 10);
	BOOST_CHECK_EQUAL(edge.ev1, 20);
}

BOOST_AUTO_TEST_CASE(edge_index_test_1) {
	Edge edge(20, 10);
	BOOST_CHECK_EQUAL(edge.ev0, 20);
	BOOST_CHECK_EQUAL(edge.ev1, 10);
}

BOOST_AUTO_TEST_CASE(face_next_vertex_test) {
	Face f(9, 3, 18);
	// throw on illegal index?
	BOOST_CHECK_THROW(f.get_next_vertex(5), std::runtime_error);
	// correct next vertex?
	BOOST_CHECK_EQUAL(f.get_next_vertex(9), 3);
	BOOST_CHECK_EQUAL(f.get_next_vertex(3), 18);
	BOOST_CHECK_EQUAL(f.get_next_vertex(18), 9);
}

BOOST_AUTO_TEST_CASE(face_vertex_order_test_0) {
	Face f(6, 2, 5);
	BOOST_CHECK_EQUAL(f.v0, 2);
	BOOST_CHECK_EQUAL(f.v1, 5);
	BOOST_CHECK_EQUAL(f.v2, 6);
}

BOOST_AUTO_TEST_CASE(face_vertex_order_test_1) {
	Face f(2, 5, 6);
	BOOST_CHECK_EQUAL(f.v0, 2);
	BOOST_CHECK_EQUAL(f.v1, 5);
	BOOST_CHECK_EQUAL(f.v2, 6);
}

BOOST_AUTO_TEST_CASE(face_vertex_order_test_2) {
	Face f(5, 6, 2);
	BOOST_CHECK_EQUAL(f.v0, 2);
	BOOST_CHECK_EQUAL(f.v1, 5);
	BOOST_CHECK_EQUAL(f.v2, 6);
}

BOOST_AUTO_TEST_CASE(mesh_add_face_test) {
	Mesh m;

	// add faces
	BOOST_CHECK_NO_THROW(m.add_face(0, 1, 2));
	BOOST_CHECK_NO_THROW(m.add_face(2, 1, 3));
	BOOST_CHECK_NO_THROW(m.add_face(2, 3, 4));
	BOOST_CHECK_EQUAL(m.faces.size(), 3);
	BOOST_CHECK_EQUAL(m._edges.size(), 9);
	// add duplicate face
	BOOST_CHECK_NO_THROW(m.add_face(2, 3, 4));
	MFacePtr f0 = m.add_face(10, 11, 12);
	MFacePtr f1 = m.add_face(12, 10, 11);
	MFacePtr f2 = m.add_face(11, 12, 10);
	BOOST_CHECK_EQUAL(f0, f1);
	BOOST_CHECK_EQUAL(f0, f2);
	// one extra face, three extra edges
	BOOST_CHECK_EQUAL(m.faces.size(), 4);
	BOOST_CHECK_EQUAL(m._edges.size(), 12);
}

BOOST_AUTO_TEST_CASE(face_faces_test_0) {
	// construct mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(1, 3, 2);
	MFacePtr f2 = m.add_face(2, 3, 4);

	// 0->-1
	//  \ / \
	//   2-<-3
	//   2->-3
	//    \ /
	//     4

	// correct faces?
	// f0->faces0 are faces opposite vertex 0, so along edge (1,2)
	BOOST_CHECK_EQUAL(f0->faces0.size(), 1);
	BOOST_CHECK_EQUAL(f0->faces0[0].lock(), f1);
	// f0->faces1 are faces opposite vertex 1, so along edge (2,0)
	BOOST_CHECK_EQUAL(f0->faces1.size(), 0);
	// f0->faces2 are faces opposite vertex 2, so along edge (0,1)
	BOOST_CHECK_EQUAL(f0->faces2.size(), 0);
	// f1->faces0 are faces opposite vertex 1, so along edge (3,2)
	BOOST_CHECK_EQUAL(f1->faces0.size(), 1);
	BOOST_CHECK_EQUAL(f1->faces0[0].lock(), f2);
	// f1->faces1 are faces opposite vertex 3, so along edge (2,1)
	BOOST_CHECK_EQUAL(f1->faces1.size(), 1);
	BOOST_CHECK_EQUAL(f1->faces1[0].lock(), f0);
	// f1->faces2 are faces opposite vertex 2, so along edge (1,3)
	BOOST_CHECK_EQUAL(f1->faces2.size(), 0);
	// f2->faces0 are faces opposite vertex 2, so along edge (3,4)
	BOOST_CHECK_EQUAL(f2->faces0.size(), 0);
	// f2->faces1 are faces opposite vertex 3, so along edge (4,2)
	BOOST_CHECK_EQUAL(f2->faces1.size(), 0);
	// f2->faces2 are faces opposite vertex 4, so along edge (2,3)
	BOOST_CHECK_EQUAL(f2->faces2.size(), 1);
	BOOST_CHECK_EQUAL(f2->faces2[0].lock(), f1);
}

BOOST_AUTO_TEST_CASE(face_faces_test_1) {
	// construct mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(1, 3, 2);
	MFacePtr f2 = m.add_face(2, 3, 4);
	MFacePtr f3 = m.add_face(2, 3, 5);
	// edge with no adjacent faces:
	BOOST_CHECK_EQUAL(f0->faces2.size(), 0); // f0 0 1
	// edge with one adjacent face
	BOOST_CHECK_EQUAL(f0->faces0.size(), 1); // f0 1 2
	BOOST_CHECK_EQUAL(f1->faces1.size(), 1); // f1 2 1
	BOOST_CHECK_EQUAL(f0->faces0[0].lock(), f1);
	BOOST_CHECK_EQUAL(f1->faces1[0].lock(), f0);
	// edge with two adjacent faces
	BOOST_CHECK_EQUAL(f1->faces0.size(), 2); // f1 3 2
	BOOST_CHECK_EQUAL(f2->faces2.size(), 1); // f2 2 3
	BOOST_CHECK_EQUAL(f3->faces2.size(), 1); // f3 2 3
	BOOST_CHECK_EQUAL(f1->faces0[0].lock(), f2);
	BOOST_CHECK_EQUAL(f1->faces0[1].lock(), f3);
	BOOST_CHECK_EQUAL(f2->faces2[0].lock(), f1);
	BOOST_CHECK_EQUAL(f3->faces2[0].lock(), f1);
}

/* XXX todo: fix

BOOST_AUTO_TEST_CASE(face_get_next_face_test_0) {
	// single triangle mesh
	Mesh m;
	MFacePtr f = m.add_face(0, 1, 2);
	BOOST_CHECK_EQUAL(f->get_next_face(0, 1), MFacePtr());
}

BOOST_AUTO_TEST_CASE(face_get_next_face_test_1) {
	// construct slightly more complicated mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 3);
	MFacePtr f2 = m.add_face(2, 3, 4);
	MFacePtr f3 = m.add_face(5, 3, 2);
	MFacePtr f4 = m.add_face(2, 1, 9); // faces added earlier on get priority when the strip is built, this is to check for that
	// check the function by doing a quick strip traversal
	MFacePtr f = f0;
	BOOST_CHECK_EQUAL(f0->get_next_face(1, 2), f1);
	BOOST_CHECK_EQUAL(f1->get_next_face(2, 3), f2);
	BOOST_CHECK_EQUAL(f2->get_next_face(3, 4), MFacePtr());
}

BOOST_AUTO_TEST_CASE(face_get_next_face_test_2) {
	MeshPtr m(new Mesh());
	MFacePtr f1 = m->add_face(5, 3, 2);
	MFacePtr f2 = m->add_face(1, 0, 8);
	MFacePtr f3 = m->add_face(0, 8, 9); // bad orientation!
	MFacePtr f4 = m->add_face(8, 0, 10);
	BOOST_CHECK_EQUAL(f2->get_next_face(0, 8), f4);
}

BOOST_AUTO_TEST_CASE(mesh_edgemap_test) {
	Edge edge_index1(0, 1);
	Edge edge_index2(0, 1);
	Edge edge_index3(1, 0);
	Edge edge_index4(2, 0);
	Mesh::EdgeMap edges;
	MEdgePtr edge(new MEdge(edge_index1));
	edges[edge_index1] = edge;
	Mesh::EdgeMap::const_iterator edge_iter;
	edge_iter = edges.find(edge_index1);
	BOOST_CHECK_EQUAL(edge_iter->second.lock(), edge);
	edge_iter = edges.find(edge_index2);
	BOOST_CHECK_EQUAL(edge_iter->second.lock(), edge);
	edge_iter = edges.find(edge_index3);
	BOOST_CHECK_EQUAL(edge_iter->second.lock(), edge);
	edge_iter = edges.find(edge_index4);
	BOOST_CHECK(edge_iter == edges.end());
}

*/

BOOST_AUTO_TEST_SUITE_END()
