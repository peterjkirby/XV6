#include "types.h"
#include "user.h"
#include "date.h"

static char* MONTHS[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dev"
};

int main(int arc, char *argv[])
{

	struct rtcdate d;

	if (date(&d)) {
		printf(2, "date failed \n");
		exit();
	}

	printf(1, "%d:%d:%d %s %d, %d \n",
    	d.hour,
		d.minute,
		d.second,
  		MONTHS[d.month - 1], 
		d.day,
		d.year); 

	exit();

}
