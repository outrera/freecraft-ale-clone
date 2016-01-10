
/* $Id: pf_high_open.h,v 1.1 2002/01/15 21:17:52 latimerius Exp $ */

#ifndef HIGH_OPEN_H
#define HIGH_OPEN_H

extern int HighOpenInit (int );
extern void HighOpenReset (void);
extern void HighOpenAdd (Region * );
extern void HighOpenDelete (Region * );
extern Region *HighOpenGetFirst (void);

#endif /* HIGH_OPEN_H */
