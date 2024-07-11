#include <unit/simple_pub/unit_base.h>

class simple_pub : public unit::simple_pub::Base {
public:


    virtual unit::simple_pub::PublishAt1Hz::Output PublishAt1Hz(const unit::simple_pub::PublishAt1Hz::Input& input) override;

};