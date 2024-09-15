/** 
 *          File       :  wlist.h
 *
 *          Contains   :  list widget, management of lists.
 *
 *          Version    :  1.0 ( 03/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */

#ifndef __WLIST_H__
#define __WLIST_H__

enum {
  LIST_SCROLL_UP,
  LIST_SCROLL_DOWN
};

//define options
enum {
  cWlistDefaultOptions = 0,          //default options
  cWlistAlignMiddle    = 1,          //text is align middle
  cWlistNoBox          = 2           //no box is drawn
};

typedef struct Wlist *WlistPtr;

/** Callbacks for each columns */
typedef struct ColCallBacks
{
  void  (*drawit)( WlistPtr list, int x, int y, int line, int col );
  char* (*tostr)(WlistPtr list, int line, int col);
  int   (*sortit)( WlistPtr list, int line, int next );
} ColCallBacks;


/** Definition of each columns */
typedef struct ColDef
{
  char            *title;             //title of column
  int             colsize;            //size of each columns.
  float           color[3];           //color of the text.
  ColCallBacks    callbacks;          //callBacks for each columns
} ColDef;

/** The list itself  */
typedef struct Wlist {
  int              width;             //width of the text in pix
  int              height;            //height of the text in pix
  int              x;                 //pos of top left corner in pix
  int              y;
  int              nblines;           //number of lines
  int              nbcols;            //number of cols by line
  ColDef          *colDefs;           //Definition of each columns
  int              current;           //is current line
  int              scroll;            //window that shows nblines
  int              rlines;            //how many lines in memory?
  int              sortcol;           //column for sorting
  int              options;           //options
  void  (*focus) ( WlistPtr list, int line ); //focus on a line
  void  (*action)( WlistPtr list);    //when double clicking or hitting enter
  void  (*mouseFocus) ( WlistPtr list, int line, Wpoint mousexy ); //mouse focus
  int             *index;             //index of lines
  char          ***lines;             //data on lines
} Wlist;


/** Creating and disposing of column defintions */
ColDef *new_colDefs( int nbcols );


/** Setting column defintions */
void    set_colDef( ColDef *colDefs, int col, char *title, int colsize,
		    float color[3],   
		    void  (*drawit)( WlistPtr list, int x, int y, int line, int col ),
		    char* (*tostr)(WlistPtr list, int line, int col),
		    int   (*sortit)( WlistPtr list, int line, int next )); 

/** Creating and disposing of lists */
Wlist *new_wlist(  int x, int y, int width, int height, int nblines, int nbcols, ColDef *colDefs, int sortcol, int options, 
		   void  (*focus) ( WlistPtr list, int line ), 
		   void  (*action)( WlistPtr list ),
		   void  (*mouseFocus) ( WlistPtr list, int line, Wpoint mousexy )	   );


void   free_wlist       ( Wlist *wlist );


/** Drawing the list */
void   draw_wlist       ( Wlist *wlist );


/** Accessing and Manipulating datas */
void   setCell_wlist    ( Wlist *wlist, void *data, int len, int line, int col );
char*  getCell_wlist    ( Wlist *wlist, int line, int col);
//void   clearCell_wlist  ( Wlist *wlist, Wcell theCell);


/** Adding and Deleting Rows */
int   addRow_wlist     ( Wlist *wlist, short nbrows );
//void   delRow_wlist     ( Wlist *wlist, short nbrows, short num );

/** Index Building */
void initindex_wlist(Wlist *wlist);
void rebuildindex_wlist(Wlist *wlist);


/** Scrolling the list */
void   scroll_wlist     ( Wlist *wlist, int dir );

/** Selections */
int getcurrent_wlist( Wlist *wlist );

/** Key events */
void key_wlist( Wlist *wlist, int key );

/** Get Rect */
Wrect getRect_wlist( Wlist *wlist );

/** Mouse events */
void mouse_wlist( Wlist *wlist, int buttons, int state, int dblClick, Wpoint mousexy );
void mouseMotion_wlist( Wlist *wlist, Wpoint mousexy );
int  mouseFocus_wlist( Wlist *wlist, Wpoint mousexy );

/** Convertions */
char *intToStr_wlist( WlistPtr list, int line, int col );
char *charToStr_wlist(WlistPtr list, int line, int col );

/** Sort        */
int   sortint_wlist( WlistPtr list, int line, int next );
#endif
