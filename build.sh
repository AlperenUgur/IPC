if [ "$1" == "clean" ];
then
    rm -rf build
elif [ "$1" == "build" ];
then
    mkdir build
    cd build
    cmake ..
    make
elif [ "$1" == "format" ];
then
    find src/ -iname '*.h' -o -iname '*.cpp' | xargs echo 
    find src/ -iname '*.h' -o -iname '*.cpp' | xargs clang-format --verbose -i --style=file:clang-format.txt
else
    echo "Wrong command!!!"
fi


