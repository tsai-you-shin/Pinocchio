/*
Copyright (c) 2007 Ilya Baran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "defmesh.h"
#include "motion.h"
#include "../Pinocchio/intersector.h"

vector<Transform<> > DefMesh::computeTransforms() const
{
    vector<Transform<> > out;
    int i;
    out.push_back(Transform<>(Vector3(0.5, 0, 0.5)));

    for(i = 1; i < (int)origSkel.fPrev().size(); ++i) {
        int prevV = origSkel.fPrev()[i];
        Transform<> cur = out[prevV];
        cur = cur * Transform<>(match[prevV]) * Transform<>(transforms[i - 1]) * Transform<>(-match[prevV]);
        if(i == 23){ cur = cur * Transform<>(Vector3(0, 0.1, 0));} // Move Front left Leg
        if(i == 27){ cur = cur * Transform<>(Vector3(0, -0.1, 0));} // Move Front Right Leg
        if(i == 15){ cur = cur * Transform<>(Vector3(0, 0.1, 0));} // Move Back left Leg
        if(i == 19){ cur = cur * Transform<>(Vector3(0, -0.1, 0));} // Move Back Right Leg

        out.push_back(cur);
    }

    out.erase(out.begin());
    return out;
}

bool reallyDeform = true;

void DefMesh::updateMesh() const
{
    vector<Transform<> > t = computeTransforms();
    curMesh = attachment.deform(origMesh, t);
}

vector<Vector3> DefMesh::getSkel() const
{
    vector<Vector3> out = match;

    vector<Transform<> > t;
    if(motion)
        t = filter.getTransforms();
    else
        t = computeTransforms();
        
    for(int i = 0; i < (int)out.size(); ++i) {
        out[i] = t[max(0, i - 1)] * out[i];
    }
    
    return out;
}

