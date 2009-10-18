// Based on:
// http://techgame.net/projects/Runeblade/browser/trunk/RBRapier/RBRapier/Tools/Geometry/Analysis/TriangleMesh.py?rev=760

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ License
//~
//- The RuneBlade Foundation library is intended to ease some
//- aspects of writing intricate Jabber, XML, and User Interface (wxPython, etc.)
//- applications, while providing the flexibility to modularly change the
//- architecture. Enjoy.
//~
//~ Copyright (C) 2002  TechGame Networks, LLC.
//~
//~ This library is free software; you can redistribute it and/or
//~ modify it under the terms of the BSD style License as found in the
//~ LICENSE file included with this distribution.
//~
//~ TechGame Networks, LLC can be reached at:
//~ 3578 E. Hartsel Drive #211
//~ Colorado Springs, Colorado, USA, 80920
//~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ Imports
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <stdexcept>

#include "trianglemesh.hpp"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ Definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Edge::Index::Index(int ev0, int ev1) {
	if (ev0 < ev1) {
		first = ev0;
		second = ev1;
	} else {
		first = ev1;
		second = ev0;
	}
};

Edge::Edge(int _ev0, int _ev1) : ev0(_ev0), ev1(_ev1), faces() {
	// ensure it is not degenerate
	if (ev0 == ev1) throw std::runtime_error("Degenerate edge.");
};

std::vector<int> Edge::get_common_vertices(const Edge & otheredge) const {
	// return [v for v in otheredge.ev if v in self.ev]
	std::vector<int> result;
	if ((ev0 == otheredge.ev0) || (ev0 == otheredge.ev1))
		result.push_back(ev0);
	if ((ev1 == otheredge.ev0) || (ev1 == otheredge.ev1))
		result.push_back(ev1);
	return result;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Face::Face(int _v0, int _v1, int _v2) : v0(_v0), v1(_v1), v2(_v2), edges() {
	// ensure it is not degenerate
	if ((v0 == v1) || (v1 == v2) || (v0 == v2))
		throw std::runtime_error("Degenerate face.");
};

int Face::get_vertex_winding(int pv0, int pv1) const {
	if (((pv0 == v0) && (pv1 == v1))
	        ||
	        ((pv0 == v1) && (pv1 == v2))
	        ||
	        ((pv0 == v2) && (pv1 == v0))) return 0;
	else if (((pv1 == v0) && (pv0 == v1))
	         ||
	         ((pv1 == v1) && (pv0 == v2))
	         ||
	         ((pv1 == v2) && (pv0 == v0))) return 1;
	// bug!
	throw std::runtime_error("Invalid vertex index.");
}

int Face::next_vertex(int vi) {
	if (vi == v0) return v1;
	else if (vi == v1) return v2;
	else if (vi == v2) return v0;
	// bug!
	throw std::runtime_error("Invalid vertex index.");
}

int Face::other_vertex(int pv0, int pv1) {
	if (pv0 == pv1)
		throw std::runtime_error("Vertex indices shouldn't be identical.");
	if (((pv0 == v0) && (pv1 == v1)) || ((pv0 == v1) && (pv1 == v0)))
		return v2;
	if (((pv0 == v1) && (pv1 == v2)) || ((pv0 == v2) && (pv1 == v1)))
		return v0;
	if (((pv0 == v2) && (pv1 == v0)) || ((pv0 == v0) && (pv1 == v2)))
		return v1;
	// bug!
	throw std::runtime_error("Invalid vertex index.");
}

boost::shared_ptr<const Edge> Face::get_edge(int ev0, int ev1) const {
	if (ev0 == ev1)
		throw std::runtime_error("Vertex indices shouldn't be identical.");
	if (((ev0 == v0) && (ev1 == v1)) || ((ev0 == v1) && (ev1 == v0)))
		return edges[0].lock();
	if (((ev0 == v1) && (ev1 == v2)) || ((ev0 == v2) && (ev1 == v1)))
		return edges[1].lock();
	if (((ev0 == v2) && (ev1 == v0)) || ((ev0 == v0) && (ev1 == v2)))
		return edges[2].lock();
	// bug!
	throw std::runtime_error("Invalid vertex index.");
}

std::vector<boost::shared_ptr<const Edge> > Face::get_common_edges(const Face & otherface) const {
	//return [edge for edge in otherface.edges if edge in self.edges]
	std::vector<boost::shared_ptr<const Edge> > result;
	for (int i=0; i<3; i++) {
		boost::shared_ptr<const Edge> edge_i = edges[i].lock();
		for (int j=0; j<3; j++) {
			if (edge_i == otherface.edges[j].lock()) {
				result.push_back(edge_i);
				break; // no otherface edges can match, so break j loop
			}
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

boost::shared_ptr<Edge> Mesh::add_edge(int ev0, int ev1) {
	Edge::Index edge_index(ev0, ev1);
	EdgeMap::const_iterator edge_iter = edges.find(edge_index);
	if (edge_iter == edges.end()) {
		boost::shared_ptr<Edge> edge(new Edge(ev0, ev1));
		edges[edge_index] = edge;
		return edge;
	} else {
		return edge_iter->second;
	};
}

Mesh::Mesh() : faces(), edges() {};

boost::shared_ptr<Face> Mesh::add_face(int v0, int v1, int v2) {
	if ((v0 == v1) || (v1 == v2) || (v2 == v0)) {
		return boost::shared_ptr<Face>();
	} else {
		// create face and edges
		boost::shared_ptr<Face> face(new Face(v0, v1, v2));
		faces.push_back(face);
		boost::shared_ptr<Edge> edge01 = add_edge(v0, v1);
		boost::shared_ptr<Edge> edge12 = add_edge(v1, v2);
		boost::shared_ptr<Edge> edge20 = add_edge(v2, v0);
		// set up weak pointers between face and edges
		face->edges.push_back(edge01);
		face->edges.push_back(edge12);
		face->edges.push_back(edge20);
		edge01->faces.push_back(face);
		edge12->faces.push_back(face);
		edge20->faces.push_back(face);
		return face;
	}
}

