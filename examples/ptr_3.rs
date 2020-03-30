const TABLE_SIZE: usize = 100;

/// Because I'm checking against TABLE_SIZE instead of slice.len() I'll mark the entire
/// function as unsafe. This CAN index beyond the end of an array! This goes to show we
/// can do _everything_ C can, just as unsafely!
unsafe fn try_index<A>(slice: &[A], index: usize) -> Result<*const A, usize> {
    // should really use "if index < slice.len() {" here instead for more safety!
    if index < TABLE_SIZE + 10 {
        // .add() here indexes by a number of elements rather than a number of bytes.
        // unsafe is explicitly required to use the add function!
        let ptr = slice.as_ptr().add(index);
        return Ok(ptr);
    }
    Err(index)
}

fn better_try_index<A>(slice: &[A], index: usize) -> Result<&A, usize> {
    if index < slice.len() {
        return Ok(&slice[index]);
    }
    Err(index)
}

fn main() {
    let mut table: [i32; TABLE_SIZE] = [0; TABLE_SIZE];
    table.iter_mut().fold(0, |acc, v| {
        *v = 2*acc+1;
        acc + 1
    });

    // I cannot index negative times (ever) because I'm not allowed to use binary
    // operators on raw pointers, and Rust's type checker requires me to have a
    // "usize" type when working with the .add() function.
    let ptr = unsafe { try_index(&table, 100) }.unwrap();
    println!("ptr val: {}", unsafe { ptr.read() });

    match better_try_index(&table, 100) {
        Ok(val) => println!("better val: {}", val),
        Err(val) => println!("error trying to get index: {}", val),
    }

    // This edge case is detectable at compile time and will not work.
    // println!("{}", table[-1]);
}