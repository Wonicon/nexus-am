NAME = xsmul
SRCS = $(shell find -L . -maxdepth 1 -name "*.c" -o -name "*.cpp")

include $(AM_HOME)/Makefile.app

$(APP_DIR)/ans_data.h: $(shell pwd)/build/ref_matmul
	$(APP_DIR)/build/ref_matmul > $@

$(APP_DIR)/build/ref_matmul: $(shell pwd)/ref/ref_matmul.c
	# Use host cc
	cc $(APP_DIR)/ref/ref_matmul.c -I$(shell pwd) -o $@

$(APP_DIR)/build/$(ARCH)//./xiangshan_mul.d: $(APP_DIR)/ans_data.h

clean: pre-clean

pre-clean:
	rm -rf $(APP_DIR)/ans_data.h

.PHONY: pre-clean
