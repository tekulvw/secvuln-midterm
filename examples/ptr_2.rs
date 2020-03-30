use std::io;
use std::thread::{spawn, sleep};
use std::time;

// Rule #1: Don't have static mutable variables in rust
static mut SAVE: String = String::new();

fn cstyle_victim() {
    let stdin = io::stdin();

    // Obviously this works, but also obviously it's a really bad idea! The
    // "unsafe" block is required here because the compiler knows that this code
    // could produce memory safety issues. It's up to the programmer at this point
    // to guarantee those issues don't exist.
    unsafe {
        stdin.read_line(&mut SAVE);
        println!("victim (0x{:x}): {}", SAVE.as_ptr() as usize, SAVE);
    }
}

/// The Rust (tm) way is to pass ownership into functions when needed instead of having
/// global variables. You can see there are no "unsafe" blocks in this code!
fn rust_victim(mut buf: String) {
    let stdin = io::stdin();
    stdin.read_line(&mut buf);
    println!("victim (0x{:x}): {}", buf.as_ptr() as usize, buf);
}

fn attacker() {
    unsafe {
        println!("attacker (0x{:x}): {:X?}", SAVE.as_ptr() as usize, SAVE.as_bytes());
    }
}

fn rust_attacker(buf: String) {
    unsafe {
        println!("attacker (0x{:x}): {:X?}", buf.as_ptr() as usize, buf.as_bytes());
    }
}

fn main() {
    // C - style
    let victim_handle = spawn(cstyle_victim);
    println!("Type something:");
    victim_handle.join().unwrap();
    let attacker_handle = spawn(attacker);
    attacker_handle.join().unwrap();

    /*
    // Rust style
    let buf = String::new();
    let victim_handle = spawn(|| rust_victim(buf));
    println!("Type something:");
    victim_handle.join().unwrap();

    // Rust will not let this compile since ownership of "buf" was moved into the victim
    // thread! In this case, the victim function can require that the data it operates on
    // is only owned by itself, guaranteeing memory safety!
    rust_attacker(buf);
    */
}