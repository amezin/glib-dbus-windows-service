project('glib-dbus-windows-service', 'c',
    version: '0.1',
    meson_version : '>=0.53.0',
    default_options : [
        'buildtype=debugoptimized',
        'warning_level=1',
        'c_std=gnu11',
    ],
)

is_win = host_machine.system() == 'windows'

gnome = import('gnome')

deps = [
    dependency('gio-2.0')
]

if is_win
    deps += [dependency('wing')]
endif

executable('service', 'service.c', dependencies : deps, install : true)
executable('client', 'client.c', dependencies : deps, install : true)
