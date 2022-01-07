#include "TraceTree.hpp"
#include "algorithm"
#include <stack>

TraceInfoTree::TraceInfoTree()
{
}
TraceInfoTree::~TraceInfoTree()
{
}

void TraceInfoTree::init()
{
  std::string rootName("");
  spRoot = TSadPtrNode(new TraceInfoNode(nullptr, rootName, ""));
  spCurNode = spRoot;
}

void TraceInfoTree::onHookCall(lua_State *L, lua_Debug *ar)
{
  if (spRoot)
  {
    lua_getinfo(L, "nS", ar);
    TSadPtrNode child = getChild(L, ar);
    if (child == spCurNode)
    {
      spCurNode->nCount += 1;
      return;
    }
    if (child->spParent == spRoot)
    {
      spRoot->stackStart();
    }
    child->functionStart();
    spCurNode = child;
  }
}

void TraceInfoTree::onHookReturn(lua_State *L, lua_Debug *ar)
{
  if (spRoot && spCurNode->spParent)
  {
    spCurNode->functionEnd();
    spCurNode = spCurNode->spParent;
    lua_getinfo(L, "nS", ar);
    if (spCurNode == spRoot)
    {
      spRoot->stackEnd();
    }
  }
}

void TraceInfoTree::onHookSampling(lua_State *L, lua_Debug *ld)
{
  if (spRoot)
  {
    lua_Debug ar;
    int level = lastlevel(L);
    while (lua_getstack(L, level--, &ar))
    {
      lua_getinfo(L, "Slnt", &ar);
      TSadPtrNode child = getChild(L, &ar);
      child->nCount += 1;
      spCurNode = child;
    }
    spCurNode = spRoot;
  }
}

void TraceInfoTree::calculateAverageTime(TSadPtrNode node)
{
  if (node)
  {
    std::sort(node->vecChildren.begin(), node->vecChildren.end(), [](const TSadPtrNode &A, const TSadPtrNode &B)
              { return A->dTotalTime > B->dTotalTime; });

    node->dAverage = node->nCount > 0 ? node->dTotalTime / node->nCount : 0;
    for (auto& it : node->vecChildren)
    {
      calculateAverageTime(it);
    }
  }
}

TSadPtrNode TraceInfoTree::getChild(lua_State *L, lua_Debug *ar)
{
  TStrID id = TStrID(ar->short_src).append(":").append(std::to_string(ar->linedefined));

  if (id == spCurNode->strId) return spCurNode;

  TSadPtrNode child = spCurNode->getChild(id);
  if (!child)
  {
    std::string funcname = getFunctionName(L, ar);
    child = TSadPtrNode(new TraceInfoNode(spCurNode, id, funcname));
    spCurNode->addChild(child);
  }
  return child;
}

void TraceInfoTree::clearingTime()
{
  if (spCurNode != spRoot)
  {
    TSadPtrNode node = spCurNode;
    while (node)
    {
      node->functionEnd();
      node = node->spParent;
    }
  }

  calculateAverageTime(spRoot);
}

void TraceInfoTree::mergeChildrenNode(TSadPtrNode node, TSadPtrNode ans)
{
  if (node)
  {
    for(auto& it : node->vecChildren)
    {
      ans->mergeChildNode(it);
      mergeChildrenNode(it, ans);
    }
  }
}

TSadPtrNode TraceInfoTree::mergeAllNodes()
{
  clearingTime();

  TSadPtrNode ans = TSadPtrNode(new TraceInfoNode(spRoot));
  mergeChildrenNode(spRoot, ans);
  return ans;
}

void TraceInfoTree::printTree(TSadPtrNode root, int depth)
{
  std::string out("");
  for (int i = 0; i < depth; ++i)
    out.append(" ");
  out.append("|-<").append(root->strName)
    .append("> totaltime:").append(std::to_string(root->dTotalTime))
    .append(" average:").append(std::to_string(root->dAverage))
    .append(" count:").append(std::to_string(root->nCount));
  PDBG("%s", out.c_str());
  for (auto &it : root->vecChildren)
  {
    printTree(it, depth + 1);
  }
}

void TraceInfoTree::printfolded(TSadPtrNode root, std::string out)
{
  if (!root->vecChildren.empty())
  {
    if (root != spRoot)
      out.append(root->strName).append(" ").append(root->strId).append(";");
  }
  else
  {
    out.append(root->strName).append(" ").append(root->strId);
    PDBG("%s %u", out.c_str(), root->nCount);
    return;
  }

  for (auto &it : root->vecChildren)
  {
    printfolded(it, out);
  }
}