#! /bin/bash

function getDependencies {
    if [[ -n "$2" ]]; then
        echo "[$2] getDependencies"
        if [[ $alldeps != *"$2"* ]]; then
            alldeps="$alldeps $2";
            echo "[$2] added to module list"
            echo "{$alldeps}"
            if [ -f $1/$2/dep.lst -a -s $1/$2/dep.lst ]; then
                local dependencies=`cat $1/$2/dep.lst`;
                echo "[$2] has dependencies {$dependencies}"
                for dependencie in $dependencies; do
                    echo "[$2]-> $dependencie"
                    if [[ $alldeps != *"$dependencie"* ]]; then
                        getDependencies $1 $dependencie;
                        if [[ $alldeps != *"$dependencie"* ]]; then
                            echo "[$2]: dependencie not added via recrusion"
                            alldeps="$alldeps $dependencie";
                            echo "[$2] $dependencie added to module list"
                            echo "{$alldeps}"
                        else
                            echo "[$2] $dependencie already in list";
                            echo "{$alldeps}"
                        fi
                    else
                        echo "[$2] $dependencie already in list";
                        echo "{$alldeps}"
                    fi;
                done;
            else
                echo "[$2] has no dependencies";
            fi;
        else
            echo "[$2] already in list";
        fi
    fi
}

deps=`cat modules.lst`
echo "--------------------------"
for mod in $deps; do
    getDependencies modules $mod;
done;
echo "all module: $alldeps"
echo "--------------------------"
