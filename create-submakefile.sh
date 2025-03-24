cd "$(dirname "$0")"

# Command line arguments set configuration variables.
for x
do
    echo "$x"
done

DFLAGS=$(
    for x
    do
        echo " -D$x  "
        echo " -D$(echo $x | tr '=' '_')=1 "
    done
)

# All *.c files under /nekot1/ are used.
KERNEL_CFILES=$(
    for x in $(cd nekot1 && echo [a-z]*.c)
    do
        echo "../nekot1/$x"
    done
)

# And all the games.
GAMES=$(
    for x in games/[a-z]*/game.c
    do
        NAME="$(basename $(dirname $x))"
        echo "$NAME.game"
    done
)

cat <<HERE

CFILES=$(echo ${KERNEL_CFILES})

GAMES=$(echo ${GAMES})

DFLAGS=$(echo ${DFLAGS})

HERE

# Slurp the common.mk file.
cat common.mk

for g in $GAMES
do
    x=$(basename $g .game)

    echo "$x.game: ../games/$x/game.c"
	echo '	python3 ../nekot1/n1preprocess.py $<' $x.c
    echo '	$(GCC) -fwhole-program -S $(GCFLAGS)' -I../games/$x -I.. $x.c

	echo '	cat _nekot1_sym.s >>' $x.s
	echo '	$(LWASM)' -o$x.o $x.s --list=$x.o.list --map=$x.o.map --symbol-dump=$x.o.sym
	echo '	$(LWLINK) --decb' --output=$x.game $x.o -lgcc --entry=__n1pre_entry --script=_game.script --map=$x.map
	echo '	grep "^Section:"' $x.map
	echo '	cp -vf' $x.game /tmp/$x.game
	echo '	cp -vf' $x.game /tmp/$x.'$$(cat _nekot1.decb.hash).game'
	echo '	echo OKAY $@'
    echo

done
