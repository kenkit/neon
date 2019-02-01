# Neon Project

[![GitHub release](https://img.shields.io/github/release/kenkit/neon.svg)](https://github.com/kenkit/neon/releases/latest)
[![Build status](https://ci.appveyor.com/api/projects/status/dapr240j1ignrsrl?svg=true)](https://ci.appveyor.com/project/kenkit/neon)
[![Build Status](https://travis-ci.org/kenkit/neon.svg?branch=master)](https://travis-ci.org/kenkit/neon)
[![codecov](https://codecov.io/gh/kenkit/neon/branch/master/graph/badge.svg)](https://codecov.io/gh/kenkit/neon)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5790aa30048346f99e3342b84a2fec8a)](https://app.codacy.com/app/kenkit/neon?utm_source=github.com&utm_medium=referral&utm_content=kenkit/neon&utm_campaign=Badge_Grade_Dashboard)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/kenkit/neon.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/kenkit/neon/context:cpp)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/kenkit/neon.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/kenkit/neon/alerts/)

Neon was designed by Neon Itelligence.

## Features

-   Video Playback with FFMPEG.
-   File download manager.
-   Everything is a WIP atm.

## Installation

Just download the zip file from the release tab on our github repo.

We are working on an automated updater

## Plugins

This project uses a number of plugins

-   Most of the features require installation of Neon Service 

## Development

Want to contribute? Great!

Just create a pull request, we will be happy to integrate new features.

## Building for source

To build Neon you must have cmake, the rest is almost automated

    mkdir build
    cd build
    cmake ../ -DCLONE_REPO=TRUE

Generating prebuilt zip archives for distribution:

    cmake --build ../build --target neon

## Todos

-   Write Test system
-   See HTML in the right

## License

MIT

## Software used in this project

   [SDL](https://hg.libsdl.org/SDL)

   [CEF](https://bitbucket.org/chromiumembedded/cef)
