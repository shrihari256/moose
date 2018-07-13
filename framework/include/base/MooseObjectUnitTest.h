#ifndef MOOSEOBJECTUNITTEST_H
#define MOOSEOBJECTUNITTEST_H

#include "gtest/gtest.h"

#include "MooseMesh.h"
#include "GeneratedMesh.h"
#include "FEProblem.h"
#include "AppFactory.h"

/**
 * Base class for building basic unit tests for MOOSE objects that can live alone (like user
 * objects, etc.)
 *
 * This class builds the basic objects that are needed in order to test a MOOSE object. Those are a
 * mesh and an FEProblem.  To build a unit test, inherit from this class and build your test using
 * the following template:
 *
 * In your .h file:
 *
 * class MyUnitTest : public MooseObjectUnitTest
 * {
 * public:
 *   MyUnitTest() : MooseObjectUnitTest("MyAppUnitApp")
 *   {
 *     // if you are using the old registration system, you want to register your objects using this
 *     // call. Otherwise, you do not need it.
 *     registerObjects(_factory);
 *     buildObjects();
 *   }
 *
 * protected:
 *   void registerObjects(Factory & factory)
 *   {
 *     // register your objects as usual, we have to be in a method like this so that the register
 *     // macros work
 *     registerUserObject(MyObjectThatIAmTesting);
 *   }
 *
 *   void buildObjects()
 *   {
 *     // build your object like so
 *     InputParameters pars = _factory.getValidParams("MyObjectThatIAmTesting");
 *     _fe_problem->addUserObject("MyObjectThatIAmTesting", "fp", uo_pars);
 *     _obj = &_fe_problem->getUserObject<MyObjectThatIAmTesting>("fp");
 *   }
 *
 *   // member variable used later in the actual tests
 *   const MyObjectThatIAmTesting * _obj;
 * };
 *
 * In your .C file
 *
 * TEST_F(MyObjectThatIAmTesting, test)
 * {
 *   EXPECT_EQ("testing", _obj->method(par1, par2));
 * }
 *
 * NOTE: Testing mesh-bound objects like Kernels, BCs, etc. is not possible with this class.
 */
class MooseObjectUnitTest : public ::testing::Test
{
public:
  /**
   * @param app_name The name of client's application
   */
  MooseObjectUnitTest(const std::string & app_name)
    : _app(AppFactory::createAppShared(app_name, 0, nullptr)), _factory(_app->getFactory())
  {
    buildObjects();
  }

protected:
  void buildObjects()
  {
    InputParameters mesh_params = _factory.getValidParams("GeneratedMesh");
    mesh_params.set<std::string>("_object_name") = "name1";
    mesh_params.set<MooseEnum>("dim") = "3";
    _mesh = libmesh_make_unique<GeneratedMesh>(mesh_params);

    InputParameters problem_params = _factory.getValidParams("FEProblem");
    problem_params.set<MooseMesh *>("mesh") = _mesh.get();
    problem_params.set<std::string>("_object_name") = "name2";
    _fe_problem = _factory.create<FEProblem>("FEProblem", "problem", problem_params);
  }

  std::unique_ptr<MooseMesh> _mesh;
  std::shared_ptr<MooseApp> _app;
  Factory & _factory;
  std::shared_ptr<FEProblem> _fe_problem;
};

#endif
