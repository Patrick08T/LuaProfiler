#pragma once

#include "TraceBase.hpp"
#include <vector>
#include <map>
#include <memory>

class TraceInfoNode;
typedef std::shared_ptr<TraceInfoNode> TSadPtrNode;

struct TraceInfoNode
{
  // function name
  std::string strName = "unknown";

  // call time of the function
  double dCallTime = 0;

  // total time of the function was called.
  double dTotalTime = 0;

  // the number of call times
  uint32 nCount = 0;

  // total time / nCount
  double dAverage = 0;

  // id of the node
  TStrID strId;

  // point of parent
  TSadPtrNode spParent = nullptr;

  // children of the node
  std::vector<TSadPtrNode> vecChildren;

  // id map for children
  std::map<TStrID, TSadPtrNode> mapChildID;

  TraceInfoNode(TSadPtrNode Other);

  TraceInfoNode(TSadPtrNode parent, TStrID &id, const std::string &funcname);

  void addChild(TSadPtrNode child);

  /**
   * @brief Get the Child object
   * @return child node or nullptr
   */
  TSadPtrNode getChild(const TStrID &id);

  // root start
  void stackStart();

  // root returned
  void stackEnd();

  // child start
  void functionStart();

  // child end
  void functionEnd();

  // merge or add child node
  void mergeChildNode(TSadPtrNode child);

  void clear();
};
