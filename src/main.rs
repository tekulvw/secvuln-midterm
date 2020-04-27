use pnet::packet::icmp::{echo_request::MutableEchoRequestPacket, IcmpCode, IcmpPacket, IcmpTypes};
use pnet::packet::ip::IpNextHeaderProtocols;
use pnet::packet::ipv4::MutableIpv4Packet;
use pnet::packet::Packet;
use pnet::transport::{icmp_packet_iter, transport_channel, TransportChannelType::Layer3};

use rand;

use std::net;
use std::str::FromStr;

const IPV4_HEADER_LEN: usize = 21;
const ICMP_HEADER_LEN: usize = 8;
const ICMP_PAYLOAD_LEN: usize = 32;

fn attempt_false_icmp(msg: &String) -> Result<(), ()> {
    let protocol = Layer3(IpNextHeaderProtocols::Icmp);
    let (mut tx, mut rx) = transport_channel(2048, protocol).unwrap();

    let mut rx = icmp_packet_iter(&mut rx);

    let ip_addr = net::Ipv4Addr::from_str("174.138.54.196").unwrap();

    let mut ip_buffer = [0u8; 100];
    let mut icmp_buffer = [0u8; 50];

    // println!("icmp buffer size: {}", icmp_buffer_size);

    let mut ipv4_packet = MutableIpv4Packet::new(&mut ip_buffer).unwrap();
    ipv4_packet.set_version(4);
    ipv4_packet.set_header_length(IPV4_HEADER_LEN as u8);
    ipv4_packet.set_total_length((IPV4_HEADER_LEN + ICMP_HEADER_LEN + ICMP_PAYLOAD_LEN) as u16);
    ipv4_packet.set_ttl(200);
    ipv4_packet.set_next_level_protocol(IpNextHeaderProtocols::Icmp);
    ipv4_packet.set_destination(ip_addr);

    let xor_key: u8 = rand::random();
    let data: Vec<u8> = msg.as_bytes().iter().map(|b| b ^ xor_key).collect();

    println!(
        "Orig: {}\nXOR Key: {}\nEncoded: {:?}",
        msg,
        xor_key,
        data.as_slice()
    );

    let mut icmp_packet = MutableEchoRequestPacket::new(&mut icmp_buffer).unwrap();
    icmp_packet.set_icmp_type(IcmpTypes::EchoRequest);
    // icmp_packet.set_identifier(xor_key as u16);
    icmp_packet.set_payload(&data);

    let checksum = pnet::util::checksum(&icmp_packet.packet(), 2);
    icmp_packet.set_checksum(checksum);

    ipv4_packet.set_payload(icmp_packet.packet());

    tx.send_to(ipv4_packet, net::IpAddr::V4(ip_addr)).unwrap();

    println!("sent!");
    Err(())
}

fn main() {
    let msg = String::from("hello noah test test test");
    match attempt_false_icmp(&msg) {
        Ok(_) => print!("Sent Fake ICMP packet with msg {}", msg),
        _ => (),
    }
}
