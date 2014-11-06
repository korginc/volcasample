#ifndef KORG_VOLCASAMPLE_PATTERN_H__
#define KORG_VOLCASAMPLE_PATTERN_H__

#include "korg_syro_type.h"

/*--------------------------------*/
/*--------- Pattern Data ---------*/
/*--------------------------------*/

#define VOLCASAMPLE_NUM_OF_PART				10
#define VOLCASAMPLE_NUM_OF_STEP				16

/*----- define bit of FuncMemoryPart ------*/
#define	VOLCASAMPLE_FUNC_BIT_NOTION			0
#define	VOLCASAMPLE_FUNC_BIT_LOOP			1
#define	VOLCASAMPLE_FUNC_BIT_REVERB			2
#define	VOLCASAMPLE_FUNC_BIT_REVERSE		3
#define	VOLCASAMPLE_FUNC_BIT_MUTE			4

#define	VOLCASAMPLE_FUNC_MOTION				(1 << VOLCASAMPLE_FUNC_BIT_NOTION)
#define	VOLCASAMPLE_FUNC_LOOP				(1 << VOLCASAMPLE_FUNC_BIT_LOOP)
#define	VOLCASAMPLE_FUNC_REVERB				(1 << VOLCASAMPLE_FUNC_BIT_REVERB)
#define	VOLCASAMPLE_FUNC_REVERSE			(1 << VOLCASAMPLE_FUNC_BIT_REVERSE)
#define	VOLCASAMPLE_FUNC_MUTE				(1 << VOLCASAMPLE_FUNC_BIT_MUTE)

/*---- Knob Parameter ID -----*/
#define	VOLCASAMPLE_PARAM_LEVEL				0
#define	VOLCASAMPLE_PARAM_PAN				1
#define	VOLCASAMPLE_PARAM_SPEED				2
#define VOLCASAMPLE_PARAM_AMPEG_ATTACK		3
#define VOLCASAMPLE_PARAM_AMPEG_DECAY		4
#define VOLCASAMPLE_PARAM_PITCHEG_INT		5
#define VOLCASAMPLE_PARAM_PITCHEG_ATTACK	6
#define VOLCASAMPLE_PARAM_PITCHEG_DECAY		7
#define VOLCASAMPLE_PARAM_START_POINT		8
#define VOLCASAMPLE_PARAM_LENGTH			9
#define VOLCASAMPLE_PARAM_HICUT				10
#define	VOLCASAMPLE_NUM_OF_PARAM			11

/*---- Motion Parameter ID -----*/
#define	VOLCASAMPLE_MOTION_LEVEL_0			0
#define	VOLCASAMPLE_MOTION_LEVEL_1			1
#define	VOLCASAMPLE_MOTION_PAN_0			2
#define	VOLCASAMPLE_MOTION_PAN_1			3
#define	VOLCASAMPLE_MOTION_SPEED_0			4
#define	VOLCASAMPLE_MOTION_SPEED_1			5
#define VOLCASAMPLE_MOTION_AMPEG_ATTACK		6
#define VOLCASAMPLE_MOTION_AMPEG_DECAY		7
#define VOLCASAMPLE_MOTION_PITCHEG_INT		8
#define VOLCASAMPLE_MOTION_PITCHEG_ATTACK	9
#define VOLCASAMPLE_MOTION_PITCHEG_DECAY	10
#define VOLCASAMPLE_MOTION_START_POINT		11
#define VOLCASAMPLE_MOTION_LENGTH			12
#define VOLCASAMPLE_MOTION_HICUT			13
#define	VOLCASAMPLE_NUM_OF_MOTION			14

#define	VOLCASAMPLE_PATTERN_HEADER			0x54535450		// 'PTST'
#define	VOLCASAMPLE_PATTERN_FOOTER			0x44455450		// 'PTED'
#define	VOLCASAMPLE_PATTERN_DEVCODE			0x33b8

/////////////////////////////////////////////
// !! notice !!
//
// *Those structures must not be padded by a compiler.
//  The declaration for forbidding it may be necessity. 
//  (for example, like #pragma pack(1) )
//
// *Uint16_t and uint32_t are must be little endian.
//

/*----- part data struct ------*/
typedef struct {
	uint16_t SampleNum;					// Sample num, 0~99
	uint16_t StepOn;					// Step on/off (b0~15 = STEP1~16)
	uint16_t Accent;					// Accent on/off (b0~15 = STEP 1~16, not supported)
	uint16_t Reserved;					// Reserved
	
	uint8_t Level;						// Part level 0~127, reccomend to set 127(not supported)
	uint8_t Param[VOLCASAMPLE_NUM_OF_PARAM];
	uint8_t FuncMemoryPart;				// setting (refer VOLCASAMPLE_FUNC_xxxx)
	uint8_t Padding1[11];
	
	uint8_t Motion[VOLCASAMPLE_NUM_OF_MOTION][VOLCASAMPLE_NUM_OF_STEP];
} VolcaSample_Part_Data;

/*----- pattern data struct ------*/
typedef struct {
	//----- +0x00 -----
	uint32_t Header;
	uint16_t DevCode;
	uint8_t Reserved[2];
	
	uint16_t ActiveStep;				// Active step on/off (b0~15 = STEP 1~16)
	
	uint8_t Padding1[0x16];
	
	//----- +0x20 - +0xA1F ------
	VolcaSample_Part_Data Part[VOLCASAMPLE_NUM_OF_PART];
	
	//----- +0xA20 - +0xA3F ------
	
	uint8_t Padding2[0x1c];
	uint32_t Footer;
} VolcaSample_Pattern_Data;


#ifdef __cplusplus
extern "C"
{
#endif

void VolcaSample_Pattern_Init(VolcaSample_Pattern_Data *pattern_data);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef KORG_VOLCASAMPLE_PATTERN_H__


