# Neon Project

[![GitHub release](https://img.shields.io/github/release/kenkit/neon.svg)](https://github.com/kenkit/neon/releases/latest)
[![Build status](https://ci.appveyor.com/api/projects/status/dapr240j1ignrsrl?svg=true)](https://ci.appveyor.com/project/kenkit/neon)
[![Build Status](https://travis-ci.org/kenkit/neon.svg?branch=master)](https://travis-ci.org/kenkit/neon)
[![codecov](https://codecov.io/gh/kenkit/neon/branch/master/graph/badge.svg)](https://codecov.io/gh/kenkit/neon)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5790aa30048346f99e3342b84a2fec8a)](https://app.codacy.com/app/kenkit/neon?utm_source=github.com&utm_medium=referral&utm_content=kenkit/neon&utm_campaign=Badge_Grade_Dashboard)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/kenkit/neon.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/kenkit/neon/context:cpp)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/kenkit/neon.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/kenkit/neon/alerts/)
[![CodeFactor](https://www.codefactor.io/repository/github/kenkit/neon/badge)](https://www.codefactor.io/repository/github/kenkit/neon)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/2473/badge)](https://bestpractices.coreinfrastructure.org/projects/2473)
[![code-size](https://img.shields.io/github/languages/code-size/kenkit/neon.svg?color=44cc11)](https://img.shields.io/github/languages/code-size/kenkit/neon.svg?color=44cc11)
![GitHub All Releases](https://img.shields.io/github/downloads/kenkit/neon/total?style=plastic)

Neon was designed by Neon Itelligence.

## Features

-   Video Playback with FFMPEG.
-   File download manager.
-   Everything is a WIP atm.

## Screenshots

<img src="https://i.imgur.com/NxbK0nY.gif"  style="max-width: 100%; min-height: 376px;" />

<img src="https://aws1.discourse-cdn.com/standard10/uploads/dearimgui/optimized/1X/db6f749f230bd57996d7accbfa025a3cb0cf2fb0_2_690x457.gif" width="690" height="455" />

## Installation

Just download the zip file from the release tab on our github repo.

We are working on an automated updater

## Plugins

This project uses a number of plugins

-   Most of the features require installation of Neon Service [Neon_Service](https://github.com/kenkit/neon_service)

## Development

Want to contribute? Great!

Just create a pull request, we will be happy to integrate new features.

Also include your codewars profile badge  in the readme.

## Developers

[![KENKIT](https://www.codewars.com/users/kenkit/badges/large)](https://www.codewars.com/users/kenkit)

<img src="https://wakatime.com/share/@Sage/248720b9-6ccd-4cf8-a45d-0801fa388c3b.svg"  width="528" height="396" />

## Building for source

To build Neon you must have cmake, the rest is almost automated

    mkdir build
    cd build
    cmake ../ -DCLONE_REPO=TRUE

Generating prebuilt zip archives for distribution:

    cmake --build ../build --target neon

## Todos

-   Merge the original branch into this one.

## License

MIT

## Software used in this project

   [SDL](https://hg.libsdl.org/SDL)

   [CEF](https://bitbucket.org/chromiumembedded/cef)
