#include "ofxOpenSurf.h"

ofxOpenSurf :: ofxOpenSurf()
{
    srcImage = srcImageTemp = NULL;
    trgImage = trgImageTemp = NULL;
}

ofxOpenSurf :: ~ofxOpenSurf()
{
    waitForThread();
}

void ofxOpenSurf :: setup ()
{
    startThread( true, false );
}

void ofxOpenSurf :: surfStaticImage ( ofxCvImage* srcImage, ofxCvImage* trgImage )
{
    lock();
    srcImageTemp = srcImage;
    trgImageTemp = trgImage;
    bSurfStaticImage = true;
    unlock();
}

void ofxOpenSurf :: surfMotionImage ( ofxCvImage* srcImage, ofxCvImage* trgImage )
{
    lock();
    srcImageTemp = srcImage;
    trgImageTemp = trgImage;
    bSurfMotionImage = true;
    unlock();
}

bool ofxOpenSurf :: findCorners ( const ofPoint src_crn[ 4 ], ofPoint dst_crn[ 4 ] )
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

void ofxOpenSurf :: update ()
{
    //
}

void ofxOpenSurf :: drawSourcePoints ( int x, int y, float scale )
{
    drawPoints( x, y, scale, srcIpts );
}

void ofxOpenSurf :: drawTargetPoints ( int x, int y, float scale )
{
    drawPoints( x, y, scale, trgIpts );
}

void ofxOpenSurf :: drawPoints ( int x, int y, float scale, vector<Ipoint> &ipts, int tailSize )
{
    glPushMatrix();
    glTranslatef( x, y, 0 );
    glScalef( scale, scale, 1 );
    Ipoint* ipt;
    float s,o;
    int r1,c1,r2,c2,lap;
    for( int i=0; i<ipts.size(); i++ )
    {
        ipt = &ipts.at( i );
        s   = ( ( 9.0f / 1.2f ) * ipt->scale ) / 3.0f;
        o   = ipt->orientation;
        lap = ipt->laplacian;
        r1  = fRound( ipt->y );
        c1  = fRound( ipt->x );
        c2  = fRound( s * cos( o ) ) + c1;
        r2  = fRound( s * sin( o ) ) + r1;
        
        if( o ) //green line = orientation
        {
            ofSetColor( ofColor :: green );
            ofLine( c1, r1, c2, r2 );
        }
        else    //green dot = upright conversion
        { 
            ofSetColor( ofColor :: green );
            ofCircle( c1, r1, 1 );
        }
        
        if( lap >= 0 )  //blue circle = dark blob on light
        { 
            ofNoFill();
            ofSetColor( ofColor :: blue );
            ofCircle( c1, r1, fRound( s ) );
        }
        else    // red circle = light blob on dark
        { 
            ofNoFill();
            ofSetColor( ofColor :: red );
            ofCircle( c1, r1, fRound( s ) );
        }
        
        if( tailSize ) //draw motion ipoint dx dy
        { 
            ofSetColor( ofColor :: white );
            ofLine( c1, r1, (int)( c1 + ipt->dx * tailSize ), (int)( r1 + ipt->dy * tailSize ) );
        }
    }
    glPopMatrix();
}

void ofxOpenSurf :: threadedFunction() 
{
    while( isThreadRunning() )
    {
        if( bSurfStaticImage )
        {
            if( srcImage != srcImageTemp )
            {
                lock();
                srcImage = srcImageTemp;
                unlock();
                
                surfDetDes
                ( 
                    srcImage->getCvImage(),     // image to find Ipoints in
                    srcIptsTemp,                // reference to vector of Ipoints
                    false,                      // run in rotation invariant mode?
                    4,                          // number of octaves to calculate
                    4,                          // number of intervals per octave
                    2,                          // initial sampling step
                    0.0006                      // blob response threshold
                 );
            }
            
            if( trgImage != trgImageTemp )
            {
                lock();
                trgImage = trgImageTemp;
                unlock();
                
                surfDetDes
                ( 
                    trgImage->getCvImage(),     // image to find Ipoints in
                    trgIptsTemp,                // reference to vector of Ipoints
                    false,                      // run in rotation invariant mode?
                    4,                          // number of octaves to calculate
                    4,                          // number of intervals per octave
                    2,                          // initial sampling step
                    0.0006                      // blob response threshold
                );
            }
            
            getMatches( srcIptsTemp, trgIptsTemp, matchesTemp );
            
            lock();
            srcIpts = srcIptsTemp;
            trgIpts = trgIptsTemp;
            matches = matchesTemp;
            bSurfStaticImage = false;
            unlock();

        }
        
        if( bSurfMotionImage )
        {
            if( srcImage != srcImageTemp )
            {
                lock();
                srcImage = srcImageTemp;
                unlock();
                
                surfDetDes
                ( 
                    srcImage->getCvImage(),     // image to find Ipoints in
                    srcIptsTemp,                // reference to vector of Ipoints
                    false,                      // run in rotation invariant mode?
                    4,                          // number of octaves to calculate
                    4,                          // number of intervals per octave
                    2,                          // initial sampling step
                    0.0004                      // blob response threshold
                );
            }
            
            lock();
            trgImage = trgImageTemp;
            unlock();
            
            surfDetDes
            ( 
                trgImage->getCvImage(),     // image to find Ipoints in
                trgIptsTemp,                // reference to vector of Ipoints
                false,                      // run in rotation invariant mode?
                4,                          // number of octaves to calculate
                4,                          // number of intervals per octave
                2,                          // initial sampling step
                0.001                       // blob response threshold
            );
            
            getMatches( srcIptsTemp, trgIptsTemp, matchesTemp );
            
            lock();
            srcIpts = srcIptsTemp;
            trgIpts = trgIptsTemp;
            matches = matchesTemp;
            bSurfMotionImage = false;
            unlock();
        }
    }
}