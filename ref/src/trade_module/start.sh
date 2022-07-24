set -x
SCRIPT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPT_PATH
cd build
pid=$(netstat -nlp | grep 8888| awk '{print $7}' | awk -F/ '{print $1 }')
kill -9 $pid
export env=sandbox
nohup ./server 2>&1 > ../logs/server.log &
