# Nginx VM Setup

## 0. Prerequisite
User has their own Azure account.

## 1. Navigate to resource creation page
Navigate to the following page.
https://portal.azure.com/#create/Microsoft.VirtualMachine

## 2. Choice the VM settings of the Nginx [Basic]
#### Subscription
Your prefered subscription

#### Resouce group
Your prefered resource group

#### Virtual machine name
Your prefered VM name

#### Region
Your prefered region

#### Availability options
No ingrastructure redundancy required

#### Image
Red Hat Enterprize Linux 9.4 (LVM) -x64 Gen2

#### VM architecture
x64

#### Size
B1ms

#### Authentication type
SSH public key

#### Username
azureuser

#### SSH public key source
RSA SSH Format

#### SSH Key Type
default value

#### Key pair name
default value

#### Inbound port rules
Allow selected prots

#### Select inbound ports
SSH (22)

## 3. Choice the VM settings of the Nginx [Disks]
Use the default setting.
## 4. Choice the VM settings of the Nginx [Networking]
Use the default setting.

## 5. Choice the VM settings of the Nginx [Management]
Use the default setting.

## 6. Choice the VM settings of the Nginx [Monitoring]
Use the default setting.

## 7. Choice the VM settings of the Nginx [Advanced]
Use the default setting.

## 8. Choice the VM settings of the Nginx [Tags]
Use the default setting.



## 3. Configure the Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

## 4. Build the Project
```bash
cmake --build build
```

## 5. Run the HTTP Server
```bash
/build/DiscreteTimeSystemRunner
```

## 6. Verify the HTTP Server
Note: 
The API here is just an example.
The API will be replaced with more pratical one
```bash
curl "http://localhost:8080/calculate?x=10&y=20"
```
![alt text](./images/curl-result.png)

## 7. Install the systemd Service
```bash
sudo vi /etc/systemd/system/discrete-time-system
```
```bash
[Unit]
 Description=Discrete Time System HTTP Server                                                       
 After=network.targe
[Service]
 ExecStart=/home/junsasaki/src/repos/DiscreteTimeSystem/build/DiscreteTimeSystemRunner
 WorkingDirectory=/home/junsasaki/src/repos/DiscreteTimeSystem/build
 Restart=on-failure
 User=your name
[Install]
 WantedBy=multi-user.target       
```
```bash
sudo systemctl daemon-reload
sudo systemctl enable --now discrete-time-system
```

## 8. Verify the Service

```bash
systemctl status discrete-time-system
```
![alt text](./images/verify-the-service-1.png)
```bash
journalctl -u discrete-time-system
```
![alt text](./images/verify-the-service-2.png)
