CC = g++
CFLAGS_SO = -I`python -c 'from distutils.sysconfig import *; print get_python_inc()'` -shared -fPIC -std=c++11 -L/usr/local/lib -lboost_python -lboost_serialization -lboost_filesystem -lboost_system -lpython2.7 -O2

model:
	$(CC) model.cpp -o model.so $(CFLAGS_SO)