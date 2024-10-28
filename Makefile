GREEN  := \033[0;32m
YELLOW := \033[0;33m
RESET  := \033[0m

all:
	cc main.c -lncurses -o game
	@echo "${GREEN}Project built!${RESET}"
	@echo "${GREEN}Please use${RESET} ${YELLOW}./game <board size x> <board size y> <number of bombs>${RESET}"
	@echo "Play with ${YELLOW}'WASD'${RESET} and select slot with ${YELLOW}'SPACE'${RESET}"

clean:
	rm -rf game
	@echo "cleaning!!!"

re: clean all
	@echo "remaking game!!!"

.PHONY: all clean re
