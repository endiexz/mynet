c_srcs := $(shell find ./src -name *.c)
c_objs := $(patsubst ./src/%.c, ./obj/%.o, $(c_srcs))
inlcude_paths := ./include

I_flag := $(inlcude_paths:%=-I %)  

./obj/%.o : ./src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $^ -o $@ $(I_flag)

workspace/exec : $(c_objs)
	@mkdir -p $(dir $@)
	@gcc $^ -lpcap -o $@ 

list :
	@echo $(c_srcs)
	@echo $(c_objs)
	@echo $(I_flag)

clean :
	@rm -rf ./obj

run :
	@./workspace/exec


.PHONY : list run clean
