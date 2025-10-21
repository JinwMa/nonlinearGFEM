#include "ElementAssembler.h"
#include <omp.h>
#include <thread>

void ElementAssembler::createDofbyElements()
{
    std::unique_ptr<ElementStrategy> EPS (new DofCreationStrategy());
    std::shared_ptr<ParallelPartProcessor> PPP = make_shared<ParallelPartProcessor>(d_db, d_mda);
    PPP->setStrategy(std::move(EPS));
    PPP->execute();
}