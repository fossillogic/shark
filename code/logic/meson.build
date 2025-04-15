dir = include_directories('.')

app_lib = library('app-code',
    files('lifecycle.c', 'app.c'),
    install: true,
    dependencies: [code_deps],
    include_directories: dir)

app_dep = declare_dependency(
    link_with: [app_lib],
    dependencies: [code_deps],
    include_directories: dir)

# This is the main executable for the app
executable('app', 'main.c', install: true, dependencies: [app_dep], include_directories: dir)
