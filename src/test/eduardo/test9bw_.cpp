#include <emmintrin.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/transaction.hpp>

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
	
	if (!access("/mnt/pmem/file", F_OK))
		remove ("/mnt/pmem/file");
	
	pop = pobj::pool<root>::create ("/mnt/pmem/file", "RECORDS", PMEMOBJ_MIN_POOL, 0666);
	auto proot = pop.root ();

	pobj::transaction::run (pop, [&] {
		proot->header = pobj::make_persistent<header_t> ();
		proot->header->counter = 0;
		proot->records = pobj::make_persistent<record_t[]> (10);
	});	
	pobj::persistent_ptr<header_t> header  = proot->header;
	pobj::persistent_ptr<record_t[]> records = proot->records;

	header->counter = 0;
	for (uint8_t i = 0; i < 10; i++) {
		header->counter++;
		if (rand() % 2 == 0) {
			snprintf (records[i].name, 63, "record #%u", i + 1);
			pop.persist (records[i].name, 63);
			records[i].valid = 2;
		} else
			records[i].valid = 1;
		pop.persist (&(records[i].valid), 1);
	}
	pop.persist (&(header->counter), 4);

	pop.close ();
	return 0;
}
