CC = g++
CFLAGS_SO = -I`python -c 'from distutils.sysconfig import *; print get_python_inc()'` -DPIC -bundle -fPIC -lboost_python -framework Python -std=c++11 -L/usr/local/lib -lboost_serialization -lboost_filesystem -lboost_system -O2

model:
	$(CC) model.cpp -o model.so $(CFLAGS_SO)