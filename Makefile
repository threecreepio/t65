DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
CFLAGS = -g -I. -I./include -I./src -I./src/nes -I./src/cpu $(DEPFLAGS)
INSTALL_DIR = /usr/local

phony: t65

install: t65
	cp t65 $(INSTALL_DIR)/bin/t65

t65: main.o
	$(CC) -o t65 main.o

-include $(wildcard $(DEPDIR)/*.d)
