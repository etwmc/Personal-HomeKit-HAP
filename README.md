**PersonalHomeKit**

---------------
Update at 12/31/2019:
For the last two and a half years, I've been working in a smart home company. Therefore, to avoid complication, I have not updated this repo since joined. And while I have continue develop on my personal build and projects using this project, it's unlikely I will be able to publish anything. 
The good thing is, since this will continue exist, and the HomeKit Accessory protocol Spec has been released for the hobbyists, there should be sufficient information updating for future iOS support. 
In the future, if anyone is interested in contribute for the project, please let me know. 
Thanks, 
Raymond Chan

---------------

This is a user based version (the characteristic is depended on the connection session). For dynamic or static version(MCU version), switch to the correspondent branch. 
Now support multiple characteristic, notify function

Plug for relative project

Want to turn off the lights when everyone is out? How about closing the gas valve? Introduing a derived project: WiFi-Radar. No apps needed, no GPS tracing. All you need is a Wi-Fi network and a *nix running system
[WiFi-Radar-HAP](https://github.com/etwmc/WiFi-RadarHAP)

Also, the first HomeKit test and benchmark app
[HomeKitTester](http://github.com/etwmc/HomeKitTester)

---------------
Reset
In iOS 9.0 Beta 1, Apple has implemented the check of status flag: a integer display where an accessory should be discoverable. And for any accessory that has paired, the flag should be 0. 
Therefore, there is a way to reset the pair record in since this version: 
By adding any argument while launching the program, the program will clear all the previous record of pairing, before start to launch the service. 
e.g. ./PHK abcdefg
This argument might be changed in the future, so if you want to deploy for the future, consider using "PHK reset" as the command. 

(
However, as it has not been implemented in iOS 8.3 or before, after any reset, iOS devices before iOS 9 will still attempt to connect, which will failed for obvious reason. 
In that case, a manual removal of the accessory on the iOS end is required. 
)



===============

This is a spin-off project from my attempt to build a Siri controlled night light, and it will provide source code to build a HomeKit support accessories.


1. Publish the device as a HomeKit
2. Allow the controller (iOS device) to pair with the accessory
3. Allow the controller connect with the accessory after pair
4. Record the paired controllers UUID and public keys
5. Provide the controller the service about the accessory
6. Allow controller to update value
7. Notify controller about value updated

Current Requirment:
1. OpenSSL
2. Avahi (For Linux)

Future Plan:
1. Support microcontroller



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
