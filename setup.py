from setuptools import setup, Extension

module = Extension(
    "simple_graphs",
    sources=["simple_graphs.cpp"],
)

setup(
    name="simple_graphs",
    version="1.0",
    ext_modules=[module]
)