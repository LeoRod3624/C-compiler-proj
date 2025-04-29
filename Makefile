CC = clang++
WARNINGS = -Werror
CFLAGS = -std=c++11 -g -fno-common $(WARNINGS)
OBJDIR = build
OBJS = $(OBJDIR)/main.o $(OBJDIR)/cst.o $(OBJDIR)/ast.o $(OBJDIR)/tokenizer.o $(OBJDIR)/codegen.o $(OBJDIR)/ir_generator.o

# Default target
leocc: $(OBJDIR) $(OBJS)
	@echo -e "\033[1;32m[Linking]\033[0m leocc"
	$(CC) $(CFLAGS) -o leocc $(OBJS)

# Make sure build directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compile source files into object files
$(OBJDIR)/%.o: %.cpp
	@echo -e "\033[1;33m[Compiling]\033[0m $<"
	$(CC) $(CFLAGS) -c $< -o $@

test: leocc
	./test.sh

clean:
	rm -rf leocc $(OBJDIR) *.s tmp*

.PHONY: test clean
