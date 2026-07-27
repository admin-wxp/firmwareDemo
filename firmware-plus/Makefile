# Makefile
# 作用：快速构建入口。make -> 自动创建 build 目录并编译

.PHONY: all clean test

all:
	@mkdir -p build && cd build && cmake .. && make

clean:
	@rm -rf build

test: all
	@cd tests && pytest -v