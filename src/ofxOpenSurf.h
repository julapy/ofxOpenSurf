#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "surflib.h"

class ofxOpenSurf : public ofThread
{
public:
    
     ofxOpenSurf();
    ~ofxOpenSurf();
    
    void setup  ();
    void update ();
    
    void surfStaticImage    ( ofxCvImage* srcImage, ofxCvImage* trgImage );
    void surfMotionImage    ( ofxCvImage* srcImage, ofxCvImage* trgImage );
    bool findCorners        ( const ofPoint src_crn[ 4 ], ofPoint dst_crn[ 4 ] );
    
    void drawSourcePoints   ( int x=0, int y=0, float scale=1 );
    void drawTargetPoints   ( int x=0, int y=0, float scale=1 );
    void drawPoints         ( int x, int y, float scale, vector<Ipoint> &ipts, int tailSize = 0 );
    
    void threadedFunction   ();
    
    ofxCvImage *srcImage, *srcImageTemp;
    ofxCvImage *trgImage, *trgImageTemp;
    
    IpVec srcIpts, srcIptsTemp;
    IpVec trgIpts, trgIptsTemp;
    
    IpPairVec matches, matchesTemp;
    
    bool bSurfStaticImage;
    bool bSurfMotionImage;
};