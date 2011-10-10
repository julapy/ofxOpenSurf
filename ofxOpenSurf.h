#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "surflib.h"

class ofxOpenSurf : public ofBaseApp
{
public:
    
     ofxOpenSurf();
    ~ofxOpenSurf();
    
    void surfStaticImage    ( ofxCvImage* srcImage, ofxCvImage* trgImage );
    void surfMotionImage    ( ofxCvImage* srcImage, ofxCvImage* trgImage );
    bool findCorners        ( const ofPoint src_crn[ 4 ], ofPoint dst_crn[ 4 ] );

    void drawSourcePoints   ( int x=0, int y=0, float scale=1 );
    void drawTargetPoints   ( int x=0, int y=0, float scale=1 );
    void drawPoints         ( int x, int y, float scale, vector<Ipoint> &ipts, int tailSize = 0 );
    
    ofxCvImage* srcImage;
    ofxCvImage* trgImage;
    
    IpVec srcIpts;
    IpVec trgIpts;
    
    IpPairVec matches;
};