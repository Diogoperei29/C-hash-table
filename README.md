# Hash Table in C

![Language](https://img.shields.io/badge/language-C-blue) [![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](https://opensource.org/licenses/Apache-2.0)

An implementation of a hash table in C, built using GCC and CMake.

## Project Structure

```
src/
├── hash_table.c   # Implementation of the hash table
├── hash_table.h   # Header file for hash table
├── main.c         # Entry point
├── prime.c        # Implementation of prime functions
├── prime.h        # Header file for prime functions
```

## Build & Run

### Prerequisites
- GCC
- CMake (>= 3.10)
- Make

### Build
```bash
mkdir build
cd build
cmake ..
make
```

### Run
```bash
./hash_table
```


## To-Do

- Resizing
- Better collision handling (separate chaining?)