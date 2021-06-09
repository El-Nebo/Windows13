build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	gcc process.c -o process.out
	gcc process_generator.c -o abbas
	./abbas processes.txt 1 30 1
	
abbas:
	gcc process.c -o process.out
	gcc process_generator.c -o abbas
	./abbas processes.txt 1 30 1
