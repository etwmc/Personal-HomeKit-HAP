/*
 * This accessory.cpp is configurated for light accessory + fan accessory
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

AccessorySet *accSet;

void initAccessorySet() {
    currentDeviceType = deviceType_bridge;
    
    printf("Initial Accessory\n");

    Accessory *lightAcc = new Accessory();

    //Add Light
    accSet = &AccessorySet::getInstance();
    addInfoServiceToAccessory(lightAcc, "Light 1", "ET", "Light", "12345678", &lightIdentify);
    accSet->addAccessory(lightAcc);

    Service *lightService = new Service(serviceType_lightBulb);
    lightAcc->addService(lightService);

    stringCharacteristics *lightServiceName = new stringCharacteristics(charType_serviceName, premission_read, 0);
    lightServiceName->setValue("Light");
    lightAcc->addCharacteristics(lightService, lightServiceName);

    boolCharacteristics *powerState = new boolCharacteristics(charType_on, premission_read|premission_write);
    powerState->setValue("true");
    lightAcc->addCharacteristics(lightService, powerState);

    intCharacteristics *brightnessState = new intCharacteristics(charType_brightness, premission_read|premission_write, 0, 100, 1, unit_percentage);
    brightnessState->setValue("50");
    lightAcc->addCharacteristics(lightService, brightnessState);

    //Add fan
    Accessory *fan = new Accessory();
    addInfoServiceToAccessory(fan, "Fan 1", "ET", "Fan", "12345678", &fanIdentify);
    accSet->addAccessory(fan);

    Service *fanService = new Service(serviceType_fan);
    fan->addService(fanService);

    stringCharacteristics *fanServiceName = new stringCharacteristics(charType_serviceName, premission_read, 0);
    fanServiceName->setValue("Fan");
    fan->addCharacteristics(fanService, lightServiceName);

    boolCharacteristics *fanPower = new boolCharacteristics(charType_on, premission_read|premission_write);
    fanPower->setValue("true");
    fan->addCharacteristics(fanService, fanPower);
};
