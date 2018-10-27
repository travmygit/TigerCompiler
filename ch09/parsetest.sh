for file in ../testcases/test*.tig
do
    ./typechecktest $file 2>&1
done
