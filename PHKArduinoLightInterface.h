#pragma once
//
//  PHKArduinoLightInterface.h
//  Workbench
//
//  Created by Wai Man Chan on 11/3/14.
//
//

#include <stdio.h>

void setupPort();
void startIdentify();
void setLightStrength(int strengthLevel);
//You need to implement this to use Fan+Light configuration
void setFanSpeed(int strengthLevel);
