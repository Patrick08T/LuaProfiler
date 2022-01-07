#pragma once

#include "lua.hpp"
#include <string>
#include <chrono>
#include "execinfo.h"
#include "string.h"
#include <time.h>
#include <sys/time.h>

typedef std::chrono::high_resolution_clock Clock;
typedef std::string TStrID;
typedef unsigned int uint32;


static double GetTimeMs() { return Clock::now().time_since_epoch().count() * 0.000001; }

/******************************************* output *****************************************/
#define PDBG(...) \
  log("[DEBUG] ", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#define PERR(...) \
  log("[ERROR] ", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

static void log(const char *header, const char *file, const char *func, int pos, const char *fmt, ...)
{
  FILE *pLog = NULL;
  va_list ap;

  pLog = fopen("sampling.log", "a+");
  if (pLog == NULL)
  {
    return;
  }

  va_start(ap, fmt);
  vfprintf(pLog, fmt, ap);
  fprintf(pLog, "\n");
  va_end(ap);
  fclose(pLog);
}
/******************************************* output end *****************************************/

/******************************************* copy from lua source *****************************************/
/*
** search for 'objidx' in table at index -1.
** return 1 + string at top if find a good name.
*/
static int findfield(lua_State *L, int objidx, int level)
{
  if (level == 0 || !lua_istable(L, -1))
    return 0;     /* not found */
  lua_pushnil(L); /* start 'next' loop */
  while (lua_next(L, -2))
  { /* for each pair in table */
    if (lua_type(L, -2) == LUA_TSTRING)
    { /* ignore non-string keys */
      if (lua_rawequal(L, objidx, -1))
      {                /* found object? */
        lua_pop(L, 1); /* remove value (but keep name) */
        return 1;
      }
      else if (findfield(L, objidx, level - 1))
      {                    /* try recursively */
        lua_remove(L, -2); /* remove table (but keep name) */
        lua_pushliteral(L, ".");
        lua_insert(L, -2); /* place '.' between the two names */
        lua_concat(L, 3);
        return 1;
      }
    }
    lua_pop(L, 1); /* remove value */
  }
  return 0; /* not found */
}

/*
** Search for a name for a function in all loaded modules
*/
static int pushglobalfuncname(lua_State *L, lua_Debug *ar)
{
  int top = lua_gettop(L);
  lua_getinfo(L, "f", ar); /* push function */
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
  if (findfield(L, top + 1, 2))
  {
    const char *name = lua_tostring(L, -1);
    if (strncmp(name, "_G.", 3) == 0)
    {                              /* name start with '_G.'? */
      lua_pushstring(L, name + 3); /* push name without prefix */
      lua_remove(L, -2);           /* remove original name */
    }
    lua_copy(L, -1, top + 1); /* move name to proper place */
    lua_pop(L, 2);            /* remove pushed values */
    return 1;
  }
  else
  {
    lua_settop(L, top); /* remove function and global table */
    return 0;
  }
}

static int lastlevel(lua_State *L)
{
  lua_Debug ar;
  int li = 1, le = 1;
  /* find the bottom index of the call stack. */
  while (lua_getstack(L, le, &ar))
  {
    li = le;
    le *= 2;
  }
  /* do a binary search */
  while (li < le)
  {
    int m = (li + le) / 2;
    if (lua_getstack(L, m, &ar))
      li = m + 1;
    else
      le = m;
  }
  return le - 1;
}
/******************************************* copy from lua source *****************************************/

static std::string getFunctionName(lua_State *L, lua_Debug *ar)
{
  lua_getinfo(L, "Slnt", ar);
  if (pushglobalfuncname(L, ar))
  { /* try first a global name */
    lua_pushfstring(L, "function '%s'", lua_tostring(L, -1));
    lua_remove(L, -2); /* remove name */
  }
  else if (*ar->namewhat != '\0')                          /* is there a name from code? */
    lua_pushfstring(L, "%s '%s'", ar->namewhat, ar->name); /* use it */
  else if (*ar->what == 'm')                               /* main? */
    lua_pushliteral(L, "main chunk");
  else if (*ar->what != 'C') /* for Lua functions, use <file:line> */
    lua_pushfstring(L, "function <%s:%d>", ar->short_src, ar->linedefined);
  else /* nothing left... */
    lua_pushliteral(L, "?");
  const char *funcname = lua_tostring(L, -1);
  lua_pop(L, 1);
  return funcname;
}