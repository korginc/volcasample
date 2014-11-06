/************************************************************************
	volca sample pattern
 ***********************************************************************/
#include <string.h>
#include "../syro/korg_syro_type.h"
#include "volcasample_pattern.h"

static const uint8_t ParamInitValue[VOLCASAMPLE_NUM_OF_PARAM] = {
	0x7f,			// VOLCASAMPLE_PARAM_LEVEL 
	0x40,			// VOLCASAMPLE_PARAM_PAN 
	0x40,			// VOLCASAMPLE_PARAM_SPEED 
	0x00,			// VOLCASAMPLE_PARAM_AMPEG_ATTACK 
	0x7F,			// VOLCASAMPLE_PARAM_AMPEG_DECAY 
	0x40,			// VOLCASAMPLE_PARAM_PITCHEG_INT 
	0x00,			// VOLCASAMPLE_PARAM_PITCHEG_ATTACK 
	0x7F,			// VOLCASAMPLE_PARAM_PITCHEG_DECAY
	0x00,			// VOLCASAMPLE_PARAM_START_POINT 
	0x7f,			// VOLCASAMPLE_PARAM_LENGTH 
	0x7f			// VOLCASAMPLE_PARAM_HICUT 
};

/*----------------------------------------------------------------------------
	Write 32Bit Value
 ----------------------------------------------------------------------------*/
static void set_32Bit_value(uint8_t *ptr, uint32_t dat)
{
	int i;
	
	for (i=0; i<4; i++) {
		*ptr++ = (uint8_t)dat;
		dat >>= 8;
	}
}

/*----------------------------------------------------------------------------
	Write 16Bit Value
 ----------------------------------------------------------------------------*/
static void set_16Bit_value(uint8_t *ptr, uint16_t dat)
{
	int i;
	
	for (i=0; i<2; i++) {
		*ptr++ = (uint8_t)dat;
		dat >>= 8;
	}
}

/*=======================================================================
	Init pattern data
 =======================================================================*/
void VolcaSample_Pattern_Init(VolcaSample_Pattern_Data *pattern_data)
{
	int part;
	
	memset((uint8_t *)pattern_data, 0, sizeof(VolcaSample_Pattern_Data));
	
	set_32Bit_value((uint8_t *)&pattern_data->Header, VOLCASAMPLE_PATTERN_HEADER);
	set_16Bit_value((uint8_t *)&pattern_data->DevCode, VOLCASAMPLE_PATTERN_DEVCODE);

	set_16Bit_value((uint8_t *)&pattern_data->ActiveStep, 0xffff);
	
	for (part=0; part<VOLCASAMPLE_NUM_OF_PART; part++) {
		pattern_data->Part[part].Level = 0x7f;
		pattern_data->Part[part].FuncMemoryPart = VOLCASAMPLE_FUNC_MUTE;
		memcpy(pattern_data->Part[part].Param, ParamInitValue, VOLCASAMPLE_NUM_OF_PARAM);
	}
	
	set_32Bit_value((uint8_t *)&pattern_data->Footer, VOLCASAMPLE_PATTERN_FOOTER);
}


