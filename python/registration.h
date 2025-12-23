#pragma once
// ============================================================================
// Rosetta Introspection Registration for Arch3 Library
// ============================================================================
// This file registers all arch3 classes with Rosetta's CORE introspection.
// No extensions used - only rosetta/rosetta.h
// ============================================================================

#include <rosetta/rosetta.h>

#include <Arch/core/Material.h>
#include <Arch/core/Model.h>
#include <Arch/core/Remote.h>
#include <Arch/core/Surface.h>
#include <Arch/core/Triangle.h>
#include <Arch/postprocess/Postprocess.h>
#include <Arch/postprocess/ParallelPostprocess.h>
#include <Arch/solvers/Gmres.h>
#include <Arch/solvers/ParallelSolver.h>
#include <Arch/solvers/Seidel.h>

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace arch_rosetta {

    using vector3  = std::array<double, 3>;
    using matrix33 = std::array<double, 6>;

    // ========================================================================
    // Registration function - call once before using introspection
    // ========================================================================

    inline void register_arch3_classes() {
        using namespace rosetta;

        // ----------------------------------------------------------------
        // Matrix33
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::Matrix33)
            .constructor<>()
            .constructor<double, double, double, double, double, double>()
            .lambda_method_const<double>("xx", 
                [](const arch::Matrix33& m) { return m.get(0, 0); })
            .lambda_method_const<double>("xy", 
                [](const arch::Matrix33& m) { return m.get(0, 1); })
            .lambda_method_const<double>("xz", 
                [](const arch::Matrix33& m) { return m.get(0, 2); })
            .lambda_method_const<double>("yy", 
                [](const arch::Matrix33& m) { return m.get(1, 1); })
            .lambda_method_const<double>("yz", 
                [](const arch::Matrix33& m) { return m.get(1, 2); })
            .lambda_method_const<double>("zz", 
                [](const arch::Matrix33& m) { return m.get(2, 2); });

        // ----------------------------------------------------------------
        // Vector3
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::Vector3)
            .constructor<double, double, double>()
            .lambda_method_const<double>("x", 
                [](const arch::Vector3& v) { return v[0]; })
            .lambda_method_const<double>("y", 
                [](const arch::Vector3& v) { return v[1]; })
            .lambda_method_const<double>("z", 
                [](const arch::Vector3& v) { return v[2]; });

        // ----------------------------------------------------------------
        // Model
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::Model)
            .constructor<>()
            .method("setHalfSpace", &arch::Model::setHalfSpace)
            .method("halfSpace", &arch::Model::halfSpace)
            .method("nbDof", &arch::Model::nbDof)
            .method("addRemote", &arch::Model::addRemote)
            .method("bounds", &arch::Model::bounds)
            .lambda_method<void, double, double, double>("setMaterial",
                [](arch::Model& model, double p, double y, double d) {
                    model.setMaterial(p, y, d);
                });

        // ----------------------------------------------------------------
        // Surface
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::Surface)
            .constructor<arch::Model*, const std::vector<double>&, const std::vector<int>&>()
            .method("vertices", &arch::Surface::vertices)
            .method("triangles", &arch::Surface::triangles)
            .method("nbTriangles", &arch::Surface::nbTriangles)
            .method("nbVertices", &arch::Surface::nbVertices)
            .method("setBcType", 
                static_cast<void (arch::Surface::*)(const String&, const String&)>(
                    &arch::Surface::setBcType))
            .method("setBcValue",
                static_cast<bool (arch::Surface::*)(const std::vector<double>&)>(
                    &arch::Surface::setBcValues))
            .method("displ", &arch::Surface::displ)
            .method("displPlus", &arch::Surface::displPlus)
            .method("displMinus", &arch::Surface::displMinus);

        // ----------------------------------------------------------------
        // BaseRemote
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::BaseRemote)
            .method("strainAt", &arch::BaseRemote::strainAt)
            .method("stressAt", &arch::BaseRemote::stressAt);
            // .method("traction", &arch::BaseRemote::traction);

        // ----------------------------------------------------------------
        // RemoteStress
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::RemoteStress)
            .inherits_from<arch::BaseRemote>("BaseRemote")
            .constructor<>()
            .method("seth", &arch::RemoteStress::seth)
            .method("setH", &arch::RemoteStress::setH)
            .method("setv", &arch::RemoteStress::setv)
            .method("setTheta", &arch::RemoteStress::setTheta)
            .method("traction",
                static_cast<arch::Vector3 (arch::RemoteStress::*)(
                    const arch::Vector3&, const arch::Vector3&)>(&arch::RemoteStress::traction));

        // ----------------------------------------------------------------
        // UserRemote
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::UserRemote)
            .inherits_from<arch::BaseRemote>("BaseRemote")
            .constructor<std::function<arch::Matrix33(double, double, double)>, bool>();

        // ----------------------------------------------------------------
        // SeidelSolver
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS(arch::SeidelSolver)
            .constructor<arch::Model&>()
            // .method("setEps", &arch::SeidelSolver::setEps)
            // .method("setMaxIter", &arch::SeidelSolver::setMaxIter)
            // .method("onMessage", &arch::SeidelSolver::onMessage)
            // .method("onWarning", &arch::SeidelSolver::onWarning)
            // .method("onError", &arch::SeidelSolver::onError)
            // .method("onProgress", &arch::SeidelSolver::onProgress)
            // .method("stopRequested", &arch::SeidelSolver::stopRequested)
            // .method("onEnd", &arch::SeidelSolver::onEnd)
            .method("run", &arch::SeidelSolver::run);

        // ----------------------------------------------------------------
        // ParallelSolver
        // ----------------------------------------------------------------
        // ROSETTA_REGISTER_CLASS(arch::ParallelSolver)
        //     .constructor<arch::Model&>()
        //     .method("setEps", &arch::ParallelSolver::setEps)
        //     .method("setMaxIter", &arch::ParallelSolver::setMaxIter)
        //     .method("setNbCores", &arch::ParallelSolver::setNbCores)
        //     .method("onMessage", &arch::ParallelSolver::onMessage)
        //     .method("onWarning", &arch::ParallelSolver::onWarning)
        //     .method("onError", &arch::ParallelSolver::onError)
        //     .method("onProgress", &arch::ParallelSolver::onProgress)
        //     .method("stopRequested", &arch::ParallelSolver::stopRequested)
        //     .method("onEnd", &arch::ParallelSolver::onEnd)
        //     .method("run", &arch::ParallelSolver::run);

        // ----------------------------------------------------------------
        // ParallelPostprocess
        // ----------------------------------------------------------------
        ROSETTA_REGISTER_CLASS_AS(arch::ParallelPostprocess, "Postprocess")
            .constructor<arch::Model&>()
            .method("onEnd", &arch::ParallelPostprocess::onEnd)
            .method("onMessage", &arch::ParallelPostprocess::onMessage)
            .method("onWarning", &arch::ParallelPostprocess::onWarning)
            .method("onError", &arch::ParallelPostprocess::onError)
            .method("onProgress", &arch::ParallelPostprocess::onProgress)
            .method("stopRequested", &arch::ParallelPostprocess::stopRequested)
            .method("setNbCores", &arch::ParallelPostprocess::setNbCores)
            .method("setDelta", &arch::ParallelPostprocess::setDelta)
            .method("displAt", &arch::ParallelPostprocess::displAt)
            .method("strainAt", &arch::ParallelPostprocess::strainAt)
            .method("stressAt", &arch::ParallelPostprocess::stressAt)
            .method("displ", &arch::ParallelPostprocess::displ)
            .method("strain", &arch::ParallelPostprocess::strain)
            .method("stress", &arch::ParallelPostprocess::stress)
            .method("burgers", &arch::ParallelPostprocess::burgers)
            .method("burgersPlus", &arch::ParallelPostprocess::burgersPlus)
            .method("burgersMinus", &arch::ParallelPostprocess::burgersMinus)
            .method("residualTractions", &arch::ParallelPostprocess::residualTractions);
    }

} // namespace arch_rosetta
