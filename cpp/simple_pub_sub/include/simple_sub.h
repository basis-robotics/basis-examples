#include <unit/simple_sub/unit_base.h>

class simple_sub : public unit::simple_sub::Base {
public:


    virtual unit::simple_sub::OnChatter::Output OnChatter(const unit::simple_sub::OnChatter::Input& input) override;

};