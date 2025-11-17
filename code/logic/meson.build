dir = include_directories('.')

app_lib = static_library('app-code',
    files(
        'app.c',
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
        'info.c',
        'link.c',
        'stat.c',
        'sync.c',
        'watch.c',
        'chat.c',
        'ask.c',
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
