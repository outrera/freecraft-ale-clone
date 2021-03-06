
/* $Id: regid.h,v 1.5 2002/08/30 16:21:14 jsalmon3 Exp $ */

#ifndef REGID_H
#define REGID_H

enum regid_dir {
	REGID_LOWEST,
	REGID_HIGHEST
};
enum regid_usage {
	REGID_UNUSED,
	REGID_USED
};

typedef struct regid_space {
	unsigned int RegidHigh;
	int BitmapSize;
	unsigned char *Bitmap;
} RegidSpace;

extern void RegidSpaceInitialize (RegidSpace * , int );
extern void RegidSpaceDestroy (RegidSpace * );
extern void RegidBitmapInflate (RegidSpace * );
extern void RegidBitmapShrink (RegidSpace * );
extern int  RegidInUse (RegidSpace * , unsigned int );
extern void RegidMarkUnused (RegidSpace * , unsigned int );
extern void RegidMarkUsed (RegidSpace * , unsigned int );
extern int  RegidFind (RegidSpace * , enum regid_dir , enum regid_usage );

#endif /* REGID_H */
