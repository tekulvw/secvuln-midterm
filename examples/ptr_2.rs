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

fn main() {
    let victim_handle = spawn(cstyle_victim);
    println!("Type something:");
    victim_handle.join().unwrap();
    let attacker_handle = spawn(attacker);
    attacker_handle.join().unwrap();
}