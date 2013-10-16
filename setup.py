#!/usr/bin/env python

import os

from setuptools import setup, Extension

mapnik_args = os.popen('mapnik-config --cflags').read()
#mapnik_args = mapnik_args.replace(' -O3', '')
#mapnik_args = mapnik_args.replace(' -O2', '')


setup(
    name='mapnik_vector_tiles',
    version='0.0.1',
    author='Blake Thompson',
    #packages=['MVT'],
    ext_modules=[
        Extension('mapnik_vector_tiles',
            ['bindings/python/vector_tile.cpp', 'src/vector_tile.pb.cc'],
            include_dirs=['/usr/include/python2.6', 'src'],
            library_dirs=['/usr/local/lib', '/usr/lib', '/usr/lib64'],
            libraries=['boost_python', 'mapnik', 'icuuc', 
                    'boost_filesystem', 'boost_system', 'boost_thread', 'cairo',
                    'xml2', 'boost_regex', 'protobuf-lite'],
            extra_compile_args=['-fpic', '-Wsign-compare', mapnik_args,]
        )
    ],
)   
