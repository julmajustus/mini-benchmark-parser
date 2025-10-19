#!/bin/bash
# **************************************************************************** #
#                                                  @@@            @@@@@@@@     #
#                                                   @@@          @@@@@@@@@@    #
#                                                    @@!         @@!   @@@@    #
#                                                     !@!        !@!  @!@!@    #
#    install-mini-benchmarker.sh                       @!!       @!@ @! !@!    #
#                                                       !!!      !@!!!  !!!    #
#    By: julmajustus <julmajustus@tutanota.com>          !!:     !!:!   !!!    #
#                                                         ::!    :!:    !:!    #
#    Created: 2025/10/20 00:26:55 by julmajustus           ::    ::::::: ::    #
#    Updated: 2025/10/20 00:26:55 by julmajustus            : :   : : :  :     #
#                                                                              #
# **************************************************************************** #

if [[ -f ./mini-benchmarker.sh && ./run-benchmarker.sh ]]; then
    echo "Installing mini-benchmarker & run-benchmarker to $HOME/.local/bin"
    echo

    if [[ ! -d $HOME/.local/share/mini-benchmarker ]]; then
        echo "Could not find $HOME/.local/share/mini-benchmarker directory!"
        echo "Creating $HOME/.local/share/mini-benchmarker"
        echo
        mkdir -p $HOME/.local/share/mini-benchmarker
    fi

    if [[ ! -d $HOME/.local/bin/ ]]; then
        echo "Could not find $HOME/.local/bin directory!"
        echo "Creating $HOME/.local/bin"
        echo
        mkdir -p $HOME/.local/bin
    fi

    echo "Installing mini-benchmarker.sh to $HOME/.local/bin"
    cp ./mini-benchmarker.sh $HOME/.local/bin
    chmod +x $HOME/.local/bin/mini-benchmarker.sh
    echo

    echo "Installing run-benchmarker.sh to $HOME/.local/bin"
    cp ./run-benchmarker.sh $HOME/.local/bin
    chmod +x $HOME/.local/bin/run-benchmarker.sh
    echo
    echo "All done installation compelete!"
    exit 0
    
fi

echo "Needed files could not be found!"
echo "Make sure to run the $0 at the directory containing mini-benchmarker.sh & run-benchmarker.sh"
exit 1
