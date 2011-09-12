#include "dat.h"
#include "fns.h"
#include "error.h"
#include "audio.h"
#include "audio-tbls.c"

Audio_t*
getaudiodev(void)
{
	return &av;
}

void 
audio_file_init(void)
{
	audio_info_init(&av);
}

void
audio_file_open(Chan *c, int omode)
{
	
}
