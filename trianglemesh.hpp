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

class Face; // forward declaration

//! A non-degenerate edge.
class Edge {
private:
	int ev0, ev1;

public:
	//! Edge index: ordered pair of edge vertices.
	typedef std::pair<int, int> Index;
	//! Make edge index.
	static Index make_index(int ev0, int ev1);

	//! Note: faces are set in Mesh::add_face.
	std::vector<boost::weak_ptr<const Face> > faces;

	//! Note: don't call directly! Use Mesh::add_face.
	Edge(int _ev0, int _ev1);

	std::vector<int> get_common_vertices(const Edge & otheredge) const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A non-degenerate face.
class Face {
private:
	int v0, v1, v2;

public:
	//! Note: edges are set in Mesh::add_face.
	std::vector<boost::weak_ptr<const Edge> > edges;

	//! Note: don't call directly! Use Mesh::add_face.
	Face(int _v0, int _v1, int _v2);

	//! Returns +1 if vertex order goes with face winding, -1 if
	//! it it goes against face winding.
	int get_vertex_winding(int pv0, int pv1) const;

	//! Get next vertex (keeps eternally looping).
	int next_vertex(int vi);

	//! Get other vertex.
	int other_vertex(int pv0, int pv1);

	//! Get pointer to edge object.
	boost::shared_ptr<const Edge>
	get_edge(int ev0, int ev1) const;

	std::vector<boost::shared_ptr<const Edge> >
	get_common_edges(const Face & otherface) const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! A mesh built from faces.
class Mesh {
private:
	//! Create new edge for mesh, or return existing edge.
	boost::shared_ptr<Edge> add_edge(int ev0, int ev1);

public:
	std::vector<boost::shared_ptr<Face > > faces; //! List of faces.
	typedef std::map<Edge::Index, boost::shared_ptr<Edge> > EdgeMap;
	EdgeMap edges; //! Map edge indices to edge objects.

	//! Initialize empty mesh.
	Mesh();

	//! Create new edge for mesh, or return existing edge.
	boost::shared_ptr<Face> add_face(int v0, int v1, int v2);
};
