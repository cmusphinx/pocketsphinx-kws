// PocketSphinxTrigger.cpp : Defines the entry point for the console application.
//

#include "psphinx_trigger.h"
#include "err.h"
#include "bio.h"
#include "ckd_alloc.h"
#include "strfuncs.h"
#include "pio.h"
#include "cmd_ln.h"
#include "bin_mdef.h"
#include "pocketsphinx.h"
#include "profile.h"

int main()
{
	const char* modelDir = "C:\\Code\\Durandal\\Data\\sphinx\\en-us-semi";
	const char* dictFile = "C:\\Code\\Durandal\\Data\\sphinx\\cmudict_SPHINX_40.txt";
	void* trigger = trigger_create((char*)modelDir, (char*)dictFile, TRUE);

	const char* configuration1 = "ACTIVATE/3.16227766016838e-13/\nEXECUTE COURSE/3.16227766016838e-13/\n";
	trigger_reconfigure(trigger, (char*)configuration1);

	// Read input file 1
	FILE* file = fopen("C:\\Code\\Durandal\\Extensions\\Pocketsphinx\\Test1.raw", "r");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	int samples = (int)(length / sizeof(short));
	short* input_file_1 = (short*)malloc(length);

	fseek(file, 0, SEEK_SET);
	fread(input_file_1, 2, samples, file);
	fclose(file);

	// Send it to the trigger in chunks
	char* hyp = (char*)calloc(512, 1);
	char* lasthyp = (char*)calloc(512, 1);
	trigger_start_processing(trigger);

	for (int cursor = 0; cursor < (samples - 159); cursor += 160)
	{
		trigger_process_samples(trigger, input_file_1 + cursor, 160);
		trigger_get_last_hyp(trigger, hyp);
		if (strlen(hyp) != 0 && strcmp(hyp, lasthyp) != 0)
		{
			printf("Got trigger %s at sample number %d\n", hyp, cursor);
			strncpy(lasthyp, hyp, 512);
		}
	}

	trigger_stop_processing(trigger);

	printf("\n\nON TO TEST #2\n\n\n");

	const char* configuration2 = "COMPUTER/3.16227766016838e-13/\n";
	trigger_reconfigure(trigger, (char*)configuration2);

	// Read input file 1
	file = fopen("C:\\Code\\Durandal\\Extensions\\Pocketsphinx\\Test2.raw", "r");
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	samples = (int)(length / sizeof(short));
	short* input_file_2 = (short*)malloc(length);

	fseek(file, 0, SEEK_SET);
	fread(input_file_2, 2, samples, file);
	fclose(file);

	// Send it to the trigger in chunks
	ptmr_t timer;
	ptmr_init(&timer);
	ptmr_start(&timer);

	hyp = (char*)calloc(512, 1);
	lasthyp = (char*)calloc(512, 1);
	trigger_start_processing(trigger);

	for (int cursor = 0; cursor < (samples - 159); cursor += 160)
	{
		trigger_process_samples(trigger, input_file_2 + cursor, 160);
		trigger_get_last_hyp(trigger, hyp);
		if (strlen(hyp) != 0 && strcmp(hyp, lasthyp) != 0)
		{
			printf("Got trigger %s at sample number %d\n", hyp, cursor);
			strncpy(lasthyp, hyp, 512);
		}
	}

	trigger_stop_processing(trigger);

	ptmr_stop(&timer);
	printf("Time: %f\n", timer.t_tot_elapsed);

	trigger_free(trigger);
}

