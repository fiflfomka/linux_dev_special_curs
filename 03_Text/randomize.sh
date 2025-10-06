# prepare file in format:
# LINE/COLUMN/CHAR_HEX
# LINE/COLUMN/CHAR_HEX
# LINE/CO...

all_file=$(mktemp)

n_line="0"
while IFS= read -r C_LINE ; do
    C_LINE_DIGIT=$(/bin/echo -n "$C_LINE" | od -v -tx1 -An);

    n_col="0"
    for digit in $C_LINE_DIGIT; do
        echo $n_line"/"$n_col"/"$digit >> $all_file;
        n_col=$((n_col+1));
    done

    n_line=$((n_line+1))
done

# prepare env to draw
FILE=`shuf < $all_file`
rm "$all_file"
SPACE_CHAR=`echo -n " " | od -tx1 -An`
tput clear
time_start=$(($(date +%s%N)/1000000))
n_chars=0
n_spaces=0

# draw file
for C_LINE in $FILE; do
    line_n=${C_LINE%%/*}
    col_n=${C_LINE%/*}
    col_n=${col_n#*/}
    char=${C_LINE##*/}
    if [ $char != $SPACE_CHAR ] ; then
        tput cup $line_n $col_n
        /bin/echo -n -e "\x$char"
        # move cursor to the same char again!
        tput cup $line_n $col_n

        if [ $1 ]; then
            sleep $1
        else
            sleep 0.1
        fi
        n_chars=$((n_chars+1))
    else
        n_spaces=$((n_spaces+1))
    fi
done

# print add info
time_end=$(($(date +%s%N)/1000000))
tput cup $n_line 0
# echo "- * - * - * - * - * - * - * - * - * -* - * - * - * - * - * - * - * -"
# echo draw time $((time_end - time_start))msec, drawed $n_chars characters, $n_spaces spaces skipped
