#include "ofMain.h"
#include "ofxMacDisplayFix.h"

class ofApp : public ofBaseApp {
    std::string storedUUID;
public:
    void setup() {
        searchUUIDs();
        registerCallback();
        printInfo();
    }
    
    void searchUUIDs() {
        auto &&uuids = ofxMacDisplayFix::getActiveDisplayUUIDs();
        for(auto &&uuid : uuids) {
            ofLogNotice("uuid") << uuid;
            auto &&rect = ofxMacDisplayFix::getDisplayRectangleFromUUID(uuid);
            ofLogNotice("rect") << rect;
        }
    }
    
    void registerCallback() {
        ofAddListener(ofxMacDisplayFix::displayChanged,
                      this,
                      &ofApp::displayChanged);
        ofxMacDisplayFix::startNotificationOnDisplaysChanged();
    }
    
    void printInfo() {
        auto display = ofxMacDisplayFix::getActiveDisplayIDs().back();
        
        auto mode = ofxMacDisplayFix::getCurrentDisplayMode(display);
        ofLogNotice("refresh rate") << mode.refreshRate << std::endl;
        
        auto detail = ofxMacDisplayFix::getDisplayDetailFromID(display);
        ofLogNotice("physical size") << detail.physicalSize.width << ", " << detail.physicalSize.height;
    }
    
    void draw() {
        ofDrawBitmapString("space: store display uuid", 20, 20);
        ofDrawBitmapString("enter: move window to stored display", 20, 40);
    }
    void exit() {
        ofxMacDisplayFix::stopNotificationOnDisplaysChanged();
    }
    
    
    void displayChanged(ofxMacDisplayFixChangeSummary &summary) {
        ofLogNotice() << "display changed " << ofToString(summary);
    }
    void keyPressed(int key) {
        if(key == ' ') {
            storedUUID = ofxMacDisplayFix::getDisplayUUIDsAtCenterOfCurrentWindow().front();
        }
        if(key == OF_KEY_RETURN) {
            if(storedUUID == "") {
                ofLogWarning() << "please store window uuid with press space key at first";
                return;
            }
            ofxMacDisplayFix::setWindowShapeToDisplayForUUID(storedUUID);
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
