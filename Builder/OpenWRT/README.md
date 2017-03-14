This is the package for compiling OpenWRT version using the toolchain. Before running it, please add the source code of the project (everything beside Extra/, and Builder/) into the PHK/src of this folder. 
Then, copy the content of the content inside the package/ (avahi/, dbus/, etc), to the package folder under the toolchain.  
The dependent should be compiled along with the PHK. 
Tested: OpenWrt Barrier Breaker 14.07, on D-Link DIR-505
