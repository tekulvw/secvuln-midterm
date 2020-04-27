# Security Vulnerability Assessment Midterm

Authors: Noah Bass, Brendan Link, Will Tekulve

## Description

We have created a bit of malware that uses the Rust standard library and a handful of external packages to read private ssh files (known hosts, config, public and private keys) and transmit that data through ICMP ping requests to a listener server.
This is only possible because ICMP type 8 and 0 (echo request and echo reply) have a variable length data section in the packet format from the ICMP RFC.
This technique can be used to perform what's known as "[ICMP Tunneling](https://en.wikipedia.org/wiki/ICMP_tunnel)" where request and response data can be transmitted between two hosts without the use of TCP or UDP.

We are using this data section to transmit data from private ssh files in 25 byte chunks and we've also added an XOR key to perform basic obfuscation in an attempt to remain undetected.
The XOR key is transmitted as the sequence number in the ICMP packet and is regenerated for each chunk.

The only issue here is that creating raw ICMP packets like this requires us to have root access.
Luckily, you, Dr. Franco, have `NOPASSWD` in your sudoers file so we've added a bit of code to automatically create a child process and run it with `sudo`.
We were originally going to attempt to hide this bit of malware in the example Rust code that mimics your example C code, but decided we didn't have enough time before the grading period to attempt to get you to run this code before informing you of the code's existence.

## ICMP RFC 792 for Echo Request and Echo Reply

Source: [https://tools.ietf.org/html/rfc792](https://tools.ietf.org/html/rfc792)

```
Echo or Echo Reply Message

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Identifier          |        Sequence Number        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Data ...
   +-+-+-+-+-

   IP Fields:

   Addresses

      The address of the source in an echo message will be the
      destination of the echo reply message.  To form an echo reply
      message, the source and destination addresses are simply reversed,
      the type code changed to 0, and the checksum recomputed.

   IP Fields:

   Type

      8 for echo message;

      0 for echo reply message.

   Code

      0

   Checksum

      The checksum is the 16-bit ones's complement of the one's
      complement sum of the ICMP message starting with the ICMP Type.
      For computing the checksum , the checksum field should be zero.
      If the total length is odd, the received data is padded with one
      octet of zeros for computing the checksum.  This checksum may be
      replaced in the future.

   Identifier

      If code = 0, an identifier to aid in matching echos and replies,
      may be zero.

   Sequence Number
```

## Malware Build Process

NOTE: This code can only be built for a UNIX system!
If you are attempting to send the ICMP packets to your own listener, make sure to change the IP of the listener in the source code.

A precompiled binary is located in the root project directory named `midterm`.
Be careful when running this code, your ssh files will be sent to our listener server!

After installing rust and setting the correct listener IP, run the following command from the root project directory:

```sh
cargo run
```

### Rust Installation

Please follow the [official rust guide](https://www.rust-lang.org/tools/install) for Rust installation.
This process is extremely quick!

## Running Rust Code Examples

You can also run the examples of C code we've made!
To do this, identify the example in the `examples` folder you want to run and execute:

```sh
cargo run --example EXAMPLE_NAME_NO_EXTENSION
```

This command will run the example code **WITHOUT** running the malware section!

## ICMP Listener Installation

Requires python >=3.8.
Run `pip install icmp_listener/requirements.txt` to install the dependency (scapy), then `python icmp_listener/main.py` to run the listener (note: needs root access).

## Parsing the Listener Log Files to Get SSH Keys

The listener aggregates together logs by the hour that the icmp packet was recieved. For example, packet data recieved at `1588011351` would be placed into the `icmp-listener-1588010400.txt` log file. Log file entries look like this:

```
Source Address: 72.49.1.96 | Timestamp: 1588011351
xDII/dIZK3pKPy9cAIsCgYEA0
```

Once the log files have been written, we can use a series of `grep` and `awk` commands to piece together the SSH key data from the individual ICMP packets.

A full example of piecing together SSH public and private keys from ICMP packet can be found in the `example-icmp-dump/` folder.

An example pieced togther SSH public key from `example-icmp-dump/` (note: this SSH key is not used for anything):

```
tekulvw@@@@@/home/tekulvw
/.ssh/id_rsa.pubssh-rsa A
AAAB3NzaC1yc2EAAAADAQABAA
ABAQCzNynoKB4Mpm4qvfDUg8/
5muXb0he/tyh9bpAzMMU8Gs4F
FGTry121HrAjG+anYk6DgKDt9
NdNznLknV1lzFq/nbPkH1lHKD
+zpSEanvZZU6hbprI2OItABvm
M8IoegphwygJtC02mWw5JP38g
Jt1m/5pFjpzaKOpe4mpWI6q4d
KrkQBrTlukxJv9Y37gnBE96kX
d6e+JdbCFJk3DEAtFGyZisUYI
OvWK9hLWQhtDhYgaG8ghfqX2z
DE9zfvB0ciyj2dWcEuBuUAyM1
r9PI9aQm+aPa3AMZS1RP75L4a
HnPjwhxI+bYiyiWwVvHELm5sH
v1q6M5FSR/5uGfHFdOZFT tek
ulvw@tekulvw-VirtualBox
```
