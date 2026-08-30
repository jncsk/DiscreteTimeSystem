# SSH Access

## 1. Overview

This document describes how to access the Azure virtual machines used by the DiscreteTimeSystem environment.

The environment consists of:

```text
Local Linux Environment
        |
        | SSH / TCP 22
        v
Nginx VM
Public IP
        |
        | Azure VNet
        | SSH / TCP 22
        v
Application VM
Private IP only
```

The Nginx VM is directly accessible from the local environment through its public IP address.

The Application VM is accessed through the Nginx VM using SSH ProxyJump. This allows the Application VM to operate without a public IP address.

---

## 2. Prerequisites

- A local Linux environment
- SSH private keys for the Nginx VM and Application VM
- SSH (TCP/22) allowed to the Nginx VM from the local environment
- SSH (TCP/22) allowed from the Nginx VM to the Application VM
- Both VMs connected to the same Azure Virtual Network or otherwise privately reachable

Example private keys:

```text
~/.ssh/dts-frontend_key.pem
~/.ssh/dts-main-application_key.pem
```

Restrict access to the private keys:

```bash
chmod 600 ~/.ssh/dts-frontend_key.pem
chmod 600 ~/.ssh/dts-main-application_key.pem
```

---

## 3. Direct SSH Access to the Nginx VM

The Nginx VM has a public IP address and can be accessed directly from the local Linux environment.

```bash
ssh -i ~/.ssh/dts-frontend_key.pem \
  azureuser@<nginx-vm-public-ip>
```

Example:

```bash
ssh -i ~/.ssh/dts-frontend_key.pem \
  azureuser@20.48.100.55
```

If the connection succeeds, SSH access to the Nginx VM is configured correctly.

---

## 4. Configure the SSH Client

To simplify SSH access, define both VMs in the local SSH client configuration.

Edit:

```bash
vi ~/.ssh/config
```

Add the following configuration:

```sshconfig
Host dts-frontend
    HostName <nginx-vm-public-ip>
    User azureuser
    IdentityFile ~/.ssh/dts-frontend_key.pem

Host dts-main-application
    HostName <application-vm-private-ip>
    User azureuser
    IdentityFile ~/.ssh/dts-main-application_key.pem
    ProxyJump dts-frontend
```

Example:

```sshconfig
Host dts-frontend
    HostName 20.48.100.55
    User azureuser
    IdentityFile ~/.ssh/dts-frontend_key.pem

Host dts-main-application
    HostName 172.16.0.4
    User azureuser
    IdentityFile ~/.ssh/dts-main-application_key.pem
    ProxyJump dts-frontend
```

Restrict access to the SSH configuration file:

```bash
chmod 600 ~/.ssh/config
```

---

## 5. SSH Configuration Parameters

### Host

`Host` defines a local alias used by the SSH client.

Example:

```sshconfig
Host dts-frontend
```

This allows the following command:

```bash
ssh dts-frontend
```

The value does not need to match the actual hostname of the VM.

### HostName

`HostName` specifies the actual IP address or DNS name of the remote host.

Example:

```sshconfig
HostName 20.48.100.55
```

### User

`User` specifies the remote Linux account used for SSH authentication.

Example:

```sshconfig
User azureuser
```

### IdentityFile

`IdentityFile` specifies the private key used to authenticate to the target host.

Example:

```sshconfig
IdentityFile ~/.ssh/dts-frontend_key.pem
```

### ProxyJump

`ProxyJump` specifies another SSH host that must be used as a jump host before connecting to the target host.

Example:

```sshconfig
ProxyJump dts-frontend
```

This means that `dts-frontend` is used as the intermediate SSH host when connecting to `dts-main-application`.

---

## 6. Connect to the Nginx VM

After configuring `~/.ssh/config`, connect to the Nginx VM with:

```bash
ssh dts-frontend
```

The SSH client reads:

```sshconfig
Host dts-frontend
    HostName <nginx-vm-public-ip>
    User azureuser
    IdentityFile ~/.ssh/dts-frontend_key.pem
```

and establishes a direct SSH connection to the Nginx VM.

The Application VM configuration is not used for this connection.

---

## 7. Connect to the Application VM Using ProxyJump

Connect to the Application VM with:

```bash
ssh dts-main-application
```

The SSH client first reads:

```sshconfig
Host dts-main-application
    HostName <application-vm-private-ip>
    User azureuser
    IdentityFile ~/.ssh/dts-main-application_key.pem
    ProxyJump dts-frontend
```

Because `ProxyJump dts-frontend` is configured, SSH then reads the `dts-frontend` configuration and establishes a connection through the Nginx VM.

The connection path is:

```text
Local Linux Environment
        |
        | Authentication:
        | dts-frontend_key.pem
        v
Nginx VM
        |
        | SSH transport over Azure VNet
        v
Application VM
        ^
        | Authentication:
        | dts-main-application_key.pem
        |
Local SSH Client
```

The private keys remain on the local Linux environment.

They do not need to be copied to the Nginx VM.

---

## 8. Equivalent ProxyJump Command

Without using `~/.ssh/config`, ProxyJump can also be specified directly on the command line.

If both VMs use the same SSH key:

```bash
ssh -i ~/.ssh/private-key.pem \
  -J azureuser@<nginx-vm-public-ip> \
  azureuser@<application-vm-private-ip>
```

When different private keys are used for the two VMs, using `~/.ssh/config` is recommended because each host can have its own `IdentityFile`.

---

## 9. Verify Access Before Removing the Application VM Public IP

Before removing the public IP address from the Application VM, verify both connections.

Verify direct access to the Nginx VM:

```bash
ssh dts-frontend
```

Verify access to the Application VM through ProxyJump:

```bash
ssh dts-main-application
```

Only remove the Application VM public IP after both connections are working as expected.

---

## 10. Troubleshooting

### Permission Denied on the Nginx VM

Example:

```text
azureuser@<nginx-vm-public-ip>: Permission denied (publickey,...)
```

This means authentication to the jump host failed before the Application VM was reached.

Verify:

- The Nginx VM private key is correct.
- `IdentityFile` points to the correct file.
- The SSH username is correct.
- The private key permissions are restricted.

Test the Nginx VM directly:

```bash
ssh -i ~/.ssh/dts-frontend_key.pem \
  azureuser@<nginx-vm-public-ip>
```

### Permission Denied on the Application VM

If the Nginx VM connection succeeds but authentication to the Application VM fails, verify the Application VM configuration:

```sshconfig
Host dts-main-application
    HostName <application-vm-private-ip>
    User azureuser
    IdentityFile ~/.ssh/dts-main-application_key.pem
    ProxyJump dts-frontend
```

Confirm that the private key matches the public key configured on the Application VM.

### Connection Timeout to the Application VM

If authentication to the Nginx VM succeeds but the Application VM connection times out, verify:

- The Application VM is running.
- The private IP address is correct.
- TCP/22 is allowed by the Application VM NSG.
- The Nginx VM can route to the Application VM through the Azure Virtual Network.
- The Application VM host firewall allows SSH.

### Display the Effective SSH Configuration

Use the following command to inspect the final SSH configuration applied to a host:

```bash
ssh -G dts-main-application
```

This is useful for verifying values such as:

```text
hostname
user
identityfile
proxyjump
```

### Enable Verbose SSH Logging

For detailed SSH troubleshooting:

```bash
ssh -v dts-main-application
```

For more detail:

```bash
ssh -vvv dts-main-application
```
