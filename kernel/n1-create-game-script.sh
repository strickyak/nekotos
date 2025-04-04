#!/bin/sh
#
# Create a script for games,
# where the game loads right after the Kernel text ends.
#
# Typical input is `_nekot1.decb`
# Typical output is `_game.script`

# Example: '''Section: .final (_nekot1.o) load at 0F02, length 0002'''
Available=` tr ',' ' ' |
            awk '/^Section: [.]data[.]startup / { print $6 }' `

cat <<HERE
section .absolute      load 0x0000
section .bss           load 0x0080
section .text.entry    load 0x${Available}
section .data
section .data.more
section .text
section .final
section .data.startup
section .text.startup
section .final.startup
HERE
