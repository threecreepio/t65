DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
CFLAGS = -g -O2 -I. -I./include -I./src -I./src/nes -I./src/cpu $(DEPFLAGS)
INSTALL_DIR = /usr/local

phony: t65

install: t65
	cp t65 $(INSTALL_DIR)/bin/t65

t65: main.o
	$(CC) -o t65 main.o

clean:
	rm -f t65 *.o
	rm -rf $(DEPDIR)

-include $(wildcard $(DEPDIR)/*.d)
