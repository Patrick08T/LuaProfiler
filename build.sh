dir=$(cd `dirname $0`;pwd)
projectdir=$dir/
builddir="$projectdir/build/debug"
rundir="$projectdir/bin/debug"
luadir="$projectdir/dep/lua-5.3.6"
lua="$projectdir/dep/lua-5.3.6.tar.gz"
cmakedir="$projectdir/dep/cmake-3.10.0-rc1-Linux-x86_64"
cmake="$projectdir/dep/cmake-3.10.0-rc1-Linux-x86_64.tar.gz"

function initDir
{
  echo "init Dir"
  if [ -f "$lua" ] && [ ! -d "$luadir" ]; then
    cd $projectdir/dep && tar zxf $lua
  fi

  if [ ! -d "$cmakedir" ]; then
    cd $projectdir/dep
    if [ ! -f "$cmake" ]; then
      wget https://cmake.org/files/v3.10/cmake-3.10.0-rc1-Linux-x86_64.tar.gz $projectdir/dep
    fi
    tar zxf $cmake 
  fi

  export PATH=$cmakedir/bin/:$PATH

  if [ -d "$builddir" ]; then
      cd $builddir
  else
      mkdir -p $builddir && cd $builddir
  fi

  if [ ! -d "$rundir" ];then
    mkdir -p $rundir

    if [ ! -d "$rundir/dep" ];then
      ln -s $projectdir/dep $rundir/dep
      echo "ln -s $projectdir/dep $rundir/dep"
    fi
  fi
}

initDir

cd $builddir
cmake -DMY_OPTION=$MY_OPTION -DCMAKE_BUILD_TYPE=debug ../../
make -j$(nproc)
makeret=$?
cd $dir
if [ $makeret -ne 0 ];then
  echo "make failed"
  exit
fi
