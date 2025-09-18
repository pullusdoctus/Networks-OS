# Gateway

## Task 1

### Step 1. Open a command prompt window on a pod host computer. Issue the ipconfig command. What is the default gateway address?

The default gateway address is `172.16.255.254`

### Step 2. Use the ping command to verify connectivty with IP address 127.0.0.1. Was the ping successful?

Yes.

### Step 3. Use the ping command to ping different IP addresses on the 127.0.0.0 network, 127.10.1.1, and 127.255.255.255. Were responses successful? If not, why?

All three were succesful.

## Task 2

### Step 1. Examine network IP address properties by Clicking on the PC > Desktop Tab> IP Configuration

- IPv4: 172.16.1.1
- Subnet Mask: 255.255.0.0
- Default Gateway: 172.16.255.254
- DNS Server: 192.168.254.254

### Step 2. To examine network properties using the ipconfig command click on the PC > Desktop Tab > Command Prompt. Then type ipconfig or ipconfig /all

```bash
C:\>ipconfig

Bluetooth Connection:(default port)

   Connection-specific DNS Suffix..: 
   Link-local IPv6 Address.........: ::
   IPv6 Address....................: ::
   IPv4 Address....................: 0.0.0.0
   Subnet Mask.....................: 0.0.0.0
   Default Gateway.................: ::
                                     0.0.0.0

FastEthernet0 Connection:

   Connection-specific DNS Suffix..: 
   Link-local IPv6 Address.........: FE80::201:64FF:FE65:8B74
   IPv6 Address....................: ::
   IPv4 Address....................: 172.16.1.1
   Subnet Mask.....................: 255.255.0.0
   Default Gateway.................: ::
                                     172.16.255.254
```

## Task 3

The issue was `FastEthernet0/0`, which was down. Also, the IP was wrong.

```bash
R2-Central>enable
R2-Central#configure terminal
Enter configuration commands, one per line.  End with CNTL/Z.
R2-Central(config)#interface FastEthernet0/0
R2-Central(config-if)#ip address 172.16.254.254 255.255.255.0
R2-Central(config-if)#no shutdown

R2-Central(config-if)#
%LINK-5-CHANGED: Interface FastEthernet0/0, changed state to up

%LINEPROTO-5-UPDOWN: Line protocol on Interface FastEthernet0/0, changed state to up
exit
R2-Central(config)#end
R2-Central#
%SYS-5-CONFIG_I: Configured from console by console
```

# Route

## Task 1

### Step 1. Access the command prompt. Click on a PC> Desktop Tab> Command Prompt

### Step 2. Type netstat -r to view the current routing table.

```bash
C:\>netstat -r

Route Table
===========================================================================
Interface List
0x1 ........................... PT TCP Loopback interface
0x2 ...00 16 6f 0d 88 ec ...... PT Ethernet interface
0x1 ........................... PT TCP Loopback interface
0x2 ...00 16 6f 0d 88 ec ...... PT Bluetooth interface
===========================================================================
===========================================================================
Active Routes:
Network Destination        Netmask          Gateway       Interface  Metric
          0.0.0.0          0.0.0.0   172.16.255.254      172.16.1.1       1
Default Gateway:      172.16.255.254
===========================================================================
Persistent Routes:
  None
```

## Task 2

### Step 1. Using the Command prompt as a Telnet client. Open the command prompt window by clicking on a PC > Desktop Tab> Command Prompt.  Next, type the command telnet followed by the IP address of the default gateway of the remote router (172.16.255.254).  The username required is ccna1, and the password is cisco.

```bash
C:\>telnet 172.16.255.254
Trying 172.16.255.254 ...Open
*****************************************************************
                This is Eagle 1 lab router R2-Central.
                Authorized access only.
*****************************************************************



User Access Verification

Username: ccna1
Password: 
R2-Central>
```

## Task 3

### Step 1. Explore privileged mode. Once logged into the remote router, type enable to enter privileged mode. The required password here is class. Once again you will not see the password while you type.

```bash
R2-Central>enable
Password: 
R2-Central#
```

### Step 2. Enter the command to display the router routing table. Use the show ip route command, to display a  much more detailed routing table then on a host computer. This is to be expected, because the job of a router is to route traffic between networks. How is IP mask information displayed in a router routing table?

```bash
R2-Central#show ip route
Codes: C - connected, S - static, I - IGRP, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter area
       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external type 2
       E1 - OSPF external type 1, E2 - OSPF external type 2, E - EGP
       i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
       * - candidate default, U - per-user static route, o - ODR
       P - periodic downloaded static route

Gateway of last resort is 10.10.10.6 to network 0.0.0.0

     10.0.0.0/30 is subnetted, 1 subnets
C       10.10.10.4 is directly connected, Serial0/0/0
C    172.16.0.0/16 is directly connected, FastEthernet0/0
S*   0.0.0.0/0 [1/0] via 10.10.10.6
```

It uses a slash to indicate the number of bits in the IP mask.

# Ping-Traceroute

## Task 1

### Step 1. Using PC-1A, Verify Network Layer. Use the command ipconfig at the command prompt to verify TCP/IP Network layer connectivity on the local host computer.

```bash
C:\>ipconfig

FastEthernet0 Connection:(default port)

   Connection-specific DNS Suffix..: 
   Link-local IPv6 Address.........: FE80::201:64FF:FE65:8B74
   IPv6 Address....................: ::
   IPv4 Address....................: 172.16.1.1
   Subnet Mask.....................: 255.255.0.0
   Default Gateway.................: ::
                                     172.16.255.254

Bluetooth Connection:

   Connection-specific DNS Suffix..: 
   Link-local IPv6 Address.........: ::
   IPv6 Address....................: ::
   IPv4 Address....................: 0.0.0.0
   Subnet Mask.....................: 0.0.0.0
   Default Gateway.................: ::
                                     0.0.0.0
```

The IP, mask, and gateway all look fine.

### Step 2. Ping Gateway. Verify TCP/IP Network layer connectivity on the LAN. Enter the command ping 172.16.255.254 to verify TCP/IP Network layer connectivity to the default gateway.

```bash
C:\>ping 172.16.255.254

Pinging 172.16.255.254 with 32 bytes of data:

Reply from 172.16.255.254: bytes=32 time<1ms TTL=255
Reply from 172.16.255.254: bytes=32 time<1ms TTL=255
Reply from 172.16.255.254: bytes=32 time<1ms TTL=255
Reply from 172.16.255.254: bytes=32 time<1ms TTL=255

Ping statistics for 172.16.255.254:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
    Minimum = 0ms, Maximum = 0ms, Average = 0ms
```

### Step 3. Ping Remote Host. Verify TCP/IP Network layer connectivity to a remote network. Enter the command ping 192.168.254.254 to verify TCP/IP Network layer connectivity to a device on a remote network. In this case the Eagle Server will be used.

```bash
C:\>ping 192.168.254.254

Pinging 192.168.254.254 with 32 bytes of data:

Request timed out.
Reply from 192.168.254.254: bytes=32 time=4ms TTL=126
Reply from 192.168.254.254: bytes=32 time=4ms TTL=126
Reply from 192.168.254.254: bytes=32 time=5ms TTL=126

Ping statistics for 192.168.254.254:
    Packets: Sent = 4, Received = 3, Lost = 1 (25% loss),
Approximate round trip times in milli-seconds:
    Minimum = 4ms, Maximum = 5ms, Average = 4ms

C:\>ping 192.168.254.254

Pinging 192.168.254.254 with 32 bytes of data:

Reply from 192.168.254.254: bytes=32 time=6ms TTL=126
Reply from 192.168.254.254: bytes=32 time=4ms TTL=126
Reply from 192.168.254.254: bytes=32 time=4ms TTL=126
Reply from 192.168.254.254: bytes=32 time=5ms TTL=126

Ping statistics for 192.168.254.254:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
    Minimum = 4ms, Maximum = 6ms, Average = 4ms
```

## Task 2

### Step 1. Verify Hops. Verify TCP/IP Network layer connectivity with the tracert command. Open the command prompt and enter the command tracert 192.168.254.254.

```bash
C:\>tracert 192.168.254.254

Tracing route to 192.168.254.254 over a maximum of 30 hops: 

  1   0 ms      0 ms      0 ms      172.16.255.254
  2   1 ms      0 ms      2 ms      10.10.10.6
  3   1 ms      2 ms      0 ms      192.168.254.254

Trace complete.
```

## Task 3

### Step 1. View Outputs. View the outputs of the traceroute command and compare them to what the command prompt displayed.

The three hops are as follows: PC 1A to R2-Central (through S1-Central), R2-Central to R1-ISP, R1-ISP to Server-PT.

# ICMP

## Task 1

### Step 1. Capture and evaluate ICMP echo messages to Eagle Server.

- Outbound PDU Details (Echo Request):
  - Type: 8
  - Code: 0

- Inbound PDU Details (Echo Reply):
  - Type: 0
  - Code: 0

### Step 2. Capture and evaluate ICMP echo messages to 192.168.253.1.

- Outbound PDU Details (Echo Request):
  - Type: 8
  - Code: 0

- Inbound PDU Details (Destination Unreachable):
  - Type: 3
  - Code: 1

### Step 3. Capture and evaluate ICMP echo messages that exceed the TTL value.

- Outbound PDU Details (Echo Request):
  - Type: 8
  - Code: 0

- Inbound PDU Details (Time Exceeded):
  - Type: 11
  - Code: 0

# ARP

## Task 1

### Step 1. Access the Command Prompt.

```bash
C:\>arp
Cisco Packet Tracer PC ARP
Display ARP entries: arp -a
Clear ARP table: arp -d
````

### Step 2. Use the ping command to dynamically add entries in the ARP cache.

```bash
C:\>ping 255.255.255.255
Ping request could not find host 255.255.255.255. Please check the name and try again.

C:\>arp -a
Internet Address  Physical Address  Type
172.16.255.254    0006.2aed.9e42    dynamic

C:\>
```
# MAC

## Task 1. Use the Telnet protocol to log into a Cisco Switch

### Step 1. Access Host PC-PT 1A Command Prompt.

Use the Command Prompt button from the Desktop Tab.

### Step 2. Use the Telnet command to access S1-Central.

Ping the switch's IP address first to resolve ARP. Then enter the command `telnet 172.16.254.1`. Packet Tracer does not support usernames on switches. When prompted, enter `cisco` as the password.

```bash
Cisco Packet Tracer PC Command Line 1.0
C:\>telnet 172.16.254.1
Trying 172.16.254.1 ...Open
*******************************************************************
This is Lab switch S1-Central.
Authorized access only.
*******************************************************************

User Access Verification

Password: 
S1-Central>
````

## Task 2. Use the Cisco IOS show mac-address-table command to examine MAC addresses and port associations

### Step 1. Examine the switch MAC address table.

The command `show mac-address-table ?` shows only the options supported by Packet Tracer.

### Step 2. Examine dynamic MAC address table entries.

The command `show mac-address-table` shows only the entry for the PC from which you accessed the switch. Using the IOS ping command to ping the router gateway and the other PCs will populate additional dynamic entries.

### Step 3. Examine the switch MAC address table.

The command `show mac-address-table aging-time` is not supported by Packet Tracer.

```bash
S1-Central>show mac-address-table 
Mac Address Table
-------------------------------------------

Vlan    Mac Address     Type       Ports
----    -----------    --------   -----
1       0001.6465.8b74 DYNAMIC    Fa0/1
1       0001.c923.2434 DYNAMIC    Fa0/22
1       0006.2aed.9e42 DYNAMIC    Fa0/24
1       0007.ec78.7d6b DYNAMIC    Fa0/2
1       0030.f29d.b162 DYNAMIC    Fa0/21
```
