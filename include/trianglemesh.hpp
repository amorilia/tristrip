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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <stdexcept>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ Definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// forward declarations

class MEdge;
typedef boost::shared_ptr<MEdge> MEdgePtr;

class MFace;
typedef boost::shared_ptr<MFace> MFacePtr;

//! A standalone non-degenerate directed edge, represented as a pair
//! of indices.
class Edge {
public:
	int ev0, ev1;

	Edge(int _ev0, int _ev1);

	bool operator<(const Edge & otheredge) const;
	bool operator==(const Edge & otheredge) const;
};

//! A non-degenerate directed edge with links to other parts of a
//! mesh.
class MEdge : public Edge {
public:
	typedef std::vector<boost::weak_ptr<MFace> > Faces;
	Faces faces; //! Note: faces are set in Mesh::add_face.

	//! Note: don't call directly! Use Mesh::add_face.
	MEdge(const Edge & edge);

	//! Dump to std::cout (e.g. for debugging).
	void dump() const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A standalone non-degenerate oriented face.
class Face {
public:
	//! Vertex indices, with v0 always being the lowest index.
	int v0, v1, v2;

	Face(int _v0, int _v1, int _v2);

	bool operator<(const Face & otherface) const;
	bool operator==(const Face & otherface) const;

	//! Returns +1 if vertex order goes with face winding, -1 if
	//! it it goes against face winding.
	int get_vertex_winding(int pv0, int pv1) const;

	//! Get next vertex (keeps eternally looping).
	int get_next_vertex(int vi) const;

	//! Get other vertex.
	int get_other_vertex(int pv0, int pv1) const;
};

//! A non-degenerate face with links to other parts of a mesh.
class MFace : public Face {
public:
	typedef std::vector<boost::weak_ptr<MFace> > Faces;
	//! Lists of connecting faces opposite vertex v0, v1, and v2.
	Faces faces0, faces1, faces2;

	//! The id of the strip the face is assigned to in final
	//! stripification.
	int strip_id;

	//! The id of the strip the face is assigned to during a
	//! stripification experiment.
	int test_strip_id;

	//! The id of the stripification experiment.
	int experiment_id;

	//! Note: don't call directly! Use Mesh::add_face.
	MFace(const Face & face);

	//! Get list of connecting faces along (undirected) edge.
	Faces get_faces(int pv0, int pv1) const;

	//! Dump to std::cout (e.g. for debugging).
	void dump() const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A mesh built from faces.
class Mesh {
private:
	//! Create new edge for mesh, or return existing edge. Lists of
	//! faces of the new edge will be empty and need to be
	//! manually updated. For internal use only.
	MEdgePtr add_edge(MFacePtr face, int ev0, int ev1);

public:
	// We use maps to avoid duplicate entries and quickly detect
	// adjacent faces.

	typedef std::map<Face, boost::weak_ptr<MFace> > FaceMap;
	typedef std::map<Edge, MEdgePtr> EdgeMap;

	//! Map for mesh faces (used internally to avoid duplicates).
	FaceMap _faces;

	//! Map for mesh edges (used internally to build lists of
	//! adjacent faces).
	EdgeMap _edges;

	//! Vector containing all faces.
	std::vector<MFacePtr> faces;

	//! Initialize empty mesh.
	Mesh();

	//! Create new face for mesh, or return existing face.
	MFacePtr add_face(int v0, int v1, int v2);

	//! Lock the mesh. Frees memory by clearing the _edges and _faces
	//! maps which are only used to update the face adjacency lists.
	void lock();

	//! Dump to std::cout (e.g. for debugging).
	void dump() const;
};

typedef boost::shared_ptr<Mesh> MeshPtr;
