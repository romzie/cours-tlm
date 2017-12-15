#include "generator.h"
#include "ROM.h"

using namespace std;

void Generator::thread(void) {
	ensitlm::data_t val = 0;
	ensitlm::data_t val_wr = 0;
	tlm::tlm_response_status status;

	cout << name() << " filling mem with ROM image" << endl;

	for (int i = 0; i < ROM_SIZE/4; i++) {
		status = initiator.read(ROM_START + 4*i, val);
		if (status != tlm::TLM_OK_RESPONSE) {
			cerr << name() << ": error while reading memory "
					  "address 0x" << hex << ROM_START+4*i
					  << endl;
		} else {
			val_wr = (val & 0xF0000000);
		        val_wr += ((val & 0x0F000000) >> 4);
			val_wr += ((val & 0x00F00000) >> 8);
		        val_wr += ((val & 0x000F0000) >> 12);
			initiator.write(MEM_START + 8*i, val_wr);

			val_wr = ((val & 0x0000F000) << 16);
		        val_wr += ((val & 0x00000F00) << 12);
			val_wr += ((val & 0x000000F0) << 8);
		        val_wr += ((val & 0x0000000F) << 4);
			initiator.write(MEM_START + 8*i+4, val_wr);
		}
	}

	cout << name() << " sending start mem addr to lcdc" << endl;
	initiator.write(LCDC_REG_START, MEM_START);

	cout << name() << " starting lcdc" << endl;
	initiator.write(LCDC_REG_START + 0x4, 0x1);

	cout << name() << " waiting for interruption" << endl;
	ensitlm::data_t translation = 0;
	while (1) {
		wait(irq_event);
		wait(sc_core::sc_time(1.0 / 2, sc_core::SC_SEC));

		translation++;
		for (int i = 0; i < ROM_SIZE/4; i++) {
			status = initiator.read(ROM_START + 4*i, val);
			if (status != tlm::TLM_OK_RESPONSE) {
				cerr << name() << ": error while reading "
						  "memory address 0x" 
						  << hex << ROM_START+4*i
						  << endl;
			} else {
				val_wr = (val & 0xF0000000);
				val_wr += ((val & 0x0F000000) >> 4);
				val_wr += ((val & 0x00F00000) >> 8);
				val_wr += ((val & 0x000F0000) >> 12);
				initiator.write(MEM_START + (320*4*translation + 8*i) % MEM_SIZE, val_wr);
				val_wr = ((val & 0x0000F000) << 16);
				val_wr += ((val & 0x00000F00) << 12);
				val_wr += ((val & 0x000000F0) << 8);
				val_wr += ((val & 0x0000000F) << 4);
				initiator.write(MEM_START + (320*4*translation + 8*i+4) % MEM_SIZE, val_wr);
			}
		}
		initiator.write(LCDC_REG_START + 0x8, 0x0);
	}
}

void Generator::interrupt() {
	if (irq == 1) {
		notify(irq_event);
		cout << name() << " interrupt received" << endl;
	}
}

Generator::Generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(thread);
	SC_METHOD(interrupt);
	sensitive << irq;
}
