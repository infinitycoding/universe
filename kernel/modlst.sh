#! /bin/bash

function getDependencies {
    if [[ $alldeps != *"$2"* ]]; then
        alldeps="$alldeps $2";
        if [ -f $1/$2/dep.lst -a -s $1/$2/dep.lst ]; then
            local dependencies=`cat $1/$2/dep.lst`;
            for dependencie in $dependencies; do
                if [[ $alldeps != *"$dependencie"* ]]; then
                    getDependencies $1 $dependencie;
                    if [[ $alldeps != *"$dependencie"* ]]; then
                        alldeps="$alldeps $dependencie";
                    fi
                fi;
            done;
        fi;
    fi
}

deps=`cat modules.lst`
for mod in $deps; do
    getDependencies modules $mod;
done;
echo $alldeps
