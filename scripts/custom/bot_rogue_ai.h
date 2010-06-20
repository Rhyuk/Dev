#include "bot_ai.h"


#define BACKSTAB        BACKSTAB_A[SPELL_LEVEL]
#define SINISTER_STRIKE SINISTER_STRIKE_A[SPELL_LEVEL]
#define SLICE_DICE      SLICE_DICE_A[SPELL_LEVEL]
#define EVISCERATE      EVISCERATE_A[SPELL_LEVEL]
#define KICK            KICK_A[SPELL_LEVEL]
#define RUPTURE         RUPTURE_A[SPELL_LEVEL]
#define WOUND_POISON    WOUND_POISON_A[SPELL_LEVEL]
#define DEADLY_POISON   DEADLY_POISON_A[SPELL_LEVEL]
#define DISMANTLE       DISMANTLE_A[SPELL_LEVEL]
#define KIDNEY_SHOT_1   8643
#define KIDNEY_SHOT_2   30832
#define KIDNEY_SHOT_3   41389
#define SHADOWSTEP      SHADOWSTEP_A[SPELL_LEVEL]
#define MUTILATE        MUTILATE_A[SPELL_LEVEL]


uint32 MUTILATE_A[] = {0, 0, 0, 0, 48664, 48664, 48664,48664, 48664};
uint32 BACKSTAB_A[] = { 53, 2589, 2591, 8721, 11279, 11280, 11281, 25300, 25300 };
uint32 SINISTER_STRIKE_A[] = { 1757, 1758, 1759, 8621, 11293, 11294, 26862, 48638, 48638 };
uint32 SLICE_DICE_A[] = { 0, 5171, 5171, 5171, 6774, 6774, 6774, 6774, 6774, 6774 };
uint32 EVISCERATE_A[]= { 11300, 11300, 11300, 11300, 11300, 11300, 11300, 11300, 11300 };
uint32 KICK_A[] = { 0, 1766, 1767, 1767, 1768, 1769, 38768, 38768, 38768 };
uint32 RUPTURE_A[] = { 0, 0, 1943, 8640, 11273, 11274, 11275, 26867, 48672, 48672 };
uint32 WOUND_POISON_A[] = { 0, 0, 0, 13218, 13222, 13223, 13224, 27189, 57974, 57974 };
uint32 DEADLY_POISON_A[] = { 0, 0, 0, 2818, 2819, 11354, 26968, 57969, 57969 };
uint32 DISMANTLE_A[] = { 0, 0, 51722, 51722, 51722, 51722, 51722, 51722, 51722 };
uint32 SHADOWSTEP_A[] = { 0, 0, 0, 0, 0, 36554, 36554, 36554, 36554 };

