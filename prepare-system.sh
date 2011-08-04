echo "make sure you have previously built the android source and that you have specified its directory with -d"
USAGE="use -d to specify the path to your current android source directory"
while getopts "d:" ARGS; do
	case $ARGS in
		d) androiddir=$OPTARG;;
		*) echo $USAGE
		    exit 1;;
	esac
if [ $androiddir ];then
cd $androiddir;
. build/envsetup.sh;
lunch full-eng;
mkdir /data;
cd /data;
hg clone https://jjlandg@bitbucket.org/jjlandg/inferno;
cd inferno;
cp stdlib.h $androiddir/bionic/libc/include/stdlib.h;
cp mkconfig-386 mkconfig;
PATH=/data/inferno/agcc:$PATH;
PATH=/data/inferno/Linux/386/bin:$PATH;
mkdir /data/inferno/Linux/arm/lib;
mkdir /data/inferno/Linux/arm/bin;
./makemk.sh;
mk nuke;
mk install;
mk nuke;
cp mkconfig-arm mkconfig;
mk install;
echo "please connect your android device and make sure you can access its root filesystem";
sleep 30;
bash parallel-push.sh;
else
echo "use -d to specify where to find a built android source";
fi
done
