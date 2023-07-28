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

#include <FL/fl_ask.H>
#include <fstream>

#include "processor.h"
#include "../Pinocchio/skeleton.h"
#include "../Pinocchio/utils.h"
#include "../Pinocchio/debugging.h"
#include "../Pinocchio/attachment.h"
#include "../Pinocchio/pinocchioApi.h"
#include "defmesh.h"
#include "animalSkeleton.h"

struct ArgData
{
    ArgData() : stopAtMesh(false), stopAfterCircles(false), skelScale(1.), noFit(false),
                skeleton(HumanSkeleton())
    {
    }

    bool stopAtMesh;
    bool stopAfterCircles;
    string filename;
    string motionname;
    Quaternion<> meshTransform;
    double skelScale;
    bool noFit;
    Skeleton skeleton;
    string skeletonfilename;
};

void printUsageAndExit()
{
    cout << "Usage: DemoUI filename.{obj | ply | off | gts | stl}" << endl;
    cout << "              [-skel skelname]" << endl;
    exit(0);
}

ArgData processArgs(const vector<string> &args)
{
    ArgData out;
    int cur = 2;
    int num = args.size();
    if (num < 2)
        printUsageAndExit();

    out.filename = args[1];

    while (cur < num)
    {
        string curStr = args[cur++];
        if (curStr == string("-skel"))
        {
            if (cur == num)
            {
                cout << "No skeleton specified; ignoring." << endl;
                continue;
            }
            curStr = args[cur++];
            out.skeletonfilename = curStr;
            out.skeleton = AnimalSkeleton(out.skeletonfilename);
            continue;
        }
        cout << "Unrecognized option: " << curStr << endl;
        printUsageAndExit();
    }
    return out;
}

void process(const vector<string> &args, MyWindow *w)
{
    int i;
    ArgData a = processArgs(args);

    Debugging::setOutStream(cout);

    Mesh m(a.filename);
    if (m.vertices.size() == 0)
    {
        cout << "Error reading file.  Aborting." << endl;
        exit(0);
        return;
    }

    for (i = 0; i < (int)m.vertices.size(); ++i)
        m.vertices[i].pos = a.meshTransform * m.vertices[i].pos;
    m.normalizeBoundingBox();
    m.computeVertexNormals();

    Skeleton given = a.skeleton;

    if (a.stopAtMesh)
    { // if early bailout
        w->addMesh(new StaticDisplayMesh(m));
    }

    PinocchioOutput o;
    o = autorig(given, m);

    if (o.embedding.size() == 0)
    {
        cout << "Error embedding" << endl;
        exit(0);
    }

    // Display Mesh
    w->addMesh(new DefMesh(m, given, o.embedding, *(o.attachment)), &given);
    

    // Output skeleton embedding
    for (i = 0; i < (int)o.embedding.size(); ++i)
        o.embedding[i] = (o.embedding[i] - m.toAdd) / m.scale;
    ofstream os("./data/skeleton.out");
    for (i = 0; i < (int)o.embedding.size(); ++i)
    {
        os << i << " " << o.embedding[i][0] << " " << o.embedding[i][1] << " " << o.embedding[i][2] << " " << a.skeleton.fPrev()[i] << endl;
    }

    // Output attachment
    std::ofstream astrm("./data/attachment.out");
    for (i = 0; i < (int)m.vertices.size(); ++i)
    {
        Vector<double, -1> v = o.attachment->getWeights(i);
        for (int j = 0; j < v.size(); ++j)
        {
            double d = floor(0.5 + v[j] * 10000.) / 10000.;
            astrm << d << " ";
        }
        astrm << endl;
    }

    delete o.attachment;
}
