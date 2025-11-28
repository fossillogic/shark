dir = include_directories('.')

app_lib = static_library('app-code',
    files(
        'app.c',
        'magic.c',
        'show.c',
        'move.c',
        'copy.c',
        'remove.c',
        'rename.c',
        'create.c',
        'search.c',
        'archive.c',
        'view.c',
        'compare.c',
        'help.c',
        'sync.c',
        'watch.c',
        'grammar.c',
        'rewrite.c',
        'introspect.c',
        'summary.c',
    ),
    install: true,
    dependencies: [code_deps],
    include_directories: dir)

app_dep = declare_dependency(
    link_with: [app_lib],
    dependencies: [code_deps],
    include_directories: dir)

# This is the main executable for the app
executable('shark', 'main.c', install: true, dependencies: [app_dep], include_directories: dir)
