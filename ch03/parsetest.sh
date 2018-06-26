for file in ../testcases/*.tig
do
    echo "$file: "
    ./parsetest $file 2>&1
done
