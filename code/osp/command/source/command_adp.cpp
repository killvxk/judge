
#include "osp\command\include\command_inc.h"
#include "product\judge\include\judge_inc.h"

int cmd_pub_run(struct cmd_vty *vty, char *szCmdBuf)
{
	int iRet = 0;

	if (NULL == szCmdBuf)
	{
		return 1;
	}
	
	memcpy(vty->buffer, szCmdBuf, CMD_BUFFER_SIZE);
	vty->buf_len = CMD_BUFFER_SIZE;
	
	iRet = cmd_run(vty);
	
	vty->cur_pos = vty->used_len = 0;
	memset(vty->buffer, 0, vty->buf_len);

	//printf("\r\nExecute command:%s. (result:%d)", vty->buffer, iRet);
	
	return iRet;
}

/* ------------------ Interface Function ----------------- */
int cmd_resolve_vty(struct cmd_vty *vty)
{
	char c = vty->c;
	int key_type = CMD_KEY_CODE_NOTCARE;	// default is not special key
	
	switch (c) 
	{
		case 0x1b:
			c = vty_getch(vty);
			//printf("\r\n~c=0x%x.", vty->c);
			if (0x5b == c)
			{
				c = vty_getch(vty);
				//printf("\r\n#c=0x%x.", vty->c);
				vty->c = c;
			}
			
			switch (c)
			{
				case 0x41:
					key_type = CMD_KEY_CODE_UP;
					break;
				case 0x42:
					key_type = CMD_KEY_CODE_DOWN;
					break;
				case 0x43:
					key_type = CMD_KEY_CODE_RIGHT;
					break;
				case 0x44:
					key_type = CMD_KEY_CODE_LEFT;
					break;
				default:
					key_type = CMD_KEY_CODE_FILTER;
					break;
			}

			break;
		case CMD_KEY_DELETE_VTY:
			key_type = CMD_KEY_CODE_DELETE;
			break;
		case CMD_KEY_BACKSPACE:  /*  */
			key_type = CMD_KEY_CODE_BACKSPACE;
			break;
		case CMD_KEY_SPACE:
			/* Linux �¿ո��س��޷�tab��ȫ��'?'���� ���޸�*/
			break;
		case CMD_KEY_CTRL_W:
			/* del the last elem */
			key_type = CMD_KEY_CODE_DEL_LASTWORD;
			break;

		case CMD_KEY_TAB:
			key_type = CMD_KEY_CODE_TAB;
			break;
		//case CMD_KEY_LF: 
		case CMD_KEY_CR: 
			key_type = CMD_KEY_CODE_ENTER;
			//printf("\r\n~c=0x%x.", c);
			break;
		case CMD_KEY_QUEST:
			key_type = CMD_KEY_CODE_QUEST;
			break;
		default:
			/* BEGIN: Added by weizengke, 2014/4/6 filter CTRL+a ~ z */
			if (c >= 0x1 && c <= 0x1d)
			{
				key_type = CMD_KEY_CODE_FILTER;
			}
			else
			{
				//printf("\r\n@c=0x%x.", vty->c);
			}
			/* END:   Added by weizengke, 2014/4/6 */
			break;
	}

	return key_type;
}
/* end key*/

/* ------------------ Interface Function ----------------- */
int cmd_resolve(struct cmd_vty *vty)
{
	char c = vty->c;
	int key_type = CMD_KEY_CODE_NOTCARE;	// default is not special key

	switch (c) {
	case CMD_KEY_ARROW1:
		c = cmd_getch();
		#ifdef _LINUX_
		if (c == CMD_KEY_ARROW2)
		{
			c = cmd_getch();
		#endif
			switch (c)
			{
				case CMD_KEY_UP:
					key_type = CMD_KEY_CODE_UP;
					break;
				case CMD_KEY_DOWN:
					key_type = CMD_KEY_CODE_DOWN;
					break;
				case CMD_KEY_RIGHT:
					key_type = CMD_KEY_CODE_RIGHT;
					break;
				case CMD_KEY_LEFT:
					key_type = CMD_KEY_CODE_LEFT;
					break;
				case CMD_KEY_DELETE:
					key_type = CMD_KEY_CODE_DELETE;
					break;
				/* BEGIN: Added by weizengke, 2014/4/6 support page up & down*/
				case CMD_KEY_PGUP:
					{
						::SendMessage(g_hWnd,WM_VSCROLL,MAKEWPARAM(SB_PAGEUP, 0),NULL);
						/*
						int nVertSP = GetScrollPos(g_hWnd, SB_VERT);
						SetScrollPos(g_hWnd, SB_VERT, nVertSP, 1);
						*/
						key_type = CMD_KEY_CODE_FILTER;
					}
					break;
				case CMD_KEY_PHDN:
					{
						::SendMessage(g_hWnd,WM_VSCROLL,MAKEWPARAM(SB_PAGEDOWN, 0),NULL);
						key_type = CMD_KEY_CODE_FILTER;
					}
					break;
				/* END:   Added by weizengke, 2014/4/6 */
				default:
					key_type = CMD_KEY_CODE_FILTER;
					break;
			}
		#ifdef _LINUX_
		}
		#endif
		break;
#ifndef _LINUX_  /* windwos */
		case CMD_KEY_ARROW2:
			c = cmd_getch();
			switch (c)
			{
				case CMD_KEY_UP:
					key_type = CMD_KEY_CODE_UP;
					break;
				case CMD_KEY_DOWN:
					key_type = CMD_KEY_CODE_DOWN;
					break;
				case CMD_KEY_RIGHT:
					key_type = CMD_KEY_CODE_RIGHT;
					break;
				case CMD_KEY_LEFT:
					key_type = CMD_KEY_CODE_LEFT;
					break;
				default:
					key_type = CMD_KEY_CODE_FILTER;
					break;
			}
		break;
#endif
	case CMD_KEY_BACKSPACE:  /*  */
		key_type = CMD_KEY_CODE_BACKSPACE;
		break;
	case CMD_KEY_SPACE:
	case CMD_KEY_CTRL_H:
		/* Linux �¿ո��س��޷�tab��ȫ��'?'���� ���޸�*/
		break;
	case CMD_KEY_CTRL_W:
		/* del the last elem */
		key_type = CMD_KEY_CODE_DEL_LASTWORD;
		break;
	case CMD_KEY_TAB:
		key_type = CMD_KEY_CODE_TAB;
		break;
	case CMD_KEY_LF:
	case CMD_KEY_CR: 
		key_type = CMD_KEY_CODE_ENTER;
		break;
	case CMD_KEY_QUEST:
		key_type = CMD_KEY_CODE_QUEST;
		break;
	default:
		/* BEGIN: Added by weizengke, 2014/4/6 filter CTRL+a ~ z */
		if (c >= 0x1 && c <= 0x1d)
		{
			key_type = CMD_KEY_CODE_FILTER;
		}
		/* END:   Added by weizengke, 2014/4/6 */
		break;
	}

	return key_type;
}
/* end key*/

void cmd_resolve_filter(struct cmd_vty *vty)
{
	return;
}


/*****************************************************************************
 �� �� ��  : cmd_resolve_tab
 ��������  : ����TAB�������ȫ
 �������  : struct cmd_vty *vty
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��17��
    ��    ��   : weizengke
    �޸�����   : �����ɺ���

*****************************************************************************/
void cmd_resolve_tab(struct cmd_vty *vty)
{
	int i;
	cmd_vector_t *v;
	struct para_desc *match[CMD_MAX_MATCH_SIZE] = {0};	// matched string
	int match_size = 0;
	int match_type = CMD_NO_MATCH;
	int isNeedMatch = 1;   /* ��TAB����(�޿ո�)������Ҫƥ�� */
	char lcd_str[1024] = {0};	// if part match, then use this
	char *last_word = NULL;

	/*
	1: ȡpos ֮ǰ��buf
	2: ��Ҫ���ǵ�ǰ�����buf
	*/
	/* BEGIN: Added by weizengke, 2013/11/17 bug for left and tab*/
	memset(&(vty->buffer[vty->cur_pos]), 0 ,strlen(vty->buffer) - vty->cur_pos);
	vty->used_len = strlen(vty->buffer);
	/* END:   Added by weizengke, 2013/11/17 */

	CMD_debug(DEBUG_TYPE_INFO,"tab in:used_len=%d, pos=%d\r\n",vty->used_len, vty->cur_pos);

	if (vty->inputMachine_prev == CMD_KEY_CODE_TAB)
	{
		cmd_delete_word(vty);
		cmd_insert_word(vty, vty->tabbingString);
	}
	else
	{
		memset(vty->tabString,0,sizeof(vty->tabString));
		vty->tabStringLenth = 0;
	}

	v = str2vec(vty->buffer);
	if (v == NULL)
	{
		/*
		v = cmd_vector_init(1);
		cmd_vector_insert(v, '\0');
		*/
		isNeedMatch = 0;
	}

	if (isspace((int)vty->buffer[vty->used_len - 1]))
	{
		isNeedMatch = 0;
	}

	if (1 == isNeedMatch && NULL != v)
	{
		match_type = cmd_match_command(v, vty, match, &match_size, lcd_str);

		last_word = (char*)cmd_vector_slot(v, cmd_vector_max(v) - 1);

		if (vty->inputMachine_prev != CMD_KEY_CODE_TAB)
		{
			strcpy(vty->tabbingString, last_word);
		}

		cmd_vector_deinit(v, 1);
	}

	vty_printf(vty, "%s", CMD_ENTER);
	switch (match_type) {
		case CMD_NO_MATCH:
			cmd_outprompt(vty);
			vty_printf(vty, "%s", vty->buffer);
			break;
		case CMD_FULL_MATCH:
			cmd_delete_word(vty);
			if (NULL != match[0])
			{
				cmd_insert_word(vty, match[0]->para);
			}
			/* BEGIN: Added by weizengke, 2013/10/14 for full match then next input*/
			cmd_insert_word(vty, " ");
			/* END:   Added by weizengke, 2013/10/14 */
			cmd_outprompt(vty);
			vty_printf(vty, "%s", vty->buffer);

			/* BEGIN: Added by weizengke, 2013/10/27 PN: fix the bug of CMD_FULL_MATCH and then continue TAB*/
			memset(vty->tabString,0,sizeof(vty->tabString));
			memset(vty->tabbingString,0,sizeof(vty->tabbingString));
			vty->tabStringLenth = 0;
			/* END:   Added by weizengke, 2013/10/27 */

			break;
		case CMD_PART_MATCH:
			/*  delete at 2013-10-05, CMD_PART_MATCH will never reach, CMD_LIST_MATCH instead.

				case like this:
					disable , display
					>di  TAB
					>dis  ==> CMD_PART_MATCH
			*/
			cmd_delete_word(vty);
			cmd_insert_word(vty, lcd_str);
			cmd_outprompt(vty);
			vty_printf(vty, "%s", vty->buffer);
			break;
		case CMD_LIST_MATCH:

			if (vty->inputMachine_prev != CMD_KEY_CODE_TAB)
			{
				memset(vty->tabString,0,sizeof(vty->tabString));
				strcpy(vty->tabString, match[0]->para);
				vty->tabStringLenth = strlen(vty->tabString);

				/* vty_printf(vty, "%s", CMD_ENTER); */
			}
			else
			{
				for (i = 0; i < match_size; i++)
				{
					if (0 == strcmp(vty->tabString, match[i]->para))
					{
						break;
					}
				}

				if (i == match_size)
				{
					CMD_debug(DEBUG_TYPE_ERROR, "TAB for completing command. bug of tab continue. (tabString=%s)", vty->tabString);
				}

				i++;

				if (i == match_size)
				{
					i = 0;
				}

				memset(vty->tabString,0,sizeof(vty->tabString));
				strcpy(vty->tabString, match[i]->para);
				vty->tabStringLenth = strlen(vty->tabString);

			}

			/*for (i = 0; i < match_size; i++) {
				if (ANOTHER_LINE(i))
					vty_printf(vty, "%s", CMD_ENTER);
				vty_printf(vty, "%-25s", match[i]->para);
			}
			*/

			cmd_delete_word(vty);
			cmd_insert_word(vty, vty->tabString);

			cmd_outprompt(vty);
			vty_printf(vty, "%s", vty->buffer);
			break;
		default:
			break;
	}


	CMD_debug(DEBUG_TYPE_INFO,"tab out:used_len=%d, pos=%d",vty->used_len, vty->cur_pos);
}

/*****************************************************************************
 �� �� ��  : cmd_resolve_enter
 ��������  : ����س�ִ��������
 �������  : struct cmd_vty *vty
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��17��
    ��    ��   : weizengke
    �޸�����   : �����ɺ���

bugs:
	1:	display loopback-detect brief
		display loopback brief

		<Jungle-config>dis loop brief
		Command 'dis loop brief ' anbigous follow:
		 brief                     brief
		<Jungle-config>

	2:
		display loopback-detect brief
		display loopback brief
		disable loopback-detect

		<Jungle-config>dis loopback
		Command 'dis loopback' anbigous follow:
		 loopback                  loopback-detect
		<Jungle-config>

*****************************************************************************/
void cmd_resolve_enter(struct cmd_vty *vty)
{
	struct para_desc *match[CMD_MAX_MATCH_SIZE];	// matched string
	int match_size = 0;

	int match_type = CMD_NO_MATCH;
	cmd_vector_t *v;

	int i = 0;
	int nomath_pos = -1;

	VIEW_ID_EN view_id_ = VIEW_NULL;
	
	//printf("enter(%d %d %s)\r\n", vty->used_len, vty->buf_len, vty->buffer);

	v = str2vec(vty->buffer);
	if (v == NULL) {
		vty_printf(vty, "%s", CMD_ENTER); /* ���ڲ���Ҫ�س� */
		cmd_outprompt(vty);
		return;
	}

	/* BEGIN: Added by weizengke, 2013/10/5   PN:for cmd end with <CR> */
	cmd_vector_insert_cr(v);
	/* END:   Added by weizengke, 2013/10/5   PN:None */

	vty_printf(vty, "%s", CMD_ENTER);

	// do command
	match_type = cmd_execute_command(v, vty, match, &match_size, &nomath_pos);

	CMD_debug(DEBUG_TYPE_FUNC, "Execute command '%s' on view '%s'. (result:%d)", vty->buffer, cmd_view_getViewName(vty->view_id), match_type);
	write_log(JUDGE_INFO, "Execute command:%s. (result:%d)", vty->buffer, match_type);

	view_id_ = vty->view_id;
		
	while (match_type == CMD_NO_MATCH)
	{
		int match_size_ = 0;
		int nomath_pos_ = -1;
		VIEW_ID_EN view_id__ = VIEW_NULL;
		
		view_id__ = vty_view_getParentViewId(vty->view_id);
		
		if (VIEW_NULL == view_id__
			|| VIEW_USER == view_id__
			|| VIEW_GLOBAL == view_id__)
		{
			vty->view_id = view_id_;
			break;
		}

		vty->view_id = view_id__;
		match_type = cmd_execute_command(v, vty, match, &match_size_, &nomath_pos_);

		CMD_debug(DEBUG_TYPE_FUNC, "Continue execute command '%s' on view '%s'. (result:%d)", vty->buffer, cmd_view_getViewName(view_id__), match_type);
	}
	
	// add executed command into history
	cmd_vty_add_history(vty);

	if (match_type == CMD_NO_MATCH)
	{
		cmd_output_missmatch(vty, nomath_pos);
	}
	else if (match_type == CMD_ERR_ARGU)
	{
		vty_printf(vty, "Too Many Arguments%s"CMD_ENTER);
	}

	if (match_type == CMD_ERR_AMBIGOUS)
	{
		if (match_size)
		{
			vty_printf(vty, "Command '%s' anbigous follow:%s", vty->buffer, CMD_ENTER);

			for (i = 0; i < match_size; i++)
			{
				if (ANOTHER_LINE(i))
					vty_printf(vty, "%s", CMD_ENTER);
				vty_printf(vty, " %-25s", match[i]->para);
			}
			vty_printf(vty, "%s", CMD_ENTER);
			/* del 10-29
			cmd_outprompt(vty);
			vty_printf(vty, "%s", vty->buffer);
			*/
			vty->cur_pos = vty->used_len = 0;
			memset(vty->buffer, 0, vty->buf_len);
			cmd_outprompt(vty);
		}
	}
	else
	{
		// ready for another command
		vty->cur_pos = vty->used_len = 0;
		memset(vty->buffer, 0, vty->buf_len);
		cmd_outprompt(vty);
	}
}

/*****************************************************************************
 �� �� ��  : cmd_resolve_quest
 ��������  : ����?�ַ�����������
 �������  : struct cmd_vty *vty
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��17��
    ��    ��   : weizengke
    �޸�����   : �����ɺ���

	1: ��ȫƥ������ʱ��ֻ���ظ����������  2013-10-27 (δʵ��)

	2:bug:
		 display loopback
		 disable loopback-detect
	   >dis loop   =======> show what

*****************************************************************************/
void cmd_resolve_quest(struct cmd_vty *vty)
{
	cmd_vector_t *v;
	struct para_desc *match[CMD_MAX_MATCH_SIZE];	// matched string
	int match_size = 0;
	int i = 0;
	int nomath_pos = -1;

	/* BEGIN: Added by weizengke, 2013/10/4   PN:need print '?' */
	cmd_put_one(vty, '?');
	/* END:   Added by weizengke, 2013/10/4   PN:need print '?' */
			
	/*
	1: ȡpos ֮ǰ��buf
	2: ��Ҫ���ǵ�ǰ�����buf
	*/
	/* BEGIN: Added by weizengke, 2013/11/17 bug for left and tab*/
	memset(&(vty->buffer[vty->cur_pos]), 0 ,strlen(vty->buffer) - vty->cur_pos);
	vty->used_len = strlen(vty->buffer);
	/* END:   Added by weizengke, 2013/11/17 */

	v = str2vec(vty->buffer);
	if (v == NULL)
	{
		v = cmd_vector_init(1);
		cmd_vector_insert_cr(v);
	}
	else if (isspace((int)vty->buffer[vty->used_len - 1]))
	{
		cmd_vector_insert_cr(v);
	}

	cmd_complete_command(v, vty, match, &match_size, &nomath_pos);

	vty_printf(vty, "%s", CMD_ENTER);

	if (match_size) {
		for (i = 0; i < match_size; i++) {
			vty_printf(vty, " %-25s%s\r\n", match[i]->para,match[i]->desc);
		}
		cmd_outprompt(vty);
		vty_printf(vty, "%s", vty->buffer);
	} else {

		cmd_output_missmatch(vty, nomath_pos);

		cmd_outprompt(vty);
		vty_printf(vty, "%s", vty->buffer);
	}

	cmd_vector_deinit(v, 0);
}


/* bug of up twice with last key is not up, the hpos not restart */
void cmd_resolve_up(struct cmd_vty *vty)
{
	int try_idx = vty->hpos == 0 ? (HISTORY_MAX_SIZE - 1) : vty->hpos - 1;

	// if no history
	if (vty->history[try_idx] == NULL)
		return;
	vty->hpos = try_idx;

	// print history command
	cmd_clear_line(vty);
	strcpy(vty->buffer, vty->history[vty->hpos]);
	vty->cur_pos = vty->used_len = strlen(vty->history[vty->hpos]);
	vty_printf(vty, "%s", vty->buffer);
}

void cmd_resolve_down(struct cmd_vty *vty)
{
	int try_idx = vty->hpos ==( HISTORY_MAX_SIZE - 1) ? 0 : vty->hpos + 1;

	// if no history
	if (vty->history[try_idx] == NULL)
		return;
	vty->hpos = try_idx;

	// print history command
	cmd_clear_line(vty);
	strcpy(vty->buffer, vty->history[vty->hpos]);
	vty->cur_pos = vty->used_len = strlen(vty->history[vty->hpos]);
	vty_printf(vty, "%s", vty->buffer);
}

// handle in read buffer, including left, right, delete, insert
void cmd_resolve_left(struct cmd_vty *vty)
{
	//printf("\r\n%d", vty->cur_pos);
	// already at leftmost, cannot move more
	if (vty->cur_pos <= 0)
	{
		return;
	}

	// move left one step
	vty->cur_pos--;
	cmd_back_one(vty);
}

void cmd_resolve_right(struct cmd_vty *vty)
{
	// already at rightmost, cannot move more
	if (vty->cur_pos >= vty->used_len)
	{
		return;
	}

	// move right one step
	cmd_put_one(vty, vty->buffer[vty->cur_pos]);
	vty->cur_pos++;
}

/*****************************************************************************
 �� �� ��  : cmd_resolve_delete
 ��������  : ����Delete������ǰɾ���ַ�
 �������  : struct cmd_vty *vty
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��17��
    ��    ��   : weizengke
    �޸�����   : �����ɺ���

*****************************************************************************/
void cmd_resolve_delete(struct cmd_vty *vty)
{
	int i, size;

	// no more to delete
	if (vty->cur_pos >= vty->used_len)
	{
		return;
	}

	/* del the  current char*/
	cmd_delete_one(vty);

	size = vty->used_len - vty->cur_pos;
	CMD_DBGASSERT(size >= 0, "cmd_resolve_delete");

	memcpy(&vty->buffer[vty->cur_pos], &vty->buffer[vty->cur_pos + 1], size);

	//vty->buffer[vty->used_len] = '\0';
	vty->buffer[vty->used_len - 1] = '\0';

	/* output the right chars */
	for (i = 0; i < size; i ++)
		cmd_put_one(vty, vty->buffer[vty->cur_pos + i]);

	vty->used_len--;

	/* back the cur_pos */
	for (i = 0; i < size; i++)
		cmd_back_one(vty);


}

void cmd_resolve_backspace(struct cmd_vty *vty)
{
	int i, size;

	// no more to delete
	if (vty->cur_pos <= 0)
	{
		return;
	}

	size = vty->used_len - vty->cur_pos;
	
	CMD_DBGASSERT(size >= 0, "cmd_resolve_backspace");

	// delete char
	vty->cur_pos--;
	vty->used_len--;
	cmd_back_one(vty);

	// print left chars
	memcpy(&vty->buffer[vty->cur_pos], &vty->buffer[vty->cur_pos + 1], size);
	vty->buffer[vty->used_len] = '\0';
	for (i = 0; i < size; i ++)
		cmd_put_one(vty, vty->buffer[vty->cur_pos + i]);
	cmd_put_one(vty, ' ');
	for (i = 0; i < size + 1; i++)
		cmd_back_one(vty);

}

void cmd_resolve_insert(struct cmd_vty *vty)
{
	int i, size;

	CMD_debug(DEBUG_TYPE_INFO,"insert in:used_len=%d, pos=%d, buffer_len=%d",vty->used_len, vty->cur_pos, strlen(vty->buffer));
	
	// no more to insert
	if (vty->used_len >= vty->buf_len)
	{
		CMD_DBGASSERT(0, "cmd_resolve_insert, used_len>=buf_len");
		return;
	}
	
	size = vty->used_len - vty->cur_pos;

	CMD_DBGASSERT(size >= 0, "cmd_resolve_insert");

	memcpy(&vty->buffer[vty->cur_pos + 1], &vty->buffer[vty->cur_pos], size);
	vty->buffer[vty->cur_pos] = vty->c;

#if 0
	/* BEGIN: del by weizengke, 2013/11/17 */
	/* BEGIN: Added by weizengke, 2013/10/4   PN:bug for continue tab */
	vty->buffer[vty->cur_pos + 1] = '\0';
	/* END:   Added by weizengke, 2013/10/4   PN:None */
	/* END: del by weizengke, 2013/11/17 */
#endif

	// print left chars, then back size
	for (i = 0; i < size + 1; i++)
		cmd_put_one(vty, vty->buffer[vty->cur_pos + i]);
	for (i = 0; i < size; i++)
		cmd_back_one(vty);

	vty->cur_pos++;
	vty->used_len++;

	CMD_debug(DEBUG_TYPE_INFO,"insert out:used_len=%d, pos=%d, buffer_len=%d",vty->used_len, vty->cur_pos, strlen(vty->buffer));

}


/*****************************************************************************
 �� �� ��  : cmd_resolve_del_lastword
 ��������  : ����CTRL_W��������ɾ�����һ��������
 �������  : struct cmd_vty *vty
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��17��
    ��    ��   : weizengke
    �޸�����   : �����ɺ���

*****************************************************************************/
void cmd_resolve_del_lastword(struct cmd_vty *vty)
{
	int i, size;

	// no more to delete
	CMD_debug(DEBUG_TYPE_INFO, "\r\ncmd_resolve_del_lastword, cur_pos=%d",vty->cur_pos);
	
	if (vty->cur_pos <= 0)
		return;

	cmd_delete_word_ctrl_W_ex(vty);

	vty_printf(vty, "%s", CMD_ENTER);
	cmd_outprompt(vty);
	vty_printf(vty, "%s", vty->buffer);

	/* BEGIN: Added by weizengke, 2014/3/23 support delete word form cur_pos*/
	for (i = 0; i < strlen(vty->buffer) - vty->cur_pos; i++)
	{
		cmd_back_one(vty);
	}
	/* END:   Added by weizengke, 2014/3/23 */

}


key_handler_t key_resolver[] = {
	// resolve a whole line
	{ CMD_KEY_CODE_FILTER, 	cmd_resolve_filter },
	{ CMD_KEY_CODE_TAB, 	cmd_resolve_tab },
	{ CMD_KEY_CODE_ENTER, 	cmd_resolve_enter },
	{ CMD_KEY_CODE_QUEST, 	cmd_resolve_quest },
	{ CMD_KEY_CODE_UP, 		cmd_resolve_up },
	{ CMD_KEY_CODE_DOWN, 	cmd_resolve_down },
	// resolve in read buffer
	{ CMD_KEY_CODE_LEFT, 	cmd_resolve_left },
	{ CMD_KEY_CODE_RIGHT, 	cmd_resolve_right },
	{ CMD_KEY_CODE_DELETE,   cmd_resolve_delete },
	{ CMD_KEY_CODE_BACKSPACE, 	cmd_resolve_backspace },
	{ CMD_KEY_CODE_DEL_LASTWORD, cmd_resolve_del_lastword},

	{ CMD_KEY_CODE_NOTCARE, cmd_resolve_insert },

};


