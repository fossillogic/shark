dir = include_directories('.')

app_lib = static_library('app-code',
    files(
         # not commands
        'app.c', 'magic.c',

        # commands
        'merge.c',
        'show.c',
        'swap.c',
        'move.c',
        'copy.c',
        'remove.c',
        'rename.c',
        'create.c',
        'cryptic.c',
        'search.c',
        'archive.c',
        'compare.c',
        'help.c',
        'sync.c',
        'watch.c',
        'grammar.c',
        'rewrite.c',
        'introspect.c',
        'split.c',
        'perm.c',
        'pipe.c',
        'play.c',
        'undo.c',
        'snapshot.c',
        'alias.c',
        'link.c',
        'dedupe.c'
    ),
    install: true,
    dependencies: [code_deps],
    include_directories: dir)

app_dep = declare_dependency(
    link_with: [app_lib],
    dependencies: [code_deps],
    include_directories: dir)

# This is the main executable for the app
executable('spino', 'main.c', install: true, dependencies: [app_dep], include_directories: dir)
