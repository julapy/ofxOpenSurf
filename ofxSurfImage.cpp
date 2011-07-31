#include "ofxSurfImage.h"

ofxSurfImage::ofxSurfImage()
{
    //ctor
}

ofxSurfImage::~ofxSurfImage()
{
    //dtor
}

void ofxSurfImage::loadImage(string url){
    ofImage img;
    img.loadImage(url);
    allocate(img.width,img.height);
    unsigned char * pix = img.getPixels();
    setFromPixels(pix,img.width,img.height);
}

void ofxSurfImage::calcSurf(int tp){
    switch(tp){
        case OFX_SURF_STATIC:
            surfDetDes(getCvImage(),ipts,true,3,4,2,0.0004f);
            break;
        case OFX_SURF_VIDEO:
            surfDetDes(getCvImage(),ipts,true,3,4,2,0.004f);
            break;
        case OFX_SURF_VIDEOMATCH:
            surfDetDes(getCvImage(),ipts,false,4,4,2,0.0001f);
            break;
    }
}
