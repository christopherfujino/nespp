main.nes: main.o config.cfg
	cl65 $< -C ./config.cfg --target nes -o $@

main.o: main.asm
	ca65 $< --target nes -o $@
