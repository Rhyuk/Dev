
#include "bot_ai.h"


enum AURAS {
    DEVOTIONAURA,
    FIRERESISTANCEAURA,
    NOAURA
} ;

AURAS Aura;


// misc
#define BANDAGE 27031
#define MANAPOTION 28499
#define REJUVEPOTION 28517
#define HEALINGPOTION 28495
#define POTIONCD 300
#define FIRSTAID 600


// Heals
#define FLASH_OF_LIGHT                FLASH_OF_LIGHT_A[SPELL_LEVEL]
#define HOLY_LIGHT                    HOLY_LIGHT_A[SPELL_LEVEL]
#define LAY_OF_HANDS                  LAY_OF_HANDS_A[SPELL_LEVEL]
#define REDEMPTION                    REDEMPTION_A[SPELL_LEVEL]

#define CLEANSE                         CLEANSE_A[SPELL_LEVEL]

uint32 FLASH_OF_LIGHT_A[] = {0, 0, 19939, 19940, 19941, 19942, 27137, 48784, 48785, 48785};
uint32 HOLY_LIGHT_A[] = {635, 647, 1026, 3472, 10328, 10329, 27135, 48781, 48782, 48782};
uint32 LAY_OF_HANDS_A[] = {0, 633, 633, 2800, 2800, 10310, 10310, 27154, 48788, 48788};
uint32 REDEMPTION_A[] = {0, 7328, 10322, 10324, 20772, 20772, 20773, 48949, 48950, 48950};
uint32 CLEANSE_A[] = {0, 0, 0, 0, 4987, 4987, 4987, 4987, 4987};

// Seals
#define SEAL_OF_LIGHT	        SEAL_OF_LIGHT_A[SPELL_LEVEL]
#define JUDGEMENT_OF_LIGHT      JUDGEMENT_OF_LIGHT_A[SPELL_LEVEL]

uint32 SEAL_OF_LIGHT_A[] = {0, 0, 0, 20165, 20165, 20165, 20165, 20165, 20165, 20165};
uint32 JUDGEMENT_OF_LIGHT_A[] = {0, 0, 0, 20185, 20185, 20185, 20185, 20185, 20185, 20185};

// Blessings
#define BLESSING_OF_WISDOM      BLESSING_OF_WISDOM_A[SPELL_LEVEL]
#define BLESSING_OF_KINGS       BLESSING_OF_KINGS_A[SPELL_LEVEL]
#define BLESSING_OF_SANCTUARY   BLESSING_OF_SANCTUARY_A[SPELL_LEVEL]

uint32 BLESSING_OF_WISDOM_A[] = {0, 19742, 19850, 19852, 19853, 19854, 25290, 48935, 48936, 48936};
uint32 BLESSING_OF_KINGS_A[] = {0, 0, 56525, 56525, 56525, 56525, 56525, 56525, 56525, 56525};
uint32 BLESSING_OF_SANCTUARY_A[] = {0, 0, 0, 67480, 67480, 67480, 67480, 67480, 67480, 67480};


// Auras
#define FIRE_RESISTANCE_AURA      FIRE_RESISTANCE_AURA_A[SPELL_LEVEL]
#define DEVOTION_AURA             DEVOITION_AURA_A[SPELL_LEVEL]

uint32 FIRE_RESISTANCE_AURA_A[] = {0, 0, 0, 19891, 19899, 19899, 19900, 27153, 48947, 48947};
uint32 DEVOITION_AURA_A[] = {465, 10290, 643, 10291, 1032, 10292, 10293, 48941, 48942, 48942};


// Others
#define HAMMER_OF_JUSTICE         HAMMER_OF_JUSTICE_A[SPELL_LEVEL]

uint32 HAMMER_OF_JUSTICE_A[] = {0, 853, 5588, 5588, 5589, 10308, 10308, 37369, 37369};
