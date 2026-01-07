# arch3-rosetta

Using [Rosetta](https://github.com/xaliphostes/rosetta.git) to bind [Arch3](https://github.com/xaliphostes/arch3.git) in
- Python
- JavaScript
- Wasm
- REST API
- ...

## Prerequisites

- CMake 3.16+
- C++20 compatible compiler
- Python 3.8+ (for Python bindings)

## Building

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build the generator
make

# Run the generator
cd ..
./arch3_rosetta_generator project.json
```

## Generated Bindings

This project generates bindings for: python, wasm, javascript, rest

After running the generator, you'll find the generated code in the `generated/` directory.

### Python Bindings

```bash
cd generated/python
pip install .
```

Then in Python:

```python
import arch3
# Use your bound classes and functions
```

## Customizing Bindings

Edit `bindings/arch3_rosetta_registration.h` to register your C++ classes and functions.

See the [Rosetta documentation](https://github.com/xaliphostes/rosetta) for more information on registration macros.

## License

MIT
