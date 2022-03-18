dir=$(cd `dirname $0`;pwd)
projectdir=$dir/
builddir="$projectdir/build/debug"
rundir="$projectdir/bin/debug"
luadir="$projectdir/dep/lua-5.3.6"
lua="$projectdir/dep/lua-5.3.6.tar.gz"

function initDir
{
  echo "init Dir"
  if [ -f "$lua" ] && [ ! -d "$luadir" ]; then
    cd $projectdir/dep && tar zxf $lua
  fi

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
cmake -DMY_OPTION=$MY_OPTION -DCMAKE_BUILD_TYPE=release ../../
make -j$(nproc)
makeret=$?
cd $dir
if [ $makeret -ne 0 ];then
  echo "make failed"
  exit
fi
