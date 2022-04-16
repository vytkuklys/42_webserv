#! /bin/bash
cd ../inc/ && sed -i '' '5s/.*/#define ARRAY_SIZE 16384/' Parsing.hpp \
&& cd .. && make && ./webserv ./config_files/test.conf & echo -ne '\n\n\n\n\n' | time ./tester http://localhost:8080 >helper ; tail -n1 helper >>helper1
