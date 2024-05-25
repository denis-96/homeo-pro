TEMPLATE = subdirs

SUBDIRS += \
    app \
    lib \
    tests


tests.depends = lib
