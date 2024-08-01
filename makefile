
srcs = tokens preprocessor symbols

tokens_inc = tokens.h
preprocessor_inc = preprocessor.h
symbols_inc = symbols.h

define comp_template =
all_files += bin/$(1).o
all_files += src/$(1).c
all_files += $$(patsubst %,inc/%,$($(1)_inc))
bin/$(1).o: src/$(1).c $$(patsubst %,inc/%,$($(1)_inc))
	@gcc -I inc -I ../utils -I ../DS/inc -g						$\
		 -c -fPIC $$< -o $$@
endef
all_files += libcparser.so

main: build lib
	@gcc -Wall -pedantic -I inc -I ../utils -I ../DS/inc -g     \
	    -L. -L../DS                                             \
	    main.c                                                  \
	    -lcparser -lds                                          \
	    -o main.out


build: $(foreach proj,$(srcs),$(proj))
$(foreach proj,$(srcs),$(eval $(proj): bin/$(proj).o))
$(foreach proj,$(srcs),$(eval $(call comp_template,$(proj))))

lib: libcparser.so
libcparser.so:
	@gcc -shared -o libcparser.so bin/*.o

clean:
	@rm -rf bin/* libcparser.so main.out

