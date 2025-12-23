"""
1. INSTALL
pip install pybind11 setuptools wheel

2. CLONE DEPENDENCIES (for local development)
git clone --depth 1 https://gitlab.com/libeigen/eigen.git extern/eigen
git clone --depth 1 https://github.com/xaliphostes/krylov.git extern/krylov

3. COMPILE
python ./setup.py build_ext --inplace

4. CLEAN
python ./setup.py clean --all
rm -f *.so *.pyd  # also remove generated binaries

5. WHEEL: create
python ./setup.py bdist_wheel

6. WHEEL: install
pip install --force-reinstall dist/pyarch3-*.whl

Note: Parallel compilation is enabled automatically (uses all CPU cores).
      Set NPY_NUM_BUILD_JOBS environment variable to override core count.
"""

from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext, ParallelCompile
from glob import glob
import os

# Enable parallel compilation using all available cores
# Can be overridden with NPY_NUM_BUILD_JOBS environment variable
ParallelCompile("NPY_NUM_BUILD_JOBS").install()

# Find all .cxx files in src/ recursively
src_files = glob("../../arch3/src/**/*.cxx", recursive=True)

# Get include directories from environment variables (for CI) or use local paths
arch3_include = os.environ.get('ARCH3_INCLUDE_DIR', '../../arch3/include')
eigen_include = os.environ.get('EIGEN_INCLUDE_DIR', '../../arch3/extern/eigen')
krylov_include = os.environ.get('KRYLOV_INCLUDE_DIR', '../../arch3/extern/krylov/include')
rosetta_include = os.environ.get('ROSETTA_INCLUDE_DIR', '../../rosetta/include')

ext_modules = [
    Pybind11Extension(
        "pyarch3",
        ["binding-simple.cxx"] + src_files,
        cxx_std=20,
        include_dirs=[arch3_include, rosetta_include, krylov_include, eigen_include],
        define_macros=[("EIGEN_HAS_STD_RESULT_OF", "0"), ("USING_SETUP_PY", "1")]
    ),
]

setup(
    name="pyarch3",
    version="1.0.0",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)