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

deps = [
    dependency('gio-2.0')
]

if is_win
    deps += [dependency('wing')]
else
    deps += [dependency('gio-unix-2.0')]
endif

gnome = import('gnome')

gdbus_src = gnome.gdbus_codegen('test-dbus-interface', 'test-dbus-interface.xml',
    interface_prefix: 'org.gtk.GDBus.'
)

executable('service', 'service.c', gdbus_src, dependencies : deps, install : true)
executable('client', 'client.c', gdbus_src, dependencies : deps, install : true)
