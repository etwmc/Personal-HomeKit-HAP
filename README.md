#PersonalHomeKit
===============

This is a spin-off project from my attempt to build a Siri controlled night light, and it will provide source code to build a HomeKit support accessories. 

# The library required to compile has not been included yet.

# Dear Lawyer of Apple, 
If you want me to pull down the code, please send me an email. (No phone call, I don't pick up unknown number. )

#Current Status: 
1. Publish the device as a HomeKit
2. Allow the controller (iOS device) to pair with the accessory
3. Allow the controller connect with the accessory after pair
4. Record the paired controllers UUID and public keys
4. Provide the controller the service about the accessory (The current configuration will publish Light Service and the Information Service)

#Current Requirment: 
1. Mac OS X 10.9-10.10
2. Xcode
 
#Future Plan: 
1. Allow the controller change attribute (The current attributes are static)
2. Add ability to add user
3. Spin off from OS X only to support microcontroller
