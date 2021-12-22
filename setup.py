from setuptools import setup, find_packages
from pybind11.setup_helpers import Pybind11Extension


ext_modules = [
    Pybind11Extension("timerpp", ["python_binding.cpp"])
]


setup(
    long_description=open("README.md", "r").read(),
    name="timerpp",
    version="0.1",
    description="timer class",
    author="Pascal Eberlein",
    author_email="pascal@eberlein.io",
    url="https://github.com/nbdy/timerpp",
    classifiers=[
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
    ],
    keywords="timer soft-timer",
    packages=find_packages(),
    long_description_content_type="text/markdown",
    ext_modules=ext_modules
)
