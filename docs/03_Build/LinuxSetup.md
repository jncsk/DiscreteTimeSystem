# Linux Setup Guide

## 1. Prerequisites

The following tools are required:

- Git
- GCC and G++
- CMake
- Make or Ninja

## 2. Clone the Repository
```bash
git clone https://github.com/jncsk/DiscreteTimeSystem.git
cd DiscreteTimeSystem
```

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
![alt text](images/curl-result.png)

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
![alt text](images/verify-the-service-1.png)
```bash
journalctl -u discrete-time-system
```
![alt text](images/verify-the-service-2.png)
