CC		:= gcc
CFLAGS	:= -O0

TARGET	:= licence_check.anti-tamper.c
BIN		:= licence_check.anti-tamper.bin

.PHONY: licence_check
licence_check:
	$(CC) $(CFLAGS) $(TARGET) -o $(BIN)

test: licence_check
	@echo -e "\n--- Invalid Credentials ---\n"
	@$(BIN) user pass
	@echo -e "\n--- Valid Credentials ---\n"
	@$(BIN) sp@rub.de dgWebu9sr