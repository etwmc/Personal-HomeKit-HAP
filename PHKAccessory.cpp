//
//  PHKAccessory.c
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#include "PHKAccessory.h"


const char hapJsonType[] = "application/hap+json";
const char pairingTlv8Type[] = "application/pairing+tlv8";

//The default logger is printf
//Personal_homekit_set_logger_function can be changed.
PERSONAL_LOGGER_FUNCTION g_homekit_logger = printf;


//Wrap to JSON
inline string wrap(const char *str) { return (string)"\""+str+"\""; }
inline string attribute(unsigned short type, unsigned short acclaim, int p, bool value) {
    string result;
    if (p & premission_read) {
        result += wrap("value")+":";
        if (value) result += "true";
        else result += "false";
        result += ",";
    }
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_notify) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    char tempStr[9];
    snprintf(tempStr, 9, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 9, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    result += "\"format\":\"bool\"";
    
    return "{"+result+"}";
}
inline string attribute(unsigned short type, unsigned short acclaim, int p, int value, int minVal, int maxVal, int step, unit valueUnit,const std::string& format ) {
    string result;
    char tempStr[9];
    
    snprintf(tempStr, 9, "%d", value);
    
    if (p & premission_read) {
        result += wrap("value")+":"+tempStr;
        result += ",";
    }
    
    snprintf(tempStr, 9, "%d", minVal);
    if (minVal != INT32_MIN)
        result += wrap("minValue")+":"+tempStr+",";
    
    snprintf(tempStr, 9, "%d", maxVal);
    if (maxVal != INT32_MAX)
        result += wrap("maxValue")+":"+tempStr+",";
    
    snprintf(tempStr, 9, "%d", step);
    if (step > 0)
        result += wrap("minStep")+":"+tempStr+",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_notify) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    snprintf(tempStr, 9, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 9, "%hd", acclaim);
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
    
    result += "\"format\":\"" + format + "\"";
    
    return "{"+result+"}";
}
inline string attribute(unsigned short type, unsigned short acclaim, int p, float value, float minVal, float maxVal, float step, unit valueUnit) {
    string result;
    char tempStr[9];
    
    snprintf(tempStr, 9, "%f", value);
    
    if (p & premission_read) {
        result += wrap("value")+":"+tempStr;
        result += ",";
    }
    
    snprintf(tempStr, 9, "%f", minVal);
    if (minVal != INT32_MIN)
        result += wrap("minValue")+":"+tempStr+",";
    
    snprintf(tempStr, 9, "%f", maxVal);
    if (maxVal != INT32_MAX)
        result += wrap("maxValue")+":"+tempStr+",";
    
    snprintf(tempStr, 9, "%f", step);
    if (step > 0)
        result += wrap("minStep")+":"+tempStr+",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_notify) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    snprintf(tempStr, 9, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 9, "%hd", acclaim);
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
inline string attribute(unsigned short type, unsigned short acclaim, int p, string value, unsigned short len) {
    string result;
    char tempStr[9];
    
    if (p & premission_read) {
        result += wrap("value")+":"+wrap(value.c_str());
        result += ",";
    }
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_notify) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    snprintf(tempStr, 9, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 9, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    if (len > 0) {
        snprintf(tempStr, 9, "%hd", len);
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
    result = result.substr(0, result.size()-1);
    
    result += "]";
    
    return result;
}
inline string dictionaryWrap(string *key, string *value, unsigned short len) {
    string result;
    
    result += "{";
    
    for (int i = 0; i < len; i++) {
        result += wrap(key[i].c_str())+":"+value[i]+",";
    }
    result = result.substr(0, result.size()-1);
    
    result += "}";
    
    return result;
}


string boolCharacteristics::describe() {
    return attribute(type, iid, premission, _value);
}

string floatCharacteristics::describe() {
    return attribute(type, iid, premission, _value, _minVal, _maxVal, _step, _unit);
}

string intCharacteristics::describe() {
    return attribute(type, iid, premission, _value, _minVal, _maxVal, _step, _unit, "int");
}

string uint8Characteristics::describe() {
    return attribute(type, iid, premission, _value, _minVal, _maxVal, _step, _unit, "uint8");
}

string stringCharacteristics::describe() {
    return attribute(type, iid, premission, _value, maxLen);
}

string Service::describe() {
    string keys[3] = {"iid", "type", "characteristics"};
    string values[3];
    {
        char temp[9];
        snprintf(temp, 9, "%d", serviceID);
        values[0] = temp;
    }
    {
        char temp[9];
        snprintf(temp, 9, "\"%X\"", uuid);
        values[1] = temp;
    }
    {
        int no = numberOfCharacteristics();
        string *chars = new string[no];
        for (int i = 0; i < no; i++) {
            chars[i] = _characteristics[i]->describe();
        }
        values[2] = arrayWrap(chars, no);
        delete [] chars;
    }
    return dictionaryWrap(keys, values, 3);
}

string Accessory::describe() {
    string keys[2];
    string values[2];
    
    {
        keys[0] = "aid";
        char temp[9];
        snprintf(temp,9, "%d", aid);
        values[0] = temp;
    }
    
    {
        //Form services list
        int noOfService = numberOfService();
        string *services = new string[noOfService];
        for (int i = 0; i < noOfService; i++) {
            services[i] = _services[i]->describe();
        }
        keys[1] = "services";
        values[1] = arrayWrap(services, noOfService);
        delete [] services;
    }
    
    string result = dictionaryWrap(keys, values, 2);
    return result;
}



void AccessorySet::lock()
{
    pthread_mutex_lock(&this->accessoryMutex);
}

void AccessorySet::unlock()
{
    pthread_mutex_unlock(&this->accessoryMutex);
}


string AccessorySet::describe() {
    int numberOfAcc = numberOfAccessory();
	vector<string> desc_vec(numberOfAcc);
    string *desc = &desc_vec[0];
    for (int i = 0; i < numberOfAcc; i++) {
        desc[i] = _accessories[i]->describe();
    }
    string result = arrayWrap(desc, numberOfAcc);
    string key = "accessories";
    result = dictionaryWrap(&key, &result, 1);
    return result;
}



struct broadcastInfo {
    pthread_t thread;
    void *sender;
    char *desc;
};

void *announce(void *info) {
    broadcastInfo *_info = (broadcastInfo *)info;
    void *sender = _info->sender;
    char *desc = _info->desc;
    
    char *reply = new char[1024];
    int len = snprintf(reply, 1024, "EVENT/1.0 200 OK\r\nContent-Type: application/hap+json\r\nContent-Length: %lu\r\n\r\n%s", strlen(desc), desc);
    
#if HomeKitLog == 1 && HomeKitReplyHeaderLog==1
    g_homekit_logger("%s\n", reply);
#endif
    pthread_t currentthread = _info->thread;
    
    broadcastMessage(sender, reply, len);
    delete [] reply;
    
    delete [] desc;
    delete [] info;

    pthread_detach(currentthread);
	return NULL;
}

void updateValueFromDeviceEnd(characteristics *c, int aid, int iid, string value) {
    c->setValue(value);
    char *broadcastTemp = new char[1024];
    snprintf(broadcastTemp, 1024, "{\"characteristics\":[{\"aid\":%d,\"iid\":%d,\"value\":%s}]}", aid, iid, value.c_str());
    broadcastInfo * info = new broadcastInfo;
    info->sender = c;
    info->desc = broadcastTemp;
    pthread_create(&info->thread, NULL, announce, info);
    
}

void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen, connectionInfo *sender) {
#if HomeKitLog == 1
    g_homekit_logger("Receive request: %s\n", request);
#endif
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
    
    char path[1024];
    int i;
    for (i = 0; i < 1024 && request[index] != ' '; i++, index++) {
        path[i] = request[index];
    }
    path[i] = 0;
#if HomeKitLog == 1
    g_homekit_logger("Path: %s\n", path);
#endif
    
    const char *dataPtr = request;
    while (true) {
        dataPtr = &dataPtr[1];
        if (dataPtr[0] == '\r' && dataPtr[1] == '\n' && dataPtr[2] == '\r' && dataPtr[3] == '\n') break;
    }
    
    dataPtr += 4;
    
    char *replyData = NULL;  unsigned short replyDataLen = 0;
    
    int statusCode = 0;
    
    const char *protocol = "HTTP/1.1";
    const char *returnType = hapJsonType;
    
    if (strcmp(path, "/accessories") == 0) {
        //Publish the characterists of the accessories
#if HomeKitLog == 1
        g_homekit_logger("Ask for accessories info\n");
#endif
        statusCode = 200;
        string desc = AccessorySet::getInstance().describe();
        replyDataLen = desc.length();
        replyData = new char[replyDataLen+1];
        bcopy(desc.c_str(), replyData, replyDataLen);
        replyData[replyDataLen] = 0;
    } else if (strcmp(path, "/pairings") == 0) {
        PHKNetworkMessage msg(request);
        statusCode = 200;
#if HomeKitLog == 1
        g_homekit_logger("%d\n", *msg.data.dataPtrForIndex(0));
#endif
		if (*msg.data.dataPtrForIndex(0) == 3) {
            //Pairing with new user
#if HomeKitLog == 1
            g_homekit_logger("Add new user\n");
#endif
            PHKKeyRecord controllerRec;
            bcopy(msg.data.dataPtrForIndex(3), controllerRec.publicKey, 32);
            bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
			AccessorySet::getInstance().Controllers.addControllerKey(controllerRec);
            PHKNetworkMessageDataRecord drec;
            drec.activate = true; drec.data = new char[1]; *drec.data = 2;
            drec.index = 6; drec.length = 1;
            PHKNetworkMessageData data;
            data.addRecord(drec);
            data.rawData((const char **)&replyData, &replyDataLen);
            returnType = pairingTlv8Type;
            statusCode = 200;
        } else {
#if HomeKitLog == 1
            g_homekit_logger("Delete user");
#endif
            PHKKeyRecord controllerRec;
            bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
			AccessorySet::getInstance().Controllers.removeControllerKey(controllerRec);
            PHKNetworkMessageDataRecord drec;
            drec.activate = true; drec.data = new char[1]; *drec.data = 2;
            drec.index = 6; drec.length = 1;
            PHKNetworkMessageData data;
            data.addRecord(drec);
            data.rawData((const char **)&replyData, &replyDataLen);
            returnType = pairingTlv8Type;
            statusCode = 200;
        }
    } else if (strncmp(path, "/characteristics", 16) == 0){
		AccessorySetAutoLock autolock;
#if HomeKitLog == 1
        g_homekit_logger("Characteristics\n");
#endif
		if (strncmp(method, "GET", 3) == 0) {
            //Read characteristics
            int aid = 0;    int iid = 0;
            
			char indexBuffer[1000+1]={0};
            sscanf(path, "/characteristics?id=%1000[^\n]", indexBuffer);
#if HomeKitLog == 1
            g_homekit_logger("Get characteristics %s with len %d\n", indexBuffer, strlen(indexBuffer));
#endif            
            statusCode = 404;
            
            string result = "[";
            
            while (strlen(indexBuffer) > 0) {
                
#if HomeKitLog == 1
                g_homekit_logger("Get characteristics %s with len %d\n", indexBuffer, strlen(indexBuffer));
#endif                
				char temp[1000+1]={0};
                //Initial the temp
                sscanf(indexBuffer, "%d.%d%1000[^\n]", &aid, &iid, temp);
#if HomeKitLog == 1
                g_homekit_logger("Get temp %s with len %d\n", temp, strlen(temp));
#endif
				strncpy(indexBuffer, temp, 1000);
#if HomeKitLog == 1
                g_homekit_logger("Get characteristics %s with len %d\n", indexBuffer, strlen(indexBuffer));
#endif
				//Trim comma
                if (indexBuffer[0] == ',') {
                    indexBuffer[0] = '0';
                }
                
                Accessory *a = AccessorySet::getInstance().accessoryAtIndex(aid);
                if (a != NULL) {
                    characteristics *c = a->characteristicsAtIndex(iid);
                    if (c != NULL) {
#if HomeKitLog == 1
                        g_homekit_logger("Ask for one characteristics: %d . %d\n", aid, iid);
#endif
                        char c1[9], c2[9];
                        snprintf(c1,9, "%d", aid);
                        snprintf(c2,9, "%d", iid);
                        string s[3] = {string(c1), string(c2), c->value()};
                        string k[3] = {"aid", "iid", "value"};
                        if (result.length() != 1) {
                            result += ",";
                        }
                        
                        string _result = dictionaryWrap(k, s, 3);
                        result += _result;
                        
                    }
                    
                }
            }
            
            result += "]";
            
            string d = "characteristics";
            result = dictionaryWrap(&d, &result, 1);
            
            replyDataLen = result.length();
            replyData = new char[replyDataLen+1];
            replyData[replyDataLen] = 0;
            bcopy(result.c_str(), replyData, replyDataLen);
            statusCode = 200;
            
        } else if (strncmp(method, "PUT", 3) == 0) {
            //Change characteristics
            
			char characteristicsBuffer[1000+1]={0};
            sscanf(dataPtr, "{\"characteristics\":[{%1000[^]]}", characteristicsBuffer);
            
            char *buffer2 = characteristicsBuffer;
            while (strlen(buffer2) && statusCode != 400) {
                bool reachLast = false; bool updateNotify = false;
                char *buffer1;
                buffer1 = strtok_r(buffer2, "}", &buffer2);
                if (*buffer2 != 0) buffer2+=2;
                
				int aid = 0;    int iid = 0; char value[16+1]={0};
                int result = sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"value\":%16s", &aid, &iid, value);
                if (result == 2) {
                    sscanf(buffer1, "\"aid\":%d,\"iid\":%d,\"ev\":%16s", &aid, &iid, value);
                    updateNotify = true;
                } else if (result == 0) {
                    sscanf(buffer1, "\"remote\":true,\"value\":%16[^,],\"aid\":%d,\"iid\":%d", value, &aid, &iid);
                    if (result == 2) {
                        sscanf(buffer1, "\"remote\":true,\"aid\":%d,\"iid\":%d,\"ev\":%16s", &aid, &iid, value);
                        updateNotify = true;
                    }
                }
                
                Accessory *a = AccessorySet::getInstance().accessoryAtIndex(aid);
                if (a==NULL) {
                    statusCode = 400;
                } else {
                    characteristics *c = a->characteristicsAtIndex(iid);
                    
                    if (updateNotify) {
#if HomeKitLog == 1
                        g_homekit_logger("Ask to notify one characteristics: %d . %d -> %s\n", aid, iid, value);
#endif
                        if (c==NULL) {
                            statusCode = 400;
                        } else {
                            if (c->notifiable()) {
                                sender->addNotify(c);
                                
                                statusCode = 204;
                            } else {
                                statusCode = 400;
                            }
                        }
                    } else {
#if HomeKitLog == 1
                        g_homekit_logger("Ask to change one characteristics: %d . %d -> %s\n", aid, iid, value);
#endif
                        if (c==NULL) {
                            statusCode = 400;
                        } else {
                            if (c->writable()) {
                                c->setValue(value);
                                
                                char *broadcastTemp = new char[1024];
                                snprintf(broadcastTemp, 1024, "{\"characteristics\":[{%s}]}", buffer1);
                                broadcastInfo * info = new broadcastInfo;
                                info->sender = c;
                                info->desc = broadcastTemp;
                                pthread_create(&info->thread, NULL, announce, info);
                                
                                statusCode = 204;
                                
                            } else {
                                statusCode = 400;
                            }
                        }
                    }
                    
                }
                
            }
            
        } else {
            return;
        }
    } else {
        //Error
#if HomeKitLog == 1
        g_homekit_logger("Ask for something I don't know\n");
        g_homekit_logger("%s\n", request);
        g_homekit_logger("%s", path);
#endif
        statusCode = 404;
    }
    
    //Calculate the length of header
	int len = 0;
	{
	    char tmp[256];
		len = snprintf(tmp, 256, "%s %d OK\r\nContent-Type: %s\r\nContent-Length: %u\r\n\r\n", protocol, statusCode, returnType, replyDataLen);
	}

    //replyLen should omit the '\0'.
    (*replyLen) = len+replyDataLen;
    //reply should add '\0', or the printf is incorrect
    *reply = new char[*replyLen + 1];
    bzero(*reply, *replyLen + 1);
    snprintf(*reply, len + 1, "%s %d OK\r\nContent-Type: %s\r\nContent-Length: %u\r\n\r\n", protocol, statusCode, returnType, replyDataLen);
    
    if (replyData) {
        bcopy(replyData, &(*reply)[len], replyDataLen);
        delete [] replyData;
    }
    
#if HomeKitLog == 1 && HomeKitReplyHeaderLog==1
    g_homekit_logger("Reply: %s\n", *reply);
#endif
    
}

void addInfoServiceToAccessory(Accessory *acc, string accName, string manufactuerName, string modelName, string serialNumber, identifyFunction identifyCallback) {
    Service *infoService = new Service(serviceType_accessoryInfo);
    acc->addService(infoService);
    
    stringCharacteristics *accNameCha = new stringCharacteristics(charType_serviceName, premission_read, 0);
    accNameCha->setValue(accName);
    acc->addCharacteristics(infoService, accNameCha);
    
    stringCharacteristics *manNameCha = new stringCharacteristics(charType_manufactuer, premission_read, 0);
    manNameCha->setValue(manufactuerName);
    acc->addCharacteristics(infoService, manNameCha);
    
    stringCharacteristics *modelNameCha = new stringCharacteristics(charType_modelName, premission_read, 0);
    modelNameCha->setValue(modelName);
    acc->addCharacteristics(infoService, modelNameCha);
    
    stringCharacteristics *serialNameCha = new stringCharacteristics(charType_serialNumber, premission_read, 0);
    serialNameCha->setValue(serialNumber);
    acc->addCharacteristics(infoService, serialNameCha);
    
    boolCharacteristics *identify = new boolCharacteristics(charType_identify, premission_write);
    identify->setValue("false");
    identify->valueChangeFunctionCall = identifyCallback;
    acc->addCharacteristics(infoService, identify);
}

void Personal_homekit_set_logger_function(PERSONAL_LOGGER_FUNCTION func)
{
    g_homekit_logger = func;
}
