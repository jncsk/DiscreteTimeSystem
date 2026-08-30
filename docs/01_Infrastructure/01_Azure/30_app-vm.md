# Application VM Setup

## 0. Prerequisites

- A local Linux environment
- An Azure account
- An active Azure subscription
- An Nginx VM configured in the same Azure Virtual Network

## 1. Create a Virtual Machine for the Application

### 1.1 Navigate to the VM Creation Page

Open the Azure Virtual Machine creation page:

https://portal.azure.com/#create/Microsoft.VirtualMachine

### 1.2 Configure the Application VM Settings [Basics]

#### Subscription

Select your preferred Azure subscription.

#### Resource group

Select the same resource group used by the DiscreteTimeSystem environment.

#### Virtual machine name

Enter your preferred VM name.

Example:

`dts-main-application`

#### Region

Select the same region as the Nginx VM.

#### Availability options

Select:

`No infrastructure redundancy required`

#### Image

Select:

`Ubuntu Server 24.04 LTS - x64 Gen2`

#### VM architecture

Select:

`x64`

#### Size

Select:

`Standard_B2s`

> **Note:**  
> The Application VM requires more CPU and memory than the Nginx VM because Docker image builds and C++ compilation are performed on this VM.  
> A smaller VM size may result in significantly slower builds or insufficient memory.

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

### 1.3 Configure the Application VM Settings [Disks]

Use the default settings.

### 1.4 Configure the Application VM Settings [Networking]

Configure the VM to use the same Azure Virtual Network as the Nginx VM.

Record the following values for later configuration:

- Virtual network
- Subnet
- Private IP address
- Public IP address
- Network Security Group

At this stage, allow SSH (TCP/22) for remote administration.

TCP port `8080`, which is used by the DiscreteTimeSystem application, will be configured later to allow communication from the Nginx VM.

> **Note:**  
> The application port should not be directly exposed to the Internet.  
> Application traffic should reach the Application VM through the Nginx VM over the Azure Virtual Network.

### 1.5 Configure the Application VM Settings [Management]

Use the default settings.

### 1.6 Configure the Application VM Settings [Monitoring]

Use the default settings.

### 1.7 Configure the Application VM Settings [Advanced]

Use the default settings.

### 1.8 Configure the Application VM Settings [Tags]

Use the default settings.

### 1.9 Create the VM

Review the configuration and create the virtual machine.

> **Note:**  
> When the VM is created using a newly generated SSH key pair, Azure allows you to download the private key file.  
> Make sure to download and store the private key in a secure location, as it is required to connect to the VM via SSH.  
> Do not commit the private key to this repository or share it with others.

After deployment is complete, record the VM's public and private IP addresses.

---

## 2. Connect to the VM from a Local Linux Environment

### 2.1 Copy the Private Key to the Linux Environment

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
ssh -i ~/.ssh/private-key-name.pem azureuser@<application-vm-public-ip>
```

---

## 3. Install Docker

### 3.1 Install Required Packages

Update the package index and install the packages required to configure the Docker repository.

```bash
sudo apt update
sudo apt install -y ca-certificates curl
```

### 3.2 Add the Docker Repository

Create a directory for the Docker repository signing key.

```bash
sudo install -m 0755 -d /etc/apt/keyrings
```

Download the Docker signing key.

```bash
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg \
  -o /etc/apt/keyrings/docker.asc
```

Allow the key to be read by APT.

```bash
sudo chmod a+r /etc/apt/keyrings/docker.asc
```

Add the Docker repository.

```bash
sudo tee /etc/apt/sources.list.d/docker.sources > /dev/null <<EOF
Types: deb
URIs: https://download.docker.com/linux/ubuntu
Suites: $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}")
Components: stable
Architectures: $(dpkg --print-architecture)
Signed-By: /etc/apt/keyrings/docker.asc
EOF
```

Update the package index.

```bash
sudo apt update
```

### 3.3 Install Docker Engine

Install Docker Engine and related components.

```bash
sudo apt install -y \
  docker-ce \
  docker-ce-cli \
  containerd.io \
  docker-buildx-plugin \
  docker-compose-plugin
```

### 3.4 Verify the Docker Service

Check the Docker service status.

```bash
sudo systemctl status docker
```

Verify that Docker can start a container.

```bash
sudo docker run hello-world
```

### 3.5 Allow the Current User to Use Docker

Add the current user to the `docker` group.

```bash
sudo usermod -aG docker $USER
```

Disconnect from the SSH session and reconnect to apply the new group membership.

```bash
exit
```

After reconnecting, verify the group membership.

```bash
groups
```

Confirm that `docker` is included in the output.

> **Note:**  
> Membership in the `docker` group provides privileges comparable to root access.  
> Only trusted users should be added to this group.

---

## 4. Deploy DiscreteTimeSystem

### 4.1 Clone the Git Repository

Create a directory for source repositories.

```bash
mkdir -p ~/src/repos
cd ~/src/repos
```

Clone the DiscreteTimeSystem repository.

```bash
git clone https://github.com/jncsk/DiscreteTimeSystem.git
```

Move to the repository root.

```bash
cd ~/src/repos/DiscreteTimeSystem
```

### 4.2 Build the Docker Image

Build the Docker image from the Dockerfile in the repository root.

```bash
docker build -t discrete-time-system .
```

> **Note:**  
> Run this command from the repository root.  
> The final `.` specifies the Docker build context. The files required by the Dockerfile, including the source code and `CMakeLists.txt`, must be included in this context.

Verify that the image was created.

```bash
docker images
```

Confirm that `discrete-time-system` is included in the output.

### 4.3 Run the Docker Container

Create and start a container from the image.

```bash
docker run -d \
  --name dts \
  -p 8080:8080 \
  --restart unless-stopped \
  discrete-time-system
```

The options have the following purposes:

```text
-d
    Run the container in the background.

--name dts
    Assign "dts" as the container name.

-p 8080:8080
    Map TCP port 8080 on the Application VM
    to TCP port 8080 in the container.

--restart unless-stopped
    Automatically restart the container after
    Docker or the VM restarts unless it was
    explicitly stopped by the user.
```

### 4.4 Verify the Container Status

Check that the container is running.

```bash
docker ps
```

The port mapping should include:

```text
0.0.0.0:8080->8080/tcp
```

Check the application logs if necessary.

```bash
docker logs dts
```

### 4.5 Verify the Application Locally

From the Application VM, send a request directly to the application.

```bash
curl "http://localhost:8080/calculate?x=10&y=20"
```

If a calculation result is returned, the application is running successfully inside the Docker container.

The communication path at this point is:

```text
Application VM
    |
    | localhost:8080
    v
Docker port mapping
    |
    v
DiscreteTimeSystem Container :8080
```

---

## 5. Configure Network Access

Configure the Azure Network Security Group to allow the Nginx VM to access the application on TCP port `8080`.

### 5.1 Configure the NSG on Azure Portal

1. Navigate to **Network settings** of the Application VM.
2. Click **Create port rule** and add an inbound port rule.
3. Configure the following settings:

```text
Source:                 IP Addresses
Source IP address:      <nginx-vm-private-ip>/32
Source port:            *
Destination:            Any
Service:                Custom
Destination port:       8080
Protocol:               TCP
Action:                 Allow
```

Replace `<nginx-vm-private-ip>` with the private IP address of the Nginx VM.

> **Note:**  
> Do not configure the source as `Any` unless it is temporarily required for troubleshooting.  
> Port `8080` should normally be reachable only from the Nginx VM or a trusted Azure subnet.

### 5.2 Check the Ubuntu Firewall

Check whether UFW is enabled.

```bash
sudo ufw status
```

If the result is:

```text
Status: inactive
```

no additional UFW configuration is required.

If UFW is active, allow the Nginx VM to access TCP port `8080`.

```bash
sudo ufw allow from <nginx-vm-private-ip> to any port 8080 proto tcp
```

Verify the configuration.

```bash
sudo ufw status
```

---

## 6. Verify Connectivity from the Nginx VM

Connect to the Nginx VM and send an HTTP request directly to the private IP address of the Application VM.

```bash
curl "http://<application-vm-private-ip>:8080/calculate?x=10&y=20"
```

If a calculation result is returned, the following communication path is working successfully:

```text
Nginx VM
    |
    | Azure VNet
    | TCP/8080
    v
Application VM
    |
    | Docker port mapping
    v
DiscreteTimeSystem Container
```

This confirms that:

- The DiscreteTimeSystem container is running.
- The application is listening on TCP port `8080`.
- Docker port mapping is configured correctly.
- The Azure NSG allows traffic from the Nginx VM.
- The Application VM is reachable through its private IP address.

---

## 7. Verify End-to-End Connectivity

From the local Linux environment, send a request to the DiscreteTimeSystem application through the Nginx reverse proxy.

```bash
curl -H "Host: discrete-time-system" \
  "http://<nginx-vm-public-ip>/calculate?x=10&y=20"
```

If the calculation result is returned, end-to-end connectivity is configured successfully.

The complete request path is:

```text
Local Linux Environment
        |
        | HTTP :80
        v
Nginx VM Public IP
        |
        v
Azure NSG
        |
        v
RHEL Firewall
        |
        v
Nginx Reverse Proxy
        |
        | Azure VNet
        | HTTP :8080
        v
Application VM Private IP
        |
        v
Docker Port Mapping
        |
        v
DiscreteTimeSystem Container
        |
        v
Calculation Result
```
