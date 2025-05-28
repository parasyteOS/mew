project('mew', 'c',
  version: '0.1.0',
  license: 'MIT',
  meson_version: '>=0.58.1',
  default_options: [
    'c_std=c11',
    'warning_level=2',
    'werror=true',
  ],
)

add_project_arguments(
  [
    '-DWLR_USE_UNSTABLE',
    '-Wundef',
    '-Wno-unused-parameter',
    '-O',
  ],
  language: 'c',
)

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'c')
endif

cc = meson.get_compiler('c')

is_freebsd = host_machine.system().startswith('freebsd')
if is_freebsd
  add_project_arguments(
    [
      '-Wno-format-extra-args',
      '-Wno-gnu-zero-variadic-macro-arguments',
    ],
    language: 'c'
  )
endif

wlroots        = dependency('wlroots-0.18', fallback: ['wlroots', 'wlroots'])
wayland_protos = dependency('wayland-protocols', version: '>=1.14')
wayland_server = dependency('wayland-server')
xkbcommon      = dependency('xkbcommon')
math           = cc.find_library('m')
drm            = dependency('libdrm',
                   version: '>=2.4.122',
                   fallback: 'libdrm',
                   default_options: [
                     'auto_features=disabled',
                     'tests=false',
                   ],
                 )

wl_protocol_dir = wayland_protos.get_variable('pkgdatadir')
wayland_scanner = find_program('wayland-scanner')
wayland_scanner_server = generator(
  wayland_scanner,
  output: '@BASENAME@-protocol.h',
  arguments: ['server-header', '@INPUT@', '@OUTPUT@'],
)

server_protocols = [
  [wl_protocol_dir, 'stable/xdg-shell/xdg-shell.xml'],
]

server_protos_headers = []

foreach p : server_protocols
  xml = join_paths(p)
  server_protos_headers += wayland_scanner_server.process(xml)
endforeach

server_protos = declare_dependency(
  sources: server_protos_headers,
)

mew_sources = [
  'mew.c',
  'output.c',
  'shm.c',
  'seat.c',
  'view.c',
  'xdg_shell.c',
]

mew_headers = [
  'output.h',
  'shm.h',
  'seat.h',
  'server.h',
  'view.h',
  'xdg_shell.h',
]

executable(
  meson.project_name(),
  mew_sources + mew_headers,
  dependencies: [
    server_protos,
    wayland_server,
    wlroots,
    xkbcommon,
    math,
    drm,
  ],
  install: true,
)
