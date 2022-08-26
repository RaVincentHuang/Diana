#include "sysy/Analysis/PostDomInfo.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include <set>
#include <unordered_map>
namespace sysy
{
namespace ir
{

PostDomInfo::PostDomInfo() 
{
    addPass(new IpdomGen);
    addPass(new PostDomTreeGen);
    addPass(new PostDomFrontierGen);
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

static void clearIpdomAll()
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

void IpdomGen::compress(BasicBlock* v)
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

BasicBlock* IpdomGen::eval(BasicBlock* v)
{
    if(!ancenstor[v])
        return best[v];
    compress(v);
    if(semi[best[ancenstor[v]]] >= semi[best[v]])
        return best[v];
    else
        return best[ancenstor[v]];
}
void IpdomGen::link(BasicBlock* v, BasicBlock* w)
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

void IpdomGen::getDfnum(Function* func)
{
    int num = 0;
    std::unordered_map<BasicBlock*, bool> vis;
    dfnumVertex = std::vector<BasicBlock*>(func->getBasicBlockList().size(), nullptr);
    for(auto bb : func->getBasicBlockList())
        vis[bb] = false;
    BasicBlock::BasicBlockDfsInv(nullptr, func->getExitBlock(), [&](BasicBlock* pre, BasicBlock* bb) -> bool{
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

void IpdomGen::run(Function* func)
{
    clearIpdomAll();
    getDfnum(func);
    semi[nullptr] = rank[nullptr] = 0;
    best[nullptr] = nullptr;
    for(auto iter = dfnumVertex.rbegin(); iter != dfnumVertex.rend(); iter++)
    {
        auto n = *iter;
        if(!parent[n]) // The Entry Block
            continue;
        auto p = parent[n];
        for(auto v : n->getSuccessor())
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
                v->setIpdom(u);
            else
                v->setIpdom(p);
        }
        bucket[p].clear();
    }
    for(auto n : dfnumVertex)
    {
        if(!parent[n])
            continue;
        if(n->getIpdom() != dfnumVertex[semi[n]])
            n->setIpdom(n->getIpdom()->getIpdom());
    }
    func->getExitBlock()->setIpdom(nullptr);
}

std::string IpdomGen::getName() { return "ipdom"; }

static std::set<BasicBlock*> pdoms;
void PostDomTreeGen::computeAllPostDom(BasicBlock* bb)
{
    pdoms.insert(bb);
    bb->getPdoms() = std::set<BasicBlock*>(pdoms);
    for(auto next : bb->getPdomTree())
        computeAllPostDom(next);
    pdoms.erase(bb);
}

void PostDomTreeGen::computePostDomLevel(BasicBlock* bb, int level)
{
    bb->setPdomLevel(level);
    for(auto succ : bb->getPdomTree())
        computePostDomLevel(succ, level + 1);
}

void PostDomTreeGen::run(Function* func)
{
    pdoms.clear();
    for(auto bb : func->getBasicBlockList())
        bb->getPdomTree().clear();
    for(auto bb : func->getBasicBlockList())
        if(bb->getIpdom())
            bb->getIpdom()->getPdomTree().push_back(bb);
    computeAllPostDom(func->getEntryBlock());
    computePostDomLevel(func->getEntryBlock(), 0);
}

void PostDomFrontierGen::computeRDF(BasicBlock* bb)
{
    std::vector<BasicBlock*> S;
    bb->getPdomFrontier().clear();
    for(auto y : bb->getPredecessor())
        if(y->getIpdom() != bb)
        {
            S.push_back(y);
        }
    for(auto c : bb->getPdomTree())
    {
        computeRDF(c);
        for(auto w : c->getPdomFrontier())
            if(!w->getPdoms().count(bb) || bb == w)
            {
                S.push_back(w);
            }
    }
    bb->getPdomFrontier() = std::move(S);
}

void PostDomFrontierGen::run(Function* func) 
{
    computeRDF(func->getExitBlock());
}


} // namespace ir
    
} // namespace sysy
