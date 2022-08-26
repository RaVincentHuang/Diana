#include "sysy/IR/PassManager.h"
#include <stack>


namespace sysy
{

// template<typename Opt> 
// void Pass<Opt>::relatedDfs(Pass<Opt>* pass, DfsFunc func)
// {
//     for(auto pre : pass->getRelated())
//         relatedDfs(pre, func);
//     func(pass);
// }

// template<typename Opt>
// void PassManager<Opt>::addPass(Pass<Opt>* pass)
// {
//     relatedDfs(pass, [this](Pass<Opt>* pass) -> void {
//         this->passes.push_back(pass);
//     });
// }

// template<typename Opt>
// void PassManager<Opt>::run(Opt opt)
// {
//     for(auto pass : passes)
//         pass->run(opt);
// }

namespace ir
{


} // namespace ir


} // namespace sysy
