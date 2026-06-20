# On MacOS run
# meson setup builddir --cross-file=macos-arm-toolchain.ini  -Denable_tests=true
# cd buildir
## either
# ninja
# ./main
## or
# or meson test ...

project(
  'synctex',
  'cpp',
  version: '0.1',
  default_options : ['cpp_std=c++17']
  )

# Déclare l'exécutable
executable(
  'main',
  'main.cpp',
  install: false
)

#####

doctest_dep = dependency(
  'doctest',
  version: '>=1.2.0',
  fallback: ['doctest', 'doctest_dep']
)

#####

# Sous-projet pour le code source
subdir('src')

# Sous-projet pour les tests
subdir('tests')

# Build your library
synctexpplib = static_library(
  'synctexpp',
  sources: [
#    'src/SyncTeXUtils.hpp',
#    'src/SyncTeXHandle.cpp',
#    'src/SyncTeXHandle.hpp',
#    'src/SyncTeXInput.cpp',
#    'src/SyncTeXInput.hpp',
    'src/SyncTeXNode.cpp',
#    'src/SyncTeXNode.hpp',
    'src/SyncTeXNodeBox.cpp',
#    'src/SyncTeXNodeForm.hpp',
#    'src/SyncTeXKern.hpp',
#    'src/SyncTeXProxy.cpp',
    'src/SyncTeXProxy.hpp',
#    'src/SyncTeXsheet.hpp',
    'src/SyncTeXBox.cpp',
#    'src/SyncTeXBox.hpp',
#    'src/SyncTeXParser.hpp',
#    'src/SyncTeXReader.hpp',
    'src/SyncTeXManager.cpp',
#    'src/SyncTeXManager.hpp'
  ],
  include_directories: [
    'include',
    '../src',
    '../..',
  ],
)
