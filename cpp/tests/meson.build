
# Build tests

# From the root project directory run
# git submodule add https://github.com/doctest/doctest.git subprojects/doctest
# To remove any local change
doctest_dep = dependency(
  'doctest',
  version: '>=1.2.0',
  fallback: ['doctest', 'doctest_dep']
)

# From the root project directory run
# git submodule add https://github.com/catchorg/Catch2.git subprojects/catch2
# catch2_dep = dependency('catch2',version: '>=3.0.0',fallback: ['catch2', 'catch2_dep'])

# Déclaration de la dépendance zlib
zlib_dep = dependency('zlib')

priority_Config_0       = 0
priority_Config_1       = priority_Config_0 - 1
priority_POC_0          = priority_Config_1 - 1
priority_POC_1          = priority_POC_0 - 1
priority_POC_2          = priority_POC_1 - 1
priority_BufferChef_0   = priority_POC_2 - 1
priority_BufferChef_1   = priority_BufferChef_0 - 1
priority_BufferChef_2   = priority_BufferChef_1 - 1
priority_BufferChef_3   = priority_BufferChef_2 - 1
priority_Synchronizer_0 = priority_BufferChef_3 - 1
priority_Node_0         = priority_Synchronizer_0 - 1
priority_Tree_0         = priority_Node_0 - 1
priority_Node_1         = priority_Tree_0 - 1
priority_Node_4         = priority_Node_1 - 1
priority_Node_5         = priority_Node_4 - 1
priority_Node_6         = priority_Node_5 - 1
priority_Node_7         = priority_Node_6 - 1
priority_Proxy_0        = priority_Node_7 - 1
priority_Input_0        = priority_Node_7 - 1
priority_Form_0         = priority_Input_0 - 1
priority_Reader_0       = priority_Form_0 - 1
priority_Manager_0      = priority_Reader_0 - 1
priority_Parser_0       = priority_Manager_0 - 1
priority_Parser_1       = priority_Parser_0 - 1
priority_Manager_1      = priority_Parser_1 - 1

########## Does doctest works?
test_Config_0 = executable(
  'Config_0',
  sources: [
    'Config_0.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'Config_0',
  test_Config_0,
  args: [],
  verbose: false,
  priority: priority_Config_0
)

########## Does std::filesystem work? (Subtleties on MacOS)
test_Config_1 = executable(
  'Config_1',
  sources: [
    'Config_1.cpp',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'Config_1',
  test_Config_1,
  args: [],
  verbose: false,
  priority: priority_Config_1
)

########## Various proofs of
test_POC_0 = executable(
  'POC_0',
  sources: [
    'POC_0.cpp',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'POC_0',
  test_POC_0,
  args: [],
  verbose: false,
  priority: priority_POC_0
)

test_POC_1 = executable(
  'POC_1',
  sources: [
    'POC_1.cpp',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'POC_1',
  test_POC_1,
  args: [],
  verbose: false,
  priority: priority_POC_1
)

test_POC_2 = executable(
  'POC_2',
  sources: [
    'POC_2.cpp',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'POC_2',
  test_POC_2,
  args: [],
  verbose: false,
  priority: priority_POC_2
)

test_BufferChef_0 = executable(
  'BufferChef_0',
  sources: [
    'BufferChef_0.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'BufferChef_0',
  test_BufferChef_0,
  args: [],
  verbose: false,
  priority: priority_BufferChef_0
)

test_BufferChef_1 = executable(
  'BufferChef_1',
  sources: [
    'BufferChef_1.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'BufferChef_1',
  test_BufferChef_1,
  args: [],
  verbose: false,
  priority: priority_BufferChef_1
)

test_BufferChef_2 = executable(
  'BufferChef_2',
  sources: [
    'BufferChef_2.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'BufferChef_2',
  test_BufferChef_2,
  args: [],
  verbose: false,
  priority: priority_BufferChef_2
)

test_BufferChef_3 = executable(
  'BufferChef_3',
  sources: [
    'BufferChef_3.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
  install: false,
)
# Enable Meson's test runner
test(
  'BufferChef_3',
  test_BufferChef_3,
  args: [],
  verbose: false,
  priority: priority_BufferChef_3,
  workdir : join_paths(meson.current_source_dir(), 'BufferChef_dir'),
)

##########
test_Node_0 = executable(
  'test_Node_0',
  sources: [
    'Node_0.cpp',
    '../src/SyncTeXNode.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
)
# Enable Meson's test runner
test(
  'Node_0',
  test_Node_0,
  args: [],
  verbose: false,
  priority: priority_Node_0
)

##########
test_Tree_0 = executable(
  'test_Tree_0',
  sources: [
    'Tree_0.cpp',
    '../src/SyncTeXNode.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Tree_0',
  test_Tree_0,
  args: [],
  verbose: false,
  priority: priority_Tree_0
)

test_Node_1 = executable(
  'test_Node_1',
  sources: [
    'Node_1.cpp',
    '../src/SyncTeXNode.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
)
# Enable Meson's test runner
test(
  'Node_1',
  test_Node_1,
  args: [],
  verbose: false,
  priority: priority_Node_1
)

test_Node_4 = executable(
  'test_Node_4',
  sources: [
    'Node_4.cpp',
    '../src/SyncTeXNode.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
)
# Enable Meson's test runner
test(
  'Node_4',
  test_Node_4,
  args: [],
  verbose: false,
  priority: priority_Node_4
)

test_Node_5 = executable(
  'test_Node_5',
  sources: [
    'Node_5.cpp',
    '../src/SyncTeXNode.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
)
# Enable Meson's test runner
test(
  'Node_5',
  test_Node_5,
  args: [],
  verbose: false,
  priority: priority_Node_5
)

test_Node_6 = executable(
  'test_Node_6',
  sources: [
    'Node_6.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXBox.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
)
# Enable Meson's test runner
test(
  'Node_6',
  test_Node_6,
  args: [],
  verbose: false,
  priority: priority_Node_6
)

test_Node_7 = executable(
  'test_Node_7',
  sources: [
    'Node_7.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXBox.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
)
# Enable Meson's test runner
test(
  'Node_7',
  test_Node_7,
  args: [],
  verbose: false,
  priority: priority_Node_7
)

##### Input
test_Input_0 = executable(
  'test_Input_0',
  sources: [
    'Input_0.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXInput.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Input_0',
  test_Input_0,
  args: [],
  verbose: false,
  priority: priority_Input_0,
  workdir : join_paths(meson.current_source_dir(), 'Input_dir')
)

##### Form
test_Form_0 = executable(
  'test_Form_0',
  sources: [
    'Form_0.cpp',
    '../src/SyncTeXNode.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [doctest_dep,],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Form_0',
  test_Form_0,
  args: [],
  verbose: false,
  priority: priority_Form_0
)

##### Synchronize interface
test_Reader_0 = executable(
  'test_Reader_0',
  sources: [
    'Reader_0.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [
    doctest_dep,
    zlib_dep,
  ],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Reader_0',
  test_Reader_0,
  args: [],
  verbose: false,
  priority: priority_Reader_0,
  workdir : join_paths(meson.current_source_dir(), 'Reader_dir'),
)

##### Manager
test_Manager_0 = executable(
  'test_Manager_0',
  sources: [
    'Manager_0.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXManager.cpp',
    '../src/SyncTeXInput.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [
    doctest_dep,
    zlib_dep,
  ],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Manager_0',
  test_Manager_0,
  args: [],
  verbose: false,
  priority: priority_Manager_0,
  workdir : join_paths(meson.current_source_dir(), 'Manager_dir'),
)
test_Manager_1 = executable(
  'test_Manager_1',
  sources: [
    'Manager_1.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXBox.cpp',
    '../src/SyncTeXManager.cpp',
    '../src/SyncTeXInput.cpp',
    '../src/SyncTeXParser.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [
    doctest_dep,
    zlib_dep,
  ],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Manager_1',
  test_Manager_1,
  args: [],
  verbose: false,
  priority: priority_Manager_1,
  workdir : join_paths(meson.current_source_dir(), 'Manager_dir'),
)

##### Proxy
test_Proxy_0 = executable(
  'test_Proxy_0',
  sources: [
    'Proxy_0.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXBox.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [
    doctest_dep,
    zlib_dep,
  ],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Proxy_0',
  test_Proxy_0,
  args: [],
  verbose: false,
  priority: priority_Proxy_0,
)

##### Parser
test_Parser_0 = executable(
  'test_Parser_0',
  sources: [
    'Parser_0.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXBox.cpp',
    '../src/SyncTeXManager.cpp',
    '../src/SyncTeXParser.cpp',
    '../src/SyncTeXInput.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [
    doctest_dep,
    zlib_dep,
  ],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Parser_0',
  test_Parser_0,
  args: [],
  verbose: false,
  priority: priority_Parser_0,
  workdir : join_paths(meson.current_source_dir(), 'Parser_dir'),
)

test_Parser_1 = executable(
  'test_Parser_1',
  sources: [
    'Parser_1.cpp',
    '../src/SyncTeXNode.cpp',
    '../src/SyncTeXBox.cpp',
    '../src/SyncTeXManager.cpp',
    '../src/SyncTeXParser.cpp',
    '../src/SyncTeXInput.cpp',
  ],
  include_directories: [
    '../include',
    '../src',
    '../../..',
  ],
  dependencies: [
    doctest_dep,
    zlib_dep,
  ],
#  link_with: [synctexpplib,],
  install: false,
  override_options: ['cpp_std=c++17'],
  cpp_args : ['-DSYNCTEX_TEST', '-DSYNCTEX_COVERAGE'],
)
# Enable Meson's test runner
test(
  'Parser_1',
  test_Parser_1,
  args: [],
  verbose: false,
  priority: priority_Parser_1,
  workdir : join_paths(meson.current_source_dir(), 'Parser_dir'),
)

