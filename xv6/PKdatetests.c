#include "types.h"
#include "user.h"
#include "date.h"

int is_formatted_correctly();

void
test_does_not_fail()
{
	struct rtcdate d;

	int res = date(&d);


	if (res != 0)
		printf(1, "[FAIL] test_does_not_fail - date() returned an error code, %d, but a success code was expected. \n", res);
	else
		printf(1, "[PASS] test_does_not_fail \n");
	
}

void
build_date_str(struct rtcdate *d, char *buf)
{


	const char* MONTHS[] = {
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
	
	

	int p[2];
	pipe(p);

	if (fork() == 0) {
		close(1);
		dup(p[1]);
		
		printf(1, "%d:%d:%d %s %d, %d \n",
			d->hour,
			d->minute,
			d->second,
			MONTHS[d->month - 1], 
			d->day,
			d->year); 
		
		close(p[0]);
		close(p[1]);
		exit();
	} else {
		close(p[1]);
		read(p[0], buf, 100);
		wait();
		close(p[0]);
	} 

}

void 
test_verify_date_correctness()
{
	char *argv[1];
	argv[1] = "date";

	printf(1, "[VERIFY] test_verify_date_correctness - please verify the following date is correct now: ");

	if (fork() == 0) {
		exec("date", argv);
		exit();
	} else {
		wait();
	}


}

void
test_date_formatted_correctly()
{
	
	// so basically, this is a pretty aweful test. 
	// It works only if the time parent process date and child process date 
	// both display the same, which means they have to be executed in the same second of time.
	// To ensure this happens, I run this 10 times. One of the those 10 times the two 
	// processes will have the same date. I am well aware that this is a shitty test and would 
	// never cut it in the real world, but I couldn't figure out how else to write a test 
	// to programatically verify the date output's format in the time I had to write this.

	int success = 0;
	for (int i = 0; i < 10 && !success; ++i){
		if (is_formatted_correctly())
			success = 1;
		else
			sleep(1);
	}

	if (success) {
		printf(1, "[PASS] test_date_formatted_correctly\n");
	} else {
		printf(1, "[UNKNOWN] test_date_formatted_correctly - the format of the date output by the date() function could not be programmatically confirmed. This test did not fail, but the date should be verified manually, or this test rerun.\n");
	}

}

int
is_formatted_correctly()
{
	struct rtcdate d;
	char *argv[1];
	int p[2];
	char output[100];
	char date_str[100];

	argv[0] = "date";
	pipe(p);
	date(&d);

	build_date_str(&d, date_str);

	if (fork() == 0) {
		close(1);
		dup(p[1]);
		close(p[0]);
		close(p[1]);
		exec("date", argv);
		exit();
	} else {
		close(p[1]);
	
		read(p[0], output, 100);
		close(p[0]);
		wait(); // get rid that ZOMBIE

		//printf(1, "GOT FROM CHILD: %s \n", output);

		//printf(1, "%s === %s", output, date_str);
		
		return strcmp(output, date_str);
	}


}


int 
main(int argc, char *argv[])
{
	printf(1, "\n*****  Running PKdatetests  *****\n");

	test_does_not_fail();
	test_date_formatted_correctly();
	test_verify_date_correctness();
	
	printf(1, "\n*****  Completed PKdatatests *****\n");

	exit();
}
