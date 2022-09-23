#include "appwindow.h"
#include <iostream>

class SimpleFilterModel : public slint::Model<slint::SharedString>
{
public:
    SimpleFilterModel(std::shared_ptr<slint::Model<slint::SharedString>> source_model)
        : m_source_model(source_model)
    {
        update_rows();
    }

    void set_filter(slint::SharedString filter)
    {
        m_filter = filter;
        update_rows();
    }

    int row_count() const override { return m_visible_rows.size(); }

    std::optional<slint::SharedString> row_data(int i) const override
    {
        return m_source_model->row_data(m_visible_rows[i]);
    }

private:
    void update_rows()
    {
        m_visible_rows.clear();
        for (int i = 0, count = m_source_model->row_count(); i < count; ++i) {
            if (accept_row(i)) {
                m_visible_rows.push_back(i);
            }
        }
        reset();
    }

    bool accept_row(int i) const
    {
        if (m_filter.empty()) {
            return true;
        }
        if (auto source = m_source_model->row_data(i)) {
            std::cerr << "checking row " << i << " text " << *source;
            return source->starts_with(m_filter);
        } else {
            return false;
        }
    }

    std::shared_ptr<slint::Model<slint::SharedString>> m_source_model;
    slint::SharedString m_filter;
    std::vector<int> m_visible_rows;
};

int main(int argc, char **argv)
{
    auto ui = AppWindow::create();

    auto orig_model = ui->get_all_objects();
    auto filtered_model = std::make_shared<SimpleFilterModel>(orig_model);

    ui->on_update_filter([&](slint::SharedString filter) { filtered_model->set_filter(filter); });

    ui->set_objects(filtered_model);

    ui->run();
    return 0;
}
