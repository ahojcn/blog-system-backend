FLAG=-std=c++11 -L/usr/lib64/mysql -lmysqlclient -ljsoncpp

db_test:db_test.cc db.hpp
	g++ db_test.cc -o db_test $(FLAG)

.PHONY:clean
	rm db_test
