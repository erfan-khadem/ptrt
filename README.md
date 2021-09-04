# ptrt
A simple path-tracing ray-tracer
# Building
You will need meson + ninja:
```bash
pip3 install --upgrade meson
pip3 install --upgrade ninja
```
You will also need OpenMP:
```bash
# Debian based distros:
sudo apt install libomp-dev
# Arch based distros:
sudo pacman -Sy openmp
```
To build use the following commands:
```bash
cd ptrt # root of the project
meson builddir
cd builddir
ninja
```
To run the compiled program:
```bash
./ptrt
```
