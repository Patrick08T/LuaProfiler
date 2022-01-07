#include "TraceNode.hpp"

TraceInfoNode::TraceInfoNode(TSadPtrNode Other)
{
  strId = Other->strId;
  strName = Other->strName;
  dCallTime = Other->dCallTime;
  dTotalTime = Other->dTotalTime;
  nCount = Other->nCount;
  spParent = Other->spParent;
}

TraceInfoNode::TraceInfoNode(TSadPtrNode parent, TStrID &id, const std::string &funcname)
{
  strId = id;
  strName = funcname;
  spParent = parent;
}

void TraceInfoNode::addChild(TSadPtrNode child)
{
  vecChildren.push_back(child);
  mapChildID.emplace(child->strId, child);
}

// root start
void TraceInfoNode::stackStart()
{
  dCallTime = GetTimeMs();
}

// root returned
void TraceInfoNode::stackEnd()
{
  double nowTime = GetTimeMs();
  dTotalTime += nowTime - dCallTime;
  dCallTime = nowTime;
}

void TraceInfoNode::functionStart()
{
  dCallTime = GetTimeMs();
  nCount += 1;
}

void TraceInfoNode::functionEnd()
{
  dTotalTime += GetTimeMs() - dCallTime;
}

TSadPtrNode TraceInfoNode::getChild(const TStrID &id)
{
  if (mapChildID.find(id) != mapChildID.end())
  {
    return mapChildID[id];
  }
  return nullptr;
}

void TraceInfoNode::clear()
{
  strName.clear();
  dCallTime = 0.f;
  dTotalTime = 0.f;
  nCount = 0;
  strId.clear();
  spParent = nullptr;
  vecChildren.clear();
  mapChildID.clear();
}

void TraceInfoNode::mergeChildNode(TSadPtrNode child)
{
  if (mapChildID.find(child->strId) != mapChildID.end())
  {
    TSadPtrNode node = mapChildID[child->strId];
    node->dTotalTime += child->dTotalTime;
    node->nCount += child->nCount;
  }
  else
  {
    addChild(TSadPtrNode(new TraceInfoNode(child)));
  }
  dTotalTime += child->dTotalTime;
}