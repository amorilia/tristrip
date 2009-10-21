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
	BOOST_CHECK_EQUAL(edge.ev0, 10);
	BOOST_CHECK_EQUAL(edge.ev1, 20);
}

BOOST_AUTO_TEST_CASE(edge_common_test_0) {
	Edge e1(10, 11);
	Edge e2(12, 13);
	std::vector<int> common = e1.get_common_vertices(e2);
	BOOST_CHECK_EQUAL(common.size(), 0);
}

BOOST_AUTO_TEST_CASE(edge_common_test_1) {
	Edge e1(10, 11);
	Edge e2(10, 13);
	std::vector<int> common = e1.get_common_vertices(e2);
	BOOST_CHECK_EQUAL(common.size(), 1);
	BOOST_CHECK_EQUAL(common[0], 10);
}

BOOST_AUTO_TEST_CASE(edge_common_test_2) {
	Edge e1(10, 13);
	Edge e2(10, 13);
	std::vector<int> common = e1.get_common_vertices(e2);
	BOOST_CHECK_EQUAL(common.size(), 2);
	BOOST_CHECK_EQUAL(common[0], 10);
	BOOST_CHECK_EQUAL(common[1], 13);
}

BOOST_AUTO_TEST_CASE(face_winding_test) {
	Face f(5, 6, 7);
	// throw on illegal index?
	BOOST_CHECK_THROW(f.get_vertex_winding(5, 8), std::runtime_error);
	BOOST_CHECK_THROW(f.get_vertex_winding(8, 5), std::runtime_error);
	BOOST_CHECK_THROW(f.get_vertex_winding(8, 9), std::runtime_error);
	// correct winding?
	BOOST_CHECK_EQUAL(f.get_vertex_winding(5, 6), 0);
	BOOST_CHECK_EQUAL(f.get_vertex_winding(6, 7), 0);
	BOOST_CHECK_EQUAL(f.get_vertex_winding(7, 5), 0);
	BOOST_CHECK_EQUAL(f.get_vertex_winding(6, 5), 1);
	BOOST_CHECK_EQUAL(f.get_vertex_winding(7, 6), 1);
	BOOST_CHECK_EQUAL(f.get_vertex_winding(5, 7), 1);
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

BOOST_AUTO_TEST_CASE(face_other_vertex_test) {
	Face f(6, 2, 5);
	// throw on illegal index?
	BOOST_CHECK_THROW(f.get_other_vertex(6, 20), std::runtime_error);
	BOOST_CHECK_THROW(f.get_other_vertex(60, 2), std::runtime_error);
	BOOST_CHECK_THROW(f.get_other_vertex(60, 20), std::runtime_error);
	// correct other vertex?
	BOOST_CHECK_EQUAL(f.get_other_vertex(6, 2), 5);
	BOOST_CHECK_EQUAL(f.get_other_vertex(2, 6), 5);
	BOOST_CHECK_EQUAL(f.get_other_vertex(2, 5), 6);
	BOOST_CHECK_EQUAL(f.get_other_vertex(5, 2), 6);
	BOOST_CHECK_EQUAL(f.get_other_vertex(6, 5), 2);
	BOOST_CHECK_EQUAL(f.get_other_vertex(5, 6), 2);
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

BOOST_AUTO_TEST_CASE(add_face_test) {
	Mesh m;

	// add faces
	BOOST_CHECK_NO_THROW(m.add_face(0, 1, 2));
	BOOST_CHECK_NO_THROW(m.add_face(2, 1, 3));
	BOOST_CHECK_NO_THROW(m.add_face(2, 3, 4));
	BOOST_CHECK_EQUAL(m.faces.size(), 3);
	BOOST_CHECK_EQUAL(m.edges.size(), 7);
	// add duplicate face
	BOOST_CHECK_NO_THROW(m.add_face(2, 3, 4));
	MFacePtr f0 = m.add_face(10, 11, 12);
	MFacePtr f1 = m.add_face(12, 10, 11);
	MFacePtr f2 = m.add_face(11, 12, 10);
	BOOST_CHECK_EQUAL(f0, f1);
	BOOST_CHECK_EQUAL(f0, f2);
	// one extra face, three extra edges
	BOOST_CHECK_EQUAL(m.faces.size(), 4);
	BOOST_CHECK_EQUAL(m.edges.size(), 10);
}

BOOST_AUTO_TEST_CASE(face_get_edge_test) {
	// construct mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 3);
	MFacePtr f2 = m.add_face(2, 3, 4);
	// throw on illegal index?
	BOOST_CHECK_THROW(f0->get_edge(0, 3), std::runtime_error);
	BOOST_CHECK_THROW(f0->get_edge(3, 0), std::runtime_error);
	BOOST_CHECK_THROW(f0->get_edge(3, 4), std::runtime_error);
	// correct edge?
	MEdgePtr e0, e1;
	e0 = f0->get_edge(0, 1);
	e1 = f0->get_edge(1, 0);
	BOOST_CHECK_EQUAL(e0, e1);
	e0 = f0->get_edge(0, 1);
	e1 = f0->get_edge(1, 2);
	BOOST_CHECK(e0 != e1);
	e0 = f0->get_edge(1, 2);
	e1 = f1->get_edge(1, 2);
	BOOST_CHECK_EQUAL(e0, e1);
	BOOST_CHECK_EQUAL(e0->ev0, 1);
	BOOST_CHECK_EQUAL(e0->ev1, 2);
	BOOST_CHECK_EQUAL(e1->ev0, 1);
	BOOST_CHECK_EQUAL(e1->ev1, 2);
	e0 = f1->get_edge(2, 3);
	e1 = f2->get_edge(2, 3);
	BOOST_CHECK_EQUAL(e0, e1);
	BOOST_CHECK_EQUAL(e0->ev0, 2);
	BOOST_CHECK_EQUAL(e0->ev1, 3);
	BOOST_CHECK_EQUAL(e1->ev0, 2);
	BOOST_CHECK_EQUAL(e1->ev1, 3);
}

BOOST_AUTO_TEST_CASE(face_get_common_edges_test) {
	// construct mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 3);
	MFacePtr f2 = m.add_face(2, 3, 4);
	// correct edge?
	std::vector<MEdgePtr> common;
	common = f0->get_common_edges(*f2);
	BOOST_CHECK_EQUAL(common.size(), 0);
	common = f0->get_common_edges(*f1);
	BOOST_CHECK_EQUAL(common.size(), 1);
	BOOST_CHECK_EQUAL(common[0]->ev0, 1);
	BOOST_CHECK_EQUAL(common[0]->ev1, 2);
	common = f0->get_common_edges(*f0);
	BOOST_CHECK_EQUAL(common.size(), 3);
	BOOST_CHECK_EQUAL(common[0]->ev0, 0);
	BOOST_CHECK_EQUAL(common[0]->ev1, 1);
	BOOST_CHECK_EQUAL(common[1]->ev0, 1);
	BOOST_CHECK_EQUAL(common[1]->ev1, 2);
	BOOST_CHECK_EQUAL(common[2]->ev0, 0);
	BOOST_CHECK_EQUAL(common[2]->ev1, 2);
}

BOOST_AUTO_TEST_CASE(edge_get_next_face_test) {
	// construct mesh
	Mesh m;
	MFacePtr f0 = m.add_face(0, 1, 2);
	MFacePtr f1 = m.add_face(2, 1, 3);
	MFacePtr f2 = m.add_face(2, 3, 4);
	MFacePtr f3 = m.add_face(5, 3, 2);
	MEdgePtr e;
	// single face: returns no next face
	e = f0->get_edge(0, 1);
	MEdge::Faces::const_iterator face_iter = e->faces.begin();
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), MFacePtr());
	// two faces: returns them alternatively
	e = f0->get_edge(1, 2);
	face_iter = e->faces.begin();
	BOOST_CHECK_EQUAL(MFacePtr(*face_iter), f0);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f1);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f0);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f1);
	// three faces...
	e = f3->get_edge(2, 3);
	face_iter = e->faces.begin();
	BOOST_CHECK_EQUAL(MFacePtr(*face_iter), f1);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f2);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f3);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f1);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f2);
	BOOST_CHECK_EQUAL(e->get_next_face(face_iter), f3);
}

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
	m->add_face(5, 3, 2);
	MFacePtr f2 = m->add_face(1, 0, 8);
	m->add_face(0, 8, 9); // bad orientation!
	MFacePtr f3 = m->add_face(8, 0, 10);
	BOOST_CHECK_EQUAL(f2->get_next_face(0, 8), f3);
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

BOOST_AUTO_TEST_SUITE_END()
