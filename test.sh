mkdir ktests
mkdir ktests/dir1
touch ktests/file1
touch ktests/file2
touch ktests/out
touch ktests/dir1/file3
cat > ./ktests/tests <<EOL
cd ktests
echo 1_THIS_IS_THE_START
echo 1_DIRECT_LS
ls -l
ls
echo _
echo _1_COMPLETED1_STARTING_2LSPIPEWC
ls | wc
echo _
echo _2_COMPLETED_STARTING_3LSREDIRECT
ls > out
cat out
echo _
echo _3_COMPLETED_STARTING4_REDIRECT_FILE_INTO_WC
wc < out
echo _
echo _4_COMPLETED_CHECKING_SUBDIR
cd dir1
ls
cd ..
ls
echo _
echo _5_THIS_IS_THE_END
exit
echo _
echo _THIS_SHOULD_NOT_PRINT______
EOL
echo _____________________
echo ___BASH_OUTPUT:___
echo _____________________
bash < ktests/tests
echo _____________________________
echo ___COMPARED_TO_YOURS:___
echo _____________________________
make
make run < ktests/tests
rm -rf ktests
