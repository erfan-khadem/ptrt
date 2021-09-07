# ptrt
A simple path-tracing ray-tracer
# Building
You will need meson + ninja:
```bash
pip3 install --upgrade meson
pip3 install --upgrade ninja
```
You will also need OpenMP + Nlohmann-JSON:
```bash
# Debian based distros:
sudo apt install libomp-dev nlohmann-json-dev
# Arch based distros:
sudo pacman -Sy openmp nlohmann-json
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
# Sample output
![sample output hallow glass](https://github.com/er888kh/ptrt/blob/main/images/hallow_glass_4k.jpeg?raw=true)
![sample output spheres](https://github.com/er888kh/ptrt/blob/main/images/raytracing_week1_compressed.jpeg?raw=true)
# Credits
Almost all calculation related code comes from these [amazing books from Peter Shirley](https://raytracing.github.io/) but I have added a few optimizations and parallelization to make it run faster.
