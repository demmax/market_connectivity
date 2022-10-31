#!/usr/bin/env bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <path_to_server> <path_to_client>"
    exit 1
fi

SERVER_BIN=$1
CLIENT_BIN=$2

echo "Server binary: ${SERVER_BIN}"
echo "Client binary: ${CLIENT_BIN}"

trap "pkill `basename ${SERVER_BIN}`" EXIT

${SERVER_BIN} --port 54673 --client_pool=4 --handler_pool=4 --interval=5 --log_level 0 -s stats.txt &
sleep 2  # wait for start

${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f 20_tokens.txt -c 20 -i client1 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f only_odd_tokens.txt -c 10 -i client2 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f 1_2_tokens.txt -c 20 -i client3 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f reverse_20_tokens.txt -c 20 -i client4 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f 20_tokens.txt -c 20 -i client5 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f only_odd_tokens.txt -c 10 -i client6 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f 1_2_tokens.txt -c 20 -i client7 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f reverse_20_tokens.txt -c 20 -i client8 -l 0 &

sleep 1
echo "Wait for stats updated..."
echo 5

${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f 20_tokens.txt -c 20 -i client9 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f only_odd_tokens.txt -c 10 -i client10 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f 1_2_tokens.txt -c 20 -i client11 -l 0 &
${CLIENT_BIN} -a 127.0.0.1 -p 54673 -f reverse_20_tokens.txt -c 20 -i client12 -l 0 &

sleep 1
echo "Wait for stats updated..."
sleep 5

echo "\n\nStats:\n"
cat stats.txt
