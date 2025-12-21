#include "binding.h"
#include <rosetta/extensions/generators/py_generator.h>

using namespace arch;

BEGIN_PY_MODULE(pyarch, "Python bindings for arch") {
    std::cerr << "--> begin rosetta registration...";
    register_rosetta();
    std::cerr << " done." << std::endl;

    std::cerr << "--> begin python registration...";

    BIND_SHARED_PTR(arch::Surface);

    BIND_PY_CLASS(Model);
    
    BIND_PY_CLASS_AS(Surface, "Surface");
    // BIND_PY_CLASS(Coulomb);
    // BIND_PY_CLASS(CoulombOrtho);

    BIND_PY_CLASS(Material);

    BIND_PY_CLASS(BaseRemote);
    BIND_PY_DERIVED_CLASS(RemoteStress, BaseRemote);
    BIND_PY_DERIVED_CLASS(UserRemote, BaseRemote);

    BIND_PY_CLASS(IterativeSolver);
    BIND_PY_DERIVED_CLASS(SeidelSolver, IterativeSolver);
    BIND_PY_CLASS(Postprocess);
    std::cerr << " done." << std::endl;
}
END_PY_MODULE()