#include "ofxOpenSurf.h"

ofxOpenSurf::ofxOpenSurf()
{
    //ctor
}

ofxOpenSurf::~ofxOpenSurf()
{
    //dtor
}

void ofxSurfStaticMatch(ofxSurfImage * src, ofxSurfImage * dst, IpPairVec * matches){
     //IpPairVec matches;
     surfDetDes(src->getCvImage(),src->ipts,false,4,4,2,0.0006f);
     surfDetDes(dst->getCvImage(),dst->ipts,false,4,4,2,0.0006f);
     getMatches(src->ipts,dst->ipts,*matches);
     //return matches;
}

void ofxSurfVideoMatch(ofxSurfImage * cam, ofxSurfImage * mrk, IpPairVec * matches){
    surfDetDes(cam->getCvImage(),cam->ipts,false,4,4,2,0.001f);
    getMatches(cam->ipts,mrk->ipts,*matches);
}

int ofxSurfObjCorners(IpPairVec & matches,const ofPoint src_crn[4],ofPoint dst_crn[4])
{
    CvPoint src_corners[ 4 ];
    CvPoint dst_corners[ 4 ];
    
    src_corners[ 0 ].x = src_crn[ 0 ].x;
    src_corners[ 0 ].y = src_crn[ 0 ].y;
    src_corners[ 1 ].x = src_crn[ 1 ].x;
    src_corners[ 1 ].y = src_crn[ 1 ].y;
    src_corners[ 2 ].x = src_crn[ 2 ].x;
    src_corners[ 2 ].y = src_crn[ 2 ].y;
    src_corners[ 3 ].x = src_crn[ 3 ].x;
    src_corners[ 3 ].y = src_crn[ 3 ].y;
    
    int success = translateCorners( matches, src_corners, dst_corners );
    
    if( success )
    {
        dst_crn[ 0 ].x = dst_corners[ 0 ].x;
        dst_crn[ 0 ].y = dst_corners[ 0 ].y;
        dst_crn[ 1 ].x = dst_corners[ 1 ].x;
        dst_crn[ 1 ].y = dst_corners[ 1 ].y;
        dst_crn[ 2 ].x = dst_corners[ 2 ].x;
        dst_crn[ 2 ].y = dst_corners[ 2 ].y;
        dst_crn[ 3 ].x = dst_corners[ 3 ].x;
        dst_crn[ 3 ].y = dst_corners[ 3 ].y;
    }
    
    return success;
}

void ofxDrawIpoints(int x, int y, float sz, std::vector<Ipoint> &ipts, int tailSize){
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(sz,sz,1);
    Ipoint * ipt;
    float s,o;
    int r1,c1,r2,c2,lap;
    for(unsigned int i=0;i<ipts.size();i++){
        ipt = &ipts.at(i);
        s = ((9.0f/1.2f) * ipt->scale) / 3.0f;
        o = ipt->orientation;
        lap = ipt->laplacian;
        r1 = fRound(ipt->y);
        c1 = fRound(ipt->x);
        c2 = fRound(s * cos(o)) + c1;
        r2 = fRound(s * sin(o)) + r1;

        if(o){ //green line = orientation
            ofSetColor(0x00ff00);
            ofLine(c1,r1,c2,r2);
        }else{ //green dot = upright conversion
            ofSetColor(0x00ff00);
            ofCircle(c1,r1,1);
        }

        if(lap>=0){ //blue circle = dark blob on light
            ofSetColor(0x0000ff);
            ofNoFill();
            ofCircle(c1,r1,fRound(s));
        }else{ // red circle = light blob on dark
            ofSetColor(0xff0000);
            ofNoFill();
            ofCircle(c1,r1,fRound(s));
        }

        if(tailSize){ //draw motion ipoint dx dy
            ofSetColor(0xffffff);
            ofLine(c1,r1,int(c1+ipt->dx*tailSize),int(r1+ipt->dy*tailSize));
        }
    }
    glPopMatrix();
}

void ofxSurfMotion::setup(ofxSurfImage * _cam){
    cam = _cam;
}

void ofxSurfMotion::update(){
    oldIpts = cam->ipts;
    motion.clear();
    surfDetDes(cam->getCvImage(),cam->ipts,true,3,4,2,0.0004f);
    getMatches(cam->ipts,oldIpts,matches);
    for(uint i=0;i<matches.size();i++){
        Motion m;
        m.src.set(matches[i].first.x,matches[i].first.y);
        m.dst.set(matches[i].second.x,matches[i].second.y);
        float dx = matches[i].first.dx;
        float dy = matches[i].first.dy;
        m.speed = sqrtf(dx*dx+dy+dy);
        motion.push_back(m);
    }
}

void ofxSurfMotion::draw(int x, int y, float sz){
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(sz,sz,1);
    ofSetColor(0xff0000);
    ofNoFill();
    for(uint i=0;i<motion.size();i++){
        Motion m = motion[i];
        ofLine(m.src.x,m.src.y,m.dst.x,m.dst.y);
        ofCircle(m.src.x,m.src.y,m.speed);
    }
    glPopMatrix();
}

/*void ofxDrawMatch(int x,int y, float sz, IpPairVec match){
    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(sz,sz,1);
    for(uint i=0;i<matches.size();i++){
        ofSetColor(0xff0000);
        ofNoFill();
        ofCircle(matches[i].first.x,matches[i].first.y,3);
        ofCircle(matches[i].second.x+marker.width,matches[i].second.y,3);
        ofSetColor(0xffff00);
        ofLine(matches[i].first.x,matches[i].first.y,matches[i].second.x+marker.width,matches[i].second.y);
    }
    glPopMatrix();
}*/
