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
### Setting Network
Plug the LAN wire to XP1 (Port LAN1) connect form IOT2050 to PC  
In PC, go Control Panel -> Network and Internet -> Network Connections
Click in Ethernet connection -> Properties
TCP/IP4 -> Using the following IP Address
IP Address: 192.168.200.100
Subnet Mask: 255.255.255.0
Using Putty, open SSH connect 192.168.200.1 port 22  
Default password and username are "root"  
After that, type "sudo apt update" and "sudo apt upgrade"  
Type "iot2050setup"
Networking -> enO1 -> Edit -> 192.168.1.10  
In PC, go Control Panel -> Network and Internet -> Network Connections  
Click in Ethernet connection -> Properties  
TCP/IP4 -> Using the following IP Address  
IP Address: 192.168.1.100  
Subnet Mask: 255.255.255.0  
### Setting MQTT Broker  
Check the Status of Mosquitto: "sudo systemctl status mosquitto"  
Check the Port Mosquitto is Listening On: "sudo netstat -tuln | grep mosquitto" or "sudo ss -tuln | grep 1883"  
If the Teminal show :  
tcp   LISTEN 0      100             127.0.0.1:1883      0.0.0.0:*    
tcp   LISTEN 0      100                 [::1]:1883         [::]:*    
That mean Mosquitto is listening on the loopback address (localhost) at port 1883  
In that case, go to config file of Mosquitto : sudo nano /etc/mosquitto/mosquitto.conf  
Add "listener 1883 0.0.0.0" and "allow_anonymous true" in the config file  
Check again  
If Terminal say  
tcp   LISTEN 0      100               0.0.0.0:1883      0.0.0.0:*  
You are success !!!
### Test the MQTT connection
In IOT2050: "hostname -I"
In the Terminal of IOT2050: "mosquitto_sub -h <IOT2050_IP> -p 1883 -t test/topic"
In another PC or Terminal: "mosquitto_pub -h <IOT2050_IP> -p 1883 -t test/topic -m "Hello, MQTT!""
Enjoy!
