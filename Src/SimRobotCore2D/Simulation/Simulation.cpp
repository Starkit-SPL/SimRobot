/**
 * @file Simulation.cpp
 *
 * This file implements a class which executes the actual simulation.
 *
 * @author Arne Hasselbring
 */

#include "Simulation.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Platform/System.h"
#include "Parser/Element.h"
#include "Parser/Parser.h"
#include "Simulation/Geometries/Geometry.h"
#include "Simulation/Scene.h"
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>

Simulation* Simulation::simulation = nullptr;

Simulation::Simulation()
{
  ASSERT(!simulation);
  simulation = this;
}

Simulation::~Simulation()
{
  for(Element* element : elements)
    delete element;

  if(staticBody)
    world->DestroyBody(staticBody);

  delete world;

  ASSERT(simulation == this);
  simulation = nullptr;
}

bool Simulation::loadFile(const std::string& fileName, std::list<std::string>& errors)
{
  ASSERT(!scene);

  // Load the scene.
  Parser parser;
  if(!parser.parse(fileName, errors))
    return false;

  ASSERT(scene);

  // Create a world (with zero gravity because of the top-down view).
  world = new b2World(b2Vec2_zero);

  world->SetContactListener(this);

  // Create a body to which all fixtures of compounds are attached.
  b2BodyDef bodyDef;
  bodyDef.type = b2_staticBody;
  bodyDef.userData = this;
  staticBody = world->CreateBody(&bodyDef);

  scene->pose.SetIdentity();
  scene->createPhysics();

  return true;
}

void Simulation::registerObjects()
{
  scene->fullName = QString::fromStdString(scene->name);
  CoreModule::application->registerObject(*CoreModule::module, *scene, nullptr);
  scene->registerObjects();
}

void Simulation::doSimulationStep()
{
  // Update internal variables.
  ++simulationStep;
  simulatedTime += scene->stepLength;

  // Execute the Box2D step.
  world->Step(scene->stepLength, scene->velocityIterations, scene->positionIterations);

  updateFrameRate();
}

void Simulation::BeginContact(b2Contact* contact)
{
  auto* const geom1 = static_cast<Geometry*>(contact->GetFixtureA()->GetUserData());
  auto* const geom2 = static_cast<Geometry*>(contact->GetFixtureB()->GetUserData());

  for(SimRobotCore2D::CollisionCallback* callback : geom1->callbacks)
    callback->collided(*geom1, *geom2);

  for(SimRobotCore2D::CollisionCallback* callback : geom2->callbacks)
    callback->collided(*geom2, *geom1);

  ++collisions;
}

void Simulation::EndContact(b2Contact* contact)
{
  --collisions;
}

void Simulation::updateFrameRate()
{
  const unsigned int currentTime = System::getTime();
  const unsigned int timeDiff = currentTime - lastFrameRateComputationTime;
  if(timeDiff > 2000)
  {
    const float frameRate = static_cast<float>(simulationStep - lastFrameRateComputationStep) / (0.001f * static_cast<float>(timeDiff));
    currentFrameRate = static_cast<unsigned int>(frameRate + 0.5f);
    lastFrameRateComputationStep = simulationStep;
    lastFrameRateComputationTime = currentTime;
  }
}
