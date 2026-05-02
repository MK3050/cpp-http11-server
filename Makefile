CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Werror

SRC = main.cpp \
      src/server/TcpServer.cpp \
      src/http/HttpParser.cpp \
      src/http/HttpResponse.cpp \
      src/http/Router.cpp

TARGET = server

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)