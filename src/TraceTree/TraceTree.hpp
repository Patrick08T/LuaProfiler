#pragma once

#include "TraceBase.hpp"
#include "TraceNode.hpp"

class TraceInfoTree
{
public:
	TraceInfoTree();
	~TraceInfoTree();

	void init();

	TSadPtrNode getspRoot() { return spRoot; }

  /**
   * @brief lua function enter call back
   */
	void onHookCall(lua_State* L, lua_Debug* ar);

  /**
   * @brief lua function exit call back
   */
	void onHookReturn(lua_State* L, lua_Debug* ar);
  /**
   * @brief lua function exit call back
   */
	void onHookSampling(lua_State* L, lua_Debug* ar);

  /**
   * @brief merge all the child nodes recursively
   * @return TSadPtrNode root node 
   */
	TSadPtrNode mergeAllNodes();

  /**
   * @brief print the trace tree
   * @param depth  stack depth
   */
  void printTree(TSadPtrNode curNode, int depth);
  void printfolded(TSadPtrNode root, std::string out);

private:
	TSadPtrNode getChild(lua_State *L, lua_Debug* ar);

  /**
   * @brief calculate the average execution time of a function
   * @param node 
   */
	void calculateAverageTime(TSadPtrNode node);

	void clearingTime();
	void mergeChildrenNode(TSadPtrNode node, TSadPtrNode ans);

private:
	TSadPtrNode spRoot;
	TSadPtrNode spCurNode;
};