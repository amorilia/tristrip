/*

A general purpose stripifier, based on NvTriStrip (http://developer.nvidia.com/)

Credit for porting NvTriStrip to Python goes to the RuneBlade Foundation
library:
http://techgame.net/projects/Runeblade/browser/trunk/RBRapier/RBRapier/Tools/Geometry/Analysis/TriangleStripifier.py?rev=760

The algorithm of this stripifier is an improved version of the RuneBlade
Foundation / NVidia stripifier; it makes no assumptions about the
underlying geometry whatsoever and is intended to produce valid
output in all circumstances.

*/

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

#include <cassert>
#include <list>
#include <set>
#include <boost/foreach.hpp>

#include "trianglemesh.hpp"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TriangleStrip {
public:
	//Heavily adapted from NvTriStrip.
	//Original can be found at http://developer.nvidia.com/view.asp?IO=nvtristrip_library.

	std::list<MFacePtr> faces; // list because we need push_front
	std::list<int> strip; // identical to faces, but written as a strip
	MFacePtr start_face;
	std::list<MFacePtr>::const_iterator start_face_iter;
	int start_vertex; // determines direction of strip
	int experiment_id;
	int strip_id;

	// Initialized to zero (just after class definition).
	static int NUM_STRIPS; //! Used to determine the next strip id.

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Public Methods
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TriangleStrip(MFacePtr _start_face, int _start_vertex,
	              int _strip_id=-1, int _experiment_id=-1);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Element Membership Tests
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//! Does the given face belong to the strip?
	bool has_face(MFacePtr face);

	//! Is the face marked in this strip?
	bool is_face_marked(MFacePtr face);

	//! Mark face in this strip.
	void mark_face(MFacePtr face);

	//! Get unmarked adjacent face.
	MFacePtr get_unmarked_adjacent_face(MFacePtr face, int vi);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//! Building face traversal list starting from the start_face and
	//! the edge opposite pv0. Returns number of faces added.
	int traverse_faces(int pv0, bool forward);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//! Builds the face strip forwards, then backwards, and returns
	//! the joined list.
	void build();

	void commit();

	// Note: TriangleListIndices is too trivial to be of interest, and
	// TriangleStripIndices not needed because we store the strip during
	// face traversal.

};

typedef boost::shared_ptr<TriangleStrip> TriangleStripPtr;

/*

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ ExperimentSelector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ExperimentSelector {
public:
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Constants / Variables / Etc.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	int num_samples;
	float strip_len_heuristic;
	int min_strip_length;
	float best_score;
	std::list<TriangleStripPtr> best_sample; // XXX rename to best_experiment?

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Definitions
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ExperimentSelector(int _num_samples, int _min_strip_length);

	//! Updates best experiment with given experiment, if given
	//! experiment beats current experiment.
	void update_score(std::list<TriangleStripPtr> experiment);

	//! Remove best experiment, to start a fresh sequence of experiments.
	void clear();
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ TriangleStripifier
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Heavily adapted from NvTriStrip.
//Origional can be found at http://developer.nvidia.com/view.asp?IO=nvtristrip_library.
class TriangleStripifier {
public:
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Constants / Variables / Etc.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ExperimentSelector selector;
	MeshPtr mesh;
	std::vector<MFacePtr>::const_iterator start_face_iter;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Public Methods
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TriangleStripifier(MeshPtr _mesh);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Protected Methods
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//! Find a good face to start stripification, potentially
	//! after some strips have already been created. Result is
	//! stored in start_face_iter. Will store mesh->faces.end()
	//! when no more faces are left.
	bool find_good_reset_point();

	//! Looks for a face and vertex to start new strip exactly at the
	//! given face in the strip. Returns true if one is found (otherface and
	//! otheredge then have those where a new strip can start), returns
	//! false if not (otheredge will be the edge of where to look next,
	//! otherface ideally would be updated too but the implementation
	//! currently doesn't do this...).
	bool is_it_here(TriangleStripPtr strip,
	                MFacePtr currentface, int currentvertex, bool is_even_face,
	                MFacePtr & otherface, int & othervertex);

	//! Find a face and edge to start a new strip, parallel to a
	//! given strip.
	bool find_traversal(TriangleStripPtr strip,
	                    MFacePtr & otherface, int & othervertex);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::list<TriangleStripPtr> find_all_strips();
};

*/
