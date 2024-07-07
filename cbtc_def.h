#ifdef BLOCK_ID_DEFINITIONS
typedef enum cbtc_block_id {
  ABC,
  DEF,
  END_OF_BLOCKS
} CBTC_BLOCK_ID;
#endif

#ifdef BLOCK_ATTRIB_DEFINITION
#ifdef CBTC_C
CBTC_BLOCK block_state[] = {
  {ABC, "ABC", 1234, TRUE}
};
#else
extern CBTC_BLOCK block_state[];
#endif
#endif // BLOCK_ATTRIB_DEFINITION
