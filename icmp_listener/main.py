###
### ICMP Listener
### Dumps source address, timestamp, and packet data into neatly organized files.
###
### Note: requires root to run
###

import os
import socket
import time
import struct

from scapy.all import IP, ICMP, raw

# Add any IP addresses to specially flag as interesting
# Example 1: Flag all packets from 129.137.4.132 (gauss.ececs.uc.edu)
# Example 2: Flag all packets from 129.*.*.* (the UC network)
IPS_TO_FLAG = ["129.137.4.132", "129"]


def log_packet(file_name, address, unix_timestamp, icmp_data):
    with open(file_name, "a+") as file:
        to_write = (
            f"Source Address: {address} | Timestamp: {unix_timestamp}\n{icmp_data}\n\n"
        )
        file.write(to_write)


## Handle ICMP packets with type 8
## Send back packet to sender with type 0 (this is the ICMP echo)
def listen():
    s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
    s.setsockopt(socket.SOL_IP, socket.IP_HDRINCL, 1)
    while True:
        unix_timestamp = int(time.time())
        data, addr = s.recvfrom(1508)
        print(f"Got packet from {addr[0]} at {unix_timestamp}:")  # {data.hex()}")

        packet = IP(data)
        # print(packet.summary())
        # print(packet.show())

        icmp = packet[ICMP]
        icmp_type = icmp.type
        icmp_code = icmp.code
        icmp_chksum = icmp.chksum
        icmp_seq = icmp.seq
        # print(icmp)
        # print(icmp_type)
        # print(icmp_code)
        # print(icmp_chksum)
        # print(icmp_seq)

        raw_icmp = raw(icmp)
        key = int.from_bytes(raw_icmp[8:9], "big")
        print("key:", key)
        payload = raw_icmp[9:]

        def decode(byte):
            return byte ^ key if byte != key and byte != 0 else byte

        decoded = [decode(byte) for byte in payload]
        # print(decoded)
        try:
            decoded = decoded[: decoded.index(0)]
        except ValueError:
            pass

        decoded_string = "".join(chr(ch) for ch in decoded)

        # print(decoded)
        print(decoded_string)

        # Bucket files by the hour
        unix_hour = (
            unix_timestamp // (60 * 60) * (60 * 60)
        )  # hour that the timestamp is in (rounded down)
        file_name = f"icmp-listener-{unix_hour}.txt"

        # Save packet data in timestamped files for a specific window of time
        # This is not the best way to do this, but whatever it works
        log_packet(file_name, addr[0], unix_timestamp, decoded_string)

        # Check if packet should be specially flagged
        if addr[0].startswith(tuple(IPS_TO_FLAG)):
            # Also log this packet to the flagged datafile
            special_file_name = f"flagged-icmp-listener-{unix_hour}.txt"
            log_packet(special_file_name, addr[0], unix_timestamp, decoded_string)


if __name__ == "__main__":
    print("IPs to flag:", IPS_TO_FLAG)
    listen()  # needs root access, test with `ping 127.0.0.1`
    print("Listener is waiting for incoming pings..")
