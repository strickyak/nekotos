

(
    echo "#line 1 \"prelude.h\""
    cat prelude.h

    for f in [a-z]*.h
    do
        echo "#line 1 \"$f\""
        cat $f
    done

    echo "#line 1 \"SYNTHETIC\""
    for f in [a-z]*.h
    do
        b=$(basename $f .h)
        python3 -c "print('struct $b %s;' % '$b'.title())"
    done

    for f in [a-z]*.c
    do
        echo "#line 1 \"$f\""
        cat $f
    done

) > _target_.c

set -x
- gcc6809  -S -std=gnu99 -f'no-builtin' -f'omit-frame-pointer' -f'whole-program' -Os    _target_.c
