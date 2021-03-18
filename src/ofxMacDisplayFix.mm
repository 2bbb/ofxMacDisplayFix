//
//  ofxMacDisplayFix.m
//
//  Created by 2bit on 2021/03/18.
//

#import "ofxMacDisplayFix.h"

#include "ofLog.h"
#include "ofEvent.h"
#include "ofEventUtils.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <ApplicationServices/ApplicationServices.h>

namespace ofxMacDisplayFix {
    constexpr std::size_t max_display_num = 32;
    
    std::vector<std::uint32_t> getActiveDisplayIDs() {
        CGDirectDisplayID displayIds[max_display_num];
        uint32_t displayCount;
        CGGetActiveDisplayList(max_display_num, displayIds, &displayCount);
        std::vector<std::uint32_t> displays;
        for(auto i = 0; i < displayCount; ++i) {
            displays.push_back(displayIds[i]);
        }
        return displays;
    }
    
    std::vector<std::string> getActiveDisplayUUIDs() {
        auto &&displays = getActiveDisplayIDs();
        std::vector<std::string> uuids;
        for(auto display : displays) {
            uuids.emplace_back(getDisplayUUIDFromID(display));
        }
        return uuids;
    }
    
    std::vector<std::string> getDisplayUUIDsAt(float x, float y) {
        CGDirectDisplayID displays[max_display_num];
        uint32_t displayCount;
        CGPoint point;
        point.x = x;
        point.y = y;
        CGGetDisplaysWithPoint(point, max_display_num, displays, &displayCount);
        std::vector<std::string> uuids;
        for(std::size_t i = 0; i < displayCount; ++i) {
            uuids.emplace_back(getDisplayUUIDFromID(displays[i]));
        }
        return uuids;
    }
    
    std::string getDisplayUUIDFromID(std::uint32_t display_id) {
        CFUUIDRef displayUUID = CGDisplayCreateUUIDFromDisplayID(display_id);
        if(!displayUUID) return "INVALID DISPLAY ID";
        CFStringRef uuidString = CFUUIDCreateString(CFAllocatorGetDefault(), displayUUID);
        std::string uuid{((__bridge NSString *)uuidString).UTF8String};
        CFRelease(uuidString);
        CFRelease(displayUUID);
        return uuid;
    }
    
    std::uint32_t getDisplayIDFromUUID(std::string uuid_str) {
        NSString *uuid_ = [[NSString alloc] initWithUTF8String:uuid_str.c_str()];
        CFStringRef str_ref = (__bridge CFStringRef)uuid_;
        CFUUIDRef uuid = CFUUIDCreateFromString(NULL, str_ref);
        
        uint32_t displayID = CGDisplayGetDisplayIDFromUUID(uuid);
        
        CFRelease(uuid);
        [uuid_ release];
        
        return displayID;
    }
    
    ofRectangle getDisplayRectangleFromUUID(std::string uuid_str) {
        std::uint32_t displayID = getDisplayIDFromUUID(uuid_str);
        if(displayID == kCGNullDirectDisplay) {
            ofLogWarning() << "No display was found for " << uuid_str << ". will return main display rectangle";
            displayID = CGMainDisplayID();
        }
        CGRect rect = CGDisplayBounds(displayID);
        return ofRectangle(
            rect.origin.x,
            rect.origin.y,
            rect.size.width,
            rect.size.height
        );
    }
    
    DisplayMode convert(const CGDisplayModeRef ref) {
        DisplayMode mode;
        if(ref == nullptr) {
            mode.valid = false;
            return mode;
        }
        mode.valid = true;
        
        mode.size.width  = CGDisplayModeGetWidth(ref);
        mode.size.height = CGDisplayModeGetHeight(ref);
        
        mode.refreshRate = CGDisplayModeGetRefreshRate(ref);
        
        mode.io.flags = CGDisplayModeGetIOFlags(ref);
        mode.io.displayModelID = CGDisplayModeGetIODisplayModeID(ref);
        
        mode.isUsableForDesktopGUI = CGDisplayModeIsUsableForDesktopGUI(ref);
        mode.typeID = CGDisplayModeGetTypeID();
        return mode;
    }
    
    DisplayMode getCurrentDisplayMode(std::uint32_t displayID) {
        CGDisplayModeRef ref = CGDisplayCopyDisplayMode(displayID);
        DisplayMode &&mode = convert(ref);
        CGDisplayModeRelease(ref);
        return mode;
    }
    
    std::vector<DisplayMode> getAvailableDisplayModes(std::uint32_t displayID) {
        std::vector<DisplayMode> modes;
        CFArrayRef arr = CGDisplayCopyAllDisplayModes(displayID, nullptr);
        std::size_t num = CFArrayGetCount(arr);
        for(std::size_t i = 0; i < num; ++i) {
            const CGDisplayModeRef ref = (const CGDisplayModeRef)CFArrayGetValueAtIndex(arr, i);
            modes.emplace_back(convert(ref));
        }
        CFRelease(arr);
        return modes;
    }
    
    Detail getDisplayDetailFromID(std::uint32_t displayID) {
        auto uuid = getDisplayUUIDFromID(displayID);
        if(uuid == "INVLID DISPLAY ID") {
            Detail detail;
            detail.valid = false;
            detail.displayID = displayID;
            detail.uuid = uuid;
            return detail;
        }
        return getDisplayDetailFromUUID(uuid);
    }
    
    Detail getDisplayDetailFromUUID(std::string uuid_str) {
        auto display = getDisplayIDFromUUID(uuid_str);
        if(display == kCGNullDirectDisplay) {
            Detail detail;
            detail.valid = false;
            detail.displayID = 0;
            detail.uuid = uuid_str;
            return detail;
        }
        Detail detail;
        detail.valid = true;
        
        detail.displayID = display;
        detail.uuid = uuid_str;
        
        detail.isBuiltIn = CGDisplayIsBuiltin(display);
        
        auto &status = detail.status;
        status.isMain = CGDisplayIsMain(display);
        status.isOnline = CGDisplayIsOnline(display);
        status.isActive = CGDisplayIsActive(display);
        status.isAsleep = CGDisplayIsAsleep(display);
        
        auto &mirroring = detail.mirroring;
        mirroring.isAlwaysInMirrorSet = CGDisplayIsAlwaysInMirrorSet(display);
        mirroring.isInHardWareMirrorSet = CGDisplayIsInHWMirrorSet(display);
        mirroring.isInMirrorSet = CGDisplayIsInMirrorSet(display);
        mirroring.primaryDisplay = CGDisplayPrimaryDisplay(display);
        mirroring.mirrorsDisplay = CGDisplayMirrorsDisplay(display);
        
        detail.rect = getDisplayRectangleFromUUID(uuid_str);
        detail.rotation = CGDisplayRotation(display);
        
        auto physicalSize = CGDisplayScreenSize(display);
        detail.physicalSize.width = physicalSize.width;
        detail.physicalSize.height = physicalSize.height;
        
        auto &product = detail.product;
        product.modelNumber = CGDisplayModelNumber(display);
        product.serialNumber = CGDisplaySerialNumber(display);
        product.unitNumber = CGDisplayUnitNumber(display);
        product.vendorNumber = CGDisplayVendorNumber(display);
        
        detail.usesOpenGLAcceleration = CGDisplayUsesOpenGLAcceleration(display);
        return detail;
    }
    
    std::uint32_t operator&(DisplayChangeSummary x, DisplayChangeSummary y)
    { return static_cast<std::uint32_t>(x) & static_cast<std::uint32_t>(y); };
    std::uint32_t operator&(std::uint32_t x, DisplayChangeSummary y)
    { return static_cast<std::uint32_t>(x) & static_cast<std::uint32_t>(y); };
    std::uint32_t operator&(DisplayChangeSummary x, std::uint32_t y)
    { return static_cast<std::uint32_t>(x) & static_cast<std::uint32_t>(y); };

    std::ostream &operator<<(std::ostream &os, DisplayChangeSummary summary) {
#define PRINT(name) ("    " # name) << (summary & DisplayChangeSummary::name ? " true\n" : " false\n")
        return os
        << "{\n"
        << PRINT(BeginConfiguration)
        << PRINT(Moved)
        << PRINT(BeginConfiguration)
        << PRINT(SetMain)
        << PRINT(SetMode)
        << PRINT(Add)
        << PRINT(Remove)
        << PRINT(Enabled)
        << PRINT(Disabled)
        << PRINT(Mirror)
        << PRINT(UnMirror)
        << PRINT(DesktopShapeChanged)
        << "}";
#undef PRINT
    }

    ofEvent<DisplayChangeSummary> displayChanged;
    
    void ReconfigurationCallback(CGDirectDisplayID display,
                                 CGDisplayChangeSummaryFlags flags,
                                 void *userInfo)
    {
        auto f = static_cast<DisplayChangeSummary>(flags);
        ofNotifyEvent(displayChanged, f);
    }
    
    void startNotificationOnDisplaysChanged() {
        CGError status = CGDisplayRegisterReconfigurationCallback(ReconfigurationCallback, nullptr);
        if(status != kCGErrorSuccess) {
            ofLogError() << "error " << status;
        }
    }
    
    void stopNotificationOnDisplaysChanged() {
        CGError status = CGDisplayRemoveReconfigurationCallback(ReconfigurationCallback, nullptr);
        if(status != kCGErrorSuccess) {
            ofLogError() << "error " << status;
        }
    }
};
