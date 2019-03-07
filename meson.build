project('poki-pona', 'c', default_options: ['warning_level=3'])

gtkdep = dependency('gtk+-3.0')
vtedep = dependency('vte-2.91')

executable('poki-pona', 'poki-pona.c', dependencies: [gtkdep, vtedep], install: true)

poki_pona_prefix = get_option('prefix')
poki_pona_datadir = join_paths(poki_pona_prefix, get_option('datadir'))

meson.add_install_script('data/desktop-install.sh', join_paths(poki_pona_datadir, 'applications'))
