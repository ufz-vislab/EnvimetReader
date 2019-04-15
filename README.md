## Build instructions

Build ParaView via super-build (macOS):

```bash
brew instal qt
ccmake ../super -DCMAKE_OSX_SDK=macosx10.14 -DQt5_DIR=/usr/local/Cellar/qt/5.12.2//lib/cmake/Qt5 -DBUILD_TESTING=OFF -DUSE_SYSTEM_qt5=ON -DENABLE_qt5=ON -G Ninja
ninja
```

Build plugin:

```bash
cmake ../envimet -DQt5_DIR=/usr/local/Cellar/qt/5.12.2//lib/cmake/Qt5 -DParaView_DIR=../super-build/superbuild/paraview/build
make
```
