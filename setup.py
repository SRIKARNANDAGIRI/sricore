from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

root = Path(__file__).parent.resolve()

ext_modules = [
    Pybind11Extension(
        "_srichakra",
        [
            "bindings/python_bindings.cpp",
            "src/complexity.cpp",
            "src/engine.cpp",
        ],
        include_dirs=["include"],
        cxx_std=20,
    ),
]

setup(
    name="srichakra",
    version="0.1.0",
    description="Graph complexity engine",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.8",
    install_requires=["pybind11>=2.13"],
)
