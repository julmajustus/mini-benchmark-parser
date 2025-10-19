#!/bin/bash
# **************************************************************************** #
#                                                  @@@            @@@@@@@@     #
#                                                   @@@          @@@@@@@@@@    #
#                                                    @@!         @@!   @@@@    #
#                                                     !@!        !@!  @!@!@    #
#    run-benhcmarker.sh                                @!!       @!@ @! !@!    #
#                                                       !!!      !@!!!  !!!    #
#    By: julmajustus <julmajustus@tutanota.com>          !!:     !!:!   !!!    #
#                                                         ::!    :!:    !:!    #
#    Created: 2025/10/20 00:17:56 by julmajustus           ::    ::::::: ::    #
#    Updated: 2025/10/20 00:17:56 by julmajustus            : :   : : :  :     #
#                                                                              #
# **************************************************************************** #

RUN_COUNT=""
BENCHMARKER="$HOME/.local/bin/mini-benchmarker.sh $HOME/.local/share/mini-benchmarker"
SLEEP_DURATION=5

if [[ -z $1 ]]; then
    RUN_COUNT=3
else
    RUN_COUNT=$1
fi

if (( ! $RUN_COUNT + 0 )); then
    echo "Run count argument must be a number!"
    exit 1
fi

for bench in 1..$RUN_COUNT; do
    $BENCHMARKER
    sleep $SLEEP_DURATION
done
