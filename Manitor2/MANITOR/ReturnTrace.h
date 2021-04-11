#ifndef __Manitor_ReturnTrace_h__#define __Manitor_ReturnTrace_h__#include <gemdos.h>#define MAX_RETURNS			250#define MAX_LAYER_RETURNS	50typedef struct {	int16 anz;	int16 maxanz;	int32 entrysize;} ReturnTable;typedef struct {	int16 layer;	void *info;	int32 time;	BASPAG *bp;	int16 is_super;} ReturnInfo;typedef struct {	ReturnTable table;	ReturnInfo entries[MAX_RETURNS];} ReturnInfoTable;extern ReturnInfoTable return_info_table;typedef struct {	int16 opcode;	int32 ret;} BiosReturnInfo;typedef struct {	ReturnTable table;	BiosReturnInfo entries[MAX_LAYER_RETURNS];} BiosReturnInfoTable;extern BiosReturnInfoTable biosreturn_info_table;typedef struct {	int16 opcode;	int32 ret;} XbiosReturnInfo;typedef struct {	ReturnTable table;	XbiosReturnInfo entries[MAX_LAYER_RETURNS];} XbiosReturnInfoTable;extern XbiosReturnInfoTable xbiosreturn_info_table;#define GEMDOSPARA_MAX_LONGS 6#define GEMDOSPARA_MAX_WORDS (GEMDOSPARA_MAX_LONGS * 2)typedef struct {	int16 opcode;	int32 ret;	int16 para[GEMDOSPARA_MAX_WORDS];} GemdosReturnInfo;typedef struct {	ReturnTable table;	GemdosReturnInfo entries[MAX_LAYER_RETURNS];} GemdosReturnInfoTable;extern GemdosReturnInfoTable gemdosreturn_info_table;typedef struct {	int16 opcode;} VDIReturnInfo;typedef struct {	ReturnTable table;	VDIReturnInfo entries[MAX_LAYER_RETURNS];} VDIReturnInfoTable;extern VDIReturnInfoTable vdireturn_info_table;typedef struct {	int16 opcode;} AESReturnInfo;typedef struct {	ReturnTable table;	AESReturnInfo entries[MAX_LAYER_RETURNS];} AESReturnInfoTable;extern AESReturnInfoTable aesreturn_info_table;void *get_next_table_entry(ReturnTable *table);void *begin_layer_table_entry(ReturnTable *layertable, int16 layer, int16 is_super);void end_layer_table_entry(int16 is_super);extern void Output_Returns();extern void Output_BiosReturn(BiosReturnInfo *info);extern void Output_XbiosReturn(XbiosReturnInfo *info);extern void Output_GemdosReturn(GemdosReturnInfo *info);extern void Output_VDIReturn(VDIReturnInfo *info);extern void Output_AESReturn(AESReturnInfo *info);#endif