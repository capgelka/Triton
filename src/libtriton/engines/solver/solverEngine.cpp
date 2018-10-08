//! \file
/*
**  Copyright (C) - Triton
**
**  This program is under the terms of the BSD License.
*/

#include <triton/exceptions.hpp>
#include <triton/solverEngine.hpp>



namespace triton {
  namespace engines {
    namespace solver {

      SolverEngine::SolverEngine(triton::engines::symbolic::SymbolicEngine* symbolicEngine) {
        this->kind           = triton::engines::solver::SOLVER_INVALID;
        this->symbolicEngine = symbolicEngine;

        #ifdef Z3_INTERFACE
        /* By default we initialized the z3 solver */
        this->setSolver(triton::engines::solver::SOLVER_Z3);
        #endif
      }


      triton::engines::solver::solvers_e SolverEngine::getSolver(void) const {
        return this->kind;
      }


      const triton::engines::solver::SolverInterface* SolverEngine::getSolverInstance(void) const {
        if (!this->solver)
          throw triton::exceptions::SolverEngine("SolverEngine::getSolver(): Solver undefined.");
        return this->solver.get();
      }


      void SolverEngine::setSolver(triton::engines::solver::solvers_e kind) {
        /* Allocate and init the good solver */
        switch (kind) {
          #ifdef Z3_INTERFACE
          case triton::engines::solver::SOLVER_Z3:
            /* init the new instance */
            this->solver.reset(new(std::nothrow) triton::engines::solver::Z3Solver(this->symbolicEngine));
            if (this->solver == nullptr)
              throw triton::exceptions::SolverEngine("SolverEngine::setSolver(): Not enough memory.");
            break;
          #endif

          default:
            throw triton::exceptions::SolverEngine("SolverEngine::setSolver(): Solver not supported.");
            break;
        }

        /* Setup global variables */
        this->kind = kind;
      }


      void SolverEngine::setCustomSolver(triton::engines::solver::SolverInterface* customSolver) {
        if (customSolver == nullptr)
          throw triton::exceptions::SolverEngine("SolverEngine::setCustomSolver(): custom solver cannot be null.");

        /* Define the custom solver as current solver */
        this->solver.reset(customSolver);

        /* Setup global variables */
        this->kind = triton::engines::solver::SOLVER_CUSTOM;
      }


      bool SolverEngine::isValid(void) const {
        if (this->kind == triton::engines::solver::SOLVER_INVALID)
          return false;
        return true;
      }


      std::map<triton::uint32, SolverModel> SolverEngine::getModel(const triton::ast::SharedAbstractNode& node) const {
        if (!this->solver)
          return std::map<triton::uint32, SolverModel>{};
        printf("%s\n", "DEBUG");
        return this->solver->getModel(node);
      }


      std::list<std::map<triton::uint32, SolverModel>> SolverEngine::getModels(const triton::ast::SharedAbstractNode& node, triton::uint32 limit) const {
        if (!this->solver)
          return std::list<std::map<triton::uint32, SolverModel>>{};
        return this->solver->getModels(node, limit);
      }


      bool SolverEngine::isSat(const triton::ast::SharedAbstractNode& node) const {
        if (!this->solver)
          return false;
        return this->solver->isSat(node);
      }


      std::string SolverEngine::getName(void) const {
        if (!this->solver)
          return "n/a";
        return this->solver->getName();
      }

    };
  };
};
