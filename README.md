# Simple example on `Geant4`

Here you have the source code of a very basic `Geant4` application to give you some exposure to the `Geant4` toolkit.

## How to get this code

To get this into your computer, just clone this repository to your local machine

```sh
git clone https://github.com/donpicoro/ASP2026_G4.git
```

## How to compile

Assuming you have a working `Geant4` installation, this example compiles just like any other `Geant4` application

```sh
cmake -S ASP2026_G4 -B build_folder
cmake --build build_folder
cd build_folder
./example
```