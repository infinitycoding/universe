#!/bin/sed -f

# usage:
# tom@tom-workstation:~/Desktop/universe$ find . -name *.c | xargs sed -i -f clean-code.sh

s/if(/if (/
s/while(/while (/
s/for(/for (/
s/}else/} else/
s/else{/else {/
s/){/) {/

# repair indentions (todo: avoid hard code)
s/^                                        /\t\t\t\t\t\t\t\t\t/
s/^                                    /\t\t\t\t\t\t\t\t\t/
s/^                                /\t\t\t\t\t\t\t\t/
s/^                            /\t\t\t\t\t\t\t/
s/^                        /\t\t\t\t\t\t/
s/^                    /\t\t\t\t\t/
s/^                /\t\t\t\t/
s/^            /\t\t\t/
s/^        /\t\t/
s/^    /\t/
