# !/bin/bash

######################
#  GLOBAL VARIABLES  #
######################
VERSION="0.1.0"

######################
#  HELPER FUNCTIONS  #
######################
getArgs() {
    while getopts :hvb:r:o:-: flag; do
        case $flag in
        h)
            echo "Usage: $0 [-h / --help] [-v / --version] [--debug]"
            exit 0
            ;;
        v)
            echo "Version: $VERSION"
            exit 0
            ;;
        -)
            longopt=$OPTARG
            case $OPTARG in
            help)
                echo "Usage: $0 [-h / --help] [-v / --version] [--debug]"
                exit 0
                ;;
            version)
                echo "Version: $VERSION"
                exit 0
                ;;
            debug)
                debug=1
                ;;
            *)
                echo "Invalid option: $OPTARG"
                exit 1
                ;;
            esac
            ;;
        *)
            echo "Invalid option: $OPTARG"
            exit 1
            ;;
        esac
    done
}

######################
#    MAIN FUNCTION   #
######################

getArgs "$@"

make clean
make cleansetup
make setup
if [ -z $debug ]; then
    make gpt
else
    make debugpt
fi
