###
### ICMP Listener
### Dumps source address, timestamp, and packet data into neatly organized files.
###
### Note: requires root to run
### Note: echo 1 > /proc/sys/net/ipv4/icmp_echo_ignore_all
###

import os
import socket
import time
import struct

from scapy.all import IP, ICMP

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
        print(f"Got packet from {addr} at {unix_timestamp}: {data.hex()}")

        data = data[56:]

        unpack_payload(data)

        # Example packet:
        # b'E\x00@\x00\x9c+\x00\x00@\x01\x00\x00\x7f\x00\x00\x01\x7f\x00\x00\x01\x00\x00\x95\xdd8\xc2\x00\x00^\xa63c\x00\x08\xb4K\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !"#$%&\'()*+,-./01234567'
        code = int(data[0])
        icmp_type = int(data[1])
        icmp_checksum = data[2:3]
        icmp_data = data[3:]

        print(type(data))
        print(type(icmp_data))

        # Custom Packet Layout
        # identifier - contains XOR key for data
        # identifier = icmp_data[:2]  # first 16 bits of 'rest of header'

        # sequence number
        # sequence_number = icmp_data[2:4]  # second 16 bits of 'rest of header'

        # payload data
        # payload_data = icmp_data[4:]  # rest of packet data

        # XOR identifier with the payload_data
        # xored_bytes = [
        #     int(identifier) ^ int(payload_data_byte)
        #     for payload_data_byte in payload_data
        # ]

        # print(xored_bytes.decode("ascii"))

        # print('code:', code)
        # print('type:', icmp_type)
        # print('checksum:', icmp_checksum)
        # print('icmp_data:', icmp_data)

        # Bucket files by the hour
        unix_hour = (
            unix_timestamp // (60 * 60) * (60 * 60)
        )  # hour that the timestamp is in (rounded down)
        file_name = f"icmp-listener-{unix_hour}.txt"

        # Save packet data in timestamped files for a specific window of time
        # This is not the best way to do this, but whatever it works
        log_packet(file_name, addr[0], unix_timestamp, icmp_data)

        # Check if packet should be specially flagged
        if addr[0].startswith(tuple(IPS_TO_FLAG)):
            # Also log this packet to the flagged datafile
            special_file_name = f"flagged-icmp-listener-{unix_hour}.txt"
            log_packet(special_file_name, addr[0], unix_timestamp, icmp_data)


def unpack_payload(data):
    icmp_type, code, checksum, identifier, sequence = struct.unpack("!ccHHH", data[:8])
    print(
        f"code: {code}, type: {icmp_type}, checksum: {checksum} identifier: {identifier}, sequence: {sequence}"
    )
    payload = data[8:]

    decoded = [byte ^ int(identifier) for byte in payload]

    print(f"decoded: {decoded}")

    return identifier, decoded


if __name__ == "__main__":
    print("hello, world.")
    print("IPs to flag:", IPS_TO_FLAG)
    listen()  # needs root access, test with `ping 127.0.0.1`
    print("Listener is waiting for incoming pings..")
