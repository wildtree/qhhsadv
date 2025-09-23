# About this game

QT based version of High High School Adventure
To play the game, you need to have data files.


# How to build and play

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

As described first, this requires data files, expected located under your AppLocalDataLocation, for Linux it should be ~/.local/share/WildTreeJP/QHHSAdv directory, for Windows, it should be $env:LOCALAPPDATA\WildTreeJP/QHHSAdv.

Copy all files including themes directory and its contents under the data folder.

# Limitations

Theme mode 'System' works if you build with Qt 6.5.0 or later version.
Otherwise, it is same as 'Light'.

