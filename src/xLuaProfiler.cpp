#include <algorithm>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <cstring>
#include "TraceTree/TraceTree.hpp"
#include "lauxlib.h"

/* 
sampling type : collect the function call times by sampling
tick type : get the all call stacks by this tick
*/
#define sampling

int grunning = 0;
bool openHook = true;
lua_State *gL;
std::shared_ptr<TraceInfoTree> curTraceTree = nullptr;

extern "C" int lrealstop(lua_State *L)
{
  lua_sethook(L, 0, 0, 0);
  grunning = 0;

  if (curTraceTree)
  {
    curTraceTree->mergeAllNodes();
    TSadPtrNode root = curTraceTree->getspRoot();
    #ifndef sampling 
    curTraceTree->printTree(root, 0);
    #else
    curTraceTree->printfolded(root, "");
    #endif
  }

#ifdef sampling
  struct itimerval timer;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  timer.it_value = timer.it_interval;
  int ret = setitimer(ITIMER_PROF, &timer, NULL);
  if (ret != 0)
  {
    PERR("setitimer fail %d", ret);
    return ret;
  }
#endif

  return 0;
}

static void SignalHandlerHookSwitch(lua_State *L, lua_Debug *par)
{
  switch (par->event)
  {
  case LUA_HOOKCALL:
  {
    if (curTraceTree)
      curTraceTree->onHookCall(L, par);
    break;
  }
  case LUA_HOOKRET:
  {
    if (curTraceTree)
      curTraceTree->onHookReturn(L, par);
    break;
  }
  case LUA_HOOKCOUNT:
  {
    if (curTraceTree)
      curTraceTree->onHookSampling(L, par);
  }
  default:
    break;
  }
}

static void SignalHandler(int sig, siginfo_t *sinfo, void *ucontext)
{
  if (openHook)
  {
    lua_sethook(gL, SignalHandlerHookSwitch, LUA_MASKCOUNT, 1);
  }
  else
  {
    lua_sethook(gL, 0, 0, 0);
  }
  openHook = openHook ? false : true;
}

static void sig_handler(int sig)
{
  lrealstop(gL);
}

extern "C" int lrealstart(lua_State *L, int frequencyPerSecond)
{
  if (grunning)
  {
    PERR("start again, failed");
    return -1;
  }
  grunning = 1;
  gL = L;
  signal(SIGINT, sig_handler);
  signal(SIGSEGV, sig_handler);

  curTraceTree = std::shared_ptr<TraceInfoTree>(new TraceInfoTree());
  if (curTraceTree)
  {
    curTraceTree->init();
  }

#ifdef sampling
  struct sigaction sa;
  sa.sa_sigaction = SignalHandler;
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGPROF, &sa, NULL) == -1)
  {
    PERR("sigaction(SIGALRM) failed");
    return -1;
  }

  // default sampling frequecy is 999Hz
  frequencyPerSecond = frequencyPerSecond < 0 ? -frequencyPerSecond : frequencyPerSecond;
  unsigned int frequency = frequencyPerSecond == 0 ? 999 : frequencyPerSecond;

  struct itimerval timer;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 1000000 / frequency;
  timer.it_value = timer.it_interval;
  int ret = setitimer(ITIMER_PROF, &timer, NULL);
  if (ret != 0)
  {
    PERR("setitimer fail %d", ret);
    return -1;
  }

  // PDBG("lrealstart frequency:%u %s", frequency, file);
#else
  lua_sethook(gL, SignalHandlerHookSwitch, LUA_MASKCALL | LUA_MASKRET, 0);
#endif
  return 0;
}

static int lstart(lua_State *L)
{
  int frequency = (int)lua_tointeger(L, 1);

  int ret = lrealstart(L, frequency);
  lua_pushinteger(L, ret);
  return 1;
}

static int lstop(lua_State *L)
{
  int ret = lrealstop(L);

  lua_pushinteger(L, ret);
  return 1;
}

extern "C" int luaopen_libluaperf(lua_State *L)
{
  luaL_checkversion(L);
  luaL_Reg l[] = {
      {"start", lstart},
      {"stop", lstop},
      {NULL, NULL},
  };
  luaL_newlib(L, l);
  return 1;
}