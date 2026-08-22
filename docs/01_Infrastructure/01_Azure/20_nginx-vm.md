# Nginx VM Setup

## 0. Prerequisites

- A local Linux environment
- An Azure account
- An active Azure subscription

## 1. Create a Virtual Machine for Nginx

### 1.1 Navigate to the VM creation page

Open the Azure Virtual Machine creation page:

https://portal.azure.com/#create/Microsoft.VirtualMachine

### 1.2 Configure the Nginx VM settings [Basics]

#### Subscription
Select your preferred Azure subscription.

#### Resource group
Select an existing resource group or create a new one.

#### Virtual machine name
Enter your preferred VM name.
Example:
`dts-frontend`

#### Region
Select your preferred Azure region.

#### Availability options
Select:
`No infrastructure redundancy required`

#### Image
Select:
`Red Hat Enterprise Linux 9.4 (LVM) - x64 Gen2`

#### VM architecture
Select:
`x64`

#### Size
Select:
`Standard_B1ms`

#### Authentication type
Select:
`SSH public key`

#### Username
Set:
`azureuser`

#### SSH public key source
Select the appropriate option for generating or using an SSH key pair.

#### SSH key type
Use the default value.

#### Key pair name
Use the default value or specify your preferred key pair name.

#### Public inbound ports
Select:
`Allow selected ports`

#### Select inbound ports
Select:
`SSH (22)`

### 1.3 Configure the Nginx VM settings [Disks]
Use the default settings.

### 1.4 Configure the Nginx VM settings [Networking]
Configure the VM to use the Azure Virtual Network used by the DiscreteTimeSystem environment.

Record the following values for later configuration:
- Virtual network
- Subnet
- Private IP address
- Public IP address
- Network Security Group

At this stage, allow SSH (TCP/22) for remote administration.

### 1.5 Configure the Nginx VM settings [Management]
Use the default settings.

### 1.6 Configure the Nginx VM settings [Monitoring]
Use the default settings.

### 1.7 Configure the Nginx VM settings [Advanced]
Use the default settings.

### 1.8 Configure the Nginx VM settings [Tags]
Use the default settings.

### 1.9 Create the VM
Review the configuration and create the virtual machine.

> **Note:**  
> When the VM is created using a newly generated SSH key pair, Azure allows you to download the private key file.  
> Make sure to download and store the private key in a secure location, as it is required to connect to the VM via SSH.
>
> Do not commit the private key to this repository or share it with others.

After deployment is complete, record the VM's public and private IP addresses.

## 2. Connect to the VM from a Local Linux Environment

### 2.1 Copy the private key to the Linux environment
If you are using WSL and the private key is stored in the Windows Downloads directory, copy it to the `.ssh` directory in your Linux home directory.
```bash
cp "/mnt/c/Users/YourAccount/Downloads/private-key-name.pem" ~/.ssh/
```
Restrict access to the private key:
```bash
chmod 600 ~/.ssh/private-key-name.pem
```

### 2.2 Connect to the VM via SSH
Use the VM's public IP address to establish an SSH connection.
```bash
ssh -i ~/.ssh/<private-key-name>.pem azureuser@<public-ip-address>
```

## 3. Nginx VM Setup

### 3.1 Install Nginx
Install Nginx using the DNF package manager.
```bash
sudo dnf install nginx
```
Check the service status.
```bash
systemctl status nginx
```

### 3.2 Start Nginx
Start the Nginx service.
```bash
sudo systemctl start nginx
```

### 3.3 Enable Nginx to start automatically at boot
Configure Nginx to start automatically when the VM boots.
```bash
sudo systemctl enable nginx  
```

### 3.4 Verify Local Connectivity
Verify that Nginx is responding locally on HTTP port 80.
```bash
curl http://localhost 
```
If the Nginx default page is returned, the Nginx service is running successfully.

## 4. Configure Network Access
Configure the Azure Network Security Group (NSG) and the RHEL firewall to allow inbound HTTP traffic on port 80.

### 4.1 NSG setting on Azure Portal
1. Navigate to **Network settings** of the Nginx VM.
2. Click **Create port rule** and add an inbound port rule.
3. Configure the following settings

```text
Source:          Any
Source port:     *
Destination:     Any
Service:         HTTP
Destination port: 80
Protocol:        TCP
Action:          Allow
```

### 4.2 Configure the RHEL Firewall
Allow HTTP traffic through the RHEL firewall.
```bash
sudo firewall-cmd --permanent --add-service=http
sudo firewall-cmd --reload
```
Verify the firewall configuration.
```bash
sudo firewall-cmd --list-services
```

### 4.3 Verify the connectivity from the local environment
From the local Linux environment, send an HTTP request to the public IP address of the Nginx VM.
```bash
curl http://<nginx-vm-public-ip>
```
If the Nginx default page is returned, inbound HTTP connectivity to the Nginx VM is configured successfully.

## 5. Configure Nginx as a Reverse Proxy
## 6. Verify End-to-End Connectivity
