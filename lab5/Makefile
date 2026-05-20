CXX      = g++
CXXFLAGS = -std=c++17 -I. -pthread -Wall
LDFLAGS  = -ldl -lpthread

# ── Головна програма (HTTP сервер) ────────────────────────────────────────────
all: setup lab5

setup:
	@echo "⬇  Завантаження залежностей..."
	@test -f httplib.h || wget -q -O httplib.h \
	    "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h" \
	    && echo "✓ httplib.h" || echo "✗ httplib.h — завантаж вручну"
	@test -f sqlite3.o || gcc -c sqlite3.c -o sqlite3.o \
	    && echo "✓ sqlite3.o"

lab5: main.cpp sqlite3.o
	$(CXX) $(CXXFLAGS) main.cpp sqlite3.o -o lab5 $(LDFLAGS)
	@echo "✓ lab5 зібрано"

# ── Тести ─────────────────────────────────────────────────────────────────────
tests: tests.cpp sqlite3.o
	$(CXX) $(CXXFLAGS) tests.cpp sqlite3.o -o tests $(LDFLAGS)
	@echo "✓ tests зібрано"

run-tests: tests
	./tests

# ── Docker ────────────────────────────────────────────────────────────────────
docker-build:
	docker-compose build

docker-up:
	docker-compose up

docker-down:
	docker-compose down

# ── Очищення ──────────────────────────────────────────────────────────────────
clean:
	rm -f lab5 tests sqlite3.o

clean-all: clean
	rm -f httplib.h estate.db events.log

.PHONY: all setup tests run-tests docker-build docker-up docker-down clean clean-all
