QT based version of High High School Adventure
To play the game, you need to have data files.


1. build and installation

This depends on some packages in Ubuntu and qt6, run following installation before build.

  $ sudo apt install -y build-essential cmake qt6-base-dev qt6-tools-dev qt6-multimedia-dev libsdl2-dev

Then build as below:

  $ mkdir build
  $ cd build
  $ cmake ..
  ...
  $ make
  ...
  $ ./qhhsadv &

As described first, this requires data files, expected located under ~/.HHSAdv directory.
