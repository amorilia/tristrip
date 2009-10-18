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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <stdexcept>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ Definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A standalone non-degenerate edge, represented as an ordered pair.
class Edge {
public:
	int ev0, ev1;

	Edge(int _ev0, int _ev1);

	bool operator<(const Edge & otheredge) const;
	bool operator==(const Edge & otheredge) const;

	std::vector<int> get_common_vertices(const Edge & otheredge) const;
};

class MFace; // forward declaration

//! A non-degenerate edge with links to other parts of a mesh.
class MEdge : public Edge {
public:
	//! Note: faces are set in Mesh::add_face.
	std::vector<boost::weak_ptr<const MFace> > faces;

	//! Note: don't call directly! Use Mesh::add_face.
	MEdge(const Edge & edge);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A standalone non-degenerate face.
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
	//! Note: edges are set in Mesh::add_face.
	std::vector<boost::weak_ptr<const MEdge> > edges;

	//! Note: don't call directly! Use Mesh::add_face.
	MFace(const Face & face);

	//! Get pointer to edge object.
	boost::shared_ptr<const MEdge>
	get_edge(int ev0, int ev1) const;

	std::vector<boost::shared_ptr<const MEdge> >
	get_common_edges(const MFace & otherface) const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A mesh built from faces.
class Mesh {
private:
	//! Create new edge for mesh, or return existing edge. List of
	//! faces of the new edge will be empty and needs to be
	//! manually updated. For internal use only.
	boost::shared_ptr<MEdge> add_edge(int ev0, int ev1);

public:
	// We use maps to avoid duplicate entries and quickly detect
	// adjacent faces and adjacent edges.

	typedef std::map<Face, boost::shared_ptr<MFace> > FaceMap;
	FaceMap faces; //! Map for mesh faces.
	typedef std::map<Edge, boost::shared_ptr<MEdge> > EdgeMap;
	EdgeMap edges; //! Map for mesh edges.

	//! Initialize empty mesh.
	Mesh();

	//! Create new face for mesh, or return existing face.
	boost::shared_ptr<MFace> add_face(int v0, int v1, int v2);
};
