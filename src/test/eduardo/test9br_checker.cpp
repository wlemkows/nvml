#include <stdio.h>
#include <stdint.h>
#include <libpmemobj++/persistent_ptr.hpp>

using namespace std;
namespace pobj = pmem::obj;

struct header_t {
	uint32_t counter;
	uint8_t reserved[60];
};
struct record_t {
	char name[63];
	char valid;
};
struct root {
	pobj::persistent_ptr<header_t> header;
	pobj::persistent_ptr<record_t[]> records;
};

pobj::pool<root> pop;


int main (int argc, char *argv[]) {

	pop = pobj::pool<root>::open ("/mnt/pmem/file", "RECORDS");
	auto proot = pop.root ();
	pobj::persistent_ptr<header_t> header = proot->header;
	pobj::persistent_ptr<record_t[]> records = proot->records;

	for (uint8_t i = 0; i < header->counter; i++) {
		if (records[i].valid < 1 or records[i].valid > 2)
			return 1;
	}

	pop.close ();
	return 0;
}
