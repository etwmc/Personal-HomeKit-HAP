/*
 * This accessory.cpp is configurated for light accessory
 */

#include "Accessory.h"

#include "PHKAccessory.h"

//Global Level of light strength
int lightStength = 0;
int fanSpeedVal = 0;

void lightIdentify(bool oldValue, bool newValue) {
    printf("Start Identify Light\n");
}

void fanIdentify(bool oldValue, bool newValue) {
    printf("Start Identify Fan\n");
}

void changeLightIntensity(int oldValue, int newValue) {
    printf("New Intensity\n");
}

AccessorySet *accSet;

void initAccessorySet() {
    printf("Initial Accessory\n");
    accSet = new AccessorySet();
    Accessory *lightAcc = new Accessory();
    addInfoServiceToAccessory(lightAcc, "Light 1", "ET", "Light", "12345678", &lightIdentify);
    accSet->addAccessory(lightAcc);
    
    Service *lightService = new Service(charType_lightBulb);
    lightAcc->addService(lightService);
    
    stringCharacteristics *lightServiceName = new stringCharacteristics(charType_serviceName, premission_read, 0);
    lightServiceName->setValue("Light");
    lightAcc->addCharacteristics(lightService, lightServiceName);
    
    boolCharacteristics *powerState = new boolCharacteristics(charType_on, premission_read|premission_write);
    powerState->setValue("true");
    lightAcc->addCharacteristics(lightService, powerState);
    
    intCharacteristics *brightnessState = new intCharacteristics(charType_brightness, premission_read|premission_write, 0, 100, 1, unit_percentage);
    brightnessState->setValue("50");
    brightnessState->valueChangeFunctionCall = &changeLightIntensity;
    lightAcc->addCharacteristics(lightService, brightnessState);
};