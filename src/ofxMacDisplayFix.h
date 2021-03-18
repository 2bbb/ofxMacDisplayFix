//
//  ofxMacDisplayFix.h
//
//  Created by 2bit on 2021/03/18.
//

#ifndef OFX_MAC_DISPLAY_FIX
#define OFX_MAC_DISPLAY_FIX

#pragma once

#include "ofAppRunner.h"
#include "ofVectorMath.h"
#include "ofRectangle.h"

#include <string>
#include <vector>
#include <iostream>

namespace ofxMacDisplayFix {
    struct DisplayMode {
        bool valid;
        struct {
            std::size_t width;
            std::size_t height;
        } size;
        
        double refreshRate;
        
        struct {
            std::uint32_t flags;
            std::int32_t displayModelID;
        } io;
        
        bool isUsableForDesktopGUI;
        unsigned long typeID;
    };
    
    struct Detail {
        bool valid{false};
        inline operator bool() const
        { return valid; };
        
        std::uint32_t displayID;
        std::string uuid;
        
        bool isBuiltIn;
        
        struct {
            bool isMain;
            
            bool isOnline;
            bool isActive;
            bool isAsleep;
        } status;
        
        struct {
            bool isAlwaysInMirrorSet;
            bool isInHardWareMirrorSet;
            bool isInMirrorSet;
        
            std::uint32_t primaryDisplay;
            std::uint32_t mirrorsDisplay;
        } mirroring;
        
        ofRectangle rect;
        double rotation;
        
        struct {
            double width; // mm
            double height; // mm
        } physicalSize;
        
        struct {
            std::uint32_t modelNumber;
            std::uint32_t serialNumber;
            std::uint32_t unitNumber;
            std::uint32_t vendorNumber;
        } product;
        
        bool usesOpenGLAcceleration;
        
        DisplayMode currentDisplayMode;
        std::vector<DisplayMode> availableDisplayModes;
    };
    
    std::vector<std::uint32_t> getActiveDisplayIDs();
    std::vector<std::string> getActiveDisplayUUIDs();

    std::vector<std::string> getDisplayUUIDsAt(float x, float y);
    
    inline std::vector<std::string> getDisplayUUIDsAtCurrentMousePosition()
    { return getDisplayUUIDsAt(ofGetMouseX(), ofGetMouseY()); }
    
    inline std::vector<std::string> getDisplayUUIDsAtCenterOfCurrentWindow() {
        return getDisplayUUIDsAt(ofGetWindowPositionX() + ofGetWindowWidth() * 0.5,
                                 ofGetWindowPositionY() + ofGetWindowHeight() * 0.5);
    }

    std::string getDisplayUUIDFromID(std::uint32_t displayID);
    std::uint32_t getDisplayIDFromUUID(std::string uuidString);
    ofRectangle getDisplayRectangleFromUUID(std::string uuidString);
    
    DisplayMode getCurrentDisplayMode(std::uint32_t displayID);
    std::vector<DisplayMode> getAvailableDisplayModes(std::uint32_t displayID);
    
    Detail getDisplayDetailFromID(std::uint32_t displayID);
    Detail getDisplayDetailFromUUID(std::string uuidString);
    
    inline void setWindowShapeToDisplayForUUID(std::string uuidString) {
        auto &&rect = getDisplayRectangleFromUUID(uuidString);
        ofSetWindowPosition(rect.x, rect.y);
        ofSetWindowShape(rect.width, rect.height);
    }
    
    enum class DisplayChangeSummary : std::uint32_t {
        BeginConfiguration      = (1 << 0),
        Moved                   = (1 << 1),
        SetMain                 = (1 << 2),
        SetMode                 = (1 << 3),
        Add                     = (1 << 4),
        Remove                  = (1 << 5),
        Enabled                 = (1 << 8),
        Disabled                = (1 << 9),
        Mirror                  = (1 << 10),
        UnMirror                = (1 << 11),
        DesktopShapeChanged     = (1 << 12),
    };
    
    std::uint32_t operator&(DisplayChangeSummary x, DisplayChangeSummary y);
    std::uint32_t operator&(std::uint32_t x, DisplayChangeSummary y);
    std::uint32_t operator&(DisplayChangeSummary x, std::uint32_t y);
    std::ostream &operator<<(std::ostream &os, DisplayChangeSummary summary);
    
    extern ofEvent<DisplayChangeSummary> displayChanged;
    struct DisplayChangeSummaryArgs : ofEventArgs {
        DisplayChangeSummary summary;
    };
    void startNotificationOnDisplaysChanged();
    void stopNotificationOnDisplaysChanged();
};

using ofxMacDisplayFixChangeSummary = ofxMacDisplayFix::DisplayChangeSummary;

#endif
