#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetVerticalSync(false);
    
    
    cam.initGrabber(320, 240);
    
    surf.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate(),0));
    
    if(cam.isFrameNew()){
        camImg.setFromPixels(cam.getPixels());
        if(bStoreMarker){
            if(marker){
                delete marker;
            }
            marker = new ofxCvColorImage();
            marker->setFromPixels(cam.getPixels());
            
            bStoreMarker = false;
        }
        
        surf.surfMotionImage(marker, &camImg);
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString("Press space to grab source image", 10, 10);
    ofTranslate(0, 30);
    ofSetColor(255);
    camImg.draw(0, 0);
    surf.drawTargetPoints();

    if(marker){
        ofSetColor(255);
        marker->draw(640,0);
        surf.drawSourcePoints(640,0);
    }
    
    ofSetColor(255, 255, 0);
    for(int i=0;i<surf.matches.size();i++){
        ofDrawLine(640+surf.matches[i].first.x, surf.matches[i].first.y,
                   surf.matches[i].second.x, surf.matches[i].second.y);
    }    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        bStoreMarker = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
