#include "bag.hpp"

namespace Eval {
const std::shared_ptr<Eval::BooleanBag> TRUE_BAG =
    std::make_shared<Eval::BooleanBag>(true);

const std::shared_ptr<Eval::BooleanBag> FALSE_BAG =
    std::make_shared<Eval::BooleanBag>(false);

const std::shared_ptr<Eval::NullBag> NULL_BAG =
    std::make_shared<Eval::NullBag>();
}  // namespace Eval