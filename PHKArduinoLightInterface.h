//
//  PHKArduinoLightInterface.h
//  Workbench
//
//  Created by Wai Man Chan on 11/3/14.
//
//

#ifndef __Workbench__PHKArduinoLightInterface__
#define __Workbench__PHKArduinoLightInterface__

#include <stdio.h>

void setupPort();
void startIdentify();
void setLightStrength(int strengthLevel);
inline void turnOnLight() { setLightStrength(255); }
inline void turnOffLight() { setLightStrength(0); }

#endif /* defined(__Workbench__PHKArduinoLightInterface__) */
