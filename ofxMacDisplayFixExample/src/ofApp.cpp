#include "ofMain.h"
#include "ofxMacDisplayFix.h"

class ofApp : public ofBaseApp {
public:
    void setup() {
        auto &&uuids = ofxMacDisplayFix::getActiveDisplayUUIDs();
        for(auto &&uuid : uuids) {
            ofLogNotice("uuid") << uuid;
            auto &&rect = ofxMacDisplayFix::getDisplayRectangleFromUUID(uuid);
            ofLogNotice("rect") << rect;
        }
        ofxMacDisplayFix::setWindowShapeToDisplayForUUID(uuids.back());
        ofxMacDisplayFix::startNotificationOnDisplaysChanged();
        ofAddListener(ofxMacDisplayFix::displayChanged,
                      this,
                      &ofApp::displayChanged);
        
        auto display = ofxMacDisplayFix::getActiveDisplayIDs().back();
        auto mode = ofxMacDisplayFix::getCurrentDisplayMode(display);
        ofLogNotice("refresh rate") << mode.refreshRate << std::endl;
        
        auto detail = ofxMacDisplayFix::getDisplayDetailFromID(display);
        ofLogNotice("physical size") << detail.physicalSize.width << ", " << detail.physicalSize.height;
    }
    void update() {
        
    }
    void draw() {
        
    }
    void exit() {
        ofxMacDisplayFix::stopNotificationOnDisplaysChanged();
    }
    
    
    void displayChanged(ofxMacDisplayFixChangeSummary &summary) {
        ofLogNotice() << "display changed " << ofToString(summary);
    }
    void keyPressed(int key) {
        if(key == ' ') {
            
        }
    }
//    void keyReleased(int key);
//    void mouseMoved(int x, int y );
//    void mouseDragged(int x, int y, int button);
//    void mousePressed(int x, int y, int button);
//    void mouseReleased(int x, int y, int button);
//    void mouseEntered(int x, int y);
//    void mouseExited(int x, int y);
//    void windowResized(int w, int h);
//    void dragEvent(ofDragInfo dragInfo);
//    void gotMessage(ofMessage msg);
};

//========================================================================
int main() {
    ofSetupOpenGL(1024, 768, OF_WINDOW);
    ofRunApp(new ofApp());

}
