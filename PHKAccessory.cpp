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

#include "Configuration.h"

const char hapJsonType[] = "application/hap+json";
const char pairingTlv8Type[] = "application/pairing+tlv8";

bool lightOn = false;

using namespace std;

typedef enum {
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

enum {
    premission_read = 1,
    premission_write = 1 << 1,
    premission_update = 1 << 2  //Update = Accessory will notice the controller
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

typedef struct {
    
} dictionary;

void getAccessoryInfo(char **reply, unsigned short *replyLen) {
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
            info[0] = attribute(charType_serviceName, attID++, premission_read, deviceName, 0);
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
            setting[0] = attribute(charType_on, attID++, premission_read|premission_write, lightOn, unit_none);
            setting[1] = attribute(charType_serviceName, attID++, premission_read, deviceName, 0);
            
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
    char value[6];  bzero(value, 6);
    int aid = 0;    int iid = 0;
    sscanf(requestData, "{\"characteristics\":[{\"aid\":%d,\"iid\":%d,\"value\":%s}]}", &aid, &iid, value);
    
    printf("Receive value update for accessory %d, characteristic %d, to value %s\n", aid, iid, value);
    
    if (strncmp(value, "true", 4) == 0) {
        lightOn = true;
    } else if (strncmp(value, "false", 5) == 0) {
        lightOn = false;
    }
    
    string cid;
    {
        string value[3];
        string key[3];
        char temp[8];
        snprintf(temp, 8, "\"%X\"", charType_lightBulb);
        
        value[0] = "1";
        key[0] = "aid"; 
        
        snprintf(temp, 8, "%d", 8);
        value[1] = temp;
        key[1] = "iid";
        
        if (lightOn) {
            value[2] = "true";
        } else
            value[2] = "false";
        key[2] = "value";
        
        cid = dictionaryWrap(key, value, 3);
    }
    cid = "{\"characteristics\":["+cid+"]}";
    
    
    *replyLen = cid.length();
    *reply = new char[*replyLen+1];
    bzero(*reply, *replyLen+1);
    bcopy(cid.c_str(), *reply, *replyLen);
    
    *reply = nullptr;
    *replyLen = 0;
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
        dataPtr = &dataPtr[1];
        if (dataPtr[0] == '\r' && dataPtr[1] == '\n' && dataPtr[2] == '\r' && dataPtr[3] == '\n') break;
    }
    
    dataPtr += 4;
    
    char *replyData = NULL;  unsigned short replyDataLen = 0;
    
    int statusCode;
    
    const char *protocol = "HTTP/1.1";
    const char *returnType = hapJsonType;
    
    if (strcmp(path, "/accessories") == 0) {
        //Publish the characterists of the accessories
        statusCode = 200;
        getAccessoryInfo(&replyData, &replyDataLen);
    } else if (strcmp(path, "/pairings") == 0) {
        //Pairing with new user
        PHKNetworkMessage msg(request);
        statusCode = 200;
        if (*msg.data.dataPtrForIndex(0) == 3) {
            PHKKeyRecord controllerRec;
            bcopy(msg.data.dataPtrForIndex(3), controllerRec.publicKey, 32);
            bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
            addControllerKey(controllerRec);
            PHKNetworkMessageDataRecord drec;
            drec.activate = true; drec.data = new char[1]; *drec.data = 2;
            drec.index = 6; drec.length = 1;
            PHKNetworkMessageData data;
            data.rawData((const char **)&replyData, &replyDataLen);
            returnType = pairingTlv8Type;
            statusCode = 200;
        }
    } else if (strcmp(path, "/characteristics") == 0){
        if (strncmp(method, "GET", 3) == 0) {
            //Read characteristics
        } else if (strncmp(method, "PUT", 3) == 0) {
            //Change characteristics
            //protocol = "EVENT/1.0";
            updatePowerState(dataPtr, &replyData, &replyDataLen);
            statusCode = 204;
        } else {
            return;
        }
    } else {
        //Error
        printf("%s\n", request);
        printf("%s", path);
        statusCode = 404;
    }
    
    *reply = new char[1024];
    int len = snprintf(*reply, 1024, "%s %d OK\r\n\
Content-Type: %s\r\n\
Content-Length: %u\r\n\r\n", protocol, statusCode, returnType, replyDataLen);
    
    bcopy(replyData, &(*reply)[len], replyDataLen);
    *replyLen = len+replyDataLen;
    
    if (replyData) delete [] replyData;
    
}