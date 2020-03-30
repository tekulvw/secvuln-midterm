use std::convert::TryInto;

const ROWS: usize = 5;
const COLS: usize = 7;

type Matrix = [[u8; COLS]; ROWS];

fn init_matrix(matrix: &mut Matrix, default: i32) -> Result<(), i32> {
    matrix.iter_mut().try_for_each(
        |row| row.iter_mut().try_for_each(|v| {
            *v = match default.try_into() {
                Ok(val) => val,
                Err(_) => return Err(default),
            };
            Ok(())
        })
    )
}

fn main() {
    let mut table: Matrix = [[0; COLS]; ROWS];
    match init_matrix(&mut table, -1) {
        Ok(_) => println!("Ok!"),
        Err(val) => println!("Cannot convert {} to u8!", val),
    }
}
