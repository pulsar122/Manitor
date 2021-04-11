#include <portab.h>
#include <string.h>

#if IS_ATARI

#include <manilibs\aes\aes.h>
#include <manilibs\sysinfo\sysinfo.h>
#include <manilibs\files\files.h>
#include <manilibs\alloc\alloc.h>
#include <manilibs\cookie\cookie.h>
#include <manilibs\vdi\vdi.h>

/* Rckgabe:	  0	kein MagiC installiert
					 -1	bin noch im AUTO-Ordner
				0x0a0b	Version a.b
	falls crdate != 0L: 0xjjjjmmtt
*/
static int get_MagiC_ver(unsigned long *crdate) {
	MAGX_COOKIE *cook;
	AESVARS *av;

	if (!get_cookie('MagX', (long *)&cook))
		return(0);
	av = cook->aesvars;
	if	(!av)
		return(-1);
	if	(crdate) {
		*crdate = av->date << 16L;					/* jjjj0000 */
		*crdate |= av->date >> 24L;				/* jjjj00tt */
		*crdate |= (av->date >> 8L) & 0xff00L;	/* jjjjmmtt */
	}
	return(av->version);
}

boolean get_sysinfo(SYSINFO *info) {
	unsigned int version = Sversion();
	info->mint = get_cookie('MiNT', 0L);
	info->gemdosversion = (int)(((version << 8) & 0xFF00) | ((version >> 8) & 0x00FF));
	info->magic_ver = get_MagiC_ver(&info->magic_date);
	info->magic = (info->magic_ver > 0);
	info->magicmac = get_cookie('MgMc', 0L);
	info->magicpc = get_cookie('MgPC', 0L);
	info->Mxalloc = (Mxalloc(-1, 0) >= 0L);
	if (info->magic_ver < 0x200 && !info->mint) {
		info->Mxmask = 0x3;
	} else {
		info->Mxmask = 0xFFFF;
	}
	{
		char path[MAXPATH0];
		current_path(path);
		info->startpath = (char *)alloc(strlen(path)+1);
		if (!info->startpath)
			return FALSE;
		strcpy(info->startpath, path);
	}
	{
		NVDI_COOKIE *cook;
		info->nvdi = get_cookie('NVDI', (long *)&cook);
		if (info->nvdi) {
			info->nvdi_cookie = *cook;
		} else {
			info->nvdi_cookie.version = 0;
			info->nvdi_cookie.date = 0L;
		}
	}
	info->gdos = (vq_gdos() != 0);
	if (info->gdos) {
		info->speedo = (get_cookie('FSMC', 0L) != 0);
	} else {
		info->speedo = FALSE;
	}
	info->EdDI = get_cookie('EdDI', (long *)&(info->EdDI_dispatch));
	if (info->EdDI) {
		info->EdDI_version = info->EdDI_dispatch(0);
	} else {
		info->EdDI_version = 0;
	}

	return TRUE;
}

static void get_obfix_raster(int *breite,int *hoehe, int *global) {
    OBJECT tree[1]={-1,-1,-1,G_BOX,NONE,NORMAL,0x00000000l,0,0,1,1};
    MT_rsrc_obfix(tree, 0, global);
    *breite=tree[0].ob_width;
    *hoehe=tree[0].ob_height;
}

void get_aesinfo(AESINFO *info, SYSINFO *sysinfo, int16 *global) {
	int16 out1, out2, out3, out4;
	info->aes_handle = MT_graf_handle(&info->wchar, &info->hchar, &out3, &out4, global);
	get_obfix_raster(&info->wobfix, &info->hobfix, global);

	if (MT_appl_xgetinfo(11, &out1, &out2, &out3, &out4, global)) {
		info->wfbevent = (boolean)(out1 & 0x20);
		info->wfowner = (boolean)(out1 & 0x10);
		info->wfbottom = (boolean)(out1 & 0x40);
		info->wfhotclose = (boolean)(out1 & 0x8);
		info->newdeskget = (boolean)(out1 & 0x2);
	} else {
		info->wfbevent = FALSE;
		info->wfowner = FALSE;
		info->wfbottom = FALSE;
		info->wfhotclose = FALSE;
		info->newdeskget = FALSE;
	}

	if (MT_appl_xgetinfo(12, &out1, &out2, &out3, &out4, global)) {
		info->ap_term = (boolean)(out1 & 0x08);
		info->wficonify = (boolean)(out1 & 0x80);
		info->wficonipos = (boolean)(out3 != 0);
	} else {
		info->ap_term = FALSE;
		info->wficonify = FALSE;
		info->wficonipos = FALSE;
	}

	info->edscroll = (sysinfo->magic && sysinfo->magic_date >= 0x19950829L);

	if (MT_appl_xgetinfo(0, &out1, &out2, &out3, &out4, global)) {
		info->aes_font = out2;
		info->aes_height = out1;
	} else {
		info->aes_font = 1;
		info->aes_height = (info->hchar < 16) ? 9 : 10;
	}

	if (MT_appl_xgetinfo(13, &out1, &out2, &out3, &out4, global)) {
		info->magicobjc = (boolean)(out4 & 0x4);	/* Bit 2: MagiC-Objekte? */
		info->gshortcut = (boolean)(out4 & 0x8);	/* Bit 3: G_SHORTCUT? */
		info->obj3d = (boolean)(out1 != 0);
		if (info->obj3d && out2) {			/* 3D-Objekte und objc_sysvar vorhanden? */
			MT_objc_sysvar(0, AD3DVALUE, 0,0, &info->breiter3d, &info->hoeher3d, global);
		}
	} else {
		info->magicobjc = (sysinfo->magic_ver >= 0x300);
		info->gshortcut = FALSE;
		info->obj3d = FALSE;
		info->breiter3d = 0;
		info->hoeher3d = 0;
	}

	{
		char cmd[MAXPATHDAT0] = "";
		char tail[256];
		if (MT_shel_read(cmd, tail, global) != 0 && cmd[0] && cut_last_dir(cmd, 0L)) {
			char *new = alloc(strlen(cmd)+1);
			if (new) {
				free(sysinfo->startpath);
				sysinfo->startpath = new;
				strcpy(sysinfo->startpath, cmd);
			}
		}
	}

	MT_wind_get_grect(0, WF_WORKXYWH, (GRECT *)&(info->desk), global);

/*
	if (!MT_appl_xgetinfo(7, &out1, &out2, &out3, &out4, global) || (out1 & 0x3) != 0x3) {
		alert(1, "[1][WDIALOG nicht installiert|oder zu alt!][Oops!]");
		return FALSE;
	}
*/

	info->multitasking = (global[1] != 1);

	if (MT_appl_xgetinfo(10, &out1, &out2, &out3, &out4, global)) {
		info->doex_bits = out1;
		info->accstart = (boolean)((out1 & 0xFF) >= 3);
		info->parallelstart = (boolean)(out2 == 0);
	} else {
		info->doex_bits = 0;
		info->accstart = FALSE;
		info->parallelstart = info->multitasking;
	}

	info->parallelstart = (info->parallelstart || sysinfo->magic);	/* Sicherheitshalber */
	if (sysinfo->magic_ver >= 0x300) {
		info->accstart = TRUE;		/* MagiC liefert's falsch! */
	}

	if (MT_appl_xgetinfo(4, &out1, &out2, &out3, &out4, global)) {
		info->mint2aesid = (boolean)out2;
		info->applsearch = (boolean)out3;
	} else {
		info->mint2aesid = FALSE;
		info->applsearch = FALSE;
	}

	if (MT_appl_xgetinfo(2, &out1, &out2, &out3, &out4, global)) {
		info->coloricons = (boolean)out3;
	} else {
		info->coloricons = global[0] >= 0x330 && !sysinfo->magic;
	}
}

void sysinfo_deinit(SYSINFO *sysinfo) {
	if (sysinfo->startpath) {
		free(sysinfo->startpath);
	}
}

#endif
