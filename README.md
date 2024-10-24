# Siemens IOT2050 with ESP32
Big project of Data Communications and Networking (EE3019) Ho Chi Minh University of Tecnology - VNU  
This project using Siemens IOT2050 as a MQTT broker, ESP32s is network nodes to build a model of smarthome.
## Table of contents
- Intallation Example OS for Siemens Simatic IOT2050
- Setting up MQTT broker in IOT2050
- Setting up ESP32
## Intallation Example OS for Siemens Simatic IOT2050
Go [this link](https://support.industry.siemens.com/cs/document/109741799/downloads-for-simatic-iot20x0?dti=0&lc=en-VN) and download IOT2050_Example_Image_V1.1.1.zip (658,6 MB)(SHA-256)  
Extract .zip and we will get .wic file  
Flash this image to SD card (>16GB) using any flash software (I using rusfus), no special setting  
Plug SD card to IOT2050, power up  
## Setting up MQTT broker in IOT2050:
Plug the LAN wire to XP1 (Port LAN1) connect form IOT2050 to PC
Using Putty, open SSH connect 192.168.200.1 port 22
Default password and username are "root"
After that, type "sudo apt update" and "sudo apt upgrade"


