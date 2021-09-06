project('ptrt', 'cpp',
  version : '0.1',
  default_options : ['cpp_std=c++20', 'cpp_args=-O3', 'warning_level=3'])


my_src = files([
  'src/camera.cpp',
  'src/color.cpp',
  'src/hittable_list.cpp',
  'src/material.cpp',
  'src/sphere.cpp',
  'src/utils.cpp',
  'src/scene.cpp',
  'src/main.cpp'
])

openmp = dependency('openmp')
nlohmann_json = dependency('nlohmann_json')

exe = executable('ptrt',
  sources: my_src,
  dependencies: [openmp, nlohmann_json],
  install : false)

test('basic', exe)
