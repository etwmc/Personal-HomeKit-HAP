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
//You need to implement this to use Fan+Light configuration
void setFanSpeed(int strengthLevel);

#endif /* defined(__Workbench__PHKArduinoLightInterface__) */
