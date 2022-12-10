nview - nano view
=================

| **Linux** | **Mac** |
|-----------|---------|
| [![Linux](https://github.com/d99kris/nview/workflows/Linux/badge.svg)](https://github.com/d99kris/nview/actions?query=workflow%3ALinux) | [![macOS](https://github.com/d99kris/nview/workflows/macOS/badge.svg)](https://github.com/d99kris/nview/actions?query=workflow%3AmacOS) |

nview is a minimalistic image viewer, implemented in C++ / Qt.

![screenshot](/res/screenshot.png) 

Usage
=====
Usage:

    nview OPTION
    nview PATH...

Command-line Options:

    -h, --help        display this help and exit
    -v, --version     output version information and exit
    PATH              file(s) or directory(s) to display

Command-line Examples:

    nview ~/Pictures  show all images in ~/Pictures
    nview hello.jpg   show hello.jpg

Interactive Commands:

    SPACE,right       next image
    BACKSPACE,left    previous image
    ESC,q             exit
    F11,f             full screen
    r                 rotate clockwise
    R                 rotate counter-clockwise
    DEL,d             delete
    m                 move to directory
    c                 copy to directory

Supported Platforms
===================
nview is primarily developed and tested on macOS, but basic functionality should work in Linux
as well.

Linux / Ubuntu
==============

**Dependencies**

    sudo apt install qt5-default qt5-qmake imagemagick

**Source**

    git clone https://github.com/d99kris/nview && cd nview

**Build**

    mkdir -p build && cd build && qmake .. && make -s

**Install**

    sudo make -s install

macOS
=====

**Dependencies**

    brew install imagemagick qt

**Source**

    git clone https://github.com/d99kris/nview && cd nview

**Build**

    mkdir -p build && cd build && qmake .. && make -s

**Installation**

    make install

License
=======
nview is distributed under GPLv2 license. See LICENSE file.

Keywords
========
image viewer, linux, macos.
