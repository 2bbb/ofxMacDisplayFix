#include "ofMain.h"
#include "ofxMacDisplayFix.h"

class ofApp : public ofBaseApp {
    std::string storedUUID;
public:
    void setup() {
        searchUUIDs();
        registerCallback();
        printInfo();
        ofxMacDisplayFix::printAllDisplayRectangles();
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
        ofDrawBitmapString("A-E:   move window to n-th display (A = main)", 20, 60);
        ofDrawBitmapString("0-9:   move cursor to center of n-th display (0 = main)", 20, 80);
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
        if('A' <= key && key <= 'E') {
            int i = key - 'A';
            auto &&uuids = ofxMacDisplayFix::getActiveDisplayUUIDs();
            if(i < uuids.size()) {
                ofxMacDisplayFix::setWindowShapeToDisplayForUUID(uuids[i]);
            } else {
                ofLogWarning() << "num display is " << key << ", but push " << key;
            }
        }
        if('0' <= key && key <= '9') {
            int i = key - '0';
            auto &&uuids = ofxMacDisplayFix::getActiveDisplayUUIDs();
            if(i < uuids.size()) {
                ofxMacDisplayFix::moveMouseToCenterOfDisplayForUUID(uuids[i]);
            } else {
                ofLogWarning() << "num display is " << key << ", but push " << key;
            }
        }
    }
    
};

//========================================================================
int main() {
    ofSetupOpenGL(1024, 768, OF_WINDOW);
    ofRunApp(new ofApp());

}
