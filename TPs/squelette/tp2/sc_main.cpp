#include "ensitlm.h"
#include "generator.h"
#include "bus.h"
#include "memory.h"
#include "LCDC.h"
#include "ROM.h"

using namespace sc_core;

int sc_main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	Generator gen("Generator1");
	Bus router("Bus");
	Memory mem("Memory", MEM_SIZE);
	LCDC lcdc("LCDC", sc_time(1.0/25, SC_SEC));
	ROM rom("ROM");

	router.map(mem.target, MEM_START, MEM_SIZE);
	gen.initiator.bind(router.target);
	router.initiator.bind(mem.target);

	router.map(lcdc.target_socket, LCDC_REG_START, LCDC_REG_SIZE);
	lcdc.initiator_socket.bind(router.target);
	lcdc.target_socket.bind(router.initiator);
	lcdc.display_int.bind(gen.irq);

	router.map(rom.socket, ROM_START, ROM_SIZE);
	rom.socket.bind(router.initiator);

	sc_core::sc_start();
	return 0;
}
