#ifndef OFXSURFIMAGE_H
#define OFXSURFIMAGE_H

#include "ofxOpenCv.h"
#include "surflib.h"

#define OFX_SURF_STATIC 0
#define OFX_SURF_VIDEO 1
#define OFX_SURF_VIDEOMATCH 2

class ofxSurfImage: public ofxCvColorImage
{
    public:
        ofxSurfImage();
        virtual ~ofxSurfImage();
        void loadImage(string url);
        void calcSurf(int tp=0);

        IpVec ipts;
    protected:
    private:
};

#endif // OFXSURFIMAGE_H
