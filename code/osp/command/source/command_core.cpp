/*
	Code source reference from Zebra.
	
	Author: Jungle Wei
	Date  : 2013-10
	Description: A common command line module

	֧������: TAB�Զ�ѡ���Բ�ȫ, '?'���� , CTRL+W ����ɾ��, ����λ����ʾ, �����д�����


				 �������

				��ͷ΢��΢�㣬
				���۰��Ű��ѡ�
				���������ޣ�
				����̫��������
				������
				������
				���紵δͣ��

*/

#include "osp\command\include\command_inc.h"
#include "product\judge\include\judge_inc.h"

/* BEGIN: Added by weizengke, 2013/12/12 for display command-tree*/
void cmd_show_command_tree(struct cmd_vty *vty)
{
	int i;
	int used_size;

	cmd_vector_t *cmd_vec_copy = cmd_vector_copy(cmd_vec);
	if (NULL == cmd_vec_copy)
	{
		return ;
	}

	used_size = cmd_vector_max(cmd_vec_copy);
	vty_printf(vty, "Command active (%u):\r\n", used_size);

	for (i = 0; i < cmd_vector_max(cmd_vec_copy); i++)
	{
		struct cmd_elem_st * cmd_elem = (struct cmd_elem_st *)cmd_vector_slot(cmd_vec_copy, i);
		if (NULL == cmd_elem)
		{
			continue;
		}

		vty_printf(vty,"%s\r\n", cmd_elem->string);
	}
}

/*****************************************************************************
 Prototype    : cmd_elem_is_para_type
 Description  : �������Ԫ���Ƿ���һ������
 Input        : CMD_ELEM_TYPE_EM type
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2013/10/5
    Author       : weizengke
    Modification : Created function

*****************************************************************************/
int cmd_elem_is_para_type(CMD_ELEM_TYPE_EM type)
{
	switch(type)
	{
		case CMD_ELEM_TYPE_INTEGER:
		case CMD_ELEM_TYPE_STRING:
			return CMD_YES;
		default:
			break;
	}

	return CMD_NO;
}


int cmd_get_elemid_by_name(int *cmd_elem_id, char *cmd_name)
{
	int i = 0;

	if (cmd_elem_id == NULL || cmd_name == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd_get_elemid_by_name, param is null");
		return 1;
	}

	for (i = 0; i < CMD_ELEM_ID_MAX; i++)
	{
		if (0 == strcmp(cmd_name, g_cmd_elem[i].para))
		{
			*cmd_elem_id = i;
			return 0;
		}
	}

	return 1;
}


int cmd_get_elem_by_id(int cmd_elem_id, struct para_desc *cmd_elem)
{
	if (cmd_elem == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd_get_elem_by_id, param is null");
		return 1;
	}

	if (cmd_elem_id <= CMD_ELEM_ID_NONE || cmd_elem_id >=  CMD_ELEM_ID_MAX)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd_get_elem_by_id, cmd_elem_id is invalid");
		return 1;
	}

	*cmd_elem = g_cmd_elem[cmd_elem_id];

	return 0;
}

int cmd_get_elem_by_name(char *cmd_name, struct para_desc *cmd_elem)
{
	int i = 0;

	if (cmd_name == NULL || cmd_elem == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd_get_elem_by_name, param is null");
		return 1;
	}

	for (i = 0; i < CMD_ELEM_ID_MAX; i++)
	{
		if ( g_cmd_elem[i].para == NULL)
		{
			continue;
		}

		if (0 == strcmp(cmd_name, g_cmd_elem[i].para))
		{
			*cmd_elem = g_cmd_elem[i];
			return 0;
		}
	}

	return 1;
}


/*****************************************************************************
 Prototype    : cmd_reg_newcmdelement
 Description  : ע������Ԫ��
 Input        : int cmd_elem_id
                char *cmd_name
                char *cmd_help
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2013/10/4
    Author       : weizengke
    Modification : Created function

*****************************************************************************/
int cmd_reg_newcmdelement(int cmd_elem_id, CMD_ELEM_TYPE_EM cmd_elem_type, const char *cmd_name, const char *cmd_help)
{

	/* BEGIN: Added by weizengke, 2013/10/4   PN:������ҪУ��Ϸ��� , �����淶����������� */

	g_cmd_elem[cmd_elem_id].para = (char*)malloc(strlen(cmd_name) + 1);
	if (g_cmd_elem[cmd_elem_id].para == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "malloc memory for para fail in regNewCmdElement.");
		return 1;
	}

	g_cmd_elem[cmd_elem_id].desc = (char*)malloc(strlen(cmd_help) + 1);
	if (g_cmd_elem[cmd_elem_id].desc == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "malloc memory for desc fail in regNewCmdElement.");
		free(g_cmd_elem[cmd_elem_id].para);
		return 1;
	}

	g_cmd_elem[cmd_elem_id].elem_id = cmd_elem_id;
	g_cmd_elem[cmd_elem_id].elem_tpye = cmd_elem_type;
	strcpy(g_cmd_elem[cmd_elem_id].para, cmd_name);
	strcpy(g_cmd_elem[cmd_elem_id].desc, cmd_help);

	CMD_debug(DEBUG_TYPE_MSG, "cmd_reg_newcmdelement(%d %d %s %s) ok.",
				cmd_elem_id,
				cmd_elem_type,
				g_cmd_elem[cmd_elem_id].para,
				g_cmd_elem[cmd_elem_id].desc);

	return 0;

}



/* vty */
void cmd_vty_init(struct cmd_vty *vty)
{
	vty->valid = 0;
	vty->view_id = VIEW_USER;
	memcpy(vty->prompt, CMD_PROMPT_DEFAULT, sizeof(CMD_PROMPT_DEFAULT));
	vty->prompt[strlen(CMD_PROMPT_DEFAULT)] = '\0';
	vty->buf_len = CMD_BUFFER_SIZE;
	vty->used_len = vty->cur_pos = 0;
	vty->hpos = vty->hindex = 0;

	vty->inputMachine_prev = CMD_KEY_CODE_NOTCARE;
	vty->inputMachine_now = CMD_KEY_CODE_NOTCARE;
	memset(vty->tabbingString, 0, CMD_MAX_CMD_ELEM_SIZE);
	memset(vty->tabString, 0, CMD_MAX_CMD_ELEM_SIZE);
	vty->tabStringLenth = 0;

	vty->user.socket = INVALID_SOCKET;
	vty->user.state = 0;
	vty->user.terminal_debugging = OS_NO;
	memset(vty->user.user_name, 0, sizeof(vty->user.user_name));
	vty->user.lastAccessTime = time(NULL);
	
	return ;
}

void cmd_vty_deinit(struct cmd_vty *vty)
{
	int i;

	if (!vty)
	{
		return;
	}

	for (i = 0; i < HISTORY_MAX_SIZE; i++)
	{
		if (vty->history[i] != NULL)
		{
			free(vty->history[i]);
			vty->history[i] = NULL;
		}
	}

	free(vty);
}

void cmd_vty_add_history(struct cmd_vty *vty)
{
	int idx =  vty->hindex ? vty->hindex - 1 : HISTORY_MAX_SIZE - 1;

	/* if same as previous command, then ignore */
	if (vty->history[idx] &&
		!strcmp(vty->buffer, vty->history[idx]))
	{
		vty->hpos = vty->hindex;
		return;
	}

	/* insert into history tail */
	if (vty->history[vty->hindex])
	{
		free(vty->history[vty->hindex]);
	}

	vty->history[vty->hindex] = strdup(vty->buffer);
	vty->hindex = (vty->hindex + 1) == HISTORY_MAX_SIZE ? 0 : vty->hindex + 1;
	vty->hpos = vty->hindex;

}

/* vector */
int cmd_vector_fetch(cmd_vector_t *v)
{
	int fetch_idx;

	for (fetch_idx = 0; fetch_idx < v->used_size; fetch_idx++)
	{
		if (v->data[fetch_idx] == NULL)
		{
			break;
		}
	}

	while (v->size < fetch_idx + 1)
	{
		v->data = (void**)realloc(v->data, sizeof(void *) * v->size * 2);
		if (!v->data)
		{
			CMD_debug(DEBUG_TYPE_ERROR, "In cmd_vector_fetch, Not Enough Memory For data");
			return -1;
		}

		memset(&v->data[v->size], 0, sizeof(void *) * v->size);
		v->size *= 2;
	}

	return fetch_idx;
}

cmd_vector_t *cmd_vector_init(int size)
{
	cmd_vector_t *v = (cmd_vector_t *)calloc(1, sizeof(struct cmd_vector));
	if (v == NULL)
	{
		return NULL;
	}

	if (size == 0)
	{
		size = 1;
	}

	v->data = (void**)calloc(1, sizeof(void *) * size);
	if (v->data == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd_vector_init, Not Enough Memory For data");
		free(v);
		return NULL;
	}

	v->used_size = 0;
	v->size = size;
	return v;
}

void cmd_vector_deinit(cmd_vector_t *v, int freeall)
{
	if (v == NULL)
	{
		return;
	}

	if (v->data)
	{
		if (freeall)
		{
			int i;
			for (i = 0; i < cmd_vector_max(v); i++)
			{
				if (cmd_vector_slot(v, i))
				{
					free(cmd_vector_slot(v, i));
				}
			}
		}

		free(v->data);
	}

	free(v);
}

cmd_vector_t *cmd_vector_copy(cmd_vector_t *v)
{
	int size;
	cmd_vector_t *new_v = (cmd_vector_t *)calloc(1, sizeof(cmd_vector_t));
	if (NULL == new_v)
	{
		CMD_DBGASSERT(0, "cmd_vector_copy");
		return NULL;
	}

	new_v->used_size = v->used_size;
	new_v->size = v->size;

	size = sizeof(void *) * (v->size);
	new_v->data = (void**)calloc(1, sizeof(void *) * size);
	memcpy(new_v->data, v->data, size);

	return new_v;
}

void cmd_vector_insert(cmd_vector_t *v, void *val)
{
	int idx;

	idx = cmd_vector_fetch(v);
	v->data[idx] = val;
	if (v->used_size <= idx)
	{
		v->used_size = idx + 1;
	}
}

/*
	�������û�����ĩβ��<CR>��������cmd_vector_t->data
*/
void cmd_vector_insert_cr(cmd_vector_t *v)
{
	char *string_cr = NULL;
	string_cr = (char*)malloc(sizeof(CMD_END));
	if (NULL == string_cr)
	{
		CMD_DBGASSERT(0,"cmd_vector_insert_cr");
		return;
	}

	memcpy(string_cr, CMD_END, sizeof(CMD_END));
	cmd_vector_insert(v, string_cr); /* cmd_vector_insert(v, CMD_END); // bug of memory free(<CR>), it's static memory*/
}

cmd_vector_t *str2vec(char *string)
{
	int str_len;
	char *cur, *start, *token;
	cmd_vector_t *vec;

	// empty string
	if (string == NULL)
	{
		return NULL;
	}

	cur = string;
	// skip white spaces
	while (isspace((int) *cur) && *cur != '\0')
	{
		cur++;
	}

	// only white spaces
	if (*cur == '\0')
	{
		return NULL;
	}

	// copy each command pieces into vector
	vec = cmd_vector_init(1);
	while (1)
	{
		start = cur;
		while (!(isspace((int) *cur) || *cur == '\r' || *cur == '\n') &&
				*cur != '\0')
		{
			cur++;
		}

		str_len = cur - start;
		token = (char *)malloc(sizeof(char) * (str_len + 1));
		if (NULL == token)
		{
			CMD_debug(DEBUG_TYPE_ERROR, "In str2vec, There is no memory for param token.");
			return NULL;
		}

		memcpy(token, start, str_len);
		*(token + str_len) = '\0';
		cmd_vector_insert(vec, (void *)token);

		while((isspace ((int) *cur) || *cur == '\n' || *cur == '\r') &&
			*cur != '\0')
		{
			cur++;
		}

		if (*cur == '\0')
		{
			return vec;
		}

	}
}

/* end vector */


/* cmd */
static int match_unique_string(struct para_desc **match, char *str, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (match[i]->para != NULL && strcmp(match[i]->para, str) == 0)
		{
			return 0;
		}
	}

	return 1;
}


int cmd_get_nth_elem_pos(char *string, int n, int *pos)
{
	int m = 0;
	char *cur, *pre, *start;

	*pos = 0;

	// empty string
	if (string == NULL)
	{
		return CMD_ERR;
	}

	cur = string;
	pre = NULL;

	// skip white spaces
	while (isspace((int) *cur) && *cur != '\0')
	{
		pre = cur;
		cur++;
	}

	// only white spaces
	if (*cur == '\0')
	{
		return CMD_ERR;
	}

	while (1)
	{
		start = cur;
		while (!(isspace((int) *cur) || *cur == '\r' || *cur == '\n') &&
			*cur != '\0')
		{
			pre = cur;
			cur++;
		}

		if (n == m)
		{
			*pos = (int)(start - string);
			break;
		}
		else
		{
			m++;
		}

		while((isspace ((int) *cur) || *cur == '\n' || *cur == '\r') &&
			*cur != '\0')
		{
			pre = cur;
			cur++;
		}

		if (*cur == '\0')
		{
			/* BEGIN: Added by weizengke, 2014/3/9 �޸������в���ȫʱ������λ����ʾ��׼ȷ������ */
			*pos = (int)(cur - string);

			if (!isspace ((int) *pre))
			{
				*pos += 1;
			}

			break;
		}

	}

	return CMD_OK;
}

/*****************************************************************************
*   Prototype    : cmd_output_missmatch
*   Description  : Output miss match command position
*   Input        : cmd_vty *vty
*                  int nomath_pos
*   Output       : None
*   Return Value : void
*   Calls        :
*   Called By    :
*
*   History:
*
*       1.  Date         : 2013/11/19
*           Author       : weizengke
*           Modification : Created function
*
*****************************************************************************/
void cmd_output_missmatch(cmd_vty *vty, int nomath_pos)
{
	int i = 0;
	int n = 0;
	int view_len = cmd_view_getaislenth(vty);
	int pos_arrow = strlen(g_sysname) + view_len + 1;

	/* ϵͳ������ͼ֮����һ�����ӷ�- */
	if (view_len > 0)
	{
		pos_arrow += 1;
	}

	(void)cmd_get_nth_elem_pos(vty->buffer, nomath_pos, &n);
	pos_arrow += n;

	for (i=0;i<pos_arrow;i++)
	{
		vty_printf(vty, " ");
	}

	vty_printf(vty, "^\r\nError: Unrecognized command at '^' position.\r\n");

}


cmd_vector_t *cmd2vec(char *string, char *doc)
{
	char *sp=NULL, *d_sp=NULL;
	char *cp = string;
	char *d_cp = doc;
	char *token=NULL, *d_token=NULL;
	int len, d_len;

	cmd_vector_t *allvec=NULL;
	struct para_desc *desc = NULL;
	struct para_desc *desc_cr = NULL;

	if(cp == NULL)
	{
		return NULL;
	}

	allvec = cmd_vector_init(1);
	while (1)
	{
		while(isspace((int) *cp) && *cp != '\0')
		{
			cp++;
		}

		if(*cp == '\0')
		{
			break;
		}

		sp = cp;

		while(!(isspace ((int) *cp) || *cp == '\r' || *cp == '\n')
			  && *cp != '\0')
		{
			cp++;
		}

		len = cp - sp;
		token = (char*)malloc(len + 1);
		if (NULL == token)
		{
			CMD_debug(DEBUG_TYPE_ERROR, "In cmd2vec, There is no memory for param token.");
			return NULL;
		}

		memcpy(token, sp, len);
		*(token + len) = '\0';

		while(isspace((int)*d_cp) && *d_cp != '\0')
		{
			d_cp++;
		}

		if (*d_cp == '\0')
		{
			d_token = NULL;
		}
		else
		{
			d_sp = d_cp;

			while(!(*d_cp == '\r' || *d_cp == '\n') && *d_cp != '\0')
			{
				d_cp++;
			}

			d_len = d_cp - d_sp;
			d_token = (char*)malloc(d_len + 1);
			if (NULL == d_token)
			{
				CMD_debug(DEBUG_TYPE_ERROR, "In cmd2vec, There is no memory for param d_token.");
				free(token);
				return NULL;
			}

			memcpy(d_token, d_sp, d_len);
			*(d_token + d_len) = '\0';
		}

		desc = (struct para_desc *)calloc(1, sizeof(struct para_desc));
		if (desc == NULL)
		{
			CMD_debug(DEBUG_TYPE_ERROR, "In cmd2Vec, calloc for desc fail. (token=%s)", token);
			free(token);
			free(d_token);
			return NULL;
		}

		/* BEGIN: Added by weizengke, 2013/10/4   PN:for regCmdElem  */
		if (0 != cmd_get_elem_by_name(token, desc))
		{
			CMD_DBGASSERT(0, "cmd2vec->cmd_get_elem_by_name, maybe element not exist.");
			CMD_debug(DEBUG_TYPE_ERROR, "In cmd2Vec, cmd_get_elem_by_name fail. (token=%s)", token);
			free(token);
			free(d_token);
			free(desc);
			return NULL;
		}
		/* END:   Added by weizengke, 2013/10/4   PN:None */

		cmd_vector_insert(allvec, (void *)desc);
	}

	/* add <CR> into command vector */
	desc_cr = (struct para_desc *)calloc(1, sizeof(struct para_desc));
	if (desc_cr == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd2Vec, calloc for desc_cr fail. (token=%s)", token);
		cmd_vector_deinit(allvec, 1);
		return NULL;
	}

	/* BEGIN: Added by weizengke, 2013/10/4   PN:for regCmdElem  */
	if (0 != cmd_get_elem_by_name((char*)CMD_END, desc_cr))
	{
		CMD_debug(DEBUG_TYPE_ERROR, "In cmd2Vec, cmd_get_elem_by_name fail. (token=%s)", token);
		free(desc_cr);
		cmd_vector_deinit(allvec, 1);
		return NULL;
	}
	/* END:   Added by weizengke, 2013/10/4   PN:None */

	cmd_vector_insert(allvec, (void *)desc_cr);

/*
	{
		struct para_desc *desc_ = NULL;
		for (int i = 0; i < cmd_vector_max(allvec); i++)
		{
			desc_ = (struct para_desc *)cmd_vector_slot(allvec, i);
			printf("cmd2vec + %s\n",(char*)desc_->para);
		}
	}
*/
	return allvec;
}


/*  get range form INTEGER<a-b> STRING<a-b>

	type for INTEGER or STRING
*/
int cmd_get_range_symbol(char *string, int *type, int *a, int *b)
{
	char *pleft = NULL;
	char *pright = NULL;
	char *pline = NULL;
	char type_string[256] = {0};
	/*
		��ȷ��string��Ч��
	*/

	if (string == NULL || a == NULL || b == NULL)
	{
		return CMD_ERR;
	}

	pleft  = strchr(string, '<');
	pline  = strchr(string, '-');
	pright = strchr(string, '>');

	if (pleft == NULL || pline == NULL || pright == NULL)
	{
		return CMD_ERR;
	}

	*a = 0;
	*b = 0;
	*type = CMD_ELEM_TYPE_VALID;

	sscanf(string,"%[A-Z]<%d-%d>", type_string, a, b);

	if (strcmp(type_string, CMD_STRING) == 0)
	{
		*type = CMD_ELEM_TYPE_STRING;
	}

	if (strcmp(type_string, CMD_INTEGER) == 0)
	{
		*type = CMD_ELEM_TYPE_INTEGER;
	}

	//CMD_debug(DEBUG_TYPE_MSG, "%s<%d-%d>",type_string, *a, *b);

	return CMD_OK;

}

/* ����ַ����ǲ���ֻ�������� */
int cmd_string_isdigit(char *string)
{
	int i = 0;
	if (string == NULL)
	{
		return CMD_ERR;
	}

	for (i = 0; i < (int)strlen(string); i++)
	{
		if (!isdigit(*(string + i)))
		{
			CMD_debug(DEBUG_TYPE_ERROR, "cmd_string_isdigit return error.");
			return CMD_ERR;
		}
	}

	CMD_debug(DEBUG_TYPE_INFO, "cmd_string_isdigit return ok.");
	return CMD_OK;
}

/* match INTEGER or STRING */
int cmd_match_special_string(char *icmd, char *dest)
{
	/*
	num

	string

	ohter

	*/
	int dest_type = CMD_ELEM_TYPE_VALID;
	int a = 0;
	int b = 0;

	if (icmd == NULL || dest == NULL)
	{
		return CMD_ERR;
	}

	if (cmd_get_range_symbol(dest, &dest_type, &a, &b))
	{
		return CMD_ERR;
	}

	switch (dest_type)
	{
		case CMD_ELEM_TYPE_INTEGER:
			if (CMD_OK == cmd_string_isdigit(icmd))
			{
				int icmd_i = atoi(icmd);
				if (icmd_i >= a && icmd_i <=b)
				{
					return CMD_OK;
				}
			}
			break;
		case CMD_ELEM_TYPE_STRING:
			{
				int icmd_len = (int)strlen(icmd);
				if (icmd_len >= a && icmd_len <= b)
				{
					return CMD_OK;
				}
			}
			break;
		default:
			break;
	}


	return CMD_ERR;
}

int match_lcd(struct para_desc **match, int size)
{
	int i, j;
	int lcd = -1;
	char *s1, *s2;
	char c1, c2;

	if (size < 2)
	{
		return 0;
	}

	for (i = 1; i < size; i++)
	{
		s1 = match[i - 1]->para;
		s2 = match[i]->para;

		for (j = 0; (c1 = s1[j]) && (c2 = s2[j]); j++)
		{
			if (c1 != c2)
			{
				break;
			}
		}

		if (lcd < 0)
		{
			lcd = j;
		}
		else
		{
			if (lcd > j)
			{
				lcd = j;
			}
		}
	}

	return lcd;
}

static int cmd_filter_command(struct cmd_vty *vty, char *cmd, cmd_vector_t *v, int index)
{
	int i;
	int match_cmd = CMD_ERR;
	struct cmd_elem_st *elem;
	struct para_desc *desc;

	// For each command, check the 'index'th parameter if it matches the
	// 'index' paramter in command, set command vector which does not
	// match cmd NULL

	/* BEGIN: Added by weizengke, 2013/10/4   PN:check cmd valid*/
	if (cmd == NULL || 0 == strlen(cmd))
	{
		return CMD_ERR;
	}

	/* <CR> ��������ˣ���ֹ�������Ӵ�Ҳ����������ʱ����� */
	if (0 == strcmp(cmd, CMD_END))
	{
		return CMD_OK;
	}

	/* END:   Added by weizengke, 2013/10/4   PN:None */

	for (i = 0; i < cmd_vector_max(v); i++)
	{
		if ((elem = (struct cmd_elem_st*)cmd_vector_slot(v, i)) != NULL)
		{
			#if 1
			//printf("\r\n@@@@%u, %u, %s", elem->view_id, vty->view_id, elem->string);
			if (elem->view_id != VIEW_GLOBAL
				&& elem->view_id != vty->view_id)
			{
				//printf("\r\n no.");
				continue;
			}
			#endif
			
			if (index >= cmd_vector_max(elem->para_vec))
			{
				cmd_vector_slot(v, i) = NULL;
				continue;
			}

			desc = (struct para_desc *)cmd_vector_slot(elem->para_vec, index);

			/* match STRING , INTEGER */
			if (CMD_OK != cmd_match_special_string(cmd, desc->para))
			{
				if(strnicmp(cmd, desc->para, strlen(cmd)) != 0)
				{
					cmd_vector_slot(v, i) = NULL;
					continue;
				}
			}

			/* BEGIN: Added by weizengke, 2013/11/19 for support unkown cmd pos*/
			match_cmd = CMD_OK;
			/* END:   Added by weizengke, 2013/11/19 */
		}
	}

	//printf("\r\n not match.");
	
	CMD_debug(DEBUG_TYPE_INFO, "cmd_filter_command. (cmd=%s, match_res=%u)", vty->buffer, match_cmd);
	
	return match_cmd;
}


int cmd_match_command(cmd_vector_t *icmd_vec, struct cmd_vty *vty,
		struct para_desc **match, int *match_size, char *lcd_str)
{
	int i;
	cmd_vector_t *cmd_vec_copy = cmd_vector_copy(cmd_vec);
	int isize = 0;
	int size = 0;

	CMD_NOUSED(vty);
	CMD_NOUSED(lcd_str);

	// Three steps to find matched commands in 'cmd_vec'
	// 1. for input command vector 'icmd_vec', check if it is matching cmd_vec
	// 2. store last matching command parameter in cmd_vec into match_vec
	// 3. according to match_vec, do no match, one match, part match, list match

	if (icmd_vec == NULL)
	{
		return CMD_NO_MATCH;
	}

	isize = cmd_vector_max(icmd_vec) - 1;

	// Step 1
	for (i = 0; i < isize; i++)
	{
		char *ipara = (char*)cmd_vector_slot(icmd_vec, i);
		cmd_filter_command(vty, ipara, cmd_vec_copy, i);
	}

	// Step 2
	// insert matched command word into match_vec, only insert the next word
	for(i = 0; i < cmd_vector_max(cmd_vec_copy); i++)
	{
		struct cmd_elem_st *elem = NULL;
		elem = (struct cmd_elem_st *)cmd_vector_slot(cmd_vec_copy, i);

		if(elem != NULL)
		{
			if (elem->para_vec == NULL)
			{
 				continue;
 			}

			if (elem->view_id != VIEW_GLOBAL
				&& elem->view_id != vty->view_id)
			{
				continue;
			}

			if (isize >= cmd_vector_max(elem->para_vec))
			{
				cmd_vector_slot(cmd_vec_copy, i) = NULL;
				continue;
			}

			struct para_desc *desc = (struct para_desc *)cmd_vector_slot(elem->para_vec, isize);
			char *str = (char*)cmd_vector_slot(icmd_vec, isize);

			if (str == NULL || strnicmp(str, desc->para, strlen(str)) == 0)
			{
				if (match_unique_string(match, desc->para, size))
					match[size++] = desc;
			}

		}

	}

	cmd_vector_deinit(cmd_vec_copy, 0);	// free cmd_vec_copy, no longer use

	// Step 3
	// No command matched
	if (size == 0)
	{
		*match_size = size;
		return CMD_NO_MATCH;
	}

	// Only one command matched
	if (size == 1)
	{
		*match_size = size;
		return CMD_FULL_MATCH;
	}

#if 0
	/* dele the part match */

	// Make it sure last element is NULL
	if (cmd_vector_slot(icmd_vec, isize) != NULL) {
		int lcd = match_lcd(match, size);
		if(lcd) {
			int len = strlen((char*)cmd_vector_slot(icmd_vec, isize));
			if (len < lcd) {
				memcpy(lcd_str, match[0]->para, lcd);
				lcd_str[lcd] = '\0';
				*match_size = size = 1;
				return CMD_PART_MATCH;
			}
		}
	}
#endif
	*match_size = size;
	return CMD_LIST_MATCH;
}

/*****************************************************************************
 Prototype    : cmd_complete_command
 Description  : complete command��for ? complete
 Input        : cmd_vector_t *icmd_vec  input cmd vector
                struct cmd_vty *vty     the input vty

 Output       : char **doc              the return doc
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2013/10/4
    Author       : weizengke
    Modification : Created function

*****************************************************************************/
int cmd_complete_command(cmd_vector_t *icmd_vec, struct cmd_vty *vty,
									 struct para_desc **match, int *match_size, int *match_pos)
{


	int i;
	cmd_vector_t *cmd_vec_copy = cmd_vector_copy(cmd_vec);
	int match_num = 0;

	char *str;
	struct para_desc *para_desc_;
	struct cmd_elem_st *elem;

	if (icmd_vec == NULL || vty == NULL || match == NULL || match_size == NULL)
	{
		return 1;
	}

	// Two steps to find matched commands in 'cmd_vec'
	// 1. for input command vector 'icmd_vec', check if it is matching cmd_vec
	// 2. check last matching command parameter in cmd_vec match cmd_vec

	*match_pos = -1;

	// Step 1
	/* BEGIN: Modified by weizengke, 2013/10/4   PN:ѭ������ÿһ������ */
	for (i = 0; i < cmd_vector_max(icmd_vec); i++)
	{
		if (CMD_OK != cmd_filter_command(vty, (char*)cmd_vector_slot(icmd_vec, i), cmd_vec_copy, i))
		{
			/* BEGIN: Added by weizengke, 2013/11/19 ��������Ż��������п��Բ���Ҫ��ƥ���� */
			/* �����ڵڼ����������޷�ƥ�� */
			*match_pos = (*match_pos == -1)?(i):(*match_pos);
		}
	}
	/* END:   Modified by weizengke, 2013/10/4   PN:None */

	CMD_debug(DEBUG_TYPE_FUNC, "cmd_complete_command, match_pos=%d", *match_pos);
	
	// Step 2
	// insert matched command word into match_vec, only insert the next word
	/* BEGIN: Added by weizengke, 2013/10/4   PN:only the last vector we need */
	for(i = 0; i < cmd_vector_max(cmd_vec_copy); i++)
	{
		elem = (struct cmd_elem_st *)cmd_vector_slot(cmd_vec_copy, i);

		if(elem  != NULL)
		{
			if (cmd_vector_max(icmd_vec) - 1 >= cmd_vector_max(elem->para_vec))
			{
				cmd_vector_slot(cmd_vec_copy, i) = NULL;
				continue;
			}

			CMD_debug(DEBUG_TYPE_FUNC, "cmd_complete_command, view_id=%d, string=%s.", elem->view_id, elem->string);
			
			if (elem->view_id != VIEW_GLOBAL
				&& elem->view_id != vty->view_id)
			{
				continue;
			}

			str = (char*)cmd_vector_slot(icmd_vec, cmd_vector_max(icmd_vec) - 1);
			para_desc_ = (struct para_desc*)cmd_vector_slot(elem->para_vec, cmd_vector_max(icmd_vec) - 1);
			if (para_desc_ == NULL)
			{
				continue;
			}

			/* BEGIN: Added by weizengke, 2013/11/19 */
			/* match STRING , INTEGER */
			if (CMD_OK == cmd_match_special_string(str, para_desc_->para))
			{
				match[match_num++] = para_desc_;

				continue;
			}
			/* END:   Added by weizengke, 2013/11/19 */

			/* match key */
			if (strnicmp(str, CMD_END, strlen(str)) == 0
			    ||(strnicmp(str, para_desc_->para, strlen(str)) == 0))
			{

				if (match_unique_string(match, para_desc_->para, match_num))
				{
					match[match_num++] = para_desc_;
				}
			}

		}
	}

	cmd_vector_deinit(cmd_vec_copy, 0);	// free cmd_vec_copy, no longer use

	CMD_debug(DEBUG_TYPE_FUNC, "cmd_complete_command, match_num=%d", match_num);
	
	/* BEGIN: Added by weizengke, 2013/10/27 sort for ? complete */
	{
		int j;
		for (i = 0; i < match_num - 1; i++)
		{
			for (j = i; j < match_num; j++)
			{
				struct para_desc *para_desc__ = NULL;
				if (0 == strnicmp(match[i]->para, CMD_END, strlen(match[i]->para))
					|| ( 1 == stricmp(match[i]->para, match[j]->para)
					&& 0 != strnicmp(match[j]->para, CMD_END, strlen(match[j]->para)))
					)
				{
					para_desc__ = match[i];
					match[i] =  match[j];
					match[j] = para_desc__;
				}
			}
		}
	}
	/* END: Added by weizengke, 2013/10/27 sort for ? complete */

	*match_size = match_num;

	return 0;
}

int cmd_execute_command(cmd_vector_t *icmd_vec, struct cmd_vty *vty, struct para_desc **match, int *match_size, int *nomatch_pos)
{
	int i;
	cmd_vector_t *cmd_vec_copy = cmd_vector_copy(cmd_vec);

	struct cmd_elem_st *match_elem = NULL;
	int match_num = 0;
	
	/*
	Two steps to find matched commands in 'cmd_vec'
	 1. for input command vector 'icmd_vec', check if it is matching cmd_vec
	 2. check last matching command parameter in cmd_vec match cmd_vec
	*/
	
	*nomatch_pos = -1;

	CMD_debug(DEBUG_TYPE_INFO, "cmd_execute_command. (cmd=%s, view_id=%u)", vty->buffer, vty->view_id);
	
	/* Step 1 */
	for (i = 0; i < cmd_vector_max(icmd_vec); i++)
	{
		if (CMD_OK != cmd_filter_command(vty, (char*)cmd_vector_slot(icmd_vec, i), cmd_vec_copy, i))
		{
			/* BEGIN: Added by weizengke, 2013/11/19 �����п��Բ���Ҫ��ƥ���� */
			/* �����ڵڼ����������޷�ƥ�� */
			*nomatch_pos = (*nomatch_pos == -1)?(i):(*nomatch_pos);
		}
	}

	/* Step 2 */
	/*  insert matched command word into match_vec, only insert the next word */
	if (*nomatch_pos == -1)
	{
		/* BEGIN: Added by weizengke, 2014/3/9 �޸������в���ȫʱ������λ����ʾ��׼ȷ������ */
		*nomatch_pos = cmd_vector_max(icmd_vec) - 1;

		for(i = 0; i < cmd_vector_max(cmd_vec_copy); i++)
		{
			char *str;
			struct para_desc *desc;
			struct cmd_elem_st *elem = NULL;

			elem = (struct cmd_elem_st *)cmd_vector_slot(cmd_vec_copy, i);
			if(elem != NULL)
			{
				if (elem->view_id != VIEW_GLOBAL
				&& elem->view_id != vty->view_id)
				{
					continue;
				}
							
				str = (char*)cmd_vector_slot(icmd_vec, cmd_vector_max(icmd_vec) - 1);
				desc = (struct para_desc *)cmd_vector_slot(elem->para_vec, cmd_vector_max(icmd_vec) - 1);

				/* modified for command without argv */
				if (cmd_vector_max(icmd_vec) == cmd_vector_max(elem->para_vec))
				{
					/* BEGIN: Added by weizengke, 2013/10/5   PN:for support STRING<a-b> & INTEGER<a-b> */
					if (CMD_OK == cmd_match_special_string(str, desc->para) ||
						str != NULL && strnicmp(str, desc->para, strlen(str)) == 0)
					{
						/* BEGIN: Added by weizengke, 2013/10/6   PN:command exec ambigous, return the last elem (not the <CR>) */
						match[match_num] = (struct para_desc *)cmd_vector_slot(elem->para_vec, cmd_vector_max(icmd_vec) - 2);
						/* END:   Added by weizengke, 2013/10/6   PN:None */

						match_num++;
						match_elem = elem;

					}
				}
			}
		}

	}

	*match_size = match_num;

	cmd_vector_deinit(cmd_vec_copy, 0);

	CMD_debug(DEBUG_TYPE_INFO, "cmd_execute_command. (cmd=%s, match_num=%u)", vty->buffer, match_num);
	
	/* check if exactly match */
	if (match_num == 0)
	{
		return CMD_NO_MATCH;
	}

	/* BEGIN: Added by weizengke, 2013/10/6   PN:command exec ambigous */
	if (match_num > 1)
	{
		return CMD_ERR_AMBIGOUS;
	}
	/* END:   Added by weizengke, 2013/10/6   PN:None */

	/* BEGIN: Added by weizengke, 2013/10/5  for support argv. PN:push param into function stack */
	/* now icmd_vec and match_elem will be the same vector ,can push into function stack */
	int argc = 0;
	char *argv[CMD_MAX_CMD_NUM];

	if (NULL == match_elem || match_elem->para_vec == NULL)
	{
		return CMD_NO_MATCH;
	}

	for (i = 0, argc = 0; i < match_elem->para_num; i ++)
	{
		struct para_desc  *desc = (struct para_desc *)cmd_vector_slot(match_elem->para_vec, i);
		if (NULL == desc)
		{
			return CMD_NO_MATCH;
		}

		/* <CR> no need to push */
		if (desc->elem_tpye == CMD_ELEM_TYPE_END)
		{
			continue;
		}

		/* push param to argv and full command*/
		if (CMD_YES == cmd_elem_is_para_type(desc->elem_tpye))
		{
			argv[argc++] = (char*)cmd_vector_slot(icmd_vec, i);
		}
		else
		{
			argv[argc++] = desc->para;
		}
	}
	/* END:   Added by weizengke, 2013/10/5   PN:None */

	/* execute command */
	(*match_elem->func)(match_elem, vty, argc, argv);

	return CMD_FULL_MATCH;
}

void install_command(VIEW_ID_EN view, struct cmd_elem_st *elem)
{
	if (cmd_vec == NULL)
	{
		CMD_debug(DEBUG_TYPE_ERROR, "Command Vector Not Exist");
		return;
	}

	CMD_debug(DEBUG_TYPE_FUNC, "install_command, view(%d), string(%s), doc(%s).", view, elem->string, elem->doc);

	elem->view_id = view;
    cmd_vector_insert(cmd_vec, elem);
	elem->para_vec = cmd2vec(elem->string, elem->doc);
    if (NULL == elem->para_vec)
    {
        return;
    }
	elem->para_num = cmd_vector_max(elem->para_vec);

	CMD_debug(DEBUG_TYPE_FUNC, "install_command ok, string(%s), generate para_num(%d). ", elem->string, elem->para_num);

}
/* end cmd */

/* resolve */

/* Note: Added by weizengke, 2013/10/04 insert a word into the tail of input buffer */
void cmd_insert_word(struct cmd_vty *vty, const char *str)
{
	strcat(vty->buffer, str);
	vty->cur_pos += (int)strlen(str);
	vty->used_len += (int)strlen(str);
}

/* Note: Added by weizengke, 2013/10/04 delete the last word from input buffer */
void cmd_delete_word(struct cmd_vty *vty)
{
	int pos = strlen(vty->buffer);

	while (pos > 0 && vty->buffer[pos - 1] != ' ')
	{
		pos--;
	}

	/* BEGIN: Added by weizengke, 2014/8/3 */
	//vty->buffer[pos] = '\0';
	memset(&vty->buffer[pos], 0, sizeof(vty->buffer) - pos);
	/* END:   Added by weizengke, 2014/8/3 */

	vty->cur_pos = strlen(vty->buffer);
	vty->used_len = strlen(vty->buffer);
}

/* Note: Added by weizengke, 2013/10/04 delete the last word from input buffer*/
void cmd_delete_word_ctrl_W(struct cmd_vty *vty)
{
	/* ɾ�����һ��elem */

	int pos = strlen(vty->buffer);

	if (pos == 0)
	{
		return;
	}

	/* ignore suffix-space */
	while (vty->buffer[pos - 1] == ' ')
	{
		pos--;
	}

	/* del the last word */
	while (pos > 0 && vty->buffer[pos - 1] != ' ')
	{
		pos--;
	}

	/* BEGIN: Modified by weizengke, 2014/3/23, for https://github.com/weizengke/jungle/issues/1 */
	//vty->buffer[pos] = '\0';
	memset(&vty->buffer[pos], 0, sizeof(vty->buffer) - pos);
	/* END:   Modified by weizengke, 2014/3/23 */

	vty->cur_pos = strlen(vty->buffer);
	vty->used_len = strlen(vty->buffer);
}

/* Note: Added by weizengke, 2014/3/23 delete the last word at current pos from input buffer*/
void cmd_delete_word_ctrl_W_ex(struct cmd_vty *vty)
{
	/* ɾ��������ڵ�ǰ��֮ǰelem */
	int start_pos = 0;
	int end_pos  = 0;
	int len = strlen(vty->buffer);
	int pos = vty->cur_pos;


	CMD_debug(DEBUG_TYPE_INFO, "ctrl_W:cur_poscur_pos = %d buffer_len = %d", pos, len);

	if (pos == 0)
	{
		return;
	}

	/* ignore suffix-space */
	if (vty->buffer[pos] == ' ' || vty->buffer[pos] == '\0')
	{
		end_pos = pos;

		pos--;
		/* �����ҵ�һ���ǿ��ַ� */
		while (pos  >= 0 && vty->buffer[pos] == ' ')
		{
			pos--;
		}

		if (pos == 0)
		{
			start_pos = 0;
		}
		else
		{
			/* ���������ҵ�һ���ո������ͷ */
			while (pos	>= 0 && vty->buffer[pos] != ' ')
			{
				pos--;
			}
			
			start_pos = pos + 1;

		}

	}
	else
	{
		/* �ֱ��������ҿո� */
		while (vty->buffer[pos + 1] != ' ')
		{
			/* BEGIN: Added by weizengke, 2014/4/5 �����λ�����������һ��Ԫ���м䣬��ִ��CTRL+W�������쳣��ʾ https://github.com/weizengke/jungle/issues/2 */
			if (vty->buffer[pos + 1] == '\0') break;
			/* END:   Added by weizengke, 2014/4/5 */

			pos++;
		}

		/* BEGIN: Modified by weizengke, 2014/4/5 https://github.com/weizengke/jungle/issues/2 */
		end_pos = pos;
		/* END:   Modified by weizengke, 2014/4/5 */

		pos = vty->cur_pos;
		while (vty->buffer[pos] != ' ')
		{
			pos--;
		}

		start_pos = pos + 1;
	}

	int len_last = strlen(&vty->buffer[end_pos]);

	memcpy(&vty->buffer[start_pos], &vty->buffer[end_pos], strlen(&vty->buffer[end_pos]));
	memset(&vty->buffer[start_pos + len_last], 0, sizeof(vty->buffer)-(start_pos + len_last));

	
	vty->cur_pos -= (vty->cur_pos - start_pos);
	vty->used_len -= (end_pos - start_pos);

	CMD_debug(DEBUG_TYPE_INFO, "ctrl+w end: buffer=%s, start_pos=%d end_pos=%d len_last=%d cur_pos=%d used_len=%d",
		vty->buffer,start_pos,end_pos,len_last,vty->cur_pos, vty->used_len);

}

static inline void free_matched(char **matched)
{
	int i;

	if (!matched)
	{
		return;
	}

	for (i = 0; matched[i] != NULL; i++)
	{
		free(matched[i]);
	}

	free(matched);
}


int cmd_run(struct cmd_vty *vty)
{
	cmd_vector_t *v = NULL;
	struct para_desc *match[CMD_MAX_MATCH_SIZE] = {0};	// matched string
	int match_size = 0;
	int match_type = CMD_NO_MATCH;
	int nomath_pos = -1;

	//printf("enter(%d %d %s)\r\n", vty->used_len, vty->buf_len, vty->buffer);

	v = str2vec(vty->buffer);
	if (v == NULL) {
		return 1;
	}

	/* BEGIN: Added by weizengke, 2013/10/5   PN:for cmd end with <CR> */
	cmd_vector_insert_cr(v);
	/* END:   Added by weizengke, 2013/10/5   PN:None */

	// do command
	match_type = cmd_execute_command(v, vty, match, &match_size, &nomath_pos);
	// add executed command into history

	CMD_debug(DEBUG_TYPE_INFO, "cmd_run. (cmd=%s, match_type=%u)", vty->buffer, match_type);
	
	if (CMD_FULL_MATCH != match_type)
	{
		return 1;
	}

	return 0;
}


void cmd_read(struct cmd_vty *vty)
{
	int key_type;
	extern ULONG g_TelnetServerEnable;

	if (NULL == vty)
	{
		return ;
	}

	while((vty->c = vty_getch(vty)) != EOF)
	{	
		/* telnet�û�����Ƿ�telnetʹ�� */
		if (1 == vty->user.type
			&& OS_NO == g_TelnetServerEnable)
		{
			break;
		}
		
		/* �������ʱ�� */
		vty->user.lastAccessTime = time(NULL);
				
		/* step 1: get input key type */
		if (0 == vty->user.type)
		{
			key_type = cmd_resolve(vty);
		}
		else
		{
			key_type = cmd_resolve_vty(vty);
		}
		
		CMD_debug(DEBUG_TYPE_INFO, "cmd_read. (key_type=%d)", key_type);
		
		vty->inputMachine_now = key_type;
		
		if (key_type <= CMD_KEY_CODE_NONE || key_type > CMD_KEY_CODE_NOTCARE)
		{
			CMD_debug(DEBUG_TYPE_ERROR, "Unidentify Key Type, c = %c, key_type = %d\n", vty->c, key_type);
			continue;
		}
		
		/* step 2: take actions according to input key */
		key_resolver[key_type].key_func(vty);
		vty->inputMachine_prev = vty->inputMachine_now;
		
		if (vty->inputMachine_now != CMD_KEY_CODE_TAB)
		{
			memset(vty->tabString,0,sizeof(vty->tabString));
			memset(vty->tabbingString,0,sizeof(vty->tabbingString));
			vty->tabStringLenth = 0;
		}

	}

	return ;

}

/* Note: Added by weizengke, 2013/10 clear current line by cur-pos */
void cmd_clear_line(struct cmd_vty *vty)
{
	int size = vty->used_len - vty->cur_pos;

	CMD_DBGASSERT(size >= 0, "cmd_clear_line");
	while (size--)
	{
		cmd_put_one(vty, ' ');
	}

	while (vty->used_len)
	{
		vty->used_len--;
		cmd_back_one(vty);
		cmd_put_one(vty, ' ');
		cmd_back_one(vty);
	}

	memset(vty->buffer, 0, HISTORY_MAX_SIZE);
}

/* Note: Added by weizengke, 2013/10 output current command-buf  */
void cmd_outcurrent()
{
	int i;

	if (NULL == g_con_vty)
	{
		return ;
	}

	vty_printf(g_con_vty,"%s", CMD_ENTER);
	cmd_outprompt(g_con_vty);
	vty_printf(g_con_vty,"%s", g_con_vty->buffer);

	/* BEGIN: Added by weizengke, 2014/3/23 support delete word form cur_pos */
	for (i = 0; i < strlen(g_con_vty->buffer) - g_con_vty->cur_pos; i++)
	{
		cmd_back_one(g_con_vty);
	}
	/* END:   Added by weizengke, 2014/3/23 */

}

struct cmd_vty* cmd_get_idlevty()
{
	int i = 0;

	for (i == 0; i < CMD_VTY_MAXUSER_NUM; i++)
	{
		if (g_vty[i].valid == 0)
		{
			g_vty[i].valid = 1;

			return &g_vty[i];
		}
	}

	return NULL;
}

void cmd_vty_offline(struct cmd_vty *vty)
{
	int i = 0;

	CMD_debug(DEBUG_TYPE_FUNC, "cmd_vty_offline. (user_name=%s, socket=%d)", vty->user.user_name, vty->user.socket);
	
	closesocket(vty->user.socket);
	
	for (i = 0; i < HISTORY_MAX_SIZE; i++)
	{
		if (vty->history[i] != NULL)
		{
			free(vty->history[i]);
			vty->history[i] = NULL;
		}
	}

	cmd_vty_init(vty);
}

void cmd_outprompt(struct cmd_vty *vty)
{
	//cmd_outstring("<%s-%s>", g_sysname, prompt);

	CMD_debug(DEBUG_TYPE_INFO, "cmd_outprompt. (view_id=%d, g_sysname=%s)", vty->view_id, g_sysname);
	
	if (vty->view_id == VIEW_USER)
	{
		vty_printf(vty, "%s>", g_sysname);
		return;
	}

	if (vty->view_id == VIEW_SYSTEM)
	{
		vty_printf(vty, "%s]", g_sysname);
		return;
	}

	view_node_st * view = NULL;
	view = cmd_view_lookup(view_list, vty->view_id);
	if (view == NULL)
	{	
		vty_printf(vty, "%s>", g_sysname);
		return ;
	}

	vty_printf(vty, "%s-%s]", g_sysname, view->view_ais_name);
	
}

int cmd_view_getaislenth(struct cmd_vty *vty)
{
	view_node_st * view = NULL;

	view = cmd_view_lookup(view_list, vty->view_id);
	if (view == NULL)
	{	
		return 0;
	}

	return strlen(view->view_ais_name);
}

char* cmd_view_getAis(VIEW_ID_EN view_id)
{
	view_node_st * view = NULL;

	view = cmd_view_lookup(view_list, view_id);
	if (view == NULL)
	{	
		return NULL;
	}

	return view->view_ais_name;
}

char* cmd_view_getViewName(VIEW_ID_EN view_id)
{
	view_node_st * view = NULL;

	view = cmd_view_lookup(view_list, view_id);
	if (view == NULL)
	{	
		return NULL;
	}

	return view->view_name;
}

VIEW_ID_EN vty_view_getParentViewId(VIEW_ID_EN view_id)
{	
	view_node_st * view = NULL;
	view_node_st * prev_view = NULL;
	
	view = cmd_view_lookup(view_list, view_id);
	if (view == NULL)
	{	
		return VIEW_NULL;
	}

	prev_view = view->pParent;
	if (prev_view == NULL)
	{
		return VIEW_NULL;
	}

	CMD_debug(DEBUG_TYPE_FUNC, "vty_view_getParentViewId. (prev_view=%s, view_id=%u)", prev_view->view_name, prev_view->view_id);
	
	return prev_view->view_id;
}

view_node_st * cmd_view_lookup(view_node_st *view, VIEW_ID_EN view_id)
{	
	view_node_st * view_son = NULL;
	view_node_st * view_ = NULL;

	if (NULL == view)
	{
		CMD_debug(DEBUG_TYPE_FUNC, "cmd_view_lookup NULL. (view_id=%d)", view_id);
		return view;
	}
	
	CMD_debug(DEBUG_TYPE_FUNC, "cmd_view_lookup. (view(%d, view_name=%s, sons=%d), view_id=%d)", view->view_id, view->view_name, view->view_son_num, view_id);

	if (view->view_id == view_id)
	{
		CMD_debug(DEBUG_TYPE_FUNC, "cmd_view_lookup found. (view_id=%d, view_name=%s, sons=%d)", view->view_id, view->view_name, view->view_son_num);
		return view;
	}

	for(int i = 0; i < view->view_son_num; i++)
	{
		view_son  = view->ppSons[i];
		
		CMD_debug(DEBUG_TYPE_FUNC, "cmd_view_lookup sons. (view_son->view_id=%d, view_son->view_name=%s)", view_son->view_id, view_son->view_name);
		view_ = cmd_view_lookup(view_son, view_id);
		if (NULL != view_)
		{
			return view_;
		}
	}

	CMD_debug(DEBUG_TYPE_FUNC, "cmd_view_lookup not found. (view_id=%d)", view_id);
	
	return NULL;
}

void vty_view_set(struct cmd_vty *vty, VIEW_ID_EN view_id)
{
	view_node_st * view = NULL;

	view = cmd_view_lookup(view_list, view_id);
	if (view == NULL)
	{	
		return;
	}

	CMD_debug(DEBUG_TYPE_FUNC, "vty_view_set. (view_id=%d)", view_id);

	if (view_id < VIEW_USER)
	{
		/* telnet �û����� */
		if (0 != vty->user.type)
		{
			cmd_vty_offline(vty);
		}
		
		return;
	}

	vty->view_id = view_id;
}


void vty_view_quit(struct cmd_vty *vty)
{	
	view_node_st * view = NULL;
	view_node_st * prev_view = NULL;
	
	view = cmd_view_lookup(view_list, vty->view_id);
	if (view == NULL)
	{	
		/* �û����� */
		cmd_vty_offline(vty);
		return;
	}

	prev_view = view->pParent;
	if (prev_view == NULL)
	{
		cmd_vty_offline(vty);
		return;
	}

	vty_view_set(vty, prev_view->view_id);
	
}

view_node_st * cmd_view_malloc(char *view_name, char *view_ais, VIEW_ID_EN view_id, view_node_st *pParent)
{
	view_node_st * view = NULL;
	view_node_st * pSons = NULL;
	
	view = (view_node_st *)malloc(sizeof(view_node_st));
	if (NULL == view)
	{
		CMD_DBGASSERT(0, "cmd_view_malloc");
		return NULL;
	}
	memset(view, 0, sizeof(view_node_st));
	view->view_id = view_id;
	strcpy(view->view_name, view_name);
	strcpy(view->view_ais_name, view_ais);

	view->ppSons = (view_node_st **)malloc(CMD_VIEW_SONS_NUM * sizeof(view_node_st));
	if (NULL == view->ppSons)
	{
		CMD_DBGASSERT(0, "cmd_view_malloc sons");
		free(view);
		return NULL;
	}
	memset(view->ppSons, 0, CMD_VIEW_SONS_NUM * sizeof(view_node_st));

	view->view_son_num = 0;
	view->pParent = pParent;
	
	if (NULL != pParent)
	{	
		if (pParent->view_son_num >= CMD_VIEW_SONS_NUM)
		{
			CMD_DBGASSERT(0, "cmd_view_malloc sons num more than 100");
			free(view);
			free(view->ppSons);
			return NULL;
		}
		
		pParent->ppSons[pParent->view_son_num++] = view;
	}
	
	return view;
}

/*
��ͼ����
user(>)
  -system(])
     - aaa(aaa)
     - user-vty(user-vty)
     - diagnose(diagnose)
*/


void cmd_view_regist()
{
	view_node_st * view_user = NULL;
	view_node_st * view_system = NULL;
	view_node_st * view_user_vty = NULL;
	view_node_st * view_diagnose = NULL;
	view_node_st * view_aaa = NULL;
	
	view_list = cmd_view_malloc("global", "", VIEW_GLOBAL, NULL);
	view_user = cmd_view_malloc("user-view", "", VIEW_USER, view_list);
	view_system = cmd_view_malloc("system-view", "", VIEW_SYSTEM, view_user);
	view_user_vty = cmd_view_malloc("user-vty-view", "user-vty", VIEW_USER_VTY, view_system);
	view_aaa = cmd_view_malloc("aaa-view", "aaa", VIEW_AAA, view_system);
	view_diagnose = cmd_view_malloc("diagnose-view", "diagnose", VIEW_DIAGNOSE, view_system);
	
	
}

int cmd_init()
{
	/* initial cmd vector */
	cmd_vec = cmd_vector_init(1);

	/* ��ͼע�� */
	cmd_view_regist();
	
	/* install cmd */
	cmd_install();

	/* initial com_vty */
	g_con_vty = (struct cmd_vty *)calloc(1, sizeof(struct cmd_vty));
	if(g_con_vty == NULL)
	{
		return OS_ERR;
	}
	cmd_vty_init(g_con_vty);
	g_con_vty->valid = 1;
	g_con_vty->user.type = 0;
	g_con_vty->user.state = 1;
	
	for (int i = 0; i < CMD_VTY_MAXUSER_NUM; i++)
	{
		cmd_vty_init(&g_vty[i]);
		g_vty[i].user.type = 1;
	}
	
	return OS_OK;
}


unsigned _stdcall  cmd_main_entry(void *pEntry)
{

	for (;;)
	{
		cmd_read(g_con_vty);
	}

	cmd_vty_deinit(g_con_vty);

	return 0;
}


APP_INFO_S g_CMDAppInfo =
{
	NULL,
	"Command",
	cmd_init,
	cmd_main_entry
};

void Cmd_RegAppInfo()
{
	RegistAppInfo(&g_CMDAppInfo);
}

