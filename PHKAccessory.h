//
//  PHKAccessory.h
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#ifndef __Workbench__PHKAccessory__
#define __Workbench__PHKAccessory__

#include <stdio.h>
extern const char *deviceName;
extern const char *deviceIdentity;
extern const char *password;

void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen);

#endif /* defined(__Workbench__PHKAccessory__) */
