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

#include "trianglemesh.hpp"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ Definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! Find another face, also adjacent to edge, with different edge windings.
MFacePtr find_other_face(int ev0, int ev1, MFacePtr face) {
	// (Note: implementation is slightly simpler compared to the
	// original algorithm but in most use cases it gives identical
	// results.)
	if (!face) throw std::runtime_error("Must specify face.");
	MEdgePtr edge = face->get_edge(ev0, ev1);
	for (MEdge::Faces::const_iterator face_iter = edge->faces.begin();
	        face_iter != edge->faces.end(); face_iter++) {
		// skip expired faces
		// (lock() returns MFacePtr() on expired faces!)
		MFacePtr otherface = face_iter->lock();
		if (!otherface) continue;
		// skip given face
		if (otherface == face) continue;
		// return other face if it has different winding along the edge
		if (face->get_vertex_winding(ev0, ev1) != otherface->get_vertex_winding(ev0, ev1))
			return otherface;
	};
	return MFacePtr();
}
/*

def _Counter():
    i = 1
    while 1:
        yield i
        i += 1

def _xwrap(idx, maxlen):
    while idx < maxlen:
        yield idx
        idx += 1
    maxlen,idx = idx,0
    while idx < maxlen:
        yield idx
        idx += 1
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TriangleStrip {
	//Heavily adapted from NvTriStrip.
	//Origional can be found at http://developer.nvidia.com/view.asp?IO=nvtristrip_library.

	std::vector<MFacePtr> faces;
	MFacePtr start_face;
	MEdgePtr start_edge;
	int start_ev0; //! start_edge->ev0 or ev1 (determines direction of edge).
	int start_ev1; //! start_edge->ev0 or ev1 (determines direction of edge).
	int experiment_id;
	int strip_id;

	// Initialized to zero (just after class definition).
	static int NUM_STRIPS; //! Used to determine the next strip id.

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Public Methods
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TriangleStrip(MFacePtr _start_face, MEdgePtr _start_edge,
	              int _strip_id=-1, int _experiment_id=-1) {
		start_face = _start_face;
		start_edge = _start_edge;
		// pick first and second vertex according to the
		// winding of start_face
		if (start_face->get_vertex_winding(start_edge->ev0, start_edge->ev1) == 0) {
			start_ev0 = start_edge->ev0;
			start_ev1 = start_edge->ev1;
		} else {
			start_ev0 = start_edge->ev1;
			start_ev1 = start_edge->ev0;
		};
		if (_strip_id != -1) {
			strip_id = _strip_id;
		} else {
			strip_id = NUM_STRIPS++;
		};
		experiment_id = _experiment_id;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~ Element Membership Tests
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//! Does the given face belong to the strip?
	bool has_face(MFacePtr face) {
		// Note: original method was called FaceInStrip and
		// could test for multiple faces - however we only
		// need to check a single face at a time
		if (experiment_id != -1) {
			return (face->test_strip_id == strip_id);
		} else {
			return (face->strip_id == strip_id);
		}
	}

	//! Is the face marked in this strip?
	bool is_face_marked(MFacePtr face) {
		// does it belong to a final strip?
		bool result = (face->strip_id != -1);
		// it does not belong to a final strip... does it
		// belong to the current experiment?
		if ((!result) && (experiment_id != -1)) {
			result = (face->experiment_id == experiment_id);
		};
		return result;
	}

	//! Mark face in this strip.
	void mark_face(MFacePtr face) {
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
};

/*

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    def Build(self):
        """Builds the face strip forwards, then backwards, and returns the joined list"""

        ForwardFaces = []
        self.Faces = BackwardFaces = []

        def _AlwaysTrue(face):
            """Utility for building face traversal list"""
            return 1

        def _UniqueFace(face):
            """Utility for building face traversal list"""
            v0,v1,v2=face.v
            bv0,bv1,bv2=0,0,0
            for faces in (ForwardFaces, BackwardFaces):
                for f in faces:
                    fv = f.v
                    if not bv0 and v0 in fv: bv0 = 1
                    if not bv1 and v1 in fv: bv1 = 1
                    if not bv2 and v2 in fv: bv2 = 1
                    if bv0 and bv1 and bv2: return 0
                else: return 1

        def _TraverseFaces(Indices, NextFace, FaceList, BreakTest):
            """Utility for building face traversal list"""
            nv0,nv1 = Indices[-2:]
            NextFace = _FindOtherFace(nv0, nv1, NextFace)
            while NextFace and not self.IsFaceMarked(NextFace):
                if not BreakTest(NextFace): break
                nv0, nv1 = nv1, NextFace.OtherVertex(nv0, nv1)
                FaceList.append(NextFace)
                self.MarkFace(NextFace)
                Indices.append(nv1)
                NextFace = _FindOtherFace(nv0, nv1, NextFace)

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        v0,v1 = self.StartEdgeOrder
        v2 = self.StartFace.OtherVertex(v0,v1)
        self.MarkFace(self.StartFace)
        ForwardFaces.append(self.StartFace)

        _TraverseFaces([v0,v1,v2], self.StartFace, ForwardFaces, _AlwaysTrue)
        _TraverseFaces([v2,v1,v0], self.StartFace, BackwardFaces, _UniqueFace)

        // Combine the Forward and Backward results
        BackwardFaces.reverse()
        self.StartFaceIndex = len(BackwardFaces)
        BackwardFaces.extend(ForwardFaces)
        self.Faces = BackwardFaces
        return self.Faces

    def Commit(self, TaskProgress=None):
        del self.ExperimentId
        count = len(map(self.MarkFace, self.Faces))
        if TaskProgress:
            TaskProgress += count
        return self

    def TriangleListIndices(self):
        result = []
        for face in self.Faces:
            result.extend(face.v)
        return result

    def TriangleStripIndices(self):
        FaceList = self.Faces
        FaceCount = len(FaceList)
        if FaceCount <= 0:
            // No faces is the easiest of all... return an empty list
            return []
        elif FaceCount == 1:
            // One face is really easy ;) just return the verticies in order
            return list(FaceList[0].v)
        elif FaceCount == 2:
            // The case of two faces is pretty simple too...
            face0,face1 = FaceList[:2]
            // Get the common edge
            edge01 = face0.GetCommonEdges(face1)[0]
            // Find the vertex on the first face not on the common edge
            result = [face0.OtherVertex(*edge01.ev)]
            // add the next two verticies on the edge in winding order
            result.append(face0.NextVertex(result[-1]))
            result.append(face0.NextVertex(result[-1]))
            // Find the vertex on the second face not on the common edge
            result.append(face1.OtherVertex(*edge01.ev))
            return result

        face0,face1,face2 = FaceList[:3]
        // Get the edge between face0 and face1
        for edge01 in face0.GetCommonEdges(face1):
            // Get the edge between face1 and face2
            for edge12 in face1.GetCommonEdges(face2):
                // Figure out which vertex we need to end on
                for  v2 in edge01.GetCommonVertices(edge12):
                    // Find the vertex on the first face not on the common edge
                    v0 = face0.OtherVertex(*edge01.ev)
                    // Find the middle vertex from the two endpoints
                    v1 = face0.OtherVertex(v0, v2)

                    // Figure out if the start triangle is backwards
                    upsidedown = face0.NextVertex(v0) != v1
                    if upsidedown:
                        // We need to add a degenerate triangle to flip the strip over
                        result = [v0,v0,v1,v2]
                    else: result = [v0,v1,v2]

                    for face in FaceList[1:]:
                        // Build the strip by repeatedly finding the missing index
                        try:
                            result.append(face.OtherVertex(*result[-2:]))
                        except KeyError:
                            break // constructing strip failed; try other starting combination
                    else:
                        // strip built, so return it
                        return result

        raise ValueError('failed to build strip from triangles')
*/

int TriangleStrip::NUM_STRIPS = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ ExperimentGLSelector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*

class ExperimentGLSelector(object):
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ Constants / Variables / Etc.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Samples = 3
    StripLenHeuristic = 1.0
    MinStripLength = 0

    BestScore = -1
    BestSample = None

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ Definitions
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    def __init__(self, Samples, MinStripLength):
        self.Samples = Samples
        self.MinStripLength = MinStripLength

    def Score(self, experiment):
        stripsize = 0
        for strip in experiment:
            stripsize += len(strip.Faces)
        score = self.StripLenHeuristic * stripsize / len(experiment)
        if score > self.BestScore:
            self.BestScore = score
            self.BestSample = experiment

    def Result(self):
        result = self.BestSample
        del self.BestScore
        del self.BestSample
        return result

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ TriangleStripifier
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Heavily adapted from NvTriStrip.
//Origional can be found at http://developer.nvidia.com/view.asp?IO=nvtristrip_library.
class TriangleStripifier(object):

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ Constants / Variables / Etc.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    GLSelector = ExperimentGLSelector(3, 3)

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ Public Methods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    def Stripify(self, mesh, TaskProgress=None):
        self.TriangleList = []
        self.TriangleStrips = []
        //self.TriangleFans = []

        // TODO: Could find triangle fans here
        Strips = self._FindAllStrips(mesh, TaskProgress)
        for strip in Strips:
            if len(strip.Faces) < self.GLSelector.MinStripLength:
                self.TriangleList.extend(strip.TriangleListIndices())
            else:
                self.TriangleStrips.append(strip.TriangleStripIndices())

        result = [('list', self.TriangleList), ('strip', self.TriangleStrips)]//, ('fan',self.TriangleFans) ]
        return result

    __call__ = Stripify

    def StripifyIter(self, mesh, TaskProgress=None):
        // TODO: Could find triangle fans here
        Strips = self._FindAllStrips(mesh, TaskProgress)
        for strip in Strips:
            if len(strip.Faces) < self.GLSelector.MinStripLength:
               yield 'list', strip.TriangleListIndices()
            else:
               yield 'strip', strip.TriangleStripIndices()

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ Protected Methods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    def _FindStartFaceIndex(self, FaceList):
        """Find a good face to start stripification with."""
        bestfaceindex = 0
        bestscore = 0
        faceindex = -1

        for face in FaceList:
            faceindex  += 1
            score = 0
            for edge in face.edges:
                score += not edge.NextFace(face) and 1 or 0
            // best possible score is 2 -- a face with only one neighbor
            // (a score of 3 signifies a lonely face)
            if bestscore < score < 3:
                bestfaceindex, bestscore = faceindex, score
                if bestscore >= 2:
                    break
        return bestfaceindex

    def _FindGoodResetPoint(self, mesh):
        FaceList = mesh.Faces
        lenFaceList = len(mesh.Faces)
        startstep = lenFaceList // 10
        startidx = self._FindStartFaceIndex(FaceList)
        while True: //startidx is not None:
            for idx in _xwrap(startidx, lenFaceList):
                face = FaceList[idx]
                // If this face isn't used by another strip
                if getattr(face, 'StripId', None) is None:
                    startidx = idx + startstep
                    while startidx >= lenFaceList:
                        startidx -= lenFaceList
                    yield face
                    break
            else:
                // We've exhausted all the faces... so lets exit this loop
                break

    def _FindTraversal(self, strip):
        mesh = strip.StartFace.mesh
        FaceList = strip.Faces
        def _IsItHere(idx, currentedge):
            face = FaceList[idx]
            // Get the next vertex in this strips' walk
            v2 = face.OtherVertex(*currentedge)
            // Find the edge parallel to the strip, namely v0 to v2
            paralleledge = mesh.GetEdge(currentedge[0], v2)
            // Find the other face off the parallel edge
            otherface = paralleledge.NextFace(face)
            if otherface and not strip.FaceInStrip(otherface) and not strip.IsFaceMarked(otherface):
                // If we can use it, then do it!
                otheredge = mesh.GetEdge(currentedge[0], otherface.OtherVertex(*paralleledge.ev))
                // TODO: See if we are getting the proper windings.  Otherwise toy with the following
                return otherface, otheredge, (otheredge.ev[0] == currentedge[0]) and 1 or 0
            else:
                // Keep looking...
                currentedge[:] = [currentedge[1], v2]

        startindex = strip.StartFaceIndex
        currentedge = list(strip.StartEdgeOrder[:])
        for idx in xrange(startindex, len(FaceList), 1):
            result = _IsItHere(idx, currentedge)
            if result is not None: return result

        currentedge = list(strip.StartEdgeOrder[:])
        currentedge.reverse()
        for idx in xrange(startindex-1, -1, -1):
            result = _IsItHere(idx, currentedge)
            if result is not None: return result

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    def _FindAllStrips(self, mesh, TaskProgress=None):
        selector = self.GLSelector
        bCleanFaces = getattr(mesh, 'CleanFaces', 0)
        GoodResetPoints = self._FindGoodResetPoint(mesh)
        experimentId = _Counter()
        stripId = _Counter()

        StripifyTask = 0
        CleanFacesTask = 0
        if TaskProgress:
            StripifyTask = TaskProgress.NewSubtask("Triangle mesh stripification", 0, len(mesh.Faces))
            if bCleanFaces:
                CleanFacesTask = TaskProgress.NewSubtask("Clean faces", 0, len(mesh.Faces))

        try:
            while 1:
                Experiments = []
                VisitedResetPoints = {}

                for nSample in xrange(selector.Samples):
                    // Get a good start face for an experiment
                    ExpFace = GoodResetPoints.next()
                    if ExpFace in VisitedResetPoints:
                        // We've seen this face already... try again
                        continue
                    VisitedResetPoints[ExpFace] = 1

                    // Create an exploration from ExpFace in each of the three edge directions
                    for ExpEdge in ExpFace.edges:
                        // See if the edge is pointing in the direction we expect
                        flag = ExpFace.GetVertexWinding(*ExpEdge.ev)
                        // Create the seed strip for the experiment
                        siSeed = TriangleStrip(ExpFace, ExpEdge, flag, stripId.next(), experimentId.next())
                        // Add the seeded experiment list to the experiment collection
                        Experiments.append([siSeed])

                while Experiments:
                    exp = Experiments.pop()
                    while 1:
                        // Build the the last face of the experiment
                        exp[-1].Build()
                        // See if there is a connecting face that we can move to
                        traversal = self._FindTraversal(exp[-1])
                        if traversal:
                            // if so, add it to the list
                            traversal += (stripId.next(), exp[0].ExperimentId)
                            exp.append(TriangleStrip(*traversal))
                        else:
                            // Otherwise, we're done
                            break
                    selector.Score(exp)

                // Get the best experiment according to the selector
                BestExperiment = selector.Result()
                // And commit it to the resultset
                for each in BestExperiment:
                    yield each.Commit(StripifyTask)
                del BestExperiment
        except StopIteration:
            pass

        if bCleanFaces:
            for face in mesh.Faces:
                try: del face.StripId
                except AttributeError: pass
                CleanFacesTask += 1
*/
