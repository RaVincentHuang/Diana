//===-------------- DomInfo.cpp - Get the dominator infomation ------------===//
// The core code of this part mainly refers to the algorithm implementation of 
// the paper [1]. This is an algorithm that can find the dominant node tree 
// in approximately linear time. In fact, it is O((V+N)α(V+N, N)) and the algorithm 
// of the core of the algorithm by rank merging and path compression can be summarized 
// as Tarjan's EVAL-LINK-UPDATE problem[2]. This information will be put into the 
// subsequent work of phi function insertion and loop optimization.
//
// The algorithm that solves the dominant node can continue to be optimized to strictly 
// linear time[3], but there is clearly no need for our compiler to optimize the execution 
// time further.
//
// Another part of the implementation is the computation of the bounds of the nodes, 
// for which we refer to the algorithm in chapter 19.1.2 of the book [4].
// 
// [1] Thomas Lengauer and Robert Endre Tarjan. 1979. A fast algorithm for finding 
// dominators in a flowgraph. ACM Trans. Program. Lang. Syst. 1, 1 (July 1979), 121–141. 
// https://doi.org/10.1145/357062.357071.
// [2] Robert Endre Tarjan. 1979. Applications of Path Compression on Balanced Trees.
//  J. ACM 26, 4 (Oct. 1979), 690–715. https://doi.org/10.1145/322154.322161.
// [3] D Harel. 1985. A linear algorithm for finding dominators in flow graphs and 
// related problems. In Proceedings of the seventeenth annual ACM symposium on 
// Theory of computing (STOC '85). Association for Computing Machinery, New York, 
// NY, USA, 185–194. https://doi.org/10.1145/22145.22166.
// [4] Andrew W. Appel and Maia Ginsburg. Modern Compiler Implementation in C.
//===----------------------------------------------------------------------===//

#include "sysy/Analysis/DomInfo.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include <set>
#include <map>

namespace sysy
{
namespace ir
{

DomInfo::DomInfo() 
{
    addPass(new IdomGen);
    addPass(new DomTreeGen);
    addPass(new DomFrontierGen);
}

static std::unordered_map<BasicBlock*, BasicBlock*> parent;
static std::unordered_map<BasicBlock*, BasicBlock*> ancenstor;
static std::unordered_map<BasicBlock*, BasicBlock*> best;
static std::unordered_map<BasicBlock*, int> semi;
static std::unordered_map<BasicBlock*, BasicBlock*> child;
static std::unordered_map<BasicBlock*, BasicBlock*> samedom;
static std::unordered_map<BasicBlock*, int> rank;
using BBset = std::set<BasicBlock*>;
static std::map<BasicBlock*, BBset> bucket;
static std::vector<BasicBlock*> dfnumVertex;

static void clearIdomAll()
{
    parent.clear();
    ancenstor.clear();
    best.clear();
    semi.clear();
    child.clear();
    samedom.clear();
    rank.clear();
    bucket.clear();
    dfnumVertex.clear();
}

void IdomGen::compress(BasicBlock* v)
{
    auto a = ancenstor[v];
    if(ancenstor[a])
    {
        compress(a);
        if(semi[best[a]] < semi[best[v]])
            best[v] = best[a];
        ancenstor[v] = ancenstor[a];
    }
}

BasicBlock* IdomGen::eval(BasicBlock* v)
{
    if(!ancenstor[v])
        return best[v];
    compress(v);
    if(semi[best[ancenstor[v]]] >= semi[best[v]])
        return best[v];
    else
        return best[ancenstor[v]];
}
void IdomGen::link(BasicBlock* v, BasicBlock* w)
{
    auto s = w;
    while(semi[best[w]] < semi[best[child[s]]])
    {
        if(rank[s] + rank[child[child[s]]] >= 2 * rank[child[s]])
        {
            ancenstor[child[s]] = s;
            child[s] = child[child[s]];
        }
        else
        {
            rank[child[s]] = rank[s];
            s = ancenstor[s] = child[s];
        }
    }
    best[s] = best[w];
    rank[v] += rank[w];
    if(rank[v] < 2 * rank[w])
        std::swap(s, child[v]);
    while(s)
    {
        ancenstor[s] = v;
        s = child[s];
    }
}

void IdomGen::getDfnum(Function* func)
{
    int num = 0;
    std::unordered_map<BasicBlock*, bool> vis;
    dfnumVertex = std::vector<BasicBlock*>(func->getBasicBlockList().size(), nullptr);
    for(auto bb : func->getBasicBlockList())
        vis[bb] = false;
    BasicBlock::BasicBlockDfs(nullptr, func->getEntryBlock(), [&](BasicBlock* pre, BasicBlock* bb) -> bool{
        if(vis[bb])
            return true;
        ancenstor[bb] = nullptr;
        child[bb] = nullptr;
        best[bb] = bb;
        semi[bb] = num;
        rank[bb] = 1;
        dfnumVertex[num] = bb;
        parent[bb] = pre;
        num++;
        vis[bb] = true;
        return false;
    });
}

void IdomGen::run(Function* func)
{
    clearIdomAll();
    getDfnum(func);
    semi[nullptr] = rank[nullptr] = 0;
    best[nullptr] = nullptr;
    for(auto iter = dfnumVertex.rbegin(); iter != dfnumVertex.rend(); iter++)
    {
        auto n = *iter;
        if(!parent[n]) // The Entry Block
            continue;
        auto p = parent[n];
        for(auto v : n->getPredecessor())
        {
            auto u = eval(v);
            if(semi[u] < semi[n])
                semi[n] = semi[u];
        }
        bucket[dfnumVertex[semi[n]]].insert(n);
        link(p, n);
        for(auto v : bucket[p])
        {
            auto u = eval(v);
            if(semi[u] < semi[v])
                v->setIdom(u);
            else
                v->setIdom(p);
        }
        bucket[p].clear();
    }
    for(auto n : dfnumVertex)
    {
        if(!parent[n])
            continue;
        if(n->getIdom() != dfnumVertex[semi[n]])
            n->setIdom(n->getIdom()->getIdom());
    }
    func->getEntryBlock()->setIdom(nullptr);
}

std::string IdomGen::getName() { return "idom"; }

static std::set<BasicBlock*> doms;
void DomTreeGen::computeAllDom(BasicBlock* bb)
{
    doms.insert(bb);
    bb->getDoms() = std::set<BasicBlock*>(doms);
    for(auto next : bb->getDomTree())
        computeAllDom(next);
    doms.erase(bb);
}

void DomTreeGen::computeDomLevel(BasicBlock* bb, int level)
{
    bb->setDomLevel(level);
    for(auto succ : bb->getDomTree())
        computeDomLevel(succ, level + 1);
}

void DomTreeGen::run(Function* func)
{
    doms.clear();
    for(auto bb : func->getBasicBlockList())
        bb->getDomTree().clear();
    for(auto bb : func->getBasicBlockList())
        if(bb->getIdom())
            bb->getIdom()->getDomTree().push_back(bb);
    computeAllDom(func->getEntryBlock());
    computeDomLevel(func->getEntryBlock(), 0);
}

void DomFrontierGen::computeDF(BasicBlock* bb)
{
    std::vector<BasicBlock*> S;
    bb->getDomFrontier().clear();
    for(auto y : bb->getSuccessor())
        if(y->getIdom() != bb)
        {
            S.push_back(y);
        }
    for(auto c : bb->getDomTree())
    {
        computeDF(c);
        for(auto w : c->getDomFrontier())
            if(!w->getDoms().count(bb) || bb == w)
            {
                S.push_back(w);
            }
    }
    bb->getDomFrontier() = std::move(S);
}

void DomFrontierGen::run(Function* func) 
{
    computeDF(func->getEntryBlock());
}

} // namespace ir

} // namespace sysy
