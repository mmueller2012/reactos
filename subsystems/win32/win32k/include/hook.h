#ifndef _WIN32K_HOOK_H
#define _WIN32K_HOOK_H

#define HOOK_THREAD_REFERENCED	(0x1)

typedef struct tagHOOK
{
  LIST_ENTRY Chain;          /* Hook chain entry */
  HHOOK      Self;           /* user handle for this hook */
  PETHREAD   Thread;         /* Thread owning the hook */
  int        HookId;         /* Hook table index */
  HOOKPROC   Proc;           /* Hook function */
  BOOLEAN    Ansi;           /* Is it an Ansi hook? */
  ULONG      Flags;          /* Some internal flags */
  UNICODE_STRING ModuleName; /* Module name for global hooks */
} HOOK, *PHOOK;

#define NB_HOOKS (WH_MAXHOOK-WH_MINHOOK+1)

typedef struct tagHOOKTABLE
{
  LIST_ENTRY Hooks[NB_HOOKS];  /* array of hook chains */
  UINT       Counts[NB_HOOKS]; /* use counts for each hook chain */
} HOOKTABLE, *PHOOKTABLE;

typedef struct tagEVENTHOOK
{
  LIST_ENTRY     Chain;      /* Event chain entry */
  HWINEVENTHOOK  Self;       /* user handle for this event */
  PETHREAD       Thread;     /* Thread owning the event */
  UINT           eventMin;
  UINT           eventMax; 
  WINEVENTPROC   Proc;       /* Event function */
  BOOLEAN        Ansi;       /* Is it an Ansi event? */
  ULONG          Flags;      /* Some internal flags */
  UNICODE_STRING ModuleName; /* Module name for global events */
} EVENTHOOK, *PEVENTHOOK;

typedef struct tagEVENTTABLE
{
  LIST_ENTRY Events;
  UINT       Counts;
} EVENTTABLE, *PEVENTTABLE;

LRESULT FASTCALL co_HOOK_CallHooks(INT HookId, INT Code, WPARAM wParam, LPARAM lParam);
LRESULT FASTCALL co_EVENT_CallEvents(DWORD, HWND, LONG, LONG);
VOID FASTCALL HOOK_DestroyThreadHooks(PETHREAD Thread);
PHOOK FASTCALL IntGetHookObject(HHOOK);

#endif /* _WIN32K_HOOK_H */

/* EOF */
