
/* $Id: types.h,v 1.1 2002/04/17 17:22:27 latimerius Exp $ */

#ifndef TYPES_H
#define TYPES_H

struct _coordinates_ {
	short X, Y;
};

typedef struct _coordinates_ AreaCoords;
typedef struct _coordinates_ FieldCoords;

typedef struct {
	int XMin, XMax, YMin, YMax;
} RectBounds ;

#endif /* TYPES_H */
