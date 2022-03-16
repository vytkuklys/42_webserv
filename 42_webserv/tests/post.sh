curl --data-binary @42.png localhost:8080 -v --trace-ascii -
# curl -H "Transfer-Encoding: chunked" -d @42.png localhost:8080 -v --trace-ascii -
