#ifndef LIB_Menu_H_
#define LIB_Menu_H_

#define MENU_NEXT(current,itemCount)      (++current>=itemCount)  ? 0 : current
#define MENU_PREVIOUS(current,itemCount)  (--current<0)  ? itemCount : current

#endif