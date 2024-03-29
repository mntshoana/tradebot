#include "objectivec.h"
#include <AppKit/AppKit.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <mach-o/dyld.h> 

/*Function Checking if MacOS 10.14 or higher apps are currently in dark mode*/
bool isDarkMode ()
{
#ifdef TARGET_OS_MAC // Won't work for 10.13 or lower (need to fix)
    NSString* appearance = [NSApp.effectiveAppearance bestMatchFromAppearancesWithNames:
                    @[ NSAppearanceNameAqua, NSAppearanceNameDarkAqua ]];
    return [appearance isEqualToString:NSAppearanceNameDarkAqua];
#else
    return false;
#endif
}
std::string absolutePath (){
    char path[1024];
#ifdef TARGET_OS_MAC
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0){ // Successfully copied
        // return the path to the directory that leads to "[only this].../tradebot.app"
        char* ptr = strstr(path, "tradebot.app");
        return std::string (path, strlen(path) - strlen(ptr));
    }
    else
        //Buffer was too small;
        return "";
    
#else
    return "";
#endif
}

/*Detect if MacOS system is configured to with any proxies */
std::string proxyInfo (){
    std::string f;
#ifdef TARGET_OS_MAC
    CFDictionaryRef proxies = SCDynamicStoreCopyProxies(NULL);
    if (proxies) {
        //NSDictionary *dict = [NSDictionary dictionaryWithDictionary:(NSDictionary*)proxies];
        if ([(NSDictionary*)proxies objectForKey:@"HTTPProxy"]){
            f = [[(NSDictionary*)proxies objectForKey:@"HTTPProxy"] UTF8String];
            f += ":";
            f += std::to_string([[(NSDictionary*)proxies objectForKey:@"HTTPPort"] intValue]);
        }
        CFRelease(proxies);
    }
    return f;
#else
    return f;
#endif
}
