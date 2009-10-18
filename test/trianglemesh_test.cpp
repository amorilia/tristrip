#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "trianglemesh.hpp"

BOOST_AUTO_TEST_SUITE(trianglemesh_test_suite)

BOOST_AUTO_TEST_CASE(declare_test) {
	BOOST_CHECK_NO_THROW(Edge::Index index(0, 1));
	BOOST_CHECK_NO_THROW(Edge edge(0, 1));
	BOOST_CHECK_NO_THROW(Face face(0, 1, 2));
	BOOST_CHECK_NO_THROW(Mesh mesh);
}

BOOST_AUTO_TEST_CASE(edge_index_test) {
	{
		Edge::Index index(10, 20);
		BOOST_CHECK_EQUAL(index.first, 10);
		BOOST_CHECK_EQUAL(index.second, 20);
	}
	{
		Edge::Index index(20, 10);
		BOOST_CHECK_EQUAL(index.first, 10);
		BOOST_CHECK_EQUAL(index.second, 20);
	}
}

BOOST_AUTO_TEST_CASE(add_face_test) {
	Mesh m;

	// add faces
	BOOST_CHECK_NO_THROW(m.add_face(0, 1, 2));
	BOOST_CHECK_NO_THROW(m.add_face(2, 1, 3));
	BOOST_CHECK_NO_THROW(m.add_face(2, 3, 4));
}

BOOST_AUTO_TEST_SUITE_END()
