cc = gcc
taget = aio_read____not_notify
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.c")
obj = $(src:%.c=%.o) 

$(taget): $(obj)
	$(cc) -o $(taget) $(obj) -lpthread -lrt
	@echo "$(taget)......^_^"

%.o: %.c $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(taget)

