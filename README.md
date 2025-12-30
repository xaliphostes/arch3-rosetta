# Arch - Rosetta

Using [Rosetta](https://github.com/xaliphostes/rosetta.git), binding [Arch3](https://github.com/xaliphostes/arch3.git) in
- Python
- JavaScript
- Wasm
- REST API
- ...

## Install

### Generate the binary for your lib
- Go to the generator folder
- Then
    ```sh
    mkdir build && cd build
    cmake ..
    make
    cd ..
    ```
### Generate the `project.json`
```sh
./generator/build/arch3_generator --init
```
and then edit this generated `project.json` file to make the necessary changes (includes, libs, name, etc...)

### Generate the generators (for python, javascript, wasm, typescript, rest-api)
```sh
./generator/build/arch3_generator project.json
```
This will create a new folder `generated` with inside several folders:
- `javascript`
- `python`
- `wasm`
- `rest`

### Finally, compile each binding
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