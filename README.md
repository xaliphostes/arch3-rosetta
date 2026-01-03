# Arch - Rosetta

Using [Rosetta](https://github.com/xaliphostes/rosetta.git), binds [Arch3](https://github.com/xaliphostes/arch3.git) in
- Python
- JavaScript
- Wasm
- REST API
- ...

## Install

### 1. Generate the binary for your lib
- Go to the generator folder
- Then
    ```sh
    mkdir build && cd build
    cmake ..
    make
    cd ..
    ```
It will generate `arch3_generator(.exe)` at the root of this project

### 2. Generate the `project.json`
Type
```sh
./arch3_generator --init
```
to generate the `project.json` file and then edit to make the necessary changes (includes, libs, name, etc...)

### 3. Generate the generators for python, javascript, wasm, typescript, rest-api...
```sh
./arch3_generator project.json
```
This will create a new folder `generated` with inside several folders:
- `javascript`
- `python`
- `wasm`
- `rest`

### 4. Finally, compile each binding
Go to each folder and read the README for compilation.

As an example, for **Python** you will do
```sh
cd generated/python
mkdir build && cd build
cmake .. && make
```
At the end of the compilation, you will endup with a **Python** library that uses your **dynamic C++** library.

## License
MIT

## Author
[xaliphostes](https://github.com/xaliphostes)