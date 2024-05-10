#pragma once
#include <MBLisp/Evaluator.h>

namespace MBTUI
{
    MBLisp::Ref<MBLisp::Scope> GetDefaultBindings(MBLisp::Evaluator& AssociatedEvaluator);
    void AddDefaultBindings(MBLisp::Evaluator& AssociatedEvaluator,MBLisp::Scope& ScopeToModify);
}
