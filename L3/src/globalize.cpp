#include <algorithm>
#include <iostream>
#include <globalize.h>

namespace L3 {
    bool comp(Label *l1, Label *l2) {
        return l1->get().length() > l2->get().length();
    }

    void labelGlobalize(Program *p) {
        vector<Label *> labels;
        for (auto f : p->functions) {
            for (auto i : f->instructions) {
                Instruction_label *l = dynamic_cast<Instruction_label *>(i);
                if (l) {
                    labels.push_back(l->label);
                }
            }
        }

        if (labels.size() == 0)
            return;
        sort(labels.begin(), labels.end(), comp);
        cout << labels[0]->get().length() << endl;;


        ::string llg = labels[0]->get().substr(1);
        llg = llg + "_global_";

        for (auto f : p->functions) {
            ::string fname = f->name.substr(1);
            for (auto i : f->instructions) {
                Instruction_label *l = dynamic_cast<Instruction_label *>(i);
                if (l) {
                    ::string old = l->label->get().substr(1);
                    l->label = new Label(llg + fname + old);
                    cout << "label: " << l->label->get() << endl;
                }
            }
        }
    }
}