cc = gcc
taget = flock
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.c")
obj = $(src:%.c=%.o) 

$(taget): $(obj)
	$(cc) -o $(taget) $(obj)
	@echo "$(taget)......^_^"

%.o: %.c $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(taget)

