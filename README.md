# Neon Project

[![GitHub release](https://img.shields.io/github/release/kenkit/neon.svg)](https://github.com/kenkit/neon/releases/latest)

[![Build status](https://ci.appveyor.com/api/projects/status/dapr240j1ignrsrl?svg=true)](https://ci.appveyor.com/project/kenkit/neon)

[![codecov](https://codecov.io/gh/kenkit/neon/branch/master/graph/badge.svg)](https://codecov.io/gh/kenkit/neon)

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5790aa30048346f99e3342b84a2fec8a)](https://app.codacy.com/app/kenkit/neon?utm_source=github.com&utm_medium=referral&utm_content=kenkit/neon&utm_campaign=Badge_Grade_Dashboard)

Neon is a project designed by Neon Itelligence.

## Features

* Video Playback with FFMPEG.
* File download manager.
* Everything is a WIP atm.
* To Do fix readme

## Installation

Just download the zip file from the release tab on our github repo.

We are working on an automated updater

## Plugins

This project uses a number of plugins

* Most of the features require installation of Neon Service 

## Development

Want to contribute? Great!

Just create a pull request, we will be happy to integrate new features.

## Building for source

To build Neon you must have cmake, the rest is almost automated
```
mkdir build
cd build
cmake ../ -DCLONE_REPO=TRUE
```

Generating prebuilt zip archives for distribution:
``` 
cmake --build ../build

```

## Todos

* Write test system
* Lot's of work

## License

MIT

## Free Software, Hell Yeah!

   [ogre3d](https://github.com/ogreCave/ogre)

   [ffmpeg](https://github.com/FFmpeg/ffmpeg)

   [libcurl](https://github.com/curl/curl)

   [json](https://github.com/nlohmann/json)

   [simplewebsocketserver](https://github.com/eidheim/Simple-WebSocket-Server)
