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

//#define DEBUG 1 // XXX remove when done debugging

#include "trianglestripifier.hpp"

#ifdef DEBUG
#include <iostream>
#endif

TriangleStrip::TriangleStrip(MFacePtr _start_face, int _start_vertex,
                             int _strip_id, int _experiment_id)
		: faces(), start_face(_start_face), start_face_iter(),
		start_vertex(_start_vertex) {
	if (_strip_id != -1) {
		strip_id = _strip_id;
	} else {
		strip_id = NUM_STRIPS++;
	};
	experiment_id = _experiment_id;
};

bool TriangleStrip::has_face(MFacePtr face) {
	// Note: original method was called FaceInStrip and
	// could test for multiple faces - however we only
	// need to check a single face at a time
	if (experiment_id != -1) {
		return (face->test_strip_id == strip_id);
	} else {
		return (face->strip_id == strip_id);
	}
}

bool TriangleStrip::is_face_marked(MFacePtr face) {
	// does it belong to a final strip?
	bool result = (face->strip_id != -1);
	// it does not belong to a final strip... does it
	// belong to the current experiment?
	if ((!result) && (experiment_id != -1)) {
		result = (face->experiment_id == experiment_id);
	};
	return result;
}

void TriangleStrip::mark_face(MFacePtr face) {
	if (experiment_id != -1) {
		face->strip_id = -1;
		face->experiment_id = experiment_id;
		face->test_strip_id = strip_id;
	} else {
		face->strip_id = strip_id;
		face->experiment_id = -1;
		face->test_strip_id = -1;
	}
}

MFacePtr TriangleStrip::get_unmarked_adjacent_face(MFacePtr face, int vi) {
	BOOST_FOREACH(boost::weak_ptr<MFace> _otherface, face->get_adjacent_faces(vi)) {
		if (MFacePtr otherface = _otherface.lock()) {
			if (!is_face_marked(otherface)) {
				return otherface;
			}
		}
	}
	return MFacePtr();
}

int TriangleStrip::traverse_faces(int pv0, bool forward) {
	int count = 0;
	int pv1 = start_face->get_next_vertex(pv0);
	int pv2 = start_face->get_next_vertex(pv1);
#ifdef DEBUG
	// XXX debug
	std::cout << "starting traversal from " << pv0 << ", " << pv1 << ", " << pv2 << std::endl;
#endif
	MFacePtr next_face = get_unmarked_adjacent_face(start_face, pv0);
	while (next_face) {
		// XXX the nvidia stripifier says the following:
		// XXX
		// XXX   this tests to see if a face is "unique",
		// XXX   meaning that its
		// XXX   vertices aren't already in the list
		// XXX   so, strips which "wrap-around" are not allowed
		// XXX
		// XXX not sure why this is a problem, or, if it would
		// XXX be a problem, why nvtristrip only
		// XXX checks this only during backward traversal, so
		// XXX I simple ignore
		// XXX this (rare) problem for now :-)
		/* if not BreakTest(NextFace): break */
		// mark face, and update count
		mark_face(next_face);
		count++;
		// update vertex indices for next_face so that strip
		// continues along edge opposite pv0, add vertex to
		// strip, and add face to strip
		if (count & 1) {
			if (forward) {
				//   pv1--x            pv0-pv1
				//   / \ /   becomes   / \ /
				// pv0-pv2            x--pv2
				pv0 = pv1;
				pv1 = next_face->get_next_vertex(pv0);
				strip.push_back(pv1);
				faces.push_back(next_face);
			} else {
				// x--pv2           pv2-pv0
				//  \ / \   becomes   \ / \
				//  pv1-pv0           pv1--x
				pv0 = pv2;
				pv2 = next_face->get_next_vertex(pv1);
				strip.push_front(pv2);
				faces.push_front(next_face);
			}
		} else {
			if (forward) {
				//  pv0-pv1             x--pv1
				//  / \ / \   becomes  / \ / \
				// x--pv2--x          x--pv0-pv2
				pv0 = pv2;
				pv2 = next_face->get_next_vertex(pv1);
				strip.push_back(pv2);
				faces.push_back(next_face);
			} else {
				//  pv2-pv0             pv2--x
				//  / \ / \   becomes   / \ / \
				// x--pv1--x          pv1-pv0--x
				pv0 = pv1;
				pv1 = next_face->get_next_vertex(pv0);
				strip.push_front(pv1);
				faces.push_front(next_face);
			}
		}
#ifdef DEBUG
		// XXX debug
		std::cout << " traversing face " << pv0 << ", " << pv1 << ", " << pv2 << std::endl;
		if (pv1 != next_face->get_next_vertex(pv0))
			throw std::runtime_error("Traversal bug.");
		if (pv2 != next_face->get_next_vertex(pv1))
			throw std::runtime_error("Traversal bug.");
		if (pv0 != next_face->get_next_vertex(pv2))
			throw std::runtime_error("Traversal bug.");
#endif
		// get next face opposite pv0
		next_face = get_unmarked_adjacent_face(next_face, pv0);
	};
	return count;
};

void TriangleStrip::build() {
	faces.clear();
	strip.clear();
	// find start indices
	int v0 = start_vertex;
	int v1 = start_face->get_next_vertex(v0);
	int v2 = start_face->get_next_vertex(v1);
	// mark start face and add it to faces and strip
	mark_face(start_face);
	faces.push_back(start_face);
	start_face_iter = faces.begin();
	strip.push_back(v0);
	strip.push_back(v1);
	strip.push_back(v2);
	// while traversing backwards, start face gets shifted forward
	// so we keep track of that (to get winding right in the end)
	traverse_faces(v0, true);
	int count = traverse_faces(v2, false);
	// fix winding by adding degenerate triangle to front, if needed
	if (count & 1) strip.push_front(strip.front());
	// XXX debug
	assert(start_face == *start_face_iter);
};

void TriangleStrip::commit() {
	experiment_id = -1;
	BOOST_FOREACH(MFacePtr face, faces) mark_face(face);
};

// Note: TriangleListIndices is too trivial to be of interest, and
// TriangleStripIndices not needed because we store the strip during
// face traversal.

int TriangleStrip::NUM_STRIPS = 0;

ExperimentSelector::ExperimentSelector(int _num_samples, int _min_strip_length)
		: num_samples(_num_samples), min_strip_length(_min_strip_length),
		strip_len_heuristic(1.0), best_score(0.0), best_sample() {};

void ExperimentSelector::update_score(std::list<TriangleStripPtr> experiment) {
	int stripsize = 0;
	for (std::list<TriangleStripPtr>::const_iterator strip = experiment.begin();
	        strip != experiment.end(); strip++) {
		stripsize += (*strip)->faces.size();
	};
	float score = strip_len_heuristic * stripsize / experiment.size();
	if (score > best_score) {
		best_score = score;
		best_sample = experiment;
	};
}

void ExperimentSelector::clear() {
	best_score = 0.0;
	best_sample.clear();
}

TriangleStripifier::TriangleStripifier(MeshPtr _mesh) : selector(10, 0), mesh(_mesh), start_face_iter(_mesh->faces.end()) {};

bool TriangleStripifier::find_good_reset_point() {
	int start_step = mesh->faces.size() / 10;
	if ((mesh->faces.end() - start_face_iter) > start_step) {
		start_face_iter += start_step;
	} else {
		start_face_iter = mesh->faces.begin() + (start_step - (mesh->faces.end() - start_face_iter));
	};
	std::vector<MFacePtr>::const_iterator face = start_face_iter;
	do {
		if ((*face)->strip_id == -1) {
			// face not used in any strip, so start there for next strip
			start_face_iter = face;
			return true;
		};
		face++;
		if (face == mesh->faces.end())
			face = mesh->faces.begin();
	} while (face != start_face_iter);
	// we have exhausted all the faces
	return false;
};

bool TriangleStripifier::is_it_here(TriangleStripPtr strip,
                                    MFacePtr currentface, int currentvertex, bool is_even_face,
                                    MFacePtr & otherface, int & othervertex) {
	// Get the next vertices in this strip's walk
	int v0 = currentvertex, v1, v2;
	if (is_even_face) {
		v1 = currentface->get_next_vertex(v0);
		v2 = currentface->get_next_vertex(v1);
	} else {
		v2 = currentface->get_next_vertex(v0);
		v1 = currentface->get_next_vertex(v2);
	};
	// Find the other face off the parallel edge
	// Edge parallel to the strip is v0 to v2
	otherface = currentface->get_next_face(v0, v2);
	if (otherface && !strip->has_face(otherface) && !strip->is_face_marked(otherface)) {
		// If we can use it, then do it!
		if (is_even_face) {
			othervertex = v2;
		} else {
			othervertex = v0;
		};
		return true;
	} else {
		// Keep looking...
		othervertex = v1;
		// XXX would like to set otherface to the next face too...
		// XXX but faces aren't linked in the strip so cannot do
		return false;
	};
}

bool TriangleStripifier::find_traversal(TriangleStripPtr strip,
                                        MFacePtr & otherface, int & othervertex) {
	// forward search
	// v0 v1 v2 ...
	int currentvertex = strip->start_vertex;
	bool is_even_face = true;
	MFacePtr currentface; // = strip->start_face; // XXX set below
	for (std::list<MFacePtr>::const_iterator iter = strip->start_face_iter;
	        iter != strip->faces.end(); iter++) {
		currentface = *iter;
		if (is_it_here(strip, currentface, currentvertex, is_even_face,
		               otherface, othervertex))
			return true;
		currentvertex = othervertex;
		is_even_face = !is_even_face;
		// XXX otherface not updated, otherwise would do "currentface = otherface"
	};
	// backward search
	// v1 v0 ...
	is_even_face = true;
	currentvertex = strip->start_face->get_next_vertex(strip->start_vertex);
	for (std::list<MFacePtr>::const_iterator iter = strip->start_face_iter;
	        iter != strip->faces.begin();) {
		currentface = *(--iter);
		if (is_it_here(strip, currentface, currentvertex, is_even_face,
		               otherface, othervertex))
			return true;
		currentvertex = othervertex;
		is_even_face = !is_even_face;
		// XXX otherface not updated, otherwise would do "currentface = otherface"
	};
	// nothing found
	return false;
}

std::list<TriangleStripPtr> TriangleStripifier::find_all_strips() {
	std::list<TriangleStripPtr> all_strips;
	int experiment_id = 1;
	int strip_id = 1;

	while (true) {
		// note: one experiment is a collection of adjacent strips
		std::list<std::list<TriangleStripPtr> > experiments;
		std::set<Face> visited_reset_points;
		for (int n_sample = 0; n_sample < selector.num_samples; n_sample++) {
			// Get a good start face for an experiment
			if (!find_good_reset_point()) {
				// done!
				break;
			};
			MFacePtr exp_face = start_face_iter->second;
			// XXX do we need to check this?
			if (visited_reset_points.find(start_face_iter->first) != visited_reset_points.end()) {
				// We've seen this face already... try again
				continue;
			}
			visited_reset_points.insert(start_face_iter->first);
			// Create an exploration from ExpFace in each of the three directions
			int vertices[] = {exp_face->v0, exp_face->v1, exp_face->v2};
			BOOST_FOREACH(int exp_vertex, vertices) {
				// Create the seed strip for the experiment
				TriangleStripPtr si_seed(new TriangleStrip(exp_face, exp_vertex, strip_id++, experiment_id++));
				// Add the seeded experiment list to the experiment collection
				std::list<TriangleStripPtr> exp;
				exp.push_back(si_seed);
				experiments.push_back(exp);
			}
		}
		if (experiments.empty()) {
			// no more experiments to run: done!!
			return all_strips;
		};
		while (!experiments.empty()) {
			std::list<TriangleStripPtr> exp = experiments.back();
			experiments.pop_back(); // no reason to keep in this list
			while (true) {
				// Build the the last face of the experiment
				exp.back()->build();
				// See if there is a connecting face that we can move to
				MFacePtr otherface;
				int othervertex;
				if (find_traversal(exp.back(), otherface, othervertex)) {
					// if so, add it to the list
					exp.push_back(TriangleStripPtr(new TriangleStrip(otherface, othervertex, strip_id++, exp.back()->experiment_id)));
				} else {
					// Otherwise, we're done
					break;
				}
			}
			selector.update_score(exp);
		};
		// Get the best experiment according to the selector
		std::list<TriangleStripPtr> best_experiment = selector.best_sample; // XXX make copy!
		selector.clear();
		// And commit it to the resultset
		BOOST_FOREACH(TriangleStripPtr strip, best_experiment) {
			strip->commit();
			all_strips.push_back(strip);
		}
		best_experiment.clear();
	}
}
