CC = g++
BOOST = /usr/local/Cellar/boost/1.65.0
INCLUDE = `python3-config --includes` -std=c++14 -I$(BOOST)/include
LDFLAGS = `python3-config --ldflags` -lboost_serialization -lboost_python3 -L$(BOOST)/lib
SOFLAGS = -shared -fPIC -march=native

install: ## Python用ライブラリをコンパイル
	$(CC) $(INCLUDE) $(LDFLAGS) $(SOFLAGS) src/python.cpp src/python/*.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o run/npylm.so -O3
	cp run/npylm.so run/semi-supervised/npylm.so
	cp run/npylm.so run/unsupervised/npylm.so
	rm -rf run/npylm.so

install_ubuntu: ## Python用ライブラリをコンパイル
	$(CC) -Wl,--no-as-needed -Wno-deprecated $(INCLUDE) $(LDFLAGS) $(SOFLAGS) src/python.cpp src/python/*.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o run/npylm.so -O3
	cp run/npylm.so run/unsupervised/npylm.so
	cp run/npylm.so run/semi-supervised/npylm.so
	rm -rf run/npylm.so

check_includes:	## Python.hの場所を確認
	python3-config --includes

check_ldflags:	## libpython3の場所を確認
	python3-config --ldflags

module_tests: ## 各モジュールのテスト.
	$(CC) test/module_tests/npylm.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o test/module_tests/npylm $(INCLUDE) $(LDFLAGS) -O0 -g
	./test/module_tests/npylm
	$(CC) test/module_tests/vpylm.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o test/module_tests/vpylm $(INCLUDE) $(LDFLAGS) -O0 -g
	./test/module_tests/vpylm
	$(CC) test/module_tests/sentence.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o test/module_tests/sentence $(INCLUDE) $(LDFLAGS) -O0 -g
	./test/module_tests/sentence
	$(CC) test/module_tests/hash.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o test/module_tests/hash $(INCLUDE) $(LDFLAGS) -O0 -g
	./test/module_tests/hash

running_tests:	## テスト
	$(CC) test/running_tests/save.cpp src/python/*.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o test/running_tests/save $(INCLUDE) $(LDFLAGS) -O3 -Wall
	$(CC) test/running_tests/train.cpp src/python/*.cpp src/npylm/*.cpp src/npylm/lm/*.cpp -o test/running_tests/train $(INCLUDE) $(LDFLAGS) -O3 -Wall

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.DEFAULT_GOAL := help