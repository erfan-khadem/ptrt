project('raytracing-week1', 'cpp',
  version : '0.1',
  default_options : ['cpp_std=c++20', 'cpp_args=-O3', 'warning_level=3'])


my_src = files([
  'src/camera.cpp',
  'src/color.cpp',
  'src/hittable_list.cpp',
  'src/material.cpp',
  'src/sphere.cpp',
  'src/utils.cpp',
  'src/main.cpp'
])

openmp = dependency('openmp')

exe = executable('raytracing_week1',
  sources: my_src,
  dependencies: [openmp],
  install : false)

test('basic', exe)
