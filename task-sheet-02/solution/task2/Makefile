build:
	gcc bytecode_function.c -o bytecode_function.bin

clean:
	rm bytecode_function.bin

test: build
	./bytecode_function.bin 2
	../../material/vm.bin 2
	./bytecode_function.bin 9
	../../material/vm.bin 9
	./bytecode_function.bin 12
	../../material/vm.bin 12
	./bytecode_function.bin 42
	../../material/vm.bin 42
	./bytecode_function.bin 21
	../../material/vm.bin 21
	./bytecode_function.bin 324
	../../material/vm.bin 324
	./bytecode_function.bin 678
	../../material/vm.bin 678

	