#/bin/sh

#
# USAGE
#
#   build [filename] [-run] 
#   

me=$(dirname $0)
root_dir=$me/..
code_dir=$root_dir/code
build_dir=$root_dir/build
filename=$1

run=0
optimize=0 

shift

for var in "$@"
do
  if [ $var = "-run" ]; then 
    run=1
  fi
done

compiler_flags="-std=c++17 -Wall -Wno-unused-function -Wno-parentheses -Wno-macro-redefined -Wno-deprecated-declarations -Wno-missing-braces"


clang++ $compiler_flags $code_dir/$filename.cpp -o $build_dir/$filename.exe

if [ $run = 1 ]; then
  pushd $build_dir  
  ./$filename.exe
  popd
fi

