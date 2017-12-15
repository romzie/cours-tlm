#include "bus.h"
#include "ensitlm.h"

#define MEM_START 0x2800
#define MEM_SIZE 0x12C00
#define LCDC_REG_START 0x15400
#define LCDC_REG_SIZE 0xC
#define ROM_START 0x1540C

struct Generator : sc_core::sc_module {
	ensitlm::initiator_socket<Generator> initiator;
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS> irq;
	sc_core::sc_event irq_event;

	void thread(void);

	void interrupt(void);

	SC_CTOR(Generator);
};
