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

// Based on:
// http://techgame.net/projects/Runeblade/browser/trunk/RBRapier/RBRapier/Tools/Geometry/Analysis/TriangleMesh.py?rev=760

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ License of original code:
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

Edge::Edge(int _ev0, int _ev1) {
	// ensure it is not degenerate
	if (_ev0 == _ev1)
		throw std::runtime_error("Degenerate edge.");
	// order indices
	if (_ev0 < _ev1) {
		ev0 = _ev0;
		ev1 = _ev1;
	} else {
		ev0 = _ev1;
		ev1 = _ev0;
	}
};

bool Edge::operator<(const Edge & otheredge) const {
	if (ev0 < otheredge.ev0) return true;
	if (ev1 < otheredge.ev1) return true;
	return false;
};

bool Edge::operator==(const Edge & otheredge) const {
	return ((ev0 == otheredge.ev0) && (ev1 == otheredge.ev1));
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

MEdge::MEdge(const Edge & edge) : Edge(edge), faces() {
	// nothing to do: faces are set in Mesh::add_face.
};

MFacePtr MEdge::get_next_face(Faces::const_iterator & face_iter) const {
	// if there is no next face: we are done
	if (faces.size() <= 1) {
		return MFacePtr();
	}
	// advance the iterator
	do {
		face_iter++;
		if (face_iter == faces.end()) face_iter = faces.begin();
	} while (face_iter->expired());
	// return result to new pointer
	return MFacePtr(*face_iter);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Face::Face(int _v0, int _v1, int _v2) {
	// ensure it is not degenerate
	if ((_v0 == _v1) || (_v1 == _v2) || (_v0 == _v2))
		throw std::runtime_error("Degenerate face.");
	// order vertex indices
	if ((_v0 < _v1) && (_v0 < _v2)) {
		v0 = _v0;
		v1 = _v1;
		v2 = _v2;
	} else if ((_v1 < _v0) && (_v1 < _v2)) {
		v0 = _v1;
		v1 = _v2;
		v2 = _v0;
	} else if ((_v2 < _v0) && (_v2 < _v1)) {
		v0 = _v2;
		v1 = _v0;
		v2 = _v1;
	} else {
		throw std::runtime_error("Oops. Face construction bug!");
	}
};

bool Face::operator<(const Face & otherface) const {
	if (v0 < otherface.v0) return true;
	if (v1 < otherface.v1) return true;
	if (v2 < otherface.v2) return true;
	return false;
};

bool Face::operator==(const Face & otherface) const {
	return ((v0 == otherface.v0) && (v1 == otherface.v1) && (v2 == otherface.v2));
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

int Face::get_next_vertex(int vi) const {
	if (vi == v0) return v1;
	else if (vi == v1) return v2;
	else if (vi == v2) return v0;
	// bug!
	throw std::runtime_error("Invalid vertex index.");
}

int Face::get_other_vertex(int pv0, int pv1) const {
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

MFace::MFace(const Face & face) : Face(face), edges() {
	// nothing to do
};

MEdgePtr MFace::get_edge(int ev0, int ev1) const {
	if (ev0 == ev1)
		throw std::runtime_error("Vertex indices shouldn't be identical.");
	if (((ev0 == v0) && (ev1 == v1)) || ((ev0 == v1) && (ev1 == v0)))
		return edges[0];
	if (((ev0 == v1) && (ev1 == v2)) || ((ev0 == v2) && (ev1 == v1)))
		return edges[1];
	if (((ev0 == v2) && (ev1 == v0)) || ((ev0 == v0) && (ev1 == v2)))
		return edges[2];
	// bug!
	throw std::runtime_error("Invalid vertex index.");
}

std::vector<MEdgePtr> MFace::get_common_edges(const MFace & otherface) const {
	//return [edge for edge in otherface.edges if edge in self.edges]
	std::vector<MEdgePtr> result;
	for (int i=0; i<3; i++) {
		for (int j=0; j<3; j++) {
			if (edges[i] == otherface.edges[j]) {
				result.push_back(edges[i]);
				break; // no otherface edges can match, so break j loop
			}
		}
	}
	return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MEdgePtr Mesh::add_edge(int ev0, int ev1) {
	// create edge index and search if edge already exists in mesh
	Edge edge_index(ev0, ev1);
	EdgeMap::const_iterator edge_iter = edges.find(edge_index);
	if ((edge_iter != edges.end()) && (!edge_iter->second.expired())) {
		// edge already exists!
		return MEdgePtr(edge_iter->second);
	} else {
		// create edge
		MEdgePtr edge(new MEdge(edge_index));
		edges[edge_index] = edge;
		return edge;
	};
}

Mesh::Mesh() : faces(), edges() {};

MFacePtr Mesh::add_face(int v0, int v1, int v2) {
	// create face index and search if face already exists in mesh
	Face face_index(v0, v1, v2);
	FaceMap::const_iterator face_iter = faces.find(face_index);
	if (face_iter != faces.end()) {
		// face already exists!
		return face_iter->second;
	} else {
		// create face
		MFacePtr face(new MFace(face_index));
		faces[face_index] = face;
		// create edges
		// careful here: face indices may have been reodered!!!
		// and edges[0] *must* correspond to (face->v0, face->v1) etc.
		// so we *must* use face->vi rather than vi
		MEdgePtr edge01 = add_edge(face->v0, face->v1);
		MEdgePtr edge12 = add_edge(face->v1, face->v2);
		MEdgePtr edge20 = add_edge(face->v2, face->v0);
		// set up weak pointers between face and edges
		face->edges.push_back(edge01);
		face->edges.push_back(edge12);
		face->edges.push_back(edge20);
		edge01->faces.push_back(face);
		edge12->faces.push_back(face);
		edge20->faces.push_back(face);
		return face;
	};
}
