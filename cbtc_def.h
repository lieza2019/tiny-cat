#ifdef BLOCK_ID_DEFINITIONS
typedef enum block_id {
  ABC,
  DEF,
  END_OF_BLOCKS
} BLOCK_ID;
#endif

#ifdef BLOCK_ATTRIB_DEFINITION
#ifdef CBTC_C
BLOCK block_state[] = {
  {ABC, "ABC", 1234, TRUE}
};
#else
extern BLOCK block_state[];
#endif
#endif // BLOCK_ATTRIB_DEFINITION
