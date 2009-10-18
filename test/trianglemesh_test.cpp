#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "trianglemesh.hpp"

BOOST_AUTO_TEST_SUITE(trianglemesh_test_suite)

BOOST_AUTO_TEST_CASE(declare_test) {
	// check that declarations work as expected
	BOOST_CHECK_NO_THROW(Edge::Index index(0, 1));
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
	Edge::Index index(10, 20);
	BOOST_CHECK_EQUAL(index.first, 10);
	BOOST_CHECK_EQUAL(index.second, 20);
}

BOOST_AUTO_TEST_CASE(edge_index_test_1) {
	Edge::Index index(20, 10);
	BOOST_CHECK_EQUAL(index.first, 10);
	BOOST_CHECK_EQUAL(index.second, 20);
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

BOOST_AUTO_TEST_CASE(add_face_test) {
	Mesh m;

	// add faces
	BOOST_CHECK_NO_THROW(m.add_face(0, 1, 2));
	BOOST_CHECK_NO_THROW(m.add_face(2, 1, 3));
	BOOST_CHECK_NO_THROW(m.add_face(2, 3, 4));
}

BOOST_AUTO_TEST_SUITE_END()
