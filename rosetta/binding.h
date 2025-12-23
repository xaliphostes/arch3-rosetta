#pragma once
#include <Arch/core/Inequality.h>
#include <Arch/core/Material.h>
#include <Arch/core/Model.h>
#include <Arch/core/Remote.h>
#include <Arch/core/Surface.h>
#include <Arch/postprocess/Postprocess.h>
#include <Arch/solvers/Seidel.h>
#include <functional>
#include <rosetta/rosetta.h>

void register_rosetta() {

    ROSETTA_REGISTER_CLASS(arch::Model)
        .constructor<>()
        .method("setHalfspace", &arch::Model::setHalfSpace)
        .method("setMaterial", &arch::Model::setMaterial)
        .method("addRemote", &arch::Model::addRemote)
        .method("nbDof", &arch::Model::nbDof)
        // .method("addSurface", &arch::Model::addSurface)
        .method("addSurface", R_OVERLOAD(arch::Model, arch::Surface *,
                                         addSurface, arch::Surface *));

    ROSETTA_REGISTER_CLASS(arch::Surface)
        .constructor<arch::Model*, const std::vector<double> &, const std::vector<int> &>()
        .method("setBcType", R_OVERLOAD(arch::Surface, void, setBcType, const arch::String &, const arch::String &))
        .method("setBcValues", R_OVERLOAD(arch::Surface, bool, setBcValues, const std::vector<double> &));
    // .method("addTic", arch::Surface::addTic)
    // .method("addDic", arch::Surface::addDic);

    // TODO
    ROSETTA_REGISTER_CLASS(arch::Coulomb).constructor<>();
    ROSETTA_REGISTER_CLASS(arch::CoulombOrtho).constructor<>();

    ROSETTA_REGISTER_CLASS(arch::Material)
        .constructor<double, double, double>()
        .method("poisson", &arch::Material::poisson)
        .method("young", &arch::Material::young)
        .method("density", &arch::Material::density)
        .method("shear", &arch::Material::shear)
        .method("lame", &arch::Material::lame)
        .method("thermalExpansion", &arch::Material::thermalExpansion)
        .method("setPoisson", &arch::Material::setPoisson)
        .method("setYoung", &arch::Material::setYoung)
        .method("setDensity", &arch::Material::setDensity)
        .method("setThermalExpansion", &arch::Material::setThermalExpansion);

    ROSETTA_REGISTER_CLASS(arch::BaseRemote);

    ROSETTA_REGISTER_CLASS(arch::RemoteStress)
        .inherits_from<arch::BaseRemote>("BaseRemote")
        .constructor<>()
        .method("seth", &arch::RemoteStress::seth) 
        .method("setH", &arch::RemoteStress::setH)
        .method("setv", &arch::RemoteStress::setv)
        .method("setTheta", &arch::RemoteStress::setTheta);

    ROSETTA_REGISTER_CLASS(arch::UserRemote)
        .inherits_from<arch::BaseRemote>("BaseRemote")
        .constructor<std::function<arch::Matrix33(double, double, double)>>();

    ROSETTA_REGISTER_CLASS(arch::IterativeSolver)
        .method("run", &arch::IterativeSolver::run);

    ROSETTA_REGISTER_CLASS_AS(arch::SeidelSolver, "SeidelSolver")
        .inherits_from<arch::IterativeSolver>("IterativeSolver")
        .constructor<arch::Model&>()
        .lambda_method<bool>("run", [](arch::SeidelSolver& self) { return self.run(); });

    ROSETTA_REGISTER_CLASS_AS(arch::Postprocess, "Forward")
        .constructor<arch::Model &>()
        .method("displ", &arch::Postprocess::displ)
        .method("strain", &arch::Postprocess::strain)
        .method("stress", &arch::Postprocess::stress)
        .method("displAt", &arch::Postprocess::displAt)
        .method("strainAt", &arch::Postprocess::strainAt)
        .method("stressAt", &arch::Postprocess::stressAt)
        .method("burgers", &arch::Postprocess::burgers)
        .method("burgersPlus", &arch::Postprocess::burgersPlus)
        .method("burgersMinus", &arch::Postprocess::burgersMinus)
        .method("residualTractions", &arch::Postprocess::residualTractions);
}
