#include "Routing.h"

std::ostream& operator<<(std::ostream& os, const Routing& rt)
{
    os << "Routing(" << rt.adep;
    if (rt.ades.size() > 0) {
        os << "->" << rt.ades << ",";
    }
    else {
        os << ",";
    }

    os << rt.minlvl << "<RFL<" << rt.maxlvl << "): ";

    for (int i = 0; i < rt.waypts.size(); i++) {
        os << rt.waypts[i];
        if (i < rt.waypts.size() - 1) {
            os << ";";
        }
    }

    return os;
}
