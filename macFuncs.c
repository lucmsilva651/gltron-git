#ifdef macintosh

/* we want GLTron to run on locked volumes (CD-ROMs etc),
   so we must save settings to the prefs folder */

#include <Folders.h>
#include "FullPath.h" /* MoreFiles Library */

void setupHomeEnvironment () {

    short  volume_ref_number;
    long   directory_id;
    short  len;
    Handle path;
    
    if ( noErr != FindFolder ( kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder,
                               &volume_ref_number, &directory_id) )
        exit (-1);
    
    if ( noErr == GetFullPath (volume_ref_number, directory_id, "\p", &len, &path) ) {
        
        char *homeEnv = (char*) malloc ( sizeof(*homeEnv) * len + 6);
        strcpy (homeEnv, "HOME=");
        strncat (homeEnv, *path, len-1); /* we want to remove the terminating ':' */        
        SDL_putenv (homeEnv);        
        free (homeEnv);
    }
}

/* some compilers don't define these very common functions yet */

int strcasecmp(const char *s1, const char *s2)
{
    char c1, c2;
    while (1)
    {
    	c1 = tolower(*s1++);
    	c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) return 0;
    }
}

/*
int strncasecmp(const char *s1, const char *s2, int n)
{
    int i;
    char c1, c2;
    for (i=0; i<n; i++)
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (!c1) return 0;
    }
    return 0;
}*/

#include "Events.h"

void mac_yield_cpu (int wait) {

    EventRecord event;
    
    WaitNextEvent (everyEvent, &event, wait, NULL);
}

#endif
