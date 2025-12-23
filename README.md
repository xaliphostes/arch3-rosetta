# Arch - Rosetta

Using [Rosetta](https://github.com/xaliphostes/rosetta.git), binding [Arch3](https://github.com/xaliphostes/arch3.git) in
- Python
- JavaScript
- Wasm
- ...

## Compilation

### Python
Go the `python` folder.

Create a venv and activate
```sh
python3.12 -m venv v312
source venv312/bon/activate
```

Then,
```sh
# Install the necessay packages if necessary
pip install pybind11 setuptools wheel

# Compile
python ./setup.py build_ext --inplace

# Create the wheel
python ./setup.py bdist_wheel

# To install the wheel
pip install --force-reinstall dist/NAME-OF-THE-WHEEL.whl

# Testing
python test.py
```

### JavaScript
TODO

### Wasm
TODO

## License
MIT

## Author

[xaliphostes](https://github.com/xaliphostes)