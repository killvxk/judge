#ifndef _SYSMNG_H_
#define _SYSMNG_H_

enum SYSMNG_CMO_TBLID_EM {
	SYSMNG_CMO_TBL,		
};

enum SYSMNG_CMO_ID_EM {
	SYSMNG_CMO_UNDO = CMD_ELEMID_DEF(MID_SYSMNG, SYSMNG_CMO_TBL, 0),
	SYSMNG_CMO_VERSION,
	SYSMNG_CMO_SYSNAME_STRING,
	SYSMNG_CMO_DIR,
};

/* web��kernel��ͨ��ʹ�õ�ħ���� */
#define SYSMNG_MSG_MAGIC_NUM 0xabcddcba

/* web��kernel��ͨ�ŵ���Ϣ���ͣ��ɴ����µ��� */
#define SYSMNG_MSG_TYPE_CMD 0x0001

extern char *SYSMNG_GetSysname();
extern ULONG SYSMNG_IsCfgRecoverOver();
extern void SYSMNG_CfgRecover();
extern int SYSMNG_PacketParse(char *pszCmd, int len);

#endif

