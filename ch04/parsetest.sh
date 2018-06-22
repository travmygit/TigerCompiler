for file in ./testcases/*.tig
do
    echo "$file: "
    ./a.out $file 2>&1
done