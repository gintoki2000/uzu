SOURCEFILE=src/toolbox/array.c
gcc -E -dI "$SOURCEFILE" -I/usr/include/SDL2 -I./include | awk '
    /^# [0-9]* "/ { ignore = 1; if ($3 == "\"'"$SOURCEFILE"'\"") show=1; else show=0; }
    { if(ignore) ignore=0; else if(show) print; }'
         
