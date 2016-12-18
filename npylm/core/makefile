CC = g++
CFLAGS = -std=c++11 -lboost_serialization -L/usr/local/lib

test_vpylm: 
		$(CC) test_vpylm.cpp $(CFLAGS) -O2

test_vpylm_debug: 
		$(CC) test_vpylm.cpp $(CFLAGS) -g

test_hpylm: 
		$(CC) test_hpylm.cpp $(CFLAGS) -O2

test_hpylm_debug: 
		$(CC) test_hpylm.cpp $(CFLAGS) -g

clean:
	rm -rf a.out
