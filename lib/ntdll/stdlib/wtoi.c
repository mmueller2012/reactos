/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <ntdll.h>

/*
 * @implemented
 */
int
_wtoi(const wchar_t *str)
{
  return (int)wcstol(str, 0, 10);
}
