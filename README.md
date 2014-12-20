#PersonalHomeKit
This is a dymanic version (runtime add/remove accessories/services). For static version(MCU version), switch to the static branch
Now support multiple characteristic, notify function
===============
Dear Lawyer of Apple, <br>
If you want me to pull down the code, just send me an email. (No phone call, I don't pick up unknown number. ) I'm a reasonable kid, and I like Apple, so no need for DMCA, just talk to me, we can make a deal. 

Build method: 
After copy the codes, please follow these steps: 

1. On non-OSX platform, please install avahi-deamon (or any dns-sd implement), clang and OpenSSL. 
2. Set the following value in Configuration.h<br>
  i. HomeKitLog -> Whether the program return message to console<br>

  ii.HomeKitReplyHeaderLog -> Whether the program should return the accessory message (Recommend debug only)<br>

  iii. Device setting: deviceName (Name), deviceIdentity (Device ID publish in HomeKit, usually your MAC address), manufactuerName (Manufactuer Name shown in HomeKit Framework), devicePassword (Password, please choose 9 non sequential, or repeat digits in XXX-XX-XXX fashion, e.g. 421-35-195), deviceUUID (UUID for iOS to verify your accessory identity)<br>

  iv. (IMPORTANT) controllerRecordsAddress -> where should the program store the pair record. It is left empty to prevent you compile without setting Configuration.h<br>

  v. numberOfClient -> Please increase or lower it based on your hardware power. HomeKit framework require the accessory to response in 1-2 seconds before timeout. <br>

  vi. (IMPORTANT) If you are porting the program to MCU (NOT OFFICIAL TESTED), change MCU to 1, and implement pair record service in PHKControllerRecord.cpp, as MCU usually use EEPROM instead a file system. <br>

3. Implement Accessory function: 
  The program publish a light service (and identify service, which is the essential function of any HomeKit accessory). However, the service does nothing as the variety configuration one could come up to achieve this project (I'm currently using a Ubuntu server, with serial connected Arduino)<br>

  Therefore, change the PHKArduinoLightInterface.c to your own configuration
    setupPort() -> setup the connection/GPIO/other thing you need to change the light
    startIdentify() -> a method to show users this accessory is the one they ask to identify (for example, blink three time)
    setLightStrength(int strengthLevel) -> change the light stength, the int is from 0-255
<br>

To create your own service, please reference to the Accessory.cpp. You can add characteristic in a service, service in an accessory, and accessory in accessory set. However, accessory id is unique in accessory set, and characteristic id is unique in accessory. (NOT SERVICE)<br>

(Please beware the number of characteristic, service and accessory, and the getter based on id is hard coded in the numberOf_ and _AtIndex. This is done to reduce the multithread complexity. So if you are going to implement a dynamic setting (for example, make a accessory bridge with PnP accessories), please keep the function be thread-safe at all time. )
<br>
4. Then, make and enjoy. 
<br>
===============

This is a spin-off project from my attempt to build a Siri controlled night light, and it will provide source code to build a HomeKit support accessories. 


1. Publish the device as a HomeKit
2. Allow the controller (iOS device) to pair with the accessory
3. Allow the controller connect with the accessory after pair
4. Record the paired controllers UUID and public keys
5. Provide the controller the service about the accessory
6. Allow controller to update value

Current Requirment: 
1. OpenSSL
2. Avahi (For Linux)

Future Plan: 
1. Support microcontroller
2. Notify controller about value updated
