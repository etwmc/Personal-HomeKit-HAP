//
//  PHKAccessory.c
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#include "PHKAccessory.h"
#include <string.h>
#include <strings.h>

#include "PHKNetworkIP.h"

#include "PHKControllerRecord.h"

const char *deviceName = "Night Light";
const char *deviceIdentity = "12:00:54:23:51:13";
const char *manufacture = "ET Chan";
const char *password = "523-12-643";
const char *deviceUUID = "9FCF7180-6CAA-4174-ABC0-E3FAE58E3ADD";

const char hapJsonType[] = "application/hap+json";
const char pairingTlv8Type[] = "application/pairing+tlv8";

bool lightOn = false;

using namespace std;

typedef enum:int {
    charType_adminOnlyAccess    = 0x1,
    charType_audioFeedback      = 0x5,
    charType_brightness         = 0x8,
    charType_coolingThreshold   = 0xD,
    charType_currentDoorState   = 0xE,
    charType_currentHeatCoolMode= 0xF,
    charType_currentHumidity    = 0x10,
    charType_currentTemperature = 0x11,
    charType_heatingThreshold   = 0x12,
    charType_hue                = 0x13,
    charType_identify           = 0x14,
    charType_lockControlPoint   = 0x19,
    charType_lockAutoTimeout    = 0x1A,
    charType_lockLastAction     = 0x1C,
    charType_lockCurrentState   = 0x1D,
    charType_lockTargetState    = 0x1E,
    charType_logs               = 0x1F,
    charType_manufactuer        = 0x20,
    charType_modelName          = 0x21,
    charType_motionDetect       = 0x22,
    charType_serviceName        = 0x23,
    charType_obstruction        = 0x24,
    charType_on                 = 0x25,
    charType_outletUse          = 0x26,
    charType_saturation         = 0x2F,
    charType_serialNumber       = 0x30,
    charType_targetDoorState    = 0x32,
    charType_targetHeatCoolMode = 0x33,
    charType_targetHumidity     = 0x34,
    charType_targetTemperature  = 0x35,
    charType_temperatureUnit    = 0x36,
    charType_version            = 0x37,
#pragma - The following is service provide
    charType_accessoryInfo      = 0x3E,
    charType_fan                = 0x3F,
    charType_garageDoorOpener   = 0x41,
    charType_lightBulb          = 0x43,
    charType_lockMechanism      = 0x45,
    charType_outlet             = 0x47,
    charType_switch             = 0x49,
    charType_thermostat         = 0x4A
} charType;

enum :int {
    premission_read = 1,
    premission_write = 1 << 1,
    premission_update = 1 << 2
};

typedef enum {
    unit_none = 0,
    unit_celsius,
    unit_percentage,
    unit_arcDegree
} unit;

//Wrap to JSON
string wrap(const char *str) { return (string)"\""+str+"\""; }
string attribute(unsigned short type, unsigned short acclaim, int p, bool value, unit valueUnit) {
    string result;
    
    result += wrap("value")+":";
    if (value) result += "true";
    else result += "false";
    result += ",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result.pop_back();
    result += "]";
    result += ",";
    
    char tempStr[4];
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    switch (valueUnit) {
        case unit_arcDegree:
            result += wrap("unit")+":"+wrap("arcdegrees");
            break;
        case unit_celsius:
            result += wrap("unit")+":"+wrap("celsius");
            break;
        case unit_percentage:
            result += wrap("unit")+":"+wrap("percentage");
            break;
    }
    
    result += "\"format\":\"bool\"";
    
    return "{"+result+"}";
}
string attribute(unsigned short type, unsigned short acclaim, int p, int value, int minVal, int maxVal, int step, unit valueUnit) {
    string result;
    char tempStr[4];
    
    snprintf(tempStr, 4, "%d", value);
    
    result += wrap("value")+":"+tempStr;
    result += ",";
    
    snprintf(tempStr, 4, "%d", minVal);
    if (minVal != INT32_MIN)
        result += wrap("minValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%d", maxVal);
    if (maxVal != INT32_MAX)
        result += wrap("maxValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%d", step);
    if (step > 0)
        result += wrap("minStep")+":"+tempStr+",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result.pop_back();
    result += "]";
    result += ",";
    
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    switch (valueUnit) {
        case unit_arcDegree:
            result += wrap("unit")+":"+wrap("arcdegrees")+",";
            break;
        case unit_celsius:
            result += wrap("unit")+":"+wrap("celsius")+",";
            break;
        case unit_percentage:
            result += wrap("unit")+":"+wrap("percentage")+",";
            break;
    }
    
    result += "\"format\":\"int\"";
    
    return "{"+result+"}";
}
string attribute(unsigned short type, unsigned short acclaim, int p, float value, float minVal, float maxVal, float step, unit valueUnit) {
    string result;
    char tempStr[4];
    
    snprintf(tempStr, 4, "%f", value);
    
    result += wrap("value")+":"+tempStr;
    result += ",";
    
    snprintf(tempStr, 4, "%f", minVal);
    if (minVal != INT32_MIN)
        result += wrap("minValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%f", maxVal);
    if (maxVal != INT32_MAX)
        result += wrap("maxValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%f", step);
    if (step > 0)
        result += wrap("minStep")+":"+tempStr+",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result.pop_back();
    result += "]";
    result += ",";
    
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    switch (valueUnit) {
        case unit_arcDegree:
            result += wrap("unit")+":"+wrap("arcdegrees")+",";
            break;
        case unit_celsius:
            result += wrap("unit")+":"+wrap("celsius")+",";
            break;
        case unit_percentage:
            result += wrap("unit")+":"+wrap("percentage")+",";
            break;
    }
    
    result += "\"format\":\"float\"";
    
    return "{"+result+"}";
}
string attribute(unsigned short type, unsigned short acclaim, int p, string value, unsigned short len) {
    string result;
    char tempStr[4];
    
    result += wrap("value")+":"+wrap(value.c_str());
    result += ",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result.pop_back();
    result += "]";
    result += ",";
    
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    if (len > 0) {
        snprintf(tempStr, 4, "%hd", len);
        result += wrap("maxLen")+":"+tempStr;
        result += ",";
    }
    
    result += "\"format\":\"string\"";
    
    return "{"+result+"}";
}
inline string arrayWrap(string *s, unsigned short len) {
    string result;
    
    result += "[";
    
    for (int i = 0; i < len; i++) {
        result += s[i]+",";
    }
    result.pop_back();
    
    result += "]";
    
    return result;
}
inline string dictionaryWrap(string *key, string *value, unsigned short len) {
    string result;
    
    result += "{";
    
    for (int i = 0; i < len; i++) {
        result += wrap(key[i].c_str())+":"+value[i]+",";
    }
    result.pop_back();
    
    result += "}";
    
    return result;
}

void getAccessoryInfo(char **reply, unsigned short *replyLen) {
    int numberOfAccessory = 0;
    
    //Map everything in JSON
    int attID = 1;
    
    string result;
    
    string serviceString[2];
    {
        string key[3];
        string value[3];
        
        char temp[8];
        snprintf(temp, 8, "\"%X\"", charType_accessoryInfo);
        
        value[0] = temp;
        key[0] = "type";
        
        snprintf(temp, 8, "%d", attID++);
        value[1] = temp;
        key[1] = "iid";
        
        {
            string info[5];
            info[0] = attribute(charType_serviceName, attID++, premission_read|premission_update, deviceName, 0);
            info[1] = attribute(charType_manufactuer, attID++, premission_read, manufacture, 0);
            info[2] = attribute(charType_modelName, attID++, premission_read, deviceName, 0);
            info[3] = attribute(charType_serialNumber, attID++, premission_read, deviceUUID, 0);
            info[4] = attribute(charType_identify, attID++, premission_write, false, unit_none);
            
            value[2] = arrayWrap(info, 5);
            key[2] = "characteristics";
        }
        
        serviceString[0] = dictionaryWrap(key, value, 3);
    }
    {
        string key[3];
        string value[3];
        
        char temp[8];
        snprintf(temp, 8, "\"%X\"", charType_lightBulb);
        
        value[0] = temp;
        key[0] = "type";
        
        snprintf(temp, 8, "%d", attID++);
        value[1] = temp;
        key[1] = "iid";
        
        {
            string setting[2];
            setting[0] = attribute(charType_on, attID++, premission_read|premission_update|premission_write, lightOn, unit_none);
            setting[1] = attribute(charType_serviceName, attID++, premission_read|premission_update, deviceName, 0);
            
            value[2] = arrayWrap(setting, 2);
            key[2] = "characteristics";
        }
        
        serviceString[1] = dictionaryWrap(key, value, 3);
    }
    
    result = arrayWrap(serviceString, 2);
    result = "{\"accessories\":[{\"aid\":1,\"services\":"+result+"}]}";
    
    *replyLen = result.length();
    *reply  = new char[*replyLen+1];
    strcpy(*reply, result.c_str());
}

void updatePowerState(const char *requestData, char **reply, unsigned short *replyLen) {
    char value[6];
    sscanf(requestData, "{\"characteristics\":[{\"aid\":1,\"iid\":8,\"value\":%s}]}", value);
    if (strcmp(value, "true")) {
        lightOn = true;
    } else {
        lightOn = false;
    }
    
    int attID = 1;
    string cid;
    {
        string value[5];
        string key[5];
        char temp[8];
        snprintf(temp, 8, "\"%X\"", charType_lightBulb);
        
        value[0] = temp;
        key[0] = "type";
        
        snprintf(temp, 8, "%d", attID++);
        value[1] = temp;
        key[1] = "iid";
        
        value[2] = "0";
        key[2] = "errorCode";
        
        value[3] = "true";
        key[3] = "value";
        
        value[4] = "{}";
        key[4] = "response";
        
        cid = dictionaryWrap(key, value, 5);
    }
    
    
    *replyLen = cid.length();
    *reply = new char[*replyLen];
    bcopy(cid.c_str(), *reply, *replyLen);
    
    printf("%s\n", *reply);
}

void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen) {
    int index = 5;
    char method[5];
    {
        //Read method
        method[4] = 0;
        bcopy(request, method, 4);
        if (method[3] == ' ') {
            method[3] = 0;
            index = 4;
        }
    }
    
    char path[32];
    int i;
    for (i = 0; i < 32 && request[index] != ' '; i++, index++) {
        path[i] = request[index];
    }
    path[i] = 0;
    
    const char *dataPtr = request;
    while (true) {
        dataPtr++;
        if (dataPtr[0] == '\r' && dataPtr[1] == '\n' && dataPtr[2] == '\r' && dataPtr[3] == '\n') break;
    }
    
    char *replyData = nullptr;  unsigned short replyDataLen = 0;
    
    int statusCode;
    
    printf("%s\n", request);
    
    const char *returnType = hapJsonType;
    
    if (strcmp(path, "/accessories") == 0) {
        //Publish the characterists of the accessories
        statusCode = 200;
        getAccessoryInfo(&replyData, &replyDataLen);
    } else if (strcmp(path, "/pairings") == 0) {
        //Pairing with new user
        statusCode = 200;
        PHKNetworkMessage msg = PHKNetworkMessage(request);
        if (*msg.data.dataPtrForIndex(0) == 3) {
            PHKKeyRecord controllerRec;
            bcopy(msg.data.dataPtrForIndex(3), controllerRec.publicKey, 32);
            bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
            addControllerKey(controllerRec);
            PHKNetworkMessageDataRecord drec;
            drec.activate = true; drec.data = new char[1]; *drec.data = 1;
            drec.index = 7; drec.length = 1;
            PHKNetworkMessageData data;
            data.addRecord(drec);
            data.rawData((const char **)&replyData, &replyDataLen);
            returnType = pairingTlv8Type;
            statusCode = 200;
        }
    } else if (strcmp(path, "/characteristics") == 0){
        //Change characteristics
        updatePowerState(dataPtr, &replyData, &replyDataLen);
        statusCode = 207;
    } else {
        //Error
        printf("%s", path);
        statusCode = 404;
    }
    
    *reply = new char[1024];
    int len = snprintf(*reply, 1024, "HTTP/1.1 %d OK\r\n\
Content-Type: %s\r\n\
Content-Length: %u\r\n\r\n", statusCode, returnType, replyDataLen);
    
    bcopy(replyData, &(*reply)[len], replyDataLen);
    *replyLen = len+replyDataLen;
    
    if (replyData) delete [] replyData;
}