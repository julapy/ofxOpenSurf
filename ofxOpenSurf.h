#ifndef OFXOPENSURF_H
#define OFXOPENSURF_H

#include "ofxOpenCv.h"
#include "surflib.h"
#include "ofxSurfImage.h"

class ofxOpenSurf
{
    public:
        ofxOpenSurf();
        virtual ~ofxOpenSurf();

        //void ofxDrawIpoints(int x, int y, float sz, std::vector<Ipoint> &ipts, int tailSize=0);
    protected:
    private:
};

/*class ofxSurfStaticMatch{
    public:
        void match(ofxSurfImage * src, ofxSurfImage * dst);

        //ofxSurfImage src;
        //ofxSurfImage dst;

        IpPairVec matches;
};*/

class ofxSurfMotion{
    public:
        void setup(ofxSurfImage * _cam);
        void update();
        void draw(int x=0, int y=0, float sz=1);

        ofxSurfImage * cam;
        IpVec oldIpts;
        IpPairVec matches;

        struct Motion{
            ofPoint src;
            ofPoint dst;
            float speed;
        };

        vector <Motion> motion;
};

void ofxDrawIpoints(int x, int y, float sz, std::vector<Ipoint> &ipts, int tailSize=0);
//void ofxDrawMatch(int x,int y, float sz, IpPairVec match);
void ofxSurfStaticMatch(ofxSurfImage * src, ofxSurfImage * dst, IpPairVec *matches);
void ofxSurfVideoMatch(ofxSurfImage * cam, ofxSurfImage * mrk, IpPairVec * matches);
int ofxSurfObjCorners(IpPairVec & matches,const ofPoint src_crn[4],ofPoint dst_crn[4]);

#endif // OFXOPENSURF_H
