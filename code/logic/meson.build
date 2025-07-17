dir = include_directories('.')

app_lib = static_library('app-code',
    files('app.c', 'create.c', 'delete.c', 'move.c', 'rename.c', 'copy.c', 'list.c',
          'show.c', 'find.c', 'where.c', 'search.c', 'backup.c', 'size.c', 'disk.c', 
          'tree.c', 'compare.c', 'color.c', 'info.c', 'ask.c', 'clean.c', 'change.c',
          'file.c'
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
